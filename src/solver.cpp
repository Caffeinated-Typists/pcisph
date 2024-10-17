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
    float d;
    for (auto& p : *particles){
        for (auto& b : boundary){
            d = glm::dot(p.position, b) - 1;
            if ((d = std::max(0.0f, d)) < Point::radius){
                p.velocity += b * (Point::radius - d) / DT;
            }
        }
    }
}

void Solver::ExternalForces(){
    for (auto& p : *particles){
        p.velocity += GRAVITY * DT;
    }
}

void Solver::Integrate(){
    for (auto& p : *particles){
        p.position += p.velocity * DT;
    }
}


void Solver::printPositions(){
    auto p = (*particles)[0];
    std::cout << p.position.x << " " << p.position.y << std::endl;
}
