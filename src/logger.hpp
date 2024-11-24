#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <memory>
#include <execution>
#include <algorithm>
#include <iomanip>
#include <point.hpp>
#include <particles.hpp>

class Solver;

enum LogType
{
    INFO, 
    WARNING,
    ERROR
};

class Logger
{
private:
    // log file
    std::ofstream log_file;
    // time of creation of object
    std::chrono::time_point<std::chrono::system_clock> start_time;

    Particles* particles;

private:
    std::string enumToString(LogType type);
    // std::string getAllInfo(int idx);
    // std::string getDvInfo(int idx);

public:
    Logger() {};
    Logger(const std::string& filename, Particles *_particles);
    ~Logger();

    // formatted logger
    void logPrint(const std::string& message, LogType type);

    void logPositions();
    // void logVelocities();
    
    /**
     * @brief Output the positions of particles whose either component is negative
     */
    void checkNegativePositions();
    
    /**
     * @brief Log the spatial indices of all particles
     */
    void logIndexes();

};

