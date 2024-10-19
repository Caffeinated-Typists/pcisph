#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <vector>
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

// Grid Parameters
private:
    constexpr static float smoothing_length = 6 * Point::radius;
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
     * @brief Update the grid after the particles have been updated
     */
    void UpdateGrid();

};