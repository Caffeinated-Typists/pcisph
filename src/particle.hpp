#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "shader.hpp"

/**
 * @class Particle
 * @brief A class to manage a single particle
 * 
 * It will contain all the information about a single particle, like position, velocity, mass, etc.
 */
class Particle
{
private:
    /* data */
    glm::vec3 position;
    glm::vec3 velocity;
    
    unsigned int num_points;
    unsigned int num_indexes;

    float* points;
    float* normals;
    short* indexes;

    unsigned int VBO;
    unsigned int IBO;
    
public:
    Particle(glm::vec3 _position);
    ~Particle();

    /**
     * @brief Set the elements of the particles in VBO and IBO
     * 
     * @param VAO The Vertex Array Object to bind the VBO and IBO to
     * @param shader The shader object to use for rendering
     */
    void render(unsigned int&, Shader&);


    /**
     * @brief Draw the particle
     * 
     * @param VAO The Vertex Array Object to bind the VBO and IBO to
     */
    void draw(unsigned int& );
};
