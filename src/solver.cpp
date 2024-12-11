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

    num_operations = (particles->num_particles + 255)/ 256;
    
    // resize spatialOffsets
    particles->spatialOffsets.resize(grid_size);

    // set the SSBO for spatial offsets
    glGenBuffers(1, &particles->spatialOffsetSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, particles->spatialOffsetSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, particles->spatialOffsets.size() * sizeof(int), particles->spatialOffsets.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, particles->spatialOffsetSSBO);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    
    externForceAndIntegrateShader = new Shader("External Forces", "./shaders/solver/exforce_integrate.comp");
    boundaryCheckShader = new Shader("Boundary Check", "./shaders/solver/boundary_check.comp");
    spatialHashingSortShader = new Shader("Spatial Hash", "./shaders/solver/spatial_hash_sort.comp");
    bitonicMergeSortShader = new Shader("Bitonic Merge Sort", "./shaders/solver/bitonic_merge_sort.comp");
    resetOffsetsShader = new Shader("Reset Offsets", "./shaders/solver/reset_offsets.comp");
    spatialOffsetShader = new Shader("Spatial Offsets", "./shaders/solver/spatial_offsets.comp");
    pressureSolveShader = new Shader("Pressure Solve", "./shaders/solver/pressure_solve.comp");
    projectionCorrectionShader = new Shader("Projection Correction", "./shaders/solver/projection_correction.comp");
}

Solver::~Solver(){
}


void Solver::Update(){
    for (int i = 0; i < SOLVER_STEPS; i++){
        ExForcesIntegrate();
        SpatialHashingSort();
        BitonicMergeSort();
        ResetOffsets();
        SpatialOffsets();
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
    spatialHashingSortShader->use();
    spatialHashingSortShader->setFloat("cellSize", smoothing_length);
    spatialHashingSortShader->setInt("gridWidth", grid_width);
    spatialHashingSortShader->setInt("gridHeight", grid_height);
    glDispatchCompute(num_operations, 1, 1);    
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void Solver::BitonicMergeSort(){
    bitonicMergeSortShader->use();
    bitonicMergeSortShader->setInt("numEntries", particles->num_particles);
    int numStages = (int)ceil(log2(particles->num_particles));

        for (int stageIndex = 0; stageIndex < numStages; stageIndex++)
        {
            for (int stepIndex = 0; stepIndex < stageIndex + 1; stepIndex++)
            {
                // Calculate some pattern stuff
                int groupWidth = 1 << (stageIndex - stepIndex);
                int groupHeight = 2 * groupWidth - 1;
                bitonicMergeSortShader->setInt("groupWidth", groupWidth);
                bitonicMergeSortShader->setInt("groupHeight", groupHeight);
                bitonicMergeSortShader->setInt("stepIndex", stepIndex);
                glDispatchCompute(1 << (numStages - 1), 1, 1);
                glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT); 
            }
        }
}


void Solver::ResetOffsets(){
    resetOffsetsShader->use();
    glDispatchCompute((grid_size + 255) / 256, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void Solver::SpatialOffsets(){
    spatialOffsetShader->use();
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
