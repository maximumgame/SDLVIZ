#include <imgui.h>
#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl3.h>

#include <memory>
#include <Window.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/ringbuffer_sink.h>

#include <gfx/ShaderProgram.hpp>

namespace ui
{
    class MainUI
    {
    public:
        MainUI(std::shared_ptr<Window> window, std::shared_ptr<gfx::ShaderProgram> shaderProgram);
        ~MainUI();

        void update();
        void render();

    protected:
        void init();

    private:
        std::shared_ptr<Window> m_window;
        std::shared_ptr<spdlog::sinks::ringbuffer_sink_mt> m_ringbuffer;
        std::shared_ptr<spdlog::logger> m_logger;

        std::shared_ptr<gfx::ShaderProgram> m_shaderProgram;
    };
}