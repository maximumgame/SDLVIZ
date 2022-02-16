#define SDL_MAIN_HANDLED

#include <spdlog/spdlog.h>

#include "Window.hpp"
#include <gfx/ShaderProgram.hpp>

#include <ui/MainUI.hpp>

#include <glad/glad.h>

#include <SDL2/SDL_opengl.h>
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif


bool initGl()
{
    //if we are emscripten, rely on em's gl loader
#ifndef __EMSCRIPTEN__
    int error = gladLoadGL();
    spdlog::info("GLAD init: {}", error);
#endif

    //vsync
    if(SDL_GL_SetSwapInterval(1) < 0)
    {
        spdlog::error("Error setting vsync: ", SDL_GetError());
    }

    return true;
}

void glSetup()
{
    //enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    //enable backface culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
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
    
    auto result = initGl();
    if(!result)
    {
        spdlog::error("Failed to initialize GLAD");
        return -1;
    }

    std::shared_ptr<gfx::ShaderProgram> shader_program(new gfx::ShaderProgram(main_window));

    spdlog::info("Shader program created");

    SDL_GL_MakeCurrent(main_window->getWindow(), main_window->getContext());
    spdlog::info("SDL_GL_MakeCurrent");


    spdlog::info("OpenGL version: {}", glGetString(GL_VERSION));
    spdlog::info("GLSL version: {}", glGetString(GL_SHADING_LANGUAGE_VERSION));
    spdlog::info("Vendor: {}", glGetString(GL_VENDOR));
    spdlog::info("Renderer: {}", glGetString(GL_RENDERER));
    

    spdlog::info("Starting main loop...");

    std::unique_ptr<ui::MainUI> main_ui(new ui::MainUI(main_window, shader_program));

    glLoop = [&]
    {
        //main loop
        SDL_Event event;
        while(true)
        {
            //handle events
            while(SDL_PollEvent(&event))
            {
                //ImGui_ImplSDL3_ProcessEvent(&event);
                main_ui->processSDLEvent(event);
                if(event.type == SDL_QUIT)
                {
                    quit = true;
                    return 0;
                }
            }
            //setup viewport
            SDL_GetWindowSize(main_window->getWindow(), &width, &height);

            glViewport(0, 0, width, height);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

            //render
            glClear(GL_COLOR_BUFFER_BIT);
            shader_program->render();
            main_ui->update();
            main_ui->render();

            SDL_GL_SwapWindow(main_window->getWindow());
        }
    };

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