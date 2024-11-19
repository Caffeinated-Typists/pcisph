#pragma once


#include <vector>
#include <algorithm>
#include <execution>
#include <memory>   // for std::unique_ptr
#include "point.hpp"

/**
 * @class Particles
 * @brief A class to manage all the particles
 * 
 * This class will contain all the particles and will be responsible for drawing them
 */
class Particles
{
private:
    // vec2
    std::vector<float> positions;
    std::vector<float> velocities;

    // vec2
    std::vector<float> previous_positions;
    std::vector<float> predicted_positions;
    std::vector<float> predicted_velocities;

    // scalar/float
    std::vector<float> densities;
    std::vector<float> dvs;
    std::vector<float> pressures;
    std::vector<float> pvs;

    // vec2 grid position
    std::vector<size_t> grid_pos;

    // neighbour information
    std::vector<std::vector<unsigned short>> neighbours;
    std::vector<std::vector<float>> distances;
    std::vector<size_t> sizes;

    // index array
    std::vector<size_t> indices;

    // constants
    constexpr static size_t MAX_NEIGHBOURS = 64;


    std::shared_ptr<std::vector<Point>> particles;
    size_t num_particles; 
    unsigned int VBO;


    friend class Solver;
    friend class Logger;

private:
    unsigned int positionSSBO;
    unsigned int velocitySSBO;
    unsigned int VAO;

    /**
     * @brief Create the SSBO for the particles
     */
    void setupSSBO();

public:
    constexpr static float radius = 0.03f;

public:
    Particles(std::vector<float> _positions);
    ~Particles();

    /**
     * @brief Draw all the particles
     * 
     * Add particles to a VAO and perform a draw call
     */
    void draw(Shader& shader);

    /**
     * @brief reserve space for the particles
     * 
     * @param n The number of particles to reserve space for
     */
};


