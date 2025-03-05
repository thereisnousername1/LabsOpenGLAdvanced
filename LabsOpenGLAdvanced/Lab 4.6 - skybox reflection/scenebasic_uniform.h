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

// lab 4 related

#include "helper/cube.h"

// lab 4.5 - skybox

#include "helper/skybox.h"

// lab 4.5 - skybox

// lab 4 related

// SceneBasic_Uniform is a subclass of the Scene class
class SceneBasic_Uniform : public Scene
{
private:

    int viewportWidth, viewportHeight;

    GLSLProgram prog;

    float angle;    // enabled in lab 3.4, works with tPrev

    glm::mat4 mv;

    // lab 4 related
    
    // Torus torus;
    // Plane plane;
    
    // lab 4.6 - skybox reflection

    // std::unique_ptr<ObjMesh> ogre; // disabled since lab 4.5

    // GLuint brick, moss;

    Teapot teapot;

    float rotSpeed;

    // Cube cube;

    SkyBox sky;

    // skybox separate shader
    GLSLProgram skyProg;

    // lab 4.6 - skybox reflection

    // lab 4 related

    float tPrev;    // spinning logic related

    // lab 4.6 - skybox reflection

    // void setMatrices();

    void setMatrices(GLSLProgram& p);

    // lab 4.6 - skybox reflection

    void compile();

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
