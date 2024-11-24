#include <logger.hpp>
#include <solver.hpp>

Logger::Logger(const std::string& filename, Solver* _solver) :  solver(_solver), particles(_solver->particles) {
    // create file if it does not exist
    log_file.open(filename, std::ios::out | std::ios::trunc);

    // get current time
    start_time = std::chrono::system_clock::now();
}

Logger::~Logger(){
    log_file.close();
}

std::string Logger::enumToString(LogType type){
    switch (type){
        case LogType::INFO:
            return "INFO";
        case LogType::WARNING:
            return "WARNING";
        case LogType::ERROR:
            return "ERROR";
    }
    return "UNKNOWN";
}

void Logger::logPrint(const std::string& message, LogType type){
    auto end_time = std::chrono::system_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    log_file << elapsed_time << "ms: " << enumToString(type) << ": " << message << std::endl;
}

void Logger::logPositions(){
    std::stringstream ss;
    ss << "Positions: ";
    for (auto& p : *particles){
        ss << std::setw(8) << p.position.x << " " << std::setw(8) << p.position.y << ", ";
    }
    logPrint(ss.str(), LogType::INFO);
}

void Logger::logVelocities(){
    std::stringstream ss;
    ss << "Velocities: " << std::endl;
    for (auto& p : *particles){
        ss << std::setw(8) << p.velocity.x << " " << std::setw(8) << p.velocity.y << ", ";
    }
    logPrint(ss.str(), LogType::INFO);
}

void Logger::logDensity(){
    std::stringstream ss;
    ss << "Density: " << std::endl;
    for (auto& p : *particles){
        ss << std::setw(8) << p.density << ", ";
    }
    logPrint(ss.str(), LogType::INFO);
}

// void Logger::logFirstPosition(){
//     std::stringstream ss;
//     ss << "First Position: " << (*particles)[0].position.x << " " << (*particles)[0].position.y;
//     logPrint(ss.str(), LogType::INFO);
// }

// void Logger::logFirstVelocity(){
//     std::stringstream ss;
//     ss << "First Velocity: " << (*particles)[0].velocity.x << " " << (*particles)[0].velocity.y;
//     logPrint(ss.str(), LogType::INFO);
// }

// void Logger::logFirstNumberOfNeighbours(){
//     std::stringstream ss;
//     ss << "First Number of Neighbours: " << (*particles)[0].size;
//     logPrint(ss.str(), LogType::INFO);
// }

// void Logger::logGridLocations(){
//     std::stringstream ss;
//     ss << "Grid Locations: " << std::endl;
//     for (auto& p : *particles){
//         ss << "(" << p.grid_x << ", " << p.grid_y << "), ";
//     }
//     logPrint(ss.str(), LogType::INFO);
// }


// void Logger::logNaNParticles(){
//     std::stringstream ss;
//     ss << "NaNs: " << std::endl;
//     int count = 0;
//     for (auto& p : *particles){
//         if (std::isnan(p.velocity.x) || std::isnan(p.velocity.y) || std::isnan(p.position.x) || std::isnan(p.position.y)){
//             ss << getAllInfo(&p - &(*particles)[0]);
//             count++;
//         }
        
//     }
//     if (count > 0)
//         logPrint(ss.str(), LogType::ERROR);
// }

// void Logger::logNaNdV(){
//     std::stringstream ss;
//     ss << "NaNs in dV: " << std::endl;
//     int count = 0;
//     for (auto& p : *particles){
//         if (std::isnan(p.dv_pressure.x) || std::isnan(p.dv_pressure.y) || std::isnan(p.dv_without_pressure.x) || std::isnan(p.dv_without_pressure.y)){
//             ss << getDvInfo(&p - &(*particles)[0]);
//             count++;
//         }
        
//     }
//     if (count > 0)
//         logPrint(ss.str(), LogType::ERROR);
// }

void Logger::logNumNaNPositions(){
    std::stringstream ss;
    std::atomic<int> count = 0;
    std::for_each(std::execution::par_unseq, particles->begin(), particles->end(), [&](Point& p){
        if (std::isnan(p.velocity.x) || std::isnan(p.velocity.y) || std::isnan(p.position.x) || std::isnan(p.position.y)){
            count++;
        }
    });
    if (count > 0){
        ss << "Number of particles with NaN positions: " << count;
        logPrint(ss.str(), LogType::ERROR);
    }
}



// std::string Logger::getAllInfo(int idx){
//     std::stringstream ss;
//     Point p = (*particles)[idx];
//     ss << "Particle " << idx << ":\n";
//     ss << "Position: " << p.position.x << " " << p.position.y << "\n";
//     ss << "Velocity: " << p.velocity.x << " " << p.velocity.y << "\n";
//     ss << "Density: " << p.density << "\n";
//     ss << "Pressure: " << p.corrected_pressure << "\n";
//     ss << "DV Pressure: " << p.dv_pressure.x << " " << p.dv_pressure.y << "\n";
//     ss << "DV without Pressure: " << p.dv_without_pressure.x << " " << p.dv_without_pressure.y << "\n";
//     ss << "Number of neighbours: " << p.size << "\n";
//     ss << "\n";
//     return ss.str();
// }

// std::string Logger::getDvInfo(int idx){
//     std::stringstream ss;
//     Point p = (*particles)[idx];
//     ss << "Particle " << idx << ":\n";
//     ss << "DV Pressure: " << p.dv_pressure.x << " " << p.dv_pressure.y << "\n";
//     ss << "Corrected Pressure: " << p.corrected_pressure << "\n";
//     ss << "Density: " << p.density << "\n";
//     ss << "Number of neighbours: " << p.size << "\n";
//     ss << "Neighbours: \n";

//     // get density and corrected pressure of neighbours
//     for (size_t i = 0; i < p.size; i++){
//         auto& neighbour = (*particles)[p.neighbours[i]];
//         ss << "Neighbour " << p.neighbours[i] << ", ";
//         ss << "Density: " << neighbour.density << ", ";
//         ss << "Corrected Pressure: " << neighbour.corrected_pressure << ", ";
//         ss << "Distance: " << glm::length(p.position - neighbour.position) << ", ";
//         ss << "\n";
//     }
    

//     ss << "\n";
//     return ss.str();
// }