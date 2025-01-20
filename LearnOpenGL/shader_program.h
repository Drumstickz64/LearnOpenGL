#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

class Shader_Program
{
public:
    // the program ID
    GLuint id;

    // constructor reads and builds the shader
    Shader_Program(const std::string& vertexSource, const std::string& fragmentSource);
    // use/activate the shader
    void use() const;
    // utility uniform functions
    void set_bool(const std::string& name, bool value) const;
    void set_int(const std::string& name, GLint value) const;
    void set_float(const std::string& name, GLfloat value) const;
    void set_mat4(const std::string& name, const glm::mat4& value) const;
};
