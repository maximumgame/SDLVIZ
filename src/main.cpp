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


bool initGl()
{
    //if we are emscripten, rely on em's gl defs
#ifdef __EMSCRIPTEN__
    EmscriptenWebGLContextAttributes attr;
    emscripten_webgl_init_context_attributes(&attr);
    attr.majorVersion = 2;
    attr.minorVersion = 0;
    auto context = emscripten_webgl_create_context("canvas", &attr);
    emscripten_webgl_make_context_current(context);
    spdlog::info("Created emscripten webgl context");
#else
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
        spdlog::error("Failed to initialize GL");
        return -1;
    }
    spdlog::info("OpenGL version: {}", glGetString(GL_VERSION));
    spdlog::info("GLSL version: {}", glGetString(GL_SHADING_LANGUAGE_VERSION));
    spdlog::info("Vendor: {}", glGetString(GL_VENDOR));
    spdlog::info("Renderer: {}", glGetString(GL_RENDERER));

    std::shared_ptr<gfx::ShaderProgram> shader_program(new gfx::ShaderProgram(main_window));
    spdlog::info("Shader program created");

    SDL_GL_MakeCurrent(main_window->getWindow(), main_window->getContext());

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