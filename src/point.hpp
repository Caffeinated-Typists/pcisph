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
    constexpr static float radius = 0.03f;
    int grid_x;
    int grid_y;


public:
    Point(glm::vec2 _position) : position(_position), velocity(glm::vec2(0.0f)) {}
    ~Point() {}

};
