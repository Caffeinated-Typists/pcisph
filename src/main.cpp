#include "shader.hpp"
#include "utils.hpp"
#include "particles.hpp"
#include "solver.hpp"

const int MAX_PARTICLES = 6000;

Shader* shader;
std::vector<Point> points;
int screenWidth = 1280;
int screenHeight = 720;
float viewport_width = 12.5f;
float viewport_height = screenHeight * viewport_width / screenWidth;

void framebuffer_size_callback(GLFWwindow *window, int width, int height){
    glViewport(0, 0, width, height);
}

void addParticles(int particles_per_row, int particles_per_col){
    if(MAX_PARTICLES < points.size() + particles_per_row * particles_per_col){
        return;
    }
    glm::vec2 start(0.25 * viewport_width, 0.95 * viewport_height);
    float x0 = start.x;
    float spacing = Point::radius;
    for (int i = 0; i < particles_per_row; i++){
        for (int j = 0; j < particles_per_col; j++){
            points.emplace_back(start);
            start.x += 2.0f * Point::radius + spacing;
        }
        start.x = x0;
        start.y -= 2.0f * Point::radius + spacing;
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS){
        // logic to add particles and resize the window
        addParticles(20, 20);
    }
}


int main(){
    GLFWwindow *window = utils::setupWindow(screenWidth, screenHeight);
    ImGuiIO &io = ImGui::GetIO();
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    unsigned int VAO;
    shader = new Shader("./shaders/circle.vert", "./shaders/circle.frag");

    points.reserve(30000);


    // keep all coordinates in the range [-1, 1]
    addParticles(50, 50);


    auto ptr = std::make_shared<std::vector<Point>>(points);
    Particles particles(ptr);
    Solver solver(ptr, viewport_width, viewport_height);

    glm::mat4 projection = glm::ortho(0.0f, viewport_width, 0.0f, viewport_height, 0.0f, 1.0f);

    shader->use();
    shader->setMat4("projection", projection);


    while (!glfwWindowShouldClose(window)){
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        
        solver.Update();

        {
            ImGui::Begin("Frames");
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        ImGui::Render();

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glDisable(GL_POINT_SMOOTH);
        shader->use();
        shader->setMat4("projection", projection);
        particles.draw(VAO, *shader);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwSwapInterval(0);

    }

    utils::cleanup(window);

    return 0;
}
