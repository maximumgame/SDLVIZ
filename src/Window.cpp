#include "Window.hpp"
#include <spdlog/spdlog.h>
#include <stdexcept>

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

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    window = SDL_CreateWindow("OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_width, m_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    glcontext = SDL_GL_CreateContext(window);

    SDL_AddEventWatch(WindowEventHandler, this);
}
