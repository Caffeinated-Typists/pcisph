#pragma once


#include <vector>
#include <algorithm>
#include <memory>   // for std::unique_ptr
#include "point.hpp"


class Particles
{
private:
    std::vector<Point> *particles;
    // size_t num_particles; 
    unsigned int VBO;

    float* points;
public:
    Particles(std::vector<Point> *_particles, int max_particles);
    ~Particles();

    /**
     * @brief Draw all the particles
     * 
     * Add particles to a VAO and perform a draw call
     */
    void draw(unsigned int& VAO, Shader& shader);

    /**
     * @brief reserve space for the particles
     * 
     * @param n The number of particles to reserve space for
     */

    /**
     * @brief Add particles to the simulation
     */
    void addParticles(std::vector<Point> *points);
};


