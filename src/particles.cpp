#include <particles.hpp>


Particles::Particles(std::shared_ptr<std::vector<Point>> _particles) : particles(_particles), num_particles(_particles->size()) {
    points = new float[num_particles * 2];
}

Particles::~Particles(){
    delete[] points;
}

void Particles::draw(unsigned int& VAO, Shader& shader){
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    for (size_t i = 0; i < num_particles; i++){
        points[i * 2] = (*particles)[i].position.x;
        points[i * 2 + 1] = (*particles)[i].position.y;
    }
    
    // std::cout << "R: " << points[0] << " " << points[1] << std::endl;

    // generate the VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, num_particles * 2 * sizeof(float), points, GL_STREAM_DRAW);
    shader.enableVertexAttribute("aPos");
    shader.setVertexAttribPointer("aPos", 2, GL_FLOAT, false, 0, 0);

    // draw the particles
    glDrawArrays(GL_POINTS, 0, num_particles);
}