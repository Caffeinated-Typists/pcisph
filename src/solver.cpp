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


void Solver::Update(){
    ExternalForces();
    UpdateNeighbourhood();

    // fill corrected pressure and dv pressure to 0
    std::for_each(std::execution::par_unseq, particles->begin(), particles->end(), [&](Point& p){
        p.corrected_pressure = 0.0f;
        p.dv_pressure = glm::vec2(0.0f);
    });

    auto it = 0;

    max_density_error.store(0.0f);

    while (it < MIN_STEPS || (max_density_error.load() > EPS && it < MAX_STEPS)){
        PredictVelocityPosition();
        BoundaryCheckPredicted();
        updateCorrectDensityPressure();
        it++;

        std::cout << "Max density error: " << max_density_error.load() << std::endl;

        if (it == MAX_STEPS){
            std::cout << "Max steps reached" << std::endl;
            break;
        }

    }
    Integrate();
    BoundaryCheck();
}

void Solver::BoundaryCheck(){
    std::for_each(std::execution::par_unseq, particles->begin(), particles->end(), [&](Point& p){
        if (p.position.x < -1.0 + BOUNDARY_THRESHOLD || p.position.x > 1.0 - BOUNDARY_THRESHOLD) {
            p.position.x = glm::clamp(p.position.x, -1.0f + BOUNDARY_THRESHOLD, 1.0f - BOUNDARY_THRESHOLD);
            p.velocity.x = -p.velocity.x;
        }
        if (p.position.y < -1.0 + BOUNDARY_THRESHOLD || p.position.y > 1.0 - BOUNDARY_THRESHOLD) {
            p.position.y = glm::clamp(p.position.y, -1.0f + BOUNDARY_THRESHOLD, 1.0f - BOUNDARY_THRESHOLD);
            p.velocity.y = -p.velocity.y;
        }
        
    });
}




void Solver::BoundaryCheckPredicted(){
    std::for_each(std::execution::par_unseq, particles->begin(), particles->end(), [&](Point& p){
        if (p.predicted_position.x < -1.0 + BOUNDARY_THRESHOLD || p.predicted_position.x > 1.0 - BOUNDARY_THRESHOLD) {
            p.predicted_position.x = glm::clamp(p.predicted_position.x, -1.0f + BOUNDARY_THRESHOLD, 1.0f - BOUNDARY_THRESHOLD);
            p.predicted_velocity.x = -p.predicted_velocity.x * ELASTICITY;
        }
        if (p.predicted_position.y < -1.0 + BOUNDARY_THRESHOLD || p.predicted_position.y > 1.0 - BOUNDARY_THRESHOLD) {
            p.predicted_position.y = glm::clamp(p.predicted_position.y, -1.0f + BOUNDARY_THRESHOLD, 1.0f - BOUNDARY_THRESHOLD);
            p.predicted_velocity.y = -p.predicted_velocity.y * ELASTICITY;
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
                        p.size++;
                    }
                }
            }
        }

    });

}

void Solver::PredictVelocityPosition(){
    std::for_each(std::execution::par_unseq, particles->begin(), particles->end(), [&](Point& p){
        p.predicted_velocity += p.velocity + (p.dv_pressure + p.dv_without_pressure) * DT;
        p.predicted_position += p.position + (p.predicted_velocity * DT);
    });
}

void Solver::updateCorrectDensityPressure(){
    std::for_each(std::execution::par_unseq, particles->begin(), particles->end(), [&](Point& p){
        p.predicted_density = p.density;

        for (size_t i = 0; i < p.size; i++){
            auto& neighbour = (*particles)[p.neighbours[i]];

            auto x_ab = p.predicted_position - neighbour.predicted_position;
            auto v_ab = p.predicted_velocity - neighbour.predicted_velocity;

            p.predicted_density += neighbour.mass * glm::dot(v_ab, grad_kernel(x_ab));
        }

        auto density_error = std::max(0.0f, p.predicted_density - p.REST_DENSITY);

        p.corrected_pressure += density_error * DELTA;
        auto err = abs(density_error);

        // update max density error atomically
        max_density_error.store(std::max(max_density_error.load(), err));

    });
}


void Solver::CalVelocityDueToPressure(){
    std::for_each(std::execution::par_unseq, particles->begin(), particles->end(), [&](Point& p){
        p.dv_pressure = glm::vec2(0.0f);

        for (size_t i = 0; i < p.size; i++){
            auto& neighbour = (*particles)[p.neighbours[i]];
            auto x_ab = p.predicted_position - neighbour.predicted_position;
            p.dv_pressure += (float)(-neighbour.mass * (p.corrected_pressure + neighbour.corrected_pressure) / (2.0 * neighbour.density)) * grad_kernel(x_ab);
        }

    });
}



inline glm::vec2 Solver::grad_kernel(glm::vec2& r){
    float r_len = glm::length(r);
    auto r_dir = r / r_len;    

    float q = r_len / smoothing_length;

    if (q < 1.0)
        return (float)((GRAD_KERNEL_FACTOR) * (-3.0 * q + 2.25 * q * q)) * r_dir;
    else if (q < 2.0)
        return (float)((GRAD_KERNEL_FACTOR) * -0.75 * ((2.0 - q) * (2.0 - q))) * r_dir;
    
    return glm::vec2(0.0f); 

}

inline float Solver::kernel(glm::vec2 r){
    float q = glm::length(r) / smoothing_length;


    if (q < 1.0)
        return (float)(KERNEL_FACTOR * (1.0 - 1.5 * q * q + 0.75 * q * q * q));
    else if (q < 2.0)
        return (float)(KERNEL_FACTOR * 0.25 * (2.0 - q) * (2.0 - q) * (2.0 - q));
    
    return 0.0f;
}

float Solver::ScalingFactor(){
    glm::vec2 grad_sum = glm::vec2(0.0f);
    float sum = 0.0f;
    auto half_range = (int)(smoothing_length / (Point::radius * 2.0));

    // filled neighbour with particle distance as radius
    for (int i = -half_range; i <= half_range; i++){
        for (int j = -half_range; j <= half_range; j++){
            auto r = glm::vec2(i * Point::radius, j * Point::radius);
            if (glm::dot(r, r) < smoothing_length2 && glm::dot(r, r) > EPS2){
                auto grad = grad_kernel(r);
                grad_sum += grad;
                sum += glm::dot(grad, grad);
            }
        }
    }

    auto beta = 2 * glm::pow((DT * Point::mass / Point::REST_DENSITY), 2);
    return -1.0 / (beta * glm::dot(grad_sum, grad_sum) - sum);

}