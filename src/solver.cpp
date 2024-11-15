#include <solver.hpp>

Solver::Solver(std::shared_ptr<std::vector<Point>> _particles, float viewport_width, float viewport_height)
    : particles(_particles), logger("log.txt", this){
    
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
}

Solver::~Solver(){
}


void Solver::Update(){
    // GridInsert();
    for (int i = 0; i < SOLVER_STEPS; i++){
        ExternalForces();
        Integrate();
        BoundaryCheck();
    }
}

void Solver::BoundaryCheck(){
    std::for_each(std::execution::par_unseq, particles->begin(), particles->end(), [&](Point& p){
        for (auto& b : boundary){
            glm::vec2 n = glm::vec2(b.x, b.y);
            float d = glm::dot(p.position, n) - b.z;
            if ((d = std::max(0.0f, d)) < Point::radius){
                p.velocity += (Point::radius - d) * n / DT;
            }
        }
    });
}


void Solver::BoundaryCheckPredicted(){
    std::for_each(std::execution::par_unseq, particles->begin(), particles->end(), [&](Point& p){
        if (p.predicted_position.x < -1.0 + BOUNDARY_THRESHOLD || p.predicted_position.x > 1.0 - BOUNDARY_THRESHOLD) {
            p.predicted_position.x = glm::clamp(p.predicted_position.x, -1.0f + BOUNDARY_THRESHOLD, 1.0f - BOUNDARY_THRESHOLD);
            p.predicted_velocity.x = -p.predicted_velocity.x;
        }
        if (p.predicted_position.y < -1.0 + BOUNDARY_THRESHOLD || p.predicted_position.y > 1.0 - BOUNDARY_THRESHOLD) {
            p.predicted_position.y = glm::clamp(p.predicted_position.y, -1.0f + BOUNDARY_THRESHOLD, 1.0f - BOUNDARY_THRESHOLD);
            p.predicted_velocity.y = -p.predicted_velocity.y;
        }

    });
}

void Solver::ExternalForces(){
    std::for_each(std::execution::par_unseq, particles->begin(), particles->end(), [&](Point& p){
        p.velocity += GRAVITY * DT;
    });
}

void Solver::Integrate(){
    std::for_each(std::execution::par_unseq ,particles->begin(), particles->end(), [&](Point& p){
        p.position += p.velocity * DT;
    });
}

void Solver::printPositions(){
    auto p = (*particles)[0];
    std::cout << p.position.x << " " << p.position.y << std::endl;
}

void Solver::printVelocities(){
    auto p = (*particles)[0];
    std::cout << p.velocity.x << " " << p.velocity.y << std::endl;
}

void Solver::GridInsert(){
    // clear the grid
    grid = std::vector<Point*>(grid_size, nullptr);


    std::for_each(std::execution::seq, particles->begin(), particles->end(), [&](Point& p){
        size_t idx = (*particles).data() - &p;
        p.grid_x = (p.position.x) / smoothing_length;
        p.grid_y = (p.position.y) / smoothing_length;
    });

}

