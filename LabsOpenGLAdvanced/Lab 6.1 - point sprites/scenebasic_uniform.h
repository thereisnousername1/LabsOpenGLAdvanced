#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"
#include <glad/glad.h>
#include "helper/glslprogram.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "helper/torus.h"
#include "helper/teapot.h"
#include "helper/plane.h"
#include "helper/objmesh.h"
#include "helper/cube.h"
#include "helper/sphere.h"

// SceneBasic_Uniform is a subclass of the Scene class
class SceneBasic_Uniform : public Scene
{
private:

    int viewportWidth, viewportHeight;

    // lab 6.1

    GLSLProgram prog;

    float angle;    // rotation logic related

    glm::mat4 mv;
    
    // std::unique_ptr<ObjMesh> spot;
    // std::unique_ptr<ObjMesh> mesh;  // disabled since lab 5.1, renamed to spot in video
    // Torus torus;    // disabled since lab 6.1
    // Sphere sphere;  // disabled since lab 5.5
    // Plane plane;    // disabled since lab 6.1
    // Teapot teapot;  // disabled since lab 6.1
    // Cube cube;

    // lab 6.1

    float rotSpeed; // rotation logic related

    GLuint sprites; // it represents vaoHandle(former fboHandle in many previous labs) in upcoming labs in lab 6
    int numSprites;
    float* locations;

    // lab 6.1

    float tPrev;    // rotation logic related

    void setMatrices();

    void compile();

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
