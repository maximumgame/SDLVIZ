#include <ui/MainUI.hpp>

#include <imgui.h>
#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl3.h>


using namespace ui;

MainUI::MainUI(std::shared_ptr<Window> window, std::shared_ptr<gfx::ShaderProgram> shaderProgram)
{
    this->m_window = window;
    this->m_shaderProgram = shaderProgram;
    init();

    spdlog::info("MainUI initialized");

    m_ringbuffer = std::make_shared<spdlog::sinks::ringbuffer_sink_mt>(256);
    m_logger = std::make_shared<spdlog::logger>("MainUI", m_ringbuffer);
    m_logger->info("MainUI initialized");
}

MainUI::~MainUI()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    spdlog::info("MainUI destroyed");
}

void MainUI::processSDLEvent(SDL_Event& event)
{
    try
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
    }
    catch(std::exception e)
    {
        m_logger->error(e.what());
    }
}

void MainUI::init()
{
    //init imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(m_window->getWindow(), m_window->getContext());
    ImGui_ImplOpenGL3_Init("#version 300 es");
}

static int BufferResizeCallback(ImGuiInputTextCallbackData* data)
{
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        std::string* buf = (std::string*)data->UserData;
        buf->resize(data->BufTextLen + 10);
        return 0;
    }
    return 1;
}

bool MainUI::Update()
{
    //this only seems to be an issue under emscripten
    #ifdef __EMSCRIPTEN__
    if(ImGui::GetFrameCount() == 0)
    {
        spdlog::error("ImGui framerate is invalid {}", ImGui::GetIO().DeltaTime);
        //return false;
    }
    #endif

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(m_window->getWindow());
    ImGui::NewFrame();

    { //main frame
        ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
        ImGui::Begin("Main");
        ImGui::Text("Framerate %.1f FPS", ImGui::GetIO().Framerate);
        static std::string shaderText = {"#version 300 es\n"
                                         "precision mediump float;\n"
                                         "out vec4 fragColor;\n"
                                         "uniform float iTime;\n"
                                         "uniform vec3 iResolution;\n"
                                         "void main()\n"
                                         "{\n"
                                         "    vec2 uv = gl_FragCoord.xy/iResolution.xy;\n"
                                         "    vec3 color = 0.5 + 0.5 * cos(iTime + uv.xyx + vec3(0, 2, 4));\n"
                                         "    fragColor = vec4(color, 1);\n"
                                         "}\n"};
        ImGui::Separator();
        ImGui::BeginChild("##ShaderWindow", ImVec2(0, 490), ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysAutoResize);
        //resize our string as needed
        ImGui::InputTextMultiline("Shader", (char*)shaderText.c_str(), shaderText.size(), ImVec2(0, 470), ImGuiInputTextFlags_CallbackResize | ImGuiWindowFlags_AlwaysAutoResize, BufferResizeCallback, &shaderText);
        ImGui::SameLine();
        if(ImGui::Button("Compile"))
        {
            std::shared_ptr<GLchar[]> err = m_shaderProgram->LinkNewFragmentShader(shaderText);
            if(err)
                m_logger->error("Compile error: {}", err.get());
            else
                m_logger->info("Compile successful");
        }
        ImGui::EndChild();

        ImGui::Separator();
        ImGui::BeginChild("##scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

        std::vector<std::string> log_lines = m_ringbuffer->last_formatted();
        for(std::string& item : log_lines)
        {
            ImGui::TextWrapped("%s", item.c_str());
        }

        ImGui::PopStyleVar();
        ImGui::EndChild();


        ImGui::End();
    }
    return true;
}

void MainUI::render()
{
    bool result = this->Update();
    if(result)
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}