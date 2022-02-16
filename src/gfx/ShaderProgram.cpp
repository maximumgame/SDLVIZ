#include "ShaderProgram.hpp"
#include <stdexcept>

#include <Window.hpp>

#include <spdlog/spdlog.h>
#include <chrono>

using namespace gfx;

ShaderProgram::ShaderProgram(std::shared_ptr<Window> window)
{
    m_window = window;
    m_timeUniform = std::make_shared<gfx::shader::input::TimeUniform>();
    m_resolutionUniform = std::make_shared<gfx::shader::input::ResolutionUniform>(m_window);

    m_program = glCreateProgram();

    m_vertexShader = std::make_unique<gfx::shader::ShaderSource>(GL_VERTEX_SHADER, "#version 300 es\nin vec2 fragColor; void main() { gl_Position = vec4( fragColor.x, fragColor.y, 0, 1 ); }");
    m_fragmentShader = std::make_unique<gfx::shader::ShaderSource>(GL_FRAGMENT_SHADER, std::string
                                                                                                    {"#version 300 es\n"
                                                                                                    "precision mediump float;\n"
                                                                                                    "out vec4 fragColor;\n"
                                                                                                    "uniform float iTime;\n"
                                                                                                    "uniform vec3 iResolution;\n"
                                                                                                    "void main()\n"
                                                                                                    "{\n"
                                                                                                    "    vec2 uv = gl_FragCoord.xy/iResolution.xy;\n"
                                                                                                    "    vec3 color = 0.5 + 0.5 * cos(iTime + uv.xyx + vec3(0, 2, 4));\n"
                                                                                                    "    fragColor = vec4(color, 1);\n"
                                                                                                    "}\n"});
    
    if(!m_vertexShader->ShaderSuccess())
    {
        spdlog::error("Vertex shader failed to compile {}", m_vertexShader->GetShaderLog().get());
        throw std::runtime_error("Vertex shader failed to compile");
    }

    if(!m_fragmentShader->ShaderSuccess())
    {
        spdlog::error("Fragment shader failed to compile {}", m_fragmentShader->GetShaderLog().get());
        throw std::runtime_error("Fragment shader failed to compile");
    }

    glAttachShader(m_program, m_vertexShader->GetShader());
    glAttachShader(m_program, m_fragmentShader->GetShader());

    //link shaders
    glLinkProgram(m_program);

    //unlink shaders
    glDetachShader(m_program, m_vertexShader->GetShader());
    glDetachShader(m_program, m_fragmentShader->GetShader());

    //check program for errors
    GLint programSuccess = GL_TRUE;
    glGetProgramiv(m_program, GL_LINK_STATUS, &programSuccess);
    if(programSuccess != GL_TRUE)
    {
        throw std::runtime_error("Failed to link default shader program");
    }

    m_gVertexPos2DLocation = glGetAttribLocation(m_program, "fragColor");
    if(m_gVertexPos2DLocation == -1)
    {
        throw std::runtime_error("Failed to get location of fragColor attribute");
    }

    typedef struct VertexData_S {
        GLfloat x, y;
    } VertexData;

    VertexData vertexData[] =
    {
        { -1.f, -1.f },
        { 1.f, -1.f },
        { 1.f, 1.f },
        { -1.f, 1.f }
    };

    //ibo
    GLuint indexData[] = { 0, 1, 2, 3 };

    glGenVertexArrays(1, &m_vao);
    m_vbo = std::make_shared<gfx::BufferObject>(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    m_ebo = std::make_shared<gfx::BufferObject>(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);

    spdlog::info("Initialized ShaderProgram");
}

std::shared_ptr<GLchar[]> ShaderProgram::LinkNewFragmentShader(const std::string &shaderSource)
{
    GLuint newProgram = glCreateProgram();
    
    auto tempVertexShader = std::make_shared<gfx::shader::ShaderSource>(GL_VERTEX_SHADER, "#version 300 es\nin vec2 fragColor; void main() { gl_Position = vec4( fragColor.x, fragColor.y, 0, 1 ); }");
    auto tempFragmentShader = std::make_shared<gfx::shader::ShaderSource>(GL_FRAGMENT_SHADER, shaderSource);

    if(!tempVertexShader->ShaderSuccess())
    {
        spdlog::error("Failed to compile vertex shader {}", tempVertexShader->GetShaderLog().get());
        glDeleteProgram(newProgram);
        return tempVertexShader->GetShaderLog();
    }

    if(!tempFragmentShader->ShaderSuccess())
    {
        spdlog::error("Failed to compile fragment shader {}", tempFragmentShader->GetShaderLog().get());
        glDeleteProgram(newProgram);
        return tempFragmentShader->GetShaderLog();
    }


    glAttachShader(newProgram, tempVertexShader->GetShader());
    glAttachShader(newProgram, tempFragmentShader->GetShader());
    glLinkProgram(newProgram);
    glDetachShader(newProgram, tempVertexShader->GetShader());
    glDetachShader(newProgram, tempFragmentShader->GetShader());

    //check program for errors
    GLint programSuccess = GL_TRUE;
    glGetProgramiv(newProgram, GL_LINK_STATUS, &programSuccess);
    if(programSuccess != GL_TRUE)
    {
        spdlog::error("Failed to link shader program");
        glDeleteProgram(newProgram);
        return nullptr;
    }

    //this must be updated when we link the new program
    m_gVertexPos2DLocation = glGetAttribLocation(m_program, "fragColor");
    if(m_gVertexPos2DLocation == -1)
    {
        throw std::runtime_error("Failed to get location of fragColor");
    }

    m_fragmentShader = tempFragmentShader;
    m_vertexShader = tempVertexShader;

    glDeleteProgram(m_program); //delete old program
    m_program = newProgram; //assign new compiled program

    return nullptr;
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(m_program);

    spdlog::info("Deinitialized ShaderProgram");
}

void ShaderProgram::Uniforms()
{
    m_timeUniform->Update();
    m_timeUniform->ApplyUniform(m_program);
    m_resolutionUniform->ApplyUniform(m_program);
}

void ShaderProgram::render()
{
    glUseProgram(m_program);

    Uniforms();

    glBindVertexArray(m_vao); //bind our vao first

    m_vbo->Bind(GL_ARRAY_BUFFER); //bind vertex array object to GL_ARRAY_BUFFER
    glEnableVertexAttribArray(m_gVertexPos2DLocation); //tells OpenGL to use the vertex array object for the vertex position
    glVertexAttribPointer(m_gVertexPos2DLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL); //tells opengl to place the vertex position data at the location specified by the location parameter from our shader source code

    m_ebo->Bind(GL_ELEMENT_ARRAY_BUFFER); //bind the vertex index ordering for drawing

    glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL); //draw our verts (4 since its a full screen quad)

    glDisableVertexAttribArray(m_gVertexPos2DLocation);

    glBindVertexArray(0);
    glUseProgram(NULL);
}
