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
private:
    std::shared_ptr<std::vector<Point>> particles;
    std::vector<float> pos_last;
    std::vector<glm::vec2> boundary;    

    friend class Logger;

    Logger logger;
// Solver Parameters
private:
    const static int MIN_STEPS = 5;
    const static int MAX_STEPS = 100;
    const static int FPS = 60;
    constexpr static float BOUNDARY_THRESHOLD = 1e-3;
    constexpr static float FLUCTUATION_THRESHOLD = 1e-3;
    // constexpr static float DT = 1.0 / (float)(FPS * 20);
    constexpr static float DT = 1e-3;
    constexpr static glm::vec2 GRAVITY = glm::vec2(0.0f, -9.81f);
    constexpr static float VISCOSITY_CONSTANT = 0.5f;
    constexpr static float C_S = 100.0f; // Speed of sound in liquid
    constexpr static float REST_DENSITY = 1000.0f;
    
    float PARTICLE_MASS;

    constexpr static float smoothing_length = 6 * Point::radius;
    constexpr static float smoothing_length2 = smoothing_length * smoothing_length;

    // TODO: Not sure if to use smoothing length or half of it
    constexpr static float KERNEL_FACTOR = 10.0 / (7.0 * M_PI * smoothing_length2);
    constexpr static float GRAD_KERNEL_FACTOR = 1.0 * KERNEL_FACTOR / smoothing_length;

    float DELTA = ScalingFactor();

    std::atomic<float> max_density_error = 0.0f;

// Grid Parameters
private:
    constexpr static float EPS = 1e-4;
    constexpr static float EPS2 = EPS * EPS;
    constexpr static unsigned int grid_width = 2 / smoothing_length;
    constexpr static unsigned int grid_height = 2 / smoothing_length;
    constexpr static unsigned int grid_size = grid_width * grid_height;


public:
    Solver() {}
    Solver (std::shared_ptr<std::vector<Point>> _particles);
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
     * @brief ensure that predicted positions are within the boundaries
     */
    void BoundaryCheckPredicted();

    /**
     * @brief Apply external forces to the particles, mainly gravity here
     */
    void ExternalForces();

    /**
     * @brief Update the position of the particles using the velocity
     */
    void Integrate();

    /**
     * @brief Print the positions of the particles
     */
    void printPositions();

    /**
     * @brief print the velocities of the particles
     */
    void printVelocities();

    /**
     * @brief Update the grid after the particles have been updated, and update the neighbourhood
     */
    void UpdateNeighbourhood();

    /**
     * @brief Calculate the density and pressure of the particles
     */
    void DensityPressure();

    /**
     * @brief predict position and velocity of the particles
     */
    void PredictVelocityPosition();

    /**
     * @brief Predict the density of the particles, and calculate the pressure
     */
    void updateCorrectDensityPressure();

    /**
     * @brief Calculate the velocity change due to pressure
     * 
     */
    void CalcDvPressure();

    /**
     * 
     * @brief Kernel function for the SPH
     * 
     * @details This is the cubic spline kernel function whose equation is given by:
     * 
     * W(r, h)  = sigma * (1 - 3/2 * q^2 + 3/4 * q^3) for 0 <= q < 1
     * 
     *          = sigma * 1/4 * (2 - q)^3 for 1 <= q < 2
     * 
     *          = 0 for q >= 2
     * 
     * Link for better equation: https://imgur.com/a/nFJuLAp
     * 
     * Where q = r / h and sigma = 10 / (7 * pi * h^2) for 2D 
     * 
     * @param r The difference vector
     * 
     * @return The kernel value
     */
    inline float kernel(glm::vec2 r);

    /**
     * @brief Calculate the gradient of the kernel function
     * 
     * @param r The difference vector
     * 
     * @return The gradient of the kernel function
     */
    inline glm::vec2 grad_kernel(glm::vec2& r);


    /**
     * @brief Calculate value of delta as described Equation 8 of PCISPH paper
     */
    float ScalingFactor();

};