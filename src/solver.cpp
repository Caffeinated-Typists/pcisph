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

    grid_width = (size_t)std::ceil(VIEWPORT_WIDTH / smoothing_length);
    grid_height = (size_t)std::ceil(VIEWPORT_HEIGHT / smoothing_length);
    grid_size = grid_width * grid_height;
    grid.resize(grid_size);

    // GridInsert();
    externForceAndIntegrateShader = new Shader("./shaders/solver/exforce_integrate.comp");
    boundaryCheckShader = new Shader("./shaders/solver/boundary_check.comp");
}

Solver::~Solver(){
}


void Solver::Update(){
    // logger.logGridLocations();
    // logger.logFirstNumberOfNeighbours();
    // logger.logPositions();
    // logger.checkNegativePositions();
    // for (int i = 0; i < SOLVER_STEPS; i++){
    //     ExternalForces();
    //     Integrate();
    //     // GridInsert();
    //     // PressureSolve();
    //     // ProjectionStep();
    //     // CorrectionStep();
    //     BoundaryCheck();
    // }

    for (int i = 0; i < SOLVER_STEPS; i++){
        ExForcesIntegrate();
        BoundaryCheck();
    }

}

void Solver::BoundaryCheck(){
    boundaryCheckShader->use();
    boundaryCheckShader->setFloat("dt", DT);
    boundaryCheckShader->setFloat("viewWidth", VIEWPORT_WIDTH);
    boundaryCheckShader->setFloat("radius", Particles::radius);

    glDispatchCompute(particles->num_particles / 256 + 1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

}



void Solver::ExForcesIntegrate(){
    externForceAndIntegrateShader->use();

    externForceAndIntegrateShader->setFloat("dt", DT);

    glDispatchCompute(particles->num_particles / 256 + 1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

}



// void Solver::GridInsert(){
//     // clear the grid
//     std::for_each(std::execution::par_unseq, grid.begin(), grid.end(), [](Point*& p){
//         p = nullptr;
//     });

//     std::for_each(std::execution::seq, particles->begin(), particles->end(), [&](Point& p){
//         p.grid_x = std::clamp((int)(p.position.x / smoothing_length), 1, (int)grid_width - 2);
//         p.grid_y = std::clamp((int)(p.position.y / smoothing_length), 1, (int)grid_height - 2);
//         p.next = grid[p.grid_x + grid_width * p.grid_y];
//         grid[p.grid_x + grid_width * p.grid_y] = &p;
//     });

// }

// void Solver::PressureSolve(){
//     std::for_each(std::execution::par_unseq, particles->begin(), particles->end(), [&](Point& p){
//         p.size = 0;
//         p.density = 0.0f;
//         p.dv = 0.0f;

//         // size_t idx = &p - &(*particles)[0];

//         // if (idx == 0){
//         //     std::cout << "particle grid: " << p.grid_x + p.grid_y * grid_width << std::endl;
//         //     std::cout << "particle position: " << p.position.x << " " << p.position.y << std::endl;
//         // }

//         for (size_t i = p.grid_x - 1; i <= p.grid_x + 1; i++){
//             for (size_t j = p.grid_y - 1; j <= p.grid_y + 1; j++){
//                 int grid_index = i + j * grid_width;
//                 // if (idx == 0){
//                 //     std::cout << "neighbour grid: " << grid_index << std::endl;
//                 // }
//                 // int cnt = 0;
//                 // int candidates = 0;
//                 for(auto pj = grid[i + j * grid_width]; pj != nullptr; pj = pj->next){
//                     // cnt++;
//                     if (p.size == Point::MAX_NEIGHBOURS) break;


//                     const Point& neighbour = *pj;

//                     float r2 = glm::dot(p.position - neighbour.position, p.position - neighbour.position);
                    
//                     // if (idx == 0){
//                     //     std::cout << "neighbour position: " << neighbour.position.x << " " << neighbour.position.y << std::endl;
//                     //     std::cout << "distance: " << r2 << std::endl;
//                     // }

//                     if (r2 > smoothing_length2 || r2 < EPS2) continue;
//                     // candidates++;
//                     float r = std::sqrt(r2);
//                     float a = 1. - r / smoothing_length;

//                     p.density += PARTICLE_MASS * a * a * a * KERNEL_FACTOR;
//                     p.dv += PARTICLE_MASS * a * a * a * a * KERNEL_NORM;

//                     if (p.size < Point::MAX_NEIGHBOURS){
//                         p.neighbours[p.size++] = pj - &(*particles)[0];
//                         p.distances[p.size - 1] = r;
//                     }
//                 }
//                 // if (idx == 0){
//                 //     std::cout << "neighbours: " << cnt << std::endl;
//                 //     std::cout << "candidates: " << candidates << std::endl;
//                 // }
//             } 
//         }

//         p.pressure = STIFFNESS * (p.density - REST_DENSITY * PARTICLE_MASS);
//         p.pv = STIFF_APPROX * p.dv;

//     });
// }

// void Solver::ProjectionStep(){
//     std::for_each(std::execution::par_unseq, particles->begin(), particles->end(), [&](Point& p){
//         p.predicted_position = p.position;
//         for (size_t i = 0; i < p.size; i++){
//             auto& neighbour = (*particles)[p.neighbours[i]];
//             float r = p.distances[i];
//             glm::vec2 dx = neighbour.position - p.position;

//             float a = 1. - r / smoothing_length;
//             float d = DT2 * ((p.pv * neighbour.pv) * a * a * a * KERNEL_NORM + (p.pressure + neighbour.pressure) * a * a * KERNEL_FACTOR) / 2.0f;

//             p.predicted_position -= d * dx / (r * PARTICLE_MASS);

//             // surface tension
//             p.predicted_position += SURFACE_TENSION * a * a * KERNEL_FACTOR * dx;

//             // viscosity
//             glm::vec2 dv = neighbour.velocity - p.velocity;
//             float u = glm::dot(dv, dx);
//             if (u > 0){
//                 u /= r;
//                 float I = 0.5f * DT * a * (LINEAR_VISC * u + QUAD_VISC * u * u);
//                 p.predicted_position -= I * dx * DT;
//             }
//         }
//     });
// }


// void Solver::CorrectionStep(){
//     std::for_each(std::execution::par_unseq, particles->begin(), particles->end(), [&](Point& p){
//         p.position = p.predicted_position;
//         p.velocity = (p.position - p.previous_position) / DT;
//     });
// }