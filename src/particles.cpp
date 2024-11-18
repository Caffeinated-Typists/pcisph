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

    grid_xs.resize(num_particles);
    grid_ys.resize(num_particles);

    neighbours.resize(num_particles);
    distances.resize(num_particles);
    sizes.resize(num_particles);

    std::for_each(std::execution::par_unseq, neighbours.begin(), neighbours.end(), [&](std::vector<unsigned short>& v){
        v.resize(MAX_NEIGHBOURS);
    });

    // create the index array
    indices.resize(num_particles);
    std::iota(indices.begin(), indices.end(), 0);
}


Particles::~Particles(){
}

void Particles::draw(unsigned int& VAO, Shader& shader){
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);


    // std::cout << "R: " << points[0] << " " << points[1] << std::endl;

    // generate the VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);


    // use the positions to draw the particles
    glBufferData(GL_ARRAY_BUFFER, num_particles * 2 * sizeof(float), positions.data(), GL_STATIC_DRAW);

    shader.enableVertexAttribute("aPos");
    shader.setVertexAttribPointer("aPos", 2, GL_FLOAT, false, 0, 0);

    // draw the particles
    glDrawArrays(GL_POINTS, 0, num_particles);
}