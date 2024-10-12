#include "particle.hpp"

Particle::Particle(glm::vec3 _position)
{
    position = _position;
    velocity = glm::vec3(0.0f, 0.0f, 0.0f);

    // only store the quad vertices
    num_points = 4;
    num_indexes = 6;

    // generate the VBO, IBO
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &IBO);
}

void Particle::render(unsigned int& VAO, Shader &shader)
{
    std::cout << num_points << " " << num_indexes << std::endl;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, num_points * 3 * sizeof(float), points, GL_STATIC_DRAW);
    shader.enableVertexAttribute("aPos");
    shader.setVertexAttribPointer("aPos", 3, GL_FLOAT, false, 0, 0);

}

void Particle::draw(unsigned int& VAO){
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, num_points);
    glBindVertexArray(0);
}


Particle::~Particle(){
    delete[] points;
}