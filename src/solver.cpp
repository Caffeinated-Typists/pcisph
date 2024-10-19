#include <solver.hpp>

Solver::Solver(std::shared_ptr<std::vector<Point>> _particles) : particles(_particles) {
    boundary = {
        glm::vec2(-1.0, 0.0),
        glm::vec2(0.0, -1.0),
        glm::vec2(1.0, 0.0),
        glm::vec2(0.0, 1.0)
    };
}

Solver::~Solver(){
}

void Solver::BoundaryCheck(){
    std::for_each(std::execution::par, particles->begin(), particles->end(), [&](Point& p){
        float d;
        for (auto& b : boundary){
            d = glm::dot(p.position, b) - 1;
            if ((d = std::max(0.0f, d)) < Point::radius){
                p.velocity += b * (Point::radius - d) / DT;
            }
        }
    });

}

void Solver::ExternalForces(){
    std::for_each(std::execution::par, particles->begin(), particles->end(), [&](Point& p){
        p.velocity += GRAVITY * DT;
    });
}

void Solver::Integrate(){
    std::for_each(std::execution::par ,particles->begin(), particles->end(), [&](Point& p){
        p.position += p.velocity * DT;
    });
}


void Solver::printPositions(){
    auto p = (*particles)[0];
    std::cout << p.position.x << " " << p.position.y << std::endl;
}
