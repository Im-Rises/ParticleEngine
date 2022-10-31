#ifndef PARTICLE_ENGINE_H
#define PARTICLE_ENGINE_H

#define PROJECT_NAME "Particle Engine 3D"

#define VERSION_MAJOR "0"
#define VERSION_MINOR "4"
#define VERSION_PATCH "1"

#define PROJECT_VERSION VERSION_MAJOR "." VERSION_MINOR "." VERSION_PATCH

#define PROJECT_GITHUB "https://github.com/Im-Rises/ParticleEngine"

#define WINDOW_WIDTH 1580
#define WINDOW_HEIGHT 720

#define CONSOLE_BUFFER_SIZE 1024

#include <array>
#include <chrono>
#include "Game.h"
#include "Scene/Scene.h"

class InputManager;

class GameObject;

struct GLFWwindow;

class ParticleEngineLauncher {
    friend class InputManager;

private:
    // Window, Scene and Game objects
    GLFWwindow* window = nullptr;
    Scene* scene;
    Game game;

    // Window variables
    bool isFullScreen = false;
    std::array<float, 4> backgroundColor = {};
    int windowWidth = WINDOW_WIDTH, windowHeight = WINDOW_HEIGHT;

    // Selected GameObject in the scene
    GameObject* gameObject = nullptr;

    // Widgets terminal variables
    std::array<char, CONSOLE_BUFFER_SIZE> consoleBuffer = {};

public:
    ParticleEngineLauncher();

    ~ParticleEngineLauncher();

    void start();

private:
    void handleEvents();

    void handleGui();

    void updateGame(std::chrono::steady_clock::time_point& start);

    void updateScreen();

private:
    void clearScreen();

    void updateViewport(int width, int height);

    void toggleFullScreen();

    bool isMinimized() const;

private:
    void focusCameraOnGameObject();
};


#endif // PARTICLE_ENGINE_H
