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
    void checkCompileErrors(unsigned int shader, const char* type);
    void checkIfAttributeExists(const char* name) const;
public:
    /**
     * @brief Constructor for the shader class
     * 
     * @param vshaderPath Path to the vertex shader file
     * @param fshaderPath Path to the fragment shader file
     */
    Shader(const char* vshaderPath, const char* fshaderPath);

    /**
     * @brief Constructor for the shader class, for compute shaders
     * 
     * @param shaderPath Path to the compute shader file
     */
    Shader(const char* shaderPath);
    
    /**
     * @brief Default constructor for the Shader class.
     */
    Shader() = default;
    
    /**
     * @brief Destructor for the Shader class.
     */
    ~Shader();

    /**
     * @brief Activates the shader program.
     */
    void use();


    /**
     * @brief enables the vertex attribute with the given name.
     * 
     * @param name The name of the vertex attribute.
     */
    void enableVertexAttribute(const char* name) const;


    /**
     * @brief set the Vertex Attribute Pointer for the given attribute name.
     * 
     * @param name The name of the vertex attribute.
     * @param size The number of components per vertex attribute.
     * @param type The data type of each component in the array.
     * @param stride The byte offset between consecutive generic vertex attributes.
     * @param offset The offset of the first component of the first generic vertex attribute in the array in the data store of the buffer currently bound to the GL_ARRAY_BUFFER target.
     * @param normalized Whether fixed-point data values should be normalized (true) or converted directly as fixed-point values (false) when they are accessed.
     *  
     */
    void setVertexAttribPointer(const char* name, int size, unsigned int type, bool normalized, int stride, const void *offset) const;

    /**
     * @brief set the value of a vec4 uniform in the shader program.
     * 
     * @param name The name of the uniform variable.
     * @param value The value of the uniform variable. Must be an array of 4 floats.
     */
    void setFloat4v(const char* name, const float value[4]) const;

    /**
     * @brief set the value of a vec3 uniform in the shader program.
     * 
     * @param name The name of the uniform variable.
     * @param value The value of the uniform variable. Must be an array of 3 floats.
     */
    void setFloat3v(const char* name, const float value[3]) const;

    /**
     * @brief set the value of a int in the shader program.
     * 
     * @param name The name of the uniform variable.
     * @param value The value of the uniform variable.
     */
    void setInt(const char* name, int value) const;

    /**
     * @brief set the value of a mat4 in the shader program
     * 
     * @param name The name of the uniform variable.
     * @param value The value of the uniform variable
     */
    void setMat4(const char* name, glm::mat4 &value) const;

    /**
     * @brief set the value of a vec2 in the shader program
     * 
     * @param name The name of the uniform variable.
     * @param x The x component of the vec2
     * @param y The y component of the vec2
     */
    void setFloat2v(const char* name, float x, float y) const;

    /**
     * @brief set the value of a float in the shader program
     * 
     * @param name The name of the uniform variable.
     * @param value The value of the uniform variable
     */
    void setFloat(const char* name, float value) const;
};
