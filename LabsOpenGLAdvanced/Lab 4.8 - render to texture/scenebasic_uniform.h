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

// lab 4 related

// SceneBasic_Uniform is a subclass of the Scene class
class SceneBasic_Uniform : public Scene
{
private:

    int viewportWidth, viewportHeight;

    GLSLProgram prog;

    float angle;    // enabled in lab 3.4, works with tPrev

    glm::mat4 mv;

    // lab 4.8
    
    // std::unique_ptr<ObjMesh> spot;
    std::unique_ptr<ObjMesh> mesh;  // enabled since lab 4.8, renamed to spot in video

    // Torus torus;
    // Plane plane;
    // Teapot teapot;

    Cube cube;

    float rotSpeed;     // added with lab 4.7 viewer logic

    GLuint fboHandle;   // added since lab 4.8

    // lab 4.8

    float tPrev;    // spinning logic related

    void setMatrices();

    void compile();

    // lab 4.8
    
    void setupFBO();
    void renderToTexture();
    void renderScene();

    // lab 4.8

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
