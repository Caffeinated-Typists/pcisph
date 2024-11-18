#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <vector>
#include <list>
#include <algorithm>
#include <execution>    
#include <memory>
#include <point.hpp>
#include <particles.hpp>
#include <logger.hpp>

class Solver
{
// Misc
private:
    float VIEWPORT_WIDTH;
    float VIEWPORT_HEIGHT;

private:
    Particles* particles;
     std::vector<float> pos_last;
    std::vector<glm::vec3> boundary;    

    friend class Logger;

    Logger logger;
// Solver Parameters
private:
    const static int SOLVER_STEPS = 10;
    const static int MAX_STEPS = 100;
    const static int FPS = 60;
    constexpr static float BOUNDARY_THRESHOLD = 1e-3;
    constexpr static float FLUCTUATION_THRESHOLD = 1e-3;
    constexpr static float DT = 1.0 / (float)(FPS * SOLVER_STEPS);
    constexpr static float DT2 = DT * DT;
    constexpr static glm::vec2 GRAVITY = glm::vec2(0.0f, -9.81f);
    constexpr static float LINEAR_VISC = 0.5f;
    constexpr static float QUAD_VISC = 0.25f;
    constexpr static float SURFACE_TENSION = 1e-4;
    constexpr static float REST_DENSITY = 45.0f;
    constexpr static float STIFFNESS = 0.08f;
    constexpr static float STIFF_APPROX = 0.1f;
    
    float PARTICLE_MASS;

    constexpr static float smoothing_length = 6 * Point::radius;
    constexpr static float smoothing_length2 = smoothing_length * smoothing_length;

    // TODO: Not sure if to use smoothing length or half of it
    constexpr static float KERNEL_FACTOR = 20. / (2 * M_PI * smoothing_length2);
    constexpr static float KERNEL_NORM = 30. / (2 * M_PI * smoothing_length2);


    std::atomic<float> max_density_error = 0.0f;

// Grid Parameters
private:
    constexpr static float EPS = 1e-7;
    constexpr static float EPS2 = EPS * EPS;
    constexpr static float grid_dx = smoothing_length;
    size_t grid_width;
    size_t grid_height;
    size_t grid_size;
    std::vector<Point*> grid;


public:
    Solver() {}
    Solver (Particles *particles, float viewport_width, float viewport_height);
    ~Solver();

    /**
     * @brief Update the particles
     */
    void Update();

    /**
     * @brief Ensure that the particles do not go out of bounds
     */
    void BoundaryCheck(); 

    /**
     * @brief Apply external forces to the particles, mainly gravity here
     */
    void ExternalForces();

    /**
     * @brief Update the position of the particles using the velocity
     */
    void Integrate();

    // /**
    //  * @brief Update the grid after the particles have been updated, and update the neighbourhood
    //  */
    // void GridInsert();

    // /**
    //  * @brief Pressure solver
    //  */
    // void PressureSolve();

    // /**
    //  * @brief Projection Step
    //  */
    // void ProjectionStep();

    // /**
    //  * @brief Correction Step
    //  */
    // void CorrectionStep();
};