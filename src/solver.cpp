#include <solver.hpp>

Solver::Solver(Particles *_particles, float viewport_width, float viewport_height)
    : particles(_particles), logger("log.txt", _particles){
    
    VIEWPORT_WIDTH = viewport_width;
    VIEWPORT_HEIGHT = viewport_height;

    boundary = {
        glm::vec3(-1.0, 0.0, -VIEWPORT_WIDTH),
        glm::vec3(0.0, -1.0, -VIEWPORT_HEIGHT),
        glm::vec3(1.0, 0.0, 0.0),
        glm::vec3(0.0, 1.0, 0.0)
    };

    Solver::PARTICLE_MASS = 1.0f;

    glDispatchCompute(num_operations, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);PARTICLE_MASS = 1.0f;

    grid_width = (size_t)std::ceil(VIEWPORT_WIDTH / smoothing_length);
    grid_height = (size_t)std::ceil(VIEWPORT_HEIGHT / smoothing_length);
    grid_size = grid_width * grid_height;
    grid.resize(grid_size);

    num_operations = particles->num_particles / 256 + 1;
    
    // resize spatialOffsets
    particles->spatialOffsets.resize(grid_size);
    // std::fill(particles->spatialOffsets.begin(), particles->spatialOffsets.end(), -1);

    // set the SSBO for spatial offsets
    glGenBuffers(1, &particles->spatialOffsetSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, particles->spatialOffsetSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, particles->spatialOffsets.size() * sizeof(int), particles->spatialOffsets.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, particles->spatialOffsetSSBO);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    
    externForceAndIntegrateShader = new Shader("External Forces", "./shaders/solver/exforce_integrate.comp");
    boundaryCheckShader = new Shader("Boundary Check", "./shaders/solver/boundary_check.comp");
    spatialHashingSortShader = new Shader("Spatial Hash", "./shaders/solver/spatial_hash_sort.comp");
    pressureSolveShader = new Shader("Pressure Solve", "./shaders/solver/pressure_solve.comp");
    projectionCorrectionShader = new Shader("Projection Correction", "./shaders/solver/projection_correction.comp");
}

Solver::~Solver(){
}


void Solver::Update(){
    for (int i = 0; i < SOLVER_STEPS; i++){
        ExForcesIntegrate();
        SpatialHashingSort();
        PressureSolve();
        ProjectionCorrection();
        BoundaryCheck();
    }

}

void Solver::BoundaryCheck(){
    boundaryCheckShader->use();
    boundaryCheckShader->setFloat("dt", DT);
    boundaryCheckShader->setFloat("viewWidth", VIEWPORT_WIDTH);
    boundaryCheckShader->setFloat("radius", Particles::radius);

    glDispatchCompute(num_operations, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

}



void Solver::ExForcesIntegrate(){
    externForceAndIntegrateShader->use();
    externForceAndIntegrateShader->setFloat("dt", DT);

    glDispatchCompute(num_operations, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

}


void Solver::SpatialHashingSort(){

    enum class Operation{
        SPATIAL_HASH, // 0
        RESET, // 1
        OFFSETS // 2
    };

    spatialHashingSortShader->use();

    // stage 1: spatial hashing
    spatialHashingSortShader->setInt("operation", (int)Operation::SPATIAL_HASH);
    spatialHashingSortShader->setFloat("cellSize", smoothing_length);
    spatialHashingSortShader->setInt("gridWidth", grid_width);
    spatialHashingSortShader->setInt("gridHeight", grid_height);
    glDispatchCompute(num_operations, 1, 1);    
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


    // get data from the SSBO
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, particles->spatialIndexSSBO);
    int* spatial_indices_data = (int*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    if (spatial_indices_data){
        std::copy(spatial_indices_data, spatial_indices_data + particles->num_particles * 4, particles->spatialIndices.begin());
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    } else{
        std::cerr << "Error mapping spatial indices SSBO" << std::endl;
        return;
    }

    // convert to glm::vec4
    std::vector<glm::vec4> spatialIndicesVec(particles->num_particles);
    for (size_t i = 0; i < particles->num_particles; i++){
        spatialIndicesVec[i] = glm::vec4(spatial_indices_data[i * 4], 
                                         spatial_indices_data[i * 4 + 1], 
                                         spatial_indices_data[i * 4 + 2],
                                         spatial_indices_data[i * 4 + 3]);
    }

    // sort the spatial indices
    std::sort(spatialIndicesVec.begin(), spatialIndicesVec.end(), [](const glm::vec4 &a, const glm::vec3 &b){
        return a.y < b.y;
    });

    for (size_t i = 0; i < particles->num_particles; i++){
        spatial_indices_data[i * 4] = spatialIndicesVec[i].x;
        spatial_indices_data[i * 4 + 1] = spatialIndicesVec[i].y;
        spatial_indices_data[i * 4 + 2] = spatialIndicesVec[i].z;
        spatial_indices_data[i * 4 + 3] = spatialIndicesVec[i].w;
    }


    // copy back to the SSBO
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, particles->spatialIndexSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, particles->num_particles * 4 * sizeof(int), spatial_indices_data, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, particles->spatialIndexSSBO);


    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // set offsets to -1
    spatialHashingSortShader->setInt("operation", (int)Operation::RESET);
    glDispatchCompute(grid_size / 256 + 1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


    // stage 3: offsets
    spatialHashingSortShader->setInt("operation", (int)Operation::OFFSETS);
    glDispatchCompute(num_operations, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

}


void Solver::PressureSolve(){
    pressureSolveShader->use();
    pressureSolveShader->setFloat("dt", DT);
    pressureSolveShader->setFloat("smoothing_length", smoothing_length);
    pressureSolveShader->setInt("gridWidth", grid_width);
    pressureSolveShader->setInt("gridHeight", grid_height);
    pressureSolveShader->setInt("MAX_NEIGHBORS", Particles::MAX_NEIGHBOURS);
    pressureSolveShader->setFloat("PARTICLE_MASS", PARTICLE_MASS);
    pressureSolveShader->setFloat("KERNEL_FACTOR", KERNEL_FACTOR);
    pressureSolveShader->setFloat("KERNEL_NORM", KERNEL_NORM);
    pressureSolveShader->setFloat("STIFFNESS", STIFFNESS);
    pressureSolveShader->setFloat("STIFF_APPROX", STIFF_APPROX);
    pressureSolveShader->setFloat("REST_DENSITY", REST_DENSITY);

    glDispatchCompute(num_operations, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

}

void Solver::ProjectionCorrection(){
    projectionCorrectionShader->use();

    projectionCorrectionShader->setFloat("dt", DT);
    projectionCorrectionShader->setFloat("smoothing_length", smoothing_length);
    projectionCorrectionShader->setInt("gridWidth", grid_width);
    projectionCorrectionShader->setInt("gridHeight", grid_height);
    projectionCorrectionShader->setInt("MAX_NEIGHBORS", Particles::MAX_NEIGHBOURS);
    projectionCorrectionShader->setFloat("PARTICLE_MASS", PARTICLE_MASS);
    projectionCorrectionShader->setFloat("KERNEL_FACTOR", KERNEL_FACTOR);
    projectionCorrectionShader->setFloat("KERNEL_NORM", KERNEL_NORM);
    projectionCorrectionShader->setFloat("REST_DENSITY", REST_DENSITY);
    projectionCorrectionShader->setFloat("LINEAR_VISC", LINEAR_VISC);
    projectionCorrectionShader->setFloat("QUAD_VISC", QUAD_VISC);
    projectionCorrectionShader->setFloat("SURFACE_TENSION", SURFACE_TENSION);

    glDispatchCompute(num_operations, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}