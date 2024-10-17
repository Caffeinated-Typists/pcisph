#include "point.hpp"

Point::Point(glm::vec2 _position)
{
    position = _position;
    velocity = glm::vec2(0.0f, 0.0f);
}


Point::~Point(){
}