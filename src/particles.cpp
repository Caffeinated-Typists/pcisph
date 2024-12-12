#include <particles.hpp>


Particles::Particles(std::vector<Point> *_particles, int max_particles) : particles(_particles) {
    points = new float[max_particles * 2];
}

Particles::~Particles(){
    delete[] points;
}

void Particles::addParticles(std::vector<Point> *newPoints){
}

void Particles::draw(unsigned int& VAO, Shader& shader){
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    for (size_t i = 0; i < particles->size(); i++){
        points[i * 2] = (*particles)[i].position.x;
        points[i * 2 + 1] = (*particles)[i].position.y;
    }
    
    // std::cout << "R: " << points[0] << " " << points[1] << std::endl;

    // generate the VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, particles->size() * 2 * sizeof(float), points, GL_STREAM_DRAW);
    shader.enableVertexAttribute("aPos");
    shader.setVertexAttribPointer("aPos", 2, GL_FLOAT, false, 0, 0);

    // draw the particles
    glDrawArrays(GL_POINTS, 0, particles->size());
}