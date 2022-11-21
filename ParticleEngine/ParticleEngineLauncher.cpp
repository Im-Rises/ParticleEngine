#include "ParticleEngineLauncher.h"

#pragma region Includes

// OpenGL Loader
#include <glad/glad.h>

// Includes
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "InputManager.h"
#include "Utility/RollingBuffer.h"
#include "Scene/Components/PhysicalComponent/Particle/Particle.h"
#include "Scene/Components/Component.h"

// Dear ImGui
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/implot.h"
#include "Utility/imGuiUtility.h"

#include <cstdio>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif

#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include <cstdlib>

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#pragma endregion

#pragma region Callback functions

static void glfw_error_callback(int error, const char *description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

#pragma endregion

#pragma region Constructor and Destructor

ParticleEngineLauncher::ParticleEngineLauncher() {
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        exit(1);

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    window = glfwCreateWindow(windowWidth, windowHeight, PROJECT_NAME, nullptr, nullptr);

    // Check if window was created
    if (window == nullptr)
        exit(1);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize GLFW callbacks
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, InputManager::key_callback);
    glfwSetScrollCallback(window, InputManager::scroll_callback);
    glfwSetCursorPosCallback(window, InputManager::cursor_position_callback);
    glfwSetMouseButtonCallback(window, InputManager::mouse_button_callback);

    // Center window
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    auto xPos = mode->width / 2 - windowWidth / 2;
    auto yPos = mode->height / 2 - windowHeight / 2;
    glfwSetWindowPos(window, xPos, yPos);

    // Initialize OpenGL loader
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        exit(1);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImPlot::CreateContext();

    // Setup background color
    backgroundColor[0] = 0.45f;
    backgroundColor[1] = 0.55f;
    backgroundColor[2] = 0.60f;
    backgroundColor[3] = 1.00f;

    // Setup scene
    //    scene = std::make_unique<Scene>(windowWidth, windowHeight);
    scene = new Scene(windowWidth, windowHeight);

    // Bind default frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ParticleEngineLauncher::~ParticleEngineLauncher() {
    delete scene;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    ImPlot::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

#pragma endregion

#pragma region Game Loop methods

void ParticleEngineLauncher::start() {
    auto start = std::chrono::steady_clock::now();

    // Create game (generate game objects into the scene)
    //    game.start(scene.get());
    game.start(scene);

    // Game loop
    while (!glfwWindowShouldClose(window)) {
        // Inputs
        handleEvents();
        handleGui();

        // Update game mechanics
        updateGame(start);

        // Refresh screen
        updateScreen();
    }
}

void ParticleEngineLauncher::handleEvents() {
    glfwPollEvents();
}

void ParticleEngineLauncher::handleGui() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    if (!isFullScreen) {
        /*------------------ImGui windows------------------*/
        {
            static bool my_tool_active = true;
            static bool showAboutPopup = false;
            if (ImGui::BeginMainMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */
                    }
                    if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */
                    }
                    if (ImGui::MenuItem("Close", "Ctrl+W")) { my_tool_active = false; }
                    if (ImGui::MenuItem("Exit", "Alt+F4")) { glfwSetWindowShouldClose(window, true); }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("View")) {
                    if (ImGui::MenuItem("FullScreen", "F11")) { toggleFullScreen(); }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Help")) {
                    if (ImGui::MenuItem("About ParticleEngineLauncher...")) {
                        showAboutPopup = true;
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }
            if (showAboutPopup)
                ImGui::OpenPopup("About ParticleEngineLauncher##AboutPopup");
            if (ImGui::BeginPopupModal("About ParticleEngineLauncher##AboutPopup", &showAboutPopup,
                                       ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize |
                                       ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse)) {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), PROJECT_NAME);
                ImGui::Text("Version: %s", PROJECT_VERSION);
                ImGui::NewLine();
                ImGui::Text("Developed by: ");
                ImGui::Text("  - Quentin MOREL");
                ImGui::Text("  - Clémence CLAVEL");
                ImGui::Text("  - Gabriel REBOUL");
                ImGui::NewLine();
                ImGui::Text("Github: %s", PROJECT_GITHUB);
                ImGui::NewLine();
                if (ImGui::Button("Close")) {
                    ImGui::CloseCurrentPopup();
                    showAboutPopup = false;
                }
                ImGui::EndPopup();
            }
        }
        {
            ImGui::Begin("Window info");
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate);
            ImGui::Text("Window width: %d", windowWidth);
            ImGui::Text("Window height: %d", windowHeight);
            ImGui::End();
        }
        {
            ImGui::Begin("Hierarchy");
            for (int i = 0; i < scene->getGameObjects().size(); i++) {
                ImGui::Selectable((scene->getPtrGameObjectByIndex(i)->getName()).c_str(),
                                  gameObject == scene->getPtrGameObjectByIndex(i));
                if (ImGui::IsItemClicked()) {
                    this->gameObject = scene->getPtrGameObjectByIndex(i);
                }
            }
            ImGui::End();
        }
        {
            ImGui::Begin("View tools");
            ImGui::Text("Background color");
            ImGui::ColorPicker4("Background color", backgroundColor.data());
            ImGui::Checkbox("Mesh: Fill/Line", scene->getPtrWireFrameState());
            ImGui::Checkbox("Show axis", scene->getPtrShowAxis());
            ImGui::End();
        }
        {
//            ImGui::Begin("Console");
//            ImGuiInputTextFlags inputTextFlags =
//                    ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CtrlEnterForNewLine;
//            ImGui::Text("Input:");
//            if (ImGui::InputTextMultiline("##console", consoleBuffer.data(), consoleBuffer.size(),
//                                          ImVec2(-1.0f, ImGui::GetTextLineHeight() * 11),
//                                          inputTextFlags)) {
//                system(consoleBuffer.data());
//                consoleBuffer.fill('\0');
//            }
//            if (ImGui::Button("Enter")) {
//                system(consoleBuffer.data());
//                consoleBuffer.fill('\0');
//            }
//            ImGui::SameLine();
//            if (ImGui::Button("Clear")) {
//                consoleBuffer.fill('\0');
//            }
//            ImGui::End();
        }
        {
            ImGui::Begin("Speed graph viewer");
            {
                static RollingBuffer rdata1, rdata2, rdata3;
                static float t = 0;
                t += ImGui::GetIO().DeltaTime;

                Particle *particle = nullptr;
                if (gameObject != nullptr) {
                    particle = dynamic_cast<Particle *>(gameObject->getComponentByName("Particle"));
                }
                if (particle != nullptr) {
                    Vector3d speed = particle->getLinearSpeed();
                    rdata1.AddPoint(t, speed.x);
                    rdata2.AddPoint(t, speed.y);
                    rdata3.AddPoint(t, speed.z);
                } else {
                    rdata1.AddPoint(t, 0);
                    rdata2.AddPoint(t, 0);
                    rdata3.AddPoint(t, 0);
                }

                static float history = 10.0f;
                static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;

                if (ImPlot::BeginPlot("GameObject Speed##Rolling", ImVec2(-1, 150))) {
                    ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
                    ImPlot::SetupAxisLimits(ImAxis_X1, 0, history, ImGuiCond_Always);
                    ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1);
                    ImPlot::PlotLine("Speed X##ImPlotSpeedX", &rdata1.Data[0].x, &rdata1.Data[0].y, rdata1.Data.size(),
                                     0, 0,
                                     2 * sizeof(float));
                    ImPlot::PlotLine("Speed Y##ImPlotSpeedY", &rdata2.Data[0].x, &rdata2.Data[0].y, rdata2.Data.size(),
                                     0, 0,
                                     2 * sizeof(float));
                    ImPlot::PlotLine("Speed Z##ImPlotSpeedZ", &rdata3.Data[0].x, &rdata3.Data[0].y, rdata3.Data.size(),
                                     0, 0,
                                     2 * sizeof(float));
                    ImPlot::EndPlot();
                }
                ImGui::SliderFloat("History", &history, 1, 30, "%.1f s");
                rdata1.Span = history;
            }
            ImGui::End();
        }
        {
            ImGui::Begin("Inspector");
            if (gameObject != nullptr) {
                ImGui::Text("Name: %s", gameObject->getName().c_str());
                if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                    gameObject->drawTransformGui();
                }
                if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen)) {
                    gameObject->drawMeshGui();
                }
                for (Component *component: gameObject->getComponents()) {
                    if (ImGui::CollapsingHeader(component->getName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                        component->drawGui();
                    }
                }
                ImGui::NewLine();
                if (ImGuiUtility::ButtonCenteredOnLine("Add component", 0.5f)) {
                    ImGui::OpenPopup("Add component##ComponentPopup");
                }
                if (ImGui::BeginPopup("Add component##ComponentPopup")) {
                    for (auto &componentName: Component::componentsNamesList) {
                        if (ImGui::MenuItem(componentName)) {
                            gameObject->addComponentByName(componentName);
                        }
                    }
                    ImGui::EndPopup();
                }
            }
            ImGui::End();
        }
        {
//            ImGui::Begin("Project files");
//
//            ImGui::End();
        }
        {
//            ImGui::Begin("Game settings");
//            ImGui::Text("Particle speed:");
//            ImGui::InputFloat("##PhysicalEngineParticleSpeed", game.getPtrSpeed(), 0.1f, 1.0f, "%.1f");
//            ImGui::End();
        }
        {
            ImGui::Begin("Scene View", nullptr, ImGuiWindowFlags_NoCollapse);
            {
                ImGui::BeginChild("GameRender");
                ImVec2 windowSize = ImGui::GetWindowSize();
                ImGui::Image((ImTextureID) scene->getFrameBufferId(), windowSize, ImVec2(0, 1), ImVec2(1, 0));
                ImGui::EndChild();
            }
            ImGui::End();
        }
    }
    ImGui::Render();
}

void ParticleEngineLauncher::updateGame(std::chrono::steady_clock::time_point &start) {
    auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start)
            .count();
    start = std::chrono::steady_clock::now();
    scene->update((float) deltaTime / 1000.0f);
    //    scene->update(1000.0f / ImGui::GetIO().Framerate);
}

void ParticleEngineLauncher::updateScreen() {
    // Update viewport
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    updateViewport(display_w, display_h);

    // If window is minimized, don't draw anything (to avoid draw in a 0x0 window)
    if (!isMinimized()) {

        // Clear Main Screen
        clearScreen();

        // If not in fullscreen, clear the opengl window
        if (!isFullScreen) {
            glBindFramebuffer(GL_FRAMEBUFFER, scene->getFrameBufferId());
            clearScreen();
        }

        // Draw game
        scene->draw(display_w, display_h);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // Swap buffers
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}

void ParticleEngineLauncher::clearScreen() {
    glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
    glClear(GL_COLOR_BUFFER_BIT);
}

void ParticleEngineLauncher::updateViewport(int width, int height) {
    glViewport(0, 0, width, height);
    scene->updateViewport(width, height);
    if (!isFullScreen) {
        windowWidth = width;
        windowHeight = height;
    }
}


void ParticleEngineLauncher::toggleFullScreen() {
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    if (isFullScreen) {
        auto xPos = mode->width / 2 - windowWidth / 2;
        auto yPos = mode->height / 2 - windowHeight / 2;
        glfwSetWindowMonitor(window, nullptr, xPos, yPos, windowWidth, windowHeight, 0);
        isFullScreen = false;
    } else {
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        isFullScreen = true;
    }
}

bool ParticleEngineLauncher::isMinimized() const {
    return (windowWidth == 0 && windowHeight == 0);
}

void ParticleEngineLauncher::focusCameraOnGameObject() {
    if (gameObject != nullptr)
        scene->setCameraPosition(gameObject->transform.getPosition());
}

#pragma endregion
