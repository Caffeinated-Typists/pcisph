#include <particles.hpp>


Particles::Particles(std::vector<float> _positions) : positions(_positions){
    num_particles = _positions.size() / 2;

    // initialize all the other vectors
    velocities.resize(num_particles * 2);
    previous_positions.resize(num_particles * 2);
    predicted_positions.resize(num_particles * 2);
    predicted_velocities.resize(num_particles * 2);

    densities.resize(num_particles);
    dvs.resize(num_particles);
    pressures.resize(num_particles);
    pvs.resize(num_particles);

    // create the index array
    indices.resize(num_particles);
    std::iota(indices.begin(), indices.end(), 0);

    setupSSBO();
}

void Particles::setupSSBO(){
    // position
    glGenBuffers(1, &positionSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, positionSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, positions.size() * sizeof(float), positions.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, positionSSBO);

    // velocity
    glGenBuffers(1, &velocitySSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocitySSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, velocities.size() * sizeof(float), velocities.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velocitySSBO);

    // previous position
    glGenBuffers(1, &previousPositionSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, previousPositionSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, previous_positions.size() * sizeof(float), previous_positions.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, previousPositionSSBO);

    // predicted position
    glGenBuffers(1, &predictedPositionSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, predictedPositionSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, predicted_positions.size() * sizeof(float), predicted_positions.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, predictedPositionSSBO);

    // predicted velocity
    glGenBuffers(1, &predictedVelocitySSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, predictedVelocitySSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, predicted_velocities.size() * sizeof(float), predicted_velocities.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, predictedVelocitySSBO);

    // density
    glGenBuffers(1, &densitySSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, densitySSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, densities.size() * sizeof(float), densities.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, densitySSBO);

    // dv
    glGenBuffers(1, &dvSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, dvSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, dvs.size() * sizeof(float), dvs.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, dvSSBO);

    // pressure
    glGenBuffers(1, &pressureSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, pressureSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, pressures.size() * sizeof(float), pressures.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, pressureSSBO);

    // pv
    glGenBuffers(1, &pvSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, pvSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, pvs.size() * sizeof(float), pvs.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, pvSSBO);



    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, positionSSBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

}


Particles::~Particles(){
}

void Particles::draw(Shader& shader){
    shader.use();
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, num_particles);
}