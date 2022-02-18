#include "Window.hpp"
#include <spdlog/spdlog.h>
#include <stdexcept>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5_webgl.h>
#endif

namespace {
    //handle events
    int SDLCALL WindowEventHandler(void* data, SDL_Event* event)
    {
        Window* window = (Window*)data;
        switch(event->type)
        {
            case SDL_WINDOWEVENT:
                switch(event->window.event)
                {
                    case SDL_WINDOWEVENT_RESIZED:
                        //hax
                        int* width = const_cast<int*>(window->getWidth());
                        int* height = const_cast<int*>(window->getHeight());

                        *height = event->window.data1;
                        *width = event->window.data2;
                        break;
                }
                break;
        }
        return 0;
    }
}

Window::Window()
{
    m_width = 1280;
    m_height = 720;
    createWindow();
    spdlog::info("Initialized window");
}

Window::~Window()
{
    spdlog::info("Destroying window...");
    SDL_DelEventWatch(WindowEventHandler, this);
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


//create window
void Window::createWindow()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        spdlog::error("Error initializing SDL: {}", SDL_GetError());
        throw std::runtime_error("SDL initialization failed");
    }

#ifdef __EMSCRIPTEN__
    EmscriptenWebGLContextAttributes attr;
    attr.majorVersion = 3;
    attr.minorVersion = 2;
    attr.alpha = 1;
    attr.powerPreference = EM_WEBGL_POWER_PREFERENCE_DEFAULT;
    attr.explicitSwapControl = 1;
    emscripten_webgl_init_context_attributes(&attr);
    attr.majorVersion = 3;
    m_emContext = emscripten_webgl_create_context("#canvas", &attr);
    if(m_emContext < 0)
    {
        throw std::runtime_error("WebGL context creation failed");
    }
    emscripten_webgl_make_context_current(m_emContext);
#endif

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    window = SDL_CreateWindow("SDLVIZ", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_width, m_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    
#ifndef __EMSCRIPTEN__
    glcontext = SDL_GL_CreateContext(window);
#else
    emscripten_webgl_make_context_current(m_emContext);
#endif

    if(SDL_GL_SetSwapInterval(1) < 0)
    {
        spdlog::error("Error setting vsync: ", SDL_GetError());
    }

    SDL_AddEventWatch(WindowEventHandler, this);
}
