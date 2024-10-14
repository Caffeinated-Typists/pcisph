#pragma once


#include <vector>
#include <algorithm>
#include "point.hpp"

class Particles
{
private:
    std::vector<Point> particles;
    size_t num_particles; 
    unsigned int VBO;
public:
    Particles(/* args */);
    Particles(std::vector<Point>& _particles);
    ~Particles();

    /**
     * @brief Draw all the particles
     * 
     * Add particles to a VAO and perform a draw call
     */
    void draw(unsigned int& VAO, Shader& shader);

    /**
     * @brief Add a particle to the list of particles
     * 
     * @param particle The particle to add
     */
    void addParticle(Point& particle);

    /**
     * @brief reserve space for the particles
     * 
     * @param n The number of particles to reserve space for
     */
    void reserve(size_t n){
        particles.reserve(n);
    }
};


