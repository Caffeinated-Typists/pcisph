#include <logger.hpp>
#include <solver.hpp>

Logger::Logger(const std::string& filename, Particles* _particles) :  particles(_particles){
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

    auto& positions = particles->positions;

    ss << "Positions: ";
    for (size_t i = 0; i < positions.size(); i += 2){
        ss << std::setw(8) << positions[i] << " " << std::setw(8) << positions[i + 1] << ", ";
    }
    logPrint(ss.str(), LogType::INFO);
}



void Logger::checkNegativePositions(){
    std::stringstream ss;
    auto& positions = particles->positions;

    int count = 0;
    for (auto& i : particles->indices){
        if (positions[i * 2] < 0 || positions[i * 2 + 1] < 0){
            ss << "Particle " << i << " has negative position: " << positions[i * 2] << " " << positions[i * 2 + 1] << std::endl;
            count++;
        }
    }
    if (count > 0)
        logPrint(ss.str(), LogType::ERROR);

}

void Logger::logIndexes(){
    // Get data from spatial index SSBO
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, particles->spatialIndexSSBO);
    int* spatial_indices_data = (int*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    if (!spatial_indices_data){
        std::cerr << "Error mapping spatial indices SSBO" << std::endl;
        return;
    }

    std::stringstream ss;
    ss << "Spatial Indices: ";
    for (size_t i = 0; i < particles->num_particles; i++){
        ss << spatial_indices_data[i* 3 + 2] << ", ";
    }

    logPrint(ss.str(), LogType::INFO);
}
