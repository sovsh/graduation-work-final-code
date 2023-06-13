#include "shader.h"

Shader::Shader() { this->shader_program = 0; }

Shader::Shader(std::string vertex_shader_file, std::string fragment_shader_file)
{
    GLuint vertex, fragment;

    create_shader(vertex_shader_file, vertex, GL_VERTEX_SHADER);
    check_error();

    create_shader(fragment_shader_file, fragment, GL_FRAGMENT_SHADER);
    check_error();

    create_shader_program(vertex, fragment);
    check_error();
}

Shader::Shader(std::string vertex_shader_file, std::string geometry_shader_file, std::string fragment_shader_file)
{
    GLuint vertex, geometry, fragment;

    create_shader(vertex_shader_file, vertex, GL_VERTEX_SHADER);
    check_error();

    create_shader(geometry_shader_file, geometry, GL_GEOMETRY_SHADER);
    check_error();

    create_shader(fragment_shader_file, fragment, GL_FRAGMENT_SHADER);
    check_error();

    create_shader_program(vertex, geometry, fragment);
    check_error();
}

Shader::Shader(std::string vertex_shader_file, std::string tess_control_shader_file, std::string tess_evaluation_shader_file, std::string fragment_shader_file)
{
    GLuint vertex, tess_control, tess_evaluation, fragment;

    create_shader(vertex_shader_file, vertex, GL_VERTEX_SHADER);
    check_error();

    create_shader(tess_control_shader_file, tess_control, GL_TESS_CONTROL_SHADER);
    check_error();

    create_shader(tess_evaluation_shader_file, tess_evaluation, GL_TESS_EVALUATION_SHADER);
    check_error();

    create_shader(fragment_shader_file, fragment, GL_FRAGMENT_SHADER);
    check_error();

    create_shader_program(vertex, tess_control, tess_evaluation, fragment);
    check_error();
}

Shader::Shader(std::string vertex_shader_file, std::string tess_control_shader_file, std::string tess_evaluation_shader_file, std::string geometry_shader_file, std::string fragment_shader_file)
{
    GLuint vertex, tess_control, tess_evaluation, geometry, fragment;

    create_shader(vertex_shader_file, vertex, GL_VERTEX_SHADER);
    check_error();

    create_shader(tess_control_shader_file, tess_control, GL_TESS_CONTROL_SHADER);
    check_error();

    create_shader(tess_evaluation_shader_file, tess_evaluation, GL_TESS_EVALUATION_SHADER);
    check_error();

    create_shader(geometry_shader_file, geometry, GL_GEOMETRY_SHADER);
    check_error();

    create_shader(fragment_shader_file, fragment, GL_FRAGMENT_SHADER);
    check_error();

    create_shader_program(vertex, tess_control, tess_evaluation, geometry, fragment);
    check_error();
}

void Shader::use() { glUseProgram(this->shader_program); }

void Shader::unuse() { glUseProgram(0); }

void Shader::delete_program() { glDeleteProgram(this->shader_program); }

GLuint Shader::get_attribute_position(const GLchar* name)
{
    GLuint attribute = glGetAttribLocation(this->shader_program, name);
    if (attribute == -1)
    {
        std::cout << "Error: get_attribute_position" << std::endl;
        return 0;
    }
    return attribute;
}

void Shader::set_uniform_int(const std::string& name, int value) const { glUniform1i(glGetUniformLocation(this->shader_program, name.c_str()), value); }

void Shader::set_uniform_float(const std::string& name, float value) const { glUniform1f(glGetUniformLocation(this->shader_program, name.c_str()), value); }

void Shader::set_uniform_vec3(const std::string& name, glm::vec3 value) const { glUniform3fv(glGetUniformLocation(this->shader_program, name.c_str()), 1, glm::value_ptr(value)); }

void Shader::set_uniform_vec4(const std::string& name, glm::vec4 value) const { glUniform4fv(glGetUniformLocation(this->shader_program, name.c_str()), 1, glm::value_ptr(value)); }

void Shader::set_uniform_mat4(const std::string& name, glm::mat4 value) const { glUniformMatrix4fv(glGetUniformLocation(this->shader_program, name.c_str()), 1, GL_FALSE, glm::value_ptr(value)); }

void Shader::create_shader(std::string shader_file, GLuint &shader, int type)
{
    std::string shader_code;
    std::ifstream shader_ifstream;

    // генерация исключений
    shader_ifstream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        shader_ifstream.open(shader_file);

        std::stringstream shader_stringstream;
        shader_stringstream << shader_ifstream.rdbuf();

        shader_ifstream.close();

        shader_code = shader_stringstream.str();
    }

    catch (std::ifstream::failure& e)
    {
        std::cout << "Error: File reading error " << e.what() << std::endl;
    }

    const char* const_shader_code = shader_code.c_str();

    check_error();

    shader = glCreateShader(type);

    glShaderSource(shader, 1, &const_shader_code, NULL);
    glCompileShader(shader);
    check_errors(shader, "vertex shader");

    check_error();
}

void Shader::create_shader_program(GLuint vertex_shader, GLuint fragment_shader)
{
    this->shader_program = glCreateProgram();
    glAttachShader(this->shader_program, vertex_shader);
    glAttachShader(this->shader_program, fragment_shader);
    glLinkProgram(this->shader_program);

    check_errors_program();

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    check_error();
}

void Shader::create_shader_program(GLuint vertex_shader, GLuint geometry_shader, GLuint fragment_shader)
{
    this->shader_program = glCreateProgram();
    glAttachShader(this->shader_program, vertex_shader);
    glAttachShader(this->shader_program, geometry_shader);
    glAttachShader(this->shader_program, fragment_shader);
    glLinkProgram(this->shader_program);

    check_errors_program();

    glDeleteShader(vertex_shader);
    glDeleteShader(geometry_shader);
    glDeleteShader(fragment_shader);

    check_error();
}

void Shader::create_shader_program(GLuint vertex_shader, GLuint tess_control_shader, GLuint tess_evaluation_shader, GLuint fragment_shader)
{
    this->shader_program = glCreateProgram();
    glAttachShader(this->shader_program, vertex_shader);
    glAttachShader(this->shader_program, tess_control_shader);
    glAttachShader(this->shader_program, tess_evaluation_shader);
    glAttachShader(this->shader_program, fragment_shader);
    glLinkProgram(this->shader_program);

    check_errors_program();

    glDeleteShader(vertex_shader);
    glDeleteShader(tess_control_shader);
    glDeleteShader(tess_evaluation_shader);
    glDeleteShader(fragment_shader);

    check_error();
}

void Shader::create_shader_program(GLuint vertex_shader, GLuint tess_control_shader, GLuint tess_evaluation_shader, GLuint geometry_shader, GLuint fragment_shader)
{
    this->shader_program = glCreateProgram();
    glAttachShader(this->shader_program, vertex_shader);
    glAttachShader(this->shader_program, tess_control_shader);
    glAttachShader(this->shader_program, tess_evaluation_shader);
    glAttachShader(this->shader_program, geometry_shader);
    glAttachShader(this->shader_program, fragment_shader);
    glLinkProgram(this->shader_program);

    check_errors_program();

    glDeleteShader(vertex_shader);
    glDeleteShader(tess_control_shader);
    glDeleteShader(tess_evaluation_shader);
    glDeleteShader(geometry_shader);
    glDeleteShader(fragment_shader);

    check_error();
}

void Shader::check_errors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "shader program")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

void Shader::check_errors_shader(unsigned int shader)
{
    int success;
    char infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "Error: check_errors_shader; " << infoLog << "\n";
    }
}

void Shader::check_errors_program()
{
    int success;
    glGetProgramiv(this->shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        std::cout << "Error: check_errors_program\n";
        return;
    }
}

void Shader::check_error() {
    GLenum errCode;
    // https://www.khronos.org/opengl/wiki/OpenGL_Error
    if ((errCode = glGetError()) != GL_NO_ERROR)
        std::cout << "OpenGl error!: " << errCode << std::endl;
}
