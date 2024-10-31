#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <vector>
#include <list>
#include <algorithm>
#include <execution>    
#include <point.hpp>
#include <particles.hpp>

class Solver
{
private:
    std::shared_ptr<std::vector<Point>> particles;
    std::vector<float> pos_last;
    std::vector<glm::vec2> boundary;    

// Solver Parameters
public:
    const static int STEPS = 10;
    const static int FPS = 60;
    constexpr static float DT = 1.0 / (float)(FPS * STEPS);
    constexpr static glm::vec2 GRAVITY = glm::vec2(0.0f, -9.81f);
    constexpr static float ELASTICITY = 0.5f;

// Grid Parameters
private:
    constexpr static float EPS = 0.0000001f;
    constexpr static float EPS2 = EPS * EPS;
    constexpr static float smoothing_length = 6 * Point::radius;
    constexpr static float smoothing_length2 = smoothing_length * smoothing_length;
    constexpr static unsigned int grid_width = 2 / smoothing_length;
    constexpr static unsigned int grid_height = 2 / smoothing_length;
    constexpr static unsigned int grid_size = grid_width * grid_height;


public:
    Solver() {}
    Solver (std::shared_ptr<std::vector<Point>> _particles);
    ~Solver();


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

    /**
     * @brief Print the positions of the particles
     */
    void printPositions();

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
    inline glm::vec3 grad_kernel(glm::vec2 r);

};