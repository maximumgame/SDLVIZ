#include <ui/MainUI.hpp>

#include <imgui.h>
#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl3.h>


using namespace ui;

const std::string singridshader = {
"#version 300 es\n"
"\n"
"precision mediump float;\n"
"out vec4 fragColor;\n"
"uniform float iTime;\n"
"uniform vec3 iResolution;\n"
"\n"
"float q(vec2 pos,float angle){\n"
"    return pos.x * cos(angle) + pos.y * sin(angle);\n"
"}\n"
"\n"
"float pi=atan(1.0,0.0)*2.0;\n"
"\n"
"float Hexagon(vec2 pos, float time){\n"
"    float c0 = 0.0;\n"
"    float c2 = 2.0;\n"
"    float c3 = 3.0;\n"
"    \n"
"    float c = cos(q(pos, pi / c3));\n"
"    c += cos(q(pos, c0));\n"
"    c += cos(q(pos, time + pi / c3));\n"
"    c += cos(q(pos, time + c0));\n"
"    c += cos(q(pos, pi / c3 * c2));\n"
"    c += cos(q(pos, time + pi / c3 * c2));\n"
"    \n"
"    \n"
"    float distanceToCenter = sqrt((pos.x * pos.x) + (pos.y * pos.y));\n"
"    return c;\n"
"}\n"
"\n"
"vec3 HexagonColor(float hex, vec2 uv, float time, float duv){\n"
"    float f = dot(uv, uv);\n"
"    float distanceToCenter = sqrt((uv.x * uv.x) + (uv.y * uv.y));\n"
"    vec3 col = 0.5 + 0.5 * cos(time + hex + uv.xyx + vec3(0, 2, 4));\n"
"    \n"
"    return col;\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"    vec2 UV = gl_FragCoord.xy/iResolution.xy-.5;\n"
"    float duv= dot(UV, UV);\n"
"    \n"
"    // Normalized pixel coordinates (from 0 to 1)\n"
"    vec2 uv = gl_FragCoord.xy / iResolution.xy;\n"
"    vec2 pos = (uv - vec2(0.5)) * iResolution.xy / iResolution.y * 200.0;\n"
"    \n"
"    float time = iTime / 15.0;\n"
"    \n"
"    float angle = atan(pos.y, pos.x) + time;\n"
"    pos = length(pos) * vec2(cos(angle), sin(angle));\n"
"    \n"
"    //time = 0.0;\n"
"    float hex = Hexagon(pos, time);\n"
"    \n"
"    // Time varying pixel color\n"
"    vec3 col = HexagonColor(hex, uv, time, duv);\n"
"\n"
"    // Output to screen\n"
"    fragColor = vec4(col * (-hex), 1.0);\n"
"}\n"
};

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

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

#ifndef __EMSCRIPTEN__
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    io.SetClipboardTextFn = [](void* user_data, const char* text)
    {
        SDL_SetClipboardText(text);
    };

    io.GetClipboardTextFn = [](void* user_data)
    {
        const char* text = SDL_GetClipboardText();
        return text;
    };
#endif

    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(m_window->getWindow(), m_window->getContext());
    ImGui_ImplOpenGL3_Init("#version 300 es");
}

static int BufferResizeCallback(ImGuiInputTextCallbackData* data)
{
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        std::string* buf = (std::string*)data->UserData;
        buf->resize(data->BufTextLen * 2);
        data->BufTextLen = buf->capacity();
        data->BufDirty = true;
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
        spdlog::error("ImGui framerate is invalid {}", ImGui::GetIO().Framerate);
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
        ImGui::InputTextMultiline("Shader", (char*)shaderText.c_str(), shaderText.size(), ImVec2(0, 470), ImGuiInputTextFlags_CallbackResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_AlwaysUseWindowPadding, BufferResizeCallback, &shaderText);
        ImGui::SameLine();
        if(ImGui::Button("Compile"))
        {
            std::shared_ptr<GLchar[]> err = m_shaderProgram->LinkNewFragmentShader(shaderText);
            if(err)
                m_logger->error("Compile error: {}", err.get());
            else
                m_logger->info("Compile successful");
        }
        ImGui::SameLine();
        if(ImGui::Button("Load Sin-Grid shader"))
        {
            shaderText.resize(singridshader.length() + 1);
            singridshader.copy(&shaderText[0], singridshader.length());

            m_shaderProgram->LinkNewFragmentShader(shaderText);
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

        auto& io = ImGui::GetIO();
        if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            SDL_GL_MakeCurrent(m_window->getWindow(), m_window->getContext());
        }
    }
}