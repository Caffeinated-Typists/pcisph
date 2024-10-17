#include "shader.hpp"
#include "utils.hpp"
#include "particles.hpp"
#include "solver.hpp"


Shader* shader;
int screenWidth = 800;
int screenHeight = 600;


void framebuffer_size_callback(GLFWwindow *window, int width, int height){
    glViewport(0, 0, width, height);
}


int main(){
    GLFWwindow *window = utils::setupWindow(800, 600);
    ImGuiIO &io = ImGui::GetIO();
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    unsigned int VAO;
    shader = new Shader("./shaders/circle.vert", "./shaders/circle.frag");

    std::vector<Point> points;
    points.reserve(48);


    // keep all coordinates in the range [-1, 1]
    int particles_per_row = 8;
    int particles_per_col = 6;
    float x = -1.0;
    float y = 1.0;

    float x_offset = 0.1;
    float y_offset = -0.2;

    float dx = 2.0 / particles_per_row;
    float dy = -2.0 / particles_per_col;

    for (int i = 0; i < particles_per_col; i++){
        for (int j = 0; j < particles_per_row; j++){
            points.emplace_back(glm::vec2(x + x_offset, y + y_offset));
            x += dx;
        }
        x = -1.0;
        y += dy;
    }


    auto ptr = std::make_shared<std::vector<Point>>(points);
    Particles particles(ptr);
    Solver solver(ptr);

    shader->use();

    while (!glfwWindowShouldClose(window)){
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // solver->printPositions();
        
        for (int _ = 0; _ < Solver::STEPS; _++){
            solver.ExternalForces();
            solver.Integrate();
            solver.BoundaryCheck();
        }


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
        particles.draw(VAO, *shader);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        // glfwSwapInterval(0);

    }

    utils::cleanup(window);

    return 0;
}
