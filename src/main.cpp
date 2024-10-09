#define STB_IMAGE_IMPLEMENTATION

#include "shader.hpp"
#include "utils.hpp"

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
};

void framebuffer_size_callback(GLFWwindow *window, int width, int height){
    glViewport(0, 0, width, height);
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

    unsigned int VBO, VAO, EBO  ;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    // Bind VAO first, then bind and set vertex buffers    
    glBindVertexArray(VAO);

    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    //  position attrib
    glVertexAttribPointer(0, sizeof(Vertex::position) / sizeof(Vertex::position[0]), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));
    glVertexAttribPointer(1, sizeof(Vertex::color) / sizeof(Vertex::color[0]), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, color));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    Shader shader("./shaders/vshader.vert", "./shaders/fshader.frag");

    shader.use();


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

        shader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);

    }

    utils::cleanup(window);

    return 0;
}
