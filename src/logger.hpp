#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <memory>
#include <iomanip>
#include <point.hpp>

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

    Solver* solver;
    std::shared_ptr<std::vector<Point>> particles;

private:
    std::string enumToString(LogType type);
    std::string getAllInfo(int idx);
    std::string getDvInfo(int idx);

public:
    Logger() {};
    Logger(const std::string& filename, Solver* _solver);
    ~Logger();

    // formatted logger
    void logPrint(const std::string& message, LogType type);

    void logPositions();
    void logVelocities();

    /**
     * @brief Print information about the particles whose velocity or position is NaN
     */
    void logNaNParticles();
    void logNaNdV();
};

