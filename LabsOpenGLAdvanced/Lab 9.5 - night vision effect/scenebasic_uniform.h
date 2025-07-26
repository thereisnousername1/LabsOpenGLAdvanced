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

#include "helper/noisetex.h" // lab 9.3


// SceneBasic_Uniform is a subclass of the Scene class
class SceneBasic_Uniform : public Scene
{
private:

    int viewportWidth, viewportHeight;

    GLSLProgram prog;

    float angle;

    float rotSpeed, tPrev;    // spinning logic related
    
    glm::mat4 mv;
    
    // std::unique_ptr<ObjMesh> mesh;
    
    Torus torus;
    Plane plane;

    // GLuint brick, moss;

    Teapot teapot;

    // lab 9.5

    GLuint fsQuad, pass1Index, pass2Index;
    GLuint renderFBO;   // former lab 5.1 fboHandle
    GLuint renderTex;   // former lab 5.1 fboTex
    GLuint noiseTex;

    // lab 9.5

    // Cube cube;
    
    // glm::vec4 lightPos;  // disabled since lab 9.5

    void setMatrices();

    void compile();

    // lab 9.5
    
    void setupFBO();

    void pass1();
    
    void pass2();

    // lab 9.5

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
