#include "shader.hpp"

Shader::Shader(std::string vshaderPath, std::string fshaderPath) {
    std::ifstream vshaderFile;
    std::ifstream fshaderFile;

    std::string vshaderSource;
    std::string fshaderSource;

    vshaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fshaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        vshaderFile.open(vshaderPath);
        fshaderFile.open(fshaderPath);

        std::stringstream vShaderStream;
        std::stringstream fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vshaderFile.rdbuf();
        fShaderStream << fshaderFile.rdbuf();
        // close file handlers
        vshaderFile.close();
        fshaderFile.close();

        vshaderSource = vShaderStream.str();
        fshaderSource = fShaderStream.str();

    } catch (const std::exception &e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_READ_SUCCESFULLY: " << e.what() << '\n';
    }

    const char *vshaderCode = vshaderSource.c_str();
    const char *fshaderCode = fshaderSource.c_str();

    // shader objects creation
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    shaderProgram = glCreateProgram();

    // vertex shader code compilation
    glShaderSource(vertexShader, 1, &vshaderCode, NULL);
    glCompileShader(vertexShader);
    checkCompileErrors(vertexShader, "VERTEX");

    // fragment shader code compilation
    glShaderSource(fragmentShader, 1, &fshaderCode, NULL);
    glCompileShader(fragmentShader);
    checkCompileErrors(fragmentShader, "FRAGMENT");

    // program code compilation
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glValidateProgram(shaderProgram);
    checkCompileErrors(shaderProgram, "PROGRAM");

    // cleanup of shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::use() { glUseProgram(shaderProgram); }

Shader::~Shader() { glDeleteProgram(shaderProgram); }
void Shader::checkCompileErrors(unsigned int shader, std::string type) {
    int success;
    char infoLog[1024];

    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr   << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED of type "
                        << type << "\n"
                        << infoLog << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout   << "ERROR::PROGRAM_LINKING_ERROR of type " << type << "\n"
                        << infoLog << std::endl;
        }
    }
}

void Shader::checkIfAttributeExists(const std::string &name) const {
    if (glGetAttribLocation(shaderProgram, name.c_str()) == -1) {
        std::cerr << "ERROR::SHADER::ATTRIBUTE_NOT_FOUND: " << name << std::endl;
    }
}

void Shader::enableVertexAttribute(const std::string &name) const {
    glEnableVertexAttribArray(glGetAttribLocation(shaderProgram, name.c_str()));
}

void Shader::setVertexAttribPointer(const std::string &name, int size, unsigned int type, bool normalized, int stride, const void *offset) const{
    glVertexAttribPointer(glGetAttribLocation(shaderProgram, name.c_str()), size, type, normalized, stride, offset);
}


void Shader::setFloat4v(const std::string &name, const float value[4]) const {
    glUniform4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, value);
}

void Shader::setFloat3v(const std::string &name, const float value[3]) const {
    glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, value);
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
}

void Shader::setMat4(const std::string &name, glm::mat4 &value) const{
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setFloat2v(const std::string &name, float x, float y) const {
    glUniform2f(glGetUniformLocation(shaderProgram, name.c_str()), x, y);
}