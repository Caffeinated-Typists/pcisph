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

    grid_pos.resize(num_particles * 2);

    neighbours.resize(num_particles);
    distances.resize(num_particles);
    sizes.resize(num_particles);

    std::for_each(std::execution::par_unseq, neighbours.begin(), neighbours.end(), [&](std::vector<unsigned short>& v){
        v.resize(MAX_NEIGHBOURS);
    });

    std::for_each(std::execution::par_unseq, distances.begin(), distances.end(), [&](std::vector<float>& v){
        v.resize(MAX_NEIGHBOURS);
    });

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