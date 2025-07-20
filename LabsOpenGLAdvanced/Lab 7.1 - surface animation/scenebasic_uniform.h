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
#include "helper/teapotpatch.h"

// SceneBasic_Uniform is a subclass of the Scene class
class SceneBasic_Uniform : public Scene
{
private:

    int viewportWidth, viewportHeight;

    // lab 7.1

    GLSLProgram prog;

    // GLSLProgram solidProg;   // disabled in lab 6.5

    float angle;    // rotation logic related

    glm::mat4 mv;
    
    // std::unique_ptr<ObjMesh> spot;
    // std::unique_ptr<ObjMesh> mesh;  // was called ogre in lab 6.2
    // Torus torus;    // disabled since lab 6.1
    // Sphere sphere;  // disabled since lab 5.5
    Plane plane;    // lab 7.1

    // teapotpatch.h and .cpp are added to the helper filter
    // TeapotPatch teapot;  // disabled in lab 7.1
    // Cube cube;

    // lab 7.1

    float time;

    float rotSpeed; // rotation logic related

    // glm::mat4 viewport;  // disabled in lab 7.1
    //GLuint sprites; // it represents vaoHandle(former fboHandle in many previous labs) in upcoming labs in lab 6
    // GLuint vaoHandle;   // disabled in lab 7.1
    //int numSprites;
    //float* locations;

    // lab 7.1

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
