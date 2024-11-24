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

    std::cout << "Grid Width: " << grid_width << std::endl;
    std::cout << "Grid Height: " << grid_height << std::endl;
    std::cout << "Grid size: " << grid_size << std::endl;


    gridIndices.resize(particles->size());
    gridOffsets.resize(grid_size);


    GridInsert();
}

Solver::~Solver(){
}


void Solver::Update(){
    for (int i = 0; i < SOLVER_STEPS; i++){
        ExternalForces();
        Integrate();
        GridInsert();
        PressureSolve();
        ProjectionStep();
        CorrectionStep();
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



void Solver::ExternalForces(){
    std::for_each(std::execution::par_unseq, particles->begin(), particles->end(), [&](Point& p){
        p.velocity += GRAVITY * DT;
    });
}

void Solver::Integrate(){
    std::for_each(std::execution::par_unseq ,particles->begin(), particles->end(), [&](Point& p){
        p.previous_position = p.position;
        p.position += p.velocity * DT;
    });
}

glm::ivec2 Solver::GetCell(Point& p){
    p.grid_x = std::clamp((int)(p.position.x / smoothing_length), 1, (int)grid_width - 2);
    p.grid_y = std::clamp((int)(p.position.y / smoothing_length), 1, (int)grid_height - 2);
    return glm::ivec2(p.grid_x, p.grid_y);
}

void Solver::GridInsert(){
    std::for_each(std::execution::par_unseq, particles->begin(), particles->end(), [&](Point& p){
        auto idx = &p - &(*particles)[0];
        int grid_x = std::clamp((int)(p.position.x / smoothing_length), 1, (int)grid_width - 2);
        int grid_y = std::clamp((int)(p.position.y / smoothing_length), 1, (int)grid_height - 2);
        int hash = grid_x + grid_y * grid_width;
        gridIndices[idx] = glm::ivec4(idx, hash, grid_x, grid_y);
    });

    std::sort(gridIndices.begin(), gridIndices.end(), [](const glm::vec4& a, const glm::vec4& b){
        return a.y < b.y;
    });


    std::fill(gridOffsets.begin(), gridOffsets.end(), -1);

    std::for_each(std::execution::par_unseq, particles->begin(), particles->end(), [&](Point& p){
        auto idx = &p - &(*particles)[0];
        int hash = gridIndices[idx].y;
        int prevHash = idx == 0 ? -1 : gridIndices[idx - 1].y;
        if (hash != prevHash){
            gridOffsets[hash] = idx;
        }
    });

}

void Solver::PressureSolve(){
    std::for_each(std::execution::par_unseq, particles->begin(), particles->end(), [&](Point& p){
        p.size = 0;
        p.density = 0.0f;
        p.dv = 0.0f;

        glm::ivec2 grid_index = GetCell(p);

        int cnt = 0;

        for (int i = -1; i <= 1; i++){
            for (int j = -1; j <= 1; j++){
                glm::ivec2 grid_pos = glm::ivec2(grid_index.x + i, grid_index.y + j);
                int hash = grid_pos.x + grid_pos.y * grid_width;
                if (hash < 0 || hash >= grid_size) continue;                

                int currIdx = gridOffsets[hash];

                while (currIdx < particles->size()){

                    // if hash of particle is not the same as the current hash, then the particle is not a neighbour
                    if (gridIndices[currIdx].y != hash) break;
                    if (cnt >= Point::MAX_NEIGHBOURS) break;

                    const Point& neighbour = (*particles)[gridIndices[currIdx].x];
                    currIdx++;

                    float r2 = glm::dot(p.position - neighbour.position, p.position - neighbour.position);
                    

                    if (r2 > smoothing_length2 || r2 < EPS2) continue;

                    cnt++;

                    // candidates++;
                    float r = std::sqrt(r2);
                    float a = 1. - r / smoothing_length;

                    p.density += PARTICLE_MASS * a * a * a * KERNEL_FACTOR;
                    p.dv += PARTICLE_MASS * a * a * a * a * KERNEL_NORM;
                    
                }
            } 
        }

        p.pressure = STIFFNESS * (p.density - REST_DENSITY * PARTICLE_MASS);
        p.pv = STIFF_APPROX * p.dv;

    });
}

void Solver::ProjectionStep(){
    std::for_each(std::execution::par_unseq, particles->begin(), particles->end(), [&](Point& p){
        p.predicted_position = p.position;
        glm::ivec2 grid_index = GetCell(p);


        int cnt = 0;
        for (int i = -1; i <= 1; i++){
            for (int j = -1; j <= 1; j++){
                glm::vec2 grid_pos = glm::vec2(grid_index.x + i, grid_index.y + j);
                int hash = grid_pos.x + grid_pos.y * grid_width;
                if (hash < 0 || hash > grid_size) continue;

                int currIdx = gridOffsets[hash];
                while (currIdx < particles->size()){
                    if (gridIndices[currIdx].y != hash) break;

                    if (cnt >= Point::MAX_NEIGHBOURS) break;

                    const Point& neighbour = (*particles)[gridIndices[currIdx].x];
                    currIdx++;

                    float r2 = glm::dot(p.position - neighbour.position, p.position - neighbour.position);
                    if (r2 > smoothing_length2 || r2 < EPS2) continue;

                    cnt++;


                    float r = std::sqrt(r2);
                    glm::vec2 dx = neighbour.position - p.position;

                    float a = 1. - r / smoothing_length;
                    float d = DT2 * ((p.pv * neighbour.pv) * a * a * a * KERNEL_NORM + (p.pressure + neighbour.pressure) * a * a * KERNEL_FACTOR) / 2.0f;

                    p.predicted_position -= d * dx / (r * PARTICLE_MASS);

                    // surface tension
                    p.predicted_position += SURFACE_TENSION * a * a * KERNEL_FACTOR * dx;

                    // viscosity
                    glm::vec2 dv = neighbour.velocity - p.velocity;
                    float u = glm::dot(dv, dx);
                    if (u > 0){
                        u /= r;
                        float I = 0.5f * DT * a * (LINEAR_VISC * u + QUAD_VISC * u * u);
                        p.predicted_position -= I * dx * DT;
                    }
                }
            }
        }

    });
}


void Solver::CorrectionStep(){
    std::for_each(std::execution::par_unseq, particles->begin(), particles->end(), [&](Point& p){
        p.position = p.predicted_position;
        p.velocity = (p.position - p.previous_position) / DT;
    });
}