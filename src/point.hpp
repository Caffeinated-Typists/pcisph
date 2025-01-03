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
class Point
{   
public:
    glm::vec2 position;
    glm::vec2 velocity;

    glm::vec2 previous_position;
    glm::vec2 predicted_position;
    glm::vec2 predicted_velocity;

    float density;
    float dv;
    float pressure;
    float pv;

    // static float mass;
    Point* next;


    constexpr static float radius = 0.03f;
    int grid_x;
    int grid_y;
    
    const static unsigned int MAX_NEIGHBOURS = 64;
    std::vector<unsigned short> neighbours; // neighbour indices
    std::vector<float> distances; // distances to neighbours
    size_t size; // current size of the neighbourhood

public:
    Point(glm::vec2 _position) 
        :   position(_position), 
            velocity(glm::vec2(0.0f)) {}
    ~Point() {}

};