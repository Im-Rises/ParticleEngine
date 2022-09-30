#ifndef PHYSICALENGINE_H
#define PHYSICALENGINE_H

#define PROJECT_NAME "Physical Engine 3D"

#include <memory>
#include <chrono>
#include "Game.h"

//Shallow declarations
class Scene;

class InputManager;

class GameObject;

struct GLFWwindow;

//Class declaration
class PhysicalEngine {
    friend class InputManager;

private:
    // Window, Scene and Game objects
    GLFWwindow *window;
    Scene *scene;
    Game m_game;

    // Window variables
    bool isFullScreen = false;
    int windowWidth = 1580, windowHeight = 720;
    struct {
        float r, g, b, a;
    } backgroundColor;

    // Variables for the game loop
    float gameSpeed = 0.5f;
    const int PHYSICAL_UPDATE_PER_SECOND = 50;
    GameObject *gameObject = nullptr;

public:
    PhysicalEngine();

    ~PhysicalEngine();

    void start();

private:
    void handleEvents();

    void handleGui();

    void updateGame(std::chrono::steady_clock::time_point &start);

    void updateScreen();

private:
    void clearScreen();

    void updateViewport(int width, int height);

    void toogleFullScreen();
};


#endif //PHYSICALENGINE_H
