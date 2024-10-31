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
    std::for_each(std::execution::par_unseq, particles->begin(), particles->end(), [&](Point& p){
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


void Solver::UpdateNeighbourhood(){

    std::vector<std::vector<unsigned short>> grid(grid_size);

    for (auto& elem : grid){
        elem.reserve(100);
    }

    std::for_each(std::execution::seq, particles->begin(), particles->end(), [&](Point& p){
        unsigned int x = (p.position.x + 1) / smoothing_length;
        unsigned int y = (p.position.y + 1) / smoothing_length;
        x = std::max((unsigned int)0, std::min(grid_width - 1, x));
        y = std::max((unsigned int)0, std::min(grid_height - 1, y));

        p.grid_x = x;
        p.grid_y = y;

        grid[x + y * grid_width].push_back(&p - &(*particles)[0]);
    });

    std::for_each(std::execution::par_unseq, particles->begin(), particles->end(), [&](Point& p){
        p.neighbours.clear();
        p.r.clear();
        p.size = 0;


        for (int i = -1; i <= 1; i++){
            for (int j = -1; j <= 1; j++){
                if (p.grid_x + i < 0 || p.grid_x + i >= grid_width || p.grid_y + j < 0 || p.grid_y + j >= grid_height){
                    continue;
                }
                for (auto& idx : grid[p.grid_x + i + (p.grid_y + j) * grid_width]){
                    if (p.size >= Point::MAX_NEIGHBOURS){
                        break;
                    }

                    auto& neighbour = (*particles)[idx];
                    float dist = glm::dot(neighbour.position - p.position, neighbour.position - p.position);
                    if (dist > EPS2 && dist < smoothing_length2){
                        p.neighbours.push_back(idx);
                        p.r.push_back(glm::sqrt(dist));
                        p.size++;
                    }
                }
            }
        }

    });

}

void Solver::PredictVelocityPosition(){
    std::for_each(std::execution::par_unseq, particles->begin(), particles->end(), [&](Point& p){
        p.predicted_velocity = p.velocity + DT * GRAVITY;
    });
}