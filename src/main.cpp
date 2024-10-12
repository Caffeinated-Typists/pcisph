#define STB_IMAGE_IMPLEMENTATION

#include "shader.hpp"
#include "utils.hpp"
#include "particle.hpp"

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
};

Shader* shader;
int screenWidth = 800;
int screenHeight = 600;

void setupModelTransformation(Shader*);
void setupViewTransformation(Shader*);
void setupProjectionTransformation(Shader*, int, int );


void framebuffer_size_callback(GLFWwindow *window, int width, int height){
    glViewport(0, 0, width, height);
    setupProjectionTransformation(shader, width, height);
}


int main(int argc, char const *argv[]){
    GLFWwindow *window = utils::setupWindow(800, 600);
    ImGuiIO &io = ImGui::GetIO();
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Vertex vertices[] = {
        {{0.5f, 0.5f, 0.0f}, {1.0f, 0.0f, 1.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 0.0f}}
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };


    unsigned int VAO;


    shader = new Shader("./shaders/circle.vert", "./shaders/circle.frag");
    Particle particle(glm::vec3(0.0f, 0.0f, 0.0f));    

    particle.render(VAO, *shader);

    shader->use();

    setupModelTransformation(shader);
    setupViewTransformation(shader);
    setupProjectionTransformation(shader, screenWidth, screenHeight);

    shader->setFloat2v("resolution", (float)screenWidth, (float)screenHeight);

    while (!glfwWindowShouldClose(window)){
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

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
        particle.draw(VAO);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);

    }

    utils::cleanup(window);

    return 0;
}

void setupModelTransformation(Shader* shader) {
    // Modelling transformations (Model -> World coordinates)
    glm::mat4 modelT = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0)); // Model coordinates are the world coordinates
    shader->setMat4("model", modelT);
}

void setupViewTransformation(Shader* shader) {
    // Viewing transformations (World -> Camera coordinates)
    glm::mat4 viewT = glm::lookAt(glm::vec3(0.0, 0.0, 5.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    shader->setMat4("view", viewT);
}

void setupProjectionTransformation(Shader* shader, int screen_width, int screen_height) {
    // Projection transformation
    glm::mat4 projectionT = glm::perspective(45.0f, (float)screen_width / (float)screen_height, 0.1f, 1000.0f);
    shader->setMat4("projection", projectionT);
}
