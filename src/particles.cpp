#include <particles.hpp>


Particles::Particles(std::vector<float> _positions) : positions(_positions){
    num_particles = _positions.size() / 2;

    // initialize all the other vectors
    velocities.resize(num_particles * 2);
    previous_positions.resize(num_particles * 2);
    predicted_positions.resize(num_particles * 2);


    pressures.resize(num_particles);
    pvs.resize(num_particles);


    spatialIndices.resize(num_particles * 4); // stores (original index, hash, and cellPosx, cellPosy)


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

    // pressure
    glGenBuffers(1, &pressureSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, pressureSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, pressures.size() * sizeof(float), pressures.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, pressureSSBO);

    // pv
    glGenBuffers(1, &pvSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, pvSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, pvs.size() * sizeof(float), pvs.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, pvSSBO);

    // spatial indices
    glGenBuffers(1, &spatialIndexSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, spatialIndexSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, spatialIndices.size() * sizeof(int), spatialIndices.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, spatialIndexSSBO);

    // // spatial offsets being set in solver


    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, positionSSBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

}


Particles::~Particles(){
}

void Particles::getSSBOData(){
}

void Particles::draw(Shader& shader){
    shader.use();
    getSSBOData();
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, num_particles);
}