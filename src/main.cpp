#define SDL_MAIN_HANDLED

#include <spdlog/spdlog.h>

#include "Window.hpp"
#include <gfx/ShaderProgram.hpp>

#include <ui/MainUI.hpp>

#include <SDL2/SDL_opengl.h>
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5_webgl.h>
#endif


int initGl()
{
    //if we are not emscripten load glad extensions
    int error = 1;
#ifndef __EMSCRIPTEN__
    //int error = gladLoadGL();
    error = gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
    spdlog::info("GLAD init: {}", error);
    spdlog::info("OpenGL {}.{}", GLVersion.major, GLVersion.minor);
#endif

    return error;
}

std::function<void()> glLoop;
static bool quit = false;
#ifdef __EMSCRIPTEN__
void EmLoop() { glLoop(); }
#endif

int main()
{
    std::shared_ptr<Window> main_window(new Window());
    int width = *main_window->getWidth();
    int height = *main_window->getHeight();

    auto errorResult = initGl();
    if(!errorResult)
    {
        spdlog::error("Failed to initialize GL");
        return -1;
    }
    
    spdlog::info("OpenGL version: {}", glGetString(GL_VERSION));
    spdlog::info("GLSL version: {}", glGetString(GL_SHADING_LANGUAGE_VERSION));
    spdlog::info("Vendor: {}", glGetString(GL_VENDOR));
    spdlog::info("Renderer: {}", glGetString(GL_RENDERER));

    std::shared_ptr<gfx::ShaderProgram> shader_program(new gfx::ShaderProgram(main_window));
    spdlog::info("Shader program created");

    std::unique_ptr<ui::MainUI> main_ui(new ui::MainUI(main_window, shader_program));

    glLoop = [&]
    {
        #ifdef __EMSCRIPTEN__
        emscripten_webgl_make_context_current(main_window->GetEMContext());
        #endif
        
        SDL_Event event;
        //handle events
        while(SDL_PollEvent(&event))
        {
            main_ui->processSDLEvent(event);
            if(event.type == SDL_QUIT)
            {
                quit = true;
                return;
            }
        }
        //setup viewport
        SDL_GetWindowSize(main_window->getWindow(), &width, &height);

        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        //render
        glClear(GL_COLOR_BUFFER_BIT);
        shader_program->render();
        main_ui->render();

        SDL_GL_SwapWindow(main_window->getWindow());
    };

    spdlog::info("Starting main loop...");
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(EmLoop, 0, true);
#else
    while(!quit){
        glLoop();
    }
#endif

    spdlog::info("Exiting main loop...");
    return 0;
}