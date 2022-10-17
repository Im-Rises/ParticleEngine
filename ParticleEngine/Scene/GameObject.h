#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <vector>
#include "../Shader/Shader.h"
#include "../Vector3d/Vector3d.h"

#include "Components/Mesh/Mesh.h"
#include "Components/Transform/Transform.h"

class Component;

class GameObject {
private:
    // Object name
    std::string name;

    // Base components
//    Transform transform;
    Mesh mesh;

    // Optional components
    std::vector<Component *> components;

    // OpenGL elements
    unsigned int VBO, VAO, EBO;
    Shader shader;


    // Engine other elements
    // std::vector<GameObject *> children;

public:

    Transform transform;

    GameObject(Mesh mesh);

private:
    void create();

public:
    ~GameObject();

private:
    void destroy();

public:
    void update(float deltaTime);

    void draw(int display_w, int display_h, glm::mat4 view, float fov);

    void addComponent(Component *component);

public:
    void drawTransformGui();

    void drawMeshGui();

public:
    std::string getName();

    std::vector<Component *> getComponents() const;
};


#endif //GAMEOBJECT_H