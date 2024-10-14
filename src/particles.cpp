#include <particles.hpp>

Particles::Particles(/* args */)
{
}

Particles::Particles(std::vector<Point>& _particles){
    particles = _particles;
    num_particles = particles.size();
}

Particles::~Particles()
{
}

void Particles::draw(unsigned int& VAO, Shader& shader){
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    float* points = new float[num_particles * 2];

    for (int i = 0; i < num_particles; i++){
        points[i * 2] = particles[i].position.x;
        points[i * 2 + 1] = particles[i].position.y;
    }
    
    // generate the VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, num_particles * 2 * sizeof(float), points, GL_STREAM_DRAW);
    shader.enableVertexAttribute("aPos");
    shader.setVertexAttribPointer("aPos", 2, GL_FLOAT, false, 0, 0);

    // draw the particles
    glDrawArrays(GL_POINTS, 0, num_particles);
}

void Particles::addParticle(Point& particle){
    particles.emplace_back(particle);
    num_particles = particles.size();
}