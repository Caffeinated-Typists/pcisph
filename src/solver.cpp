#include <solver.hpp>

Solver::Solver(std::shared_ptr<std::vector<Point>> _particles) 
    : particles(_particles), logger("log.txt", this) {
    boundary = {
        glm::vec2(-1.0, 0.0),
        glm::vec2(0.0, -1.0),
        glm::vec2(1.0, 0.0),
        glm::vec2(0.0, 1.0)
    };

    // calculate mass based on rest density
    float density_sum = 0.0f;

    auto half_range = (int)(smoothing_length / (Point::radius));

    for (int i = -half_range; i <= half_range; i++){
        for (int j = -half_range; j <= half_range; j++){
            auto r = glm::vec2(i * Point::radius, j * Point::radius);
            density_sum += kernel(r);
        }
    }
    
    Solver::PARTICLE_MASS = REST_DENSITY / density_sum;

    DELTA = ScalingFactor();

    // initialize density
    std::for_each(std::execution::par_unseq, particles->begin(), particles->end(), [&](Point& p){
        p.density = 1000.0f;
    });

    std::cout << "Particle mass: " << Solver::PARTICLE_MASS << std::endl;
    std::cout << "Delta: " << DELTA << std::endl;
}

Solver::~Solver(){
}


void Solver::Update(){
    UpdateNeighbourhood();
    ExternalForces();

    // fill corrected pressure and dv pressure to 0
    std::for_each(std::execution::par_unseq, particles->begin(), particles->end(), [&](Point& p){
        p.corrected_pressure = 0.0f;
        p.dv_pressure = glm::vec2(0.0f);
    });

    auto it = 0;
    max_density_error.store(0.0f);

    // std::cout << "Max density error before correction loop: " << max_density_error.load() << std::endl;

    while (it < MIN_STEPS || (max_density_error.load() > FLUCTUATION_THRESHOLD && it < MAX_STEPS)){
        PredictVelocityPosition();
        BoundaryCheckPredicted();
        updateCorrectDensityPressure();
        CalcDvPressure();
        it++;

        logger.logNaNdV();


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
        p.dv_without_pressure = glm::vec2(0.0f);

        for (size_t i = 0; i < p.size; i++){

            auto& neighbour = (*particles)[p.neighbours[i]];
            auto x_ab = p.position - neighbour.position;
            auto v_ab = p.velocity - neighbour.velocity;


            auto v_dot_x = glm::dot(v_ab, x_ab);
            if (v_dot_x < 0){
                auto mu = 2.0 * VISCOSITY_CONSTANT * smoothing_length * C_S / (p.density + neighbour.density);
                auto PI_ab = -mu * v_dot_x / (glm::dot(x_ab, x_ab) + 0.01 * smoothing_length2);
                p.dv_without_pressure += (float)(-Solver::PARTICLE_MASS * PI_ab) * grad_kernel(x_ab);
            }
        }
        p.dv_without_pressure += GRAVITY;
    });
}

void Solver::Integrate(){
    std::for_each(std::execution::par_unseq ,particles->begin(), particles->end(), [&](Point& p){
        p.velocity += (p.dv_pressure + p.dv_without_pressure) * DT;
        p.position += p.velocity * DT;
        p.density = 0.0f;

        for (size_t i = 0; i < p.size; i++){
            auto& neighbour = (*particles)[p.neighbours[i]];
            auto x_ab = p.position - neighbour.position;

            p.density += Solver::PARTICLE_MASS * kernel(x_ab);
        }

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
        p.predicted_velocity = p.velocity + (p.dv_pressure + p.dv_without_pressure) * DT;
        p.predicted_position = p.position + (p.predicted_velocity * DT);
    });
}

void Solver::updateCorrectDensityPressure(){
    std::for_each(std::execution::par_unseq, particles->begin(), particles->end(), [&](Point& p){
        p.predicted_density = p.density;

        for (size_t i = 0; i < p.size; i++){
            auto& neighbour = (*particles)[p.neighbours[i]];

            auto x_ab = p.predicted_position - neighbour.predicted_position;
            auto v_ab = p.predicted_velocity - neighbour.predicted_velocity;

            p.predicted_density += Solver::PARTICLE_MASS * glm::dot(v_ab, grad_kernel(x_ab)) * DT;
        }

        auto density_error = std::max(0.0f, p.predicted_density - REST_DENSITY);

        p.corrected_pressure += density_error * DELTA;
        auto err = abs(density_error);

        // update max density error atomically
        max_density_error.store(std::max(max_density_error.load(), err));

    });
}


void Solver::CalcDvPressure(){
    std::for_each(std::execution::par_unseq, particles->begin(), particles->end(), [&](Point& p){
        p.dv_pressure = glm::vec2(0.0f);

        for (size_t i = 0; i < p.size; i++){
            auto& neighbour = (*particles)[p.neighbours[i]];
            auto x_ab = p.predicted_position - neighbour.predicted_position;
            p.dv_pressure += (float)(-Solver::PARTICLE_MASS * ( (p.corrected_pressure           / (p.density * p.density))  +
                                                                (neighbour.corrected_pressure   / (neighbour.density * neighbour.density)))
                                                ) * grad_kernel(x_ab);
        }


    });
}



inline glm::vec2 Solver::grad_kernel(glm::vec2& r){
    float r_len = glm::length(r);

    if (r_len < EPS)
        return glm::vec2(0.0f);

    auto r_dir = r / r_len;    
    float q = r_len / smoothing_length;


    glm::vec2 rval = glm::vec2(0.0f);

    if (q < 1.0)
        rval = (float)((GRAD_KERNEL_FACTOR) * (-3.0 * q + 2.25 * q * q)) * r_dir;
    else if (q < 2.0)
        rval = (float)((GRAD_KERNEL_FACTOR) * -0.75 * ((2.0 - q) * (2.0 - q))) * r_dir;
    
    return rval;

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
    auto half_range = (int)(smoothing_length / (Point::radius));

    // filled neighbour with particle distance as radius
    for (int i = -half_range; i <= half_range; i++){
        for (int j = -half_range; j <= half_range; j++){
            auto r = glm::vec2(i * Point::radius, j * Point::radius);
            auto dist2 = glm::dot(r, r);
            if (dist2 < smoothing_length2 && dist2 > EPS2){
                auto grad = grad_kernel(r);
                grad_sum += grad;
                sum += glm::dot(grad, grad);
            }
        }
    }

    double ratio = (DT * Solver::PARTICLE_MASS / REST_DENSITY);
    double beta = 2 * ratio * ratio;

    return -1.0 / (beta * (glm::dot(grad_sum, grad_sum) - sum));

}