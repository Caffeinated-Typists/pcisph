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
    std::vector<float> previous_positions;
    std::vector<float> predicted_positions;

    // scalar/float
    std::vector<float> densities;
    std::vector<float> dvs;
    std::vector<float> pressures;
    std::vector<float> pvs;

    // index array
    std::vector<size_t> indices;
    std::vector<size_t> num_neighbours;
    size_t num_particles; 

    // spatial hashing
    std::vector<int> spatialIndices;
    std::vector<int> spatialOffsets;


    // constants
    constexpr static size_t MAX_NEIGHBOURS = 64;



    friend class Solver;
    friend class Logger;

private:
    unsigned int positionSSBO;
    unsigned int velocitySSBO;
    unsigned int previousPositionSSBO;
    unsigned int predictedPositionSSBO;

    unsigned int densitySSBO;
    unsigned int dvSSBO;
    unsigned int pressureSSBO;
    unsigned int pvSSBO;

    unsigned int spatialIndexSSBO; // stores (original index, hash, and key) Stored as (original index, hash, key, 0)
    unsigned int spatialOffsetSSBO;

    unsigned int numNeighboursSSBO;


    unsigned int VAO;

    /**
     * @brief Create the SSBO for the particles
     */
    void setupSSBO();


    /**
     * @brief get back the SSBO data
     */
    void getSSBOData();

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


