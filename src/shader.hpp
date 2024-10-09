#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
 * @class Shader
 * @brief A class to manage OpenGL shader programs.
 * 
 * The Shader class is responsible for loading, compiling, and linking vertex and fragment shaders.
 * It also provides a method to activate the shader program.
 */

class Shader
{
private:
    unsigned int shaderProgram;
    /**
     * @brief Checks and prints any compile or link errors.
     * 
     * @param shader The ID of the shader or program.
     * @param type The type of the shader (vertex, fragment, or program).
     */
    void checkCompileErrors(unsigned int shader, std::string type);
public:
    /**
     * @brief Constructor for the shader class
     * 
     * @param vshaderPath Path to the vertex shader file
     * @param fshaderPath Path to the fragment shader file
     */
    Shader(std::string vshaderPath, std::string fshaderPath);
    /**
     * @brief Destructor for the Shader class.
     */
    ~Shader();

    /**
     * @brief Activates the shader program.
     */
    void use();


    /**
     * @brief set the value of a vec4 uniform in the shader program.
     * 
     * @param name The name of the uniform variable.
     * @param value The value of the uniform variable. Must be an array of 4 floats.
     */
    void setFloat4v(const std::string &name, const float value[4]) const;

    /**
     * @brief set the value of a vec3 uniform in the shader program.
     * 
     * @param name The name of the uniform variable.
     * @param value The value of the uniform variable. Must be an array of 3 floats.
     */
    void setFloat3v(const std::string &name, const float value[3]) const;

    /**
     * @brief set the value of a int in the shader program.
     * 
     * @param name The name of the uniform variable.
     * @param value The value of the uniform variable.
     */
    void setInt(const std::string &name, int value) const;

    /**
     * @brief set the value of a mat4 in the shader program
     * 
     * @param name The name of the uniform variable.
     * @param value The value of the uniform variable
     */
    void setMat4(const std::string &name, glm::mat4 &value) const;

};
