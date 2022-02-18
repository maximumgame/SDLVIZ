
#include <memory>
#include <Window.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/ringbuffer_sink.h>

#include <gfx/ShaderProgram.hpp>
#include <SDL2/SDL.h>

namespace ui
{
    class MainUI
    {
    public:
        MainUI(std::shared_ptr<Window> window, std::shared_ptr<gfx::ShaderProgram> shaderProgram);
        ~MainUI();

        void processSDLEvent(SDL_Event& event);
        void render();

    protected:
        bool Update();
        void init();

    private:
        std::shared_ptr<Window> m_window;
        std::shared_ptr<spdlog::sinks::ringbuffer_sink_mt> m_ringbuffer;
        std::shared_ptr<spdlog::logger> m_logger;

        std::shared_ptr<gfx::ShaderProgram> m_shaderProgram;
    };
}