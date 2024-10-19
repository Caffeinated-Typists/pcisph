#pragma once 

#include <point.hpp>
#include <vector>



/**
 * @class Grid
 * @brief A class to manage the grid of the simulation
 * 
 */
class Grid
{
private:
    constexpr static float smoothing_length = 6 * Point::radius;
    constexpr static unsigned int grid_width = 2 / smoothing_length;
    constexpr static unsigned int grid_height = 2 / smoothing_length;
    constexpr static unsigned int grid_size = grid_width * grid_height;
    const static unsigned int max_neighbours = 64;

    float grid[grid_width][grid_height][max_neighbours];

public:
    Grid(/* args */);
    ~Grid();
};

Grid::Grid(/* args */)
{
}

Grid::~Grid()
{
}
