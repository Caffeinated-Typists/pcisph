#include "shader.hpp"

Shader::Shader(const char* vshaderPath, const char* fshaderPath) {
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

Shader::Shader(const char *shaderPath) {
    std::ifstream shaderFile;
    std::string shaderSource;

    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        shaderFile.open(shaderPath);

        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();

        shaderSource = shaderStream.str();

    } catch (const std::exception &e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_READ_SUCCESFULLY: " << e.what() << '\n';
    }

    const char *shaderCode = shaderSource.c_str();

    // shader objects creation
    unsigned int shader = glCreateShader(GL_COMPUTE_SHADER);
    shaderProgram = glCreateProgram();

    // shader code compilation
    glShaderSource(shader, 1, &shaderCode, NULL);
    glCompileShader(shader);
    checkCompileErrors(shader, "COMPUTE");

    // program code compilation
    glAttachShader(shaderProgram, shader);
    glLinkProgram(shaderProgram);
    glValidateProgram(shaderProgram);
    checkCompileErrors(shaderProgram, "PROGRAM");

    // cleanup of shaders
    glDeleteShader(shader);
}


void Shader::use() { glUseProgram(shaderProgram); }

Shader::~Shader() { glDeleteProgram(shaderProgram); }
void Shader::checkCompileErrors(unsigned int shader, const char* type) {
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

void Shader::checkIfAttributeExists(const char* name) const {
    if (glGetAttribLocation(shaderProgram, name) == -1) {
        std::cerr << "ERROR::SHADER::ATTRIBUTE_NOT_FOUND: " << name << std::endl;
    }
}

void Shader::enableVertexAttribute(const char* name) const {
    glEnableVertexAttribArray(glGetAttribLocation(shaderProgram, name));
}

void Shader::setVertexAttribPointer(const char* name, int size, unsigned int type, bool normalized, int stride, const void *offset) const{
    glVertexAttribPointer(glGetAttribLocation(shaderProgram, name), size, type, normalized, stride, offset);
}


void Shader::setFloat4v(const char* name, const float value[4]) const {
    glUniform4fv(glGetUniformLocation(shaderProgram, name), 1, value);
}

void Shader::setFloat3v(const char* name, const float value[3]) const {
    glUniform3fv(glGetUniformLocation(shaderProgram, name), 1, value);
}

void Shader::setInt(const char* name, int value) const {
    glUniform1i(glGetUniformLocation(shaderProgram, name), value);
}

void Shader::setMat4(const char* name, glm::mat4 &value) const{
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setFloat2v(const char* name, float x, float y) const {
    glUniform2f(glGetUniformLocation(shaderProgram, name), x, y);
}

void Shader::setFloat(const char* name, float value) const {
    glUniform1f(glGetUniformLocation(shaderProgram, name), value);
}