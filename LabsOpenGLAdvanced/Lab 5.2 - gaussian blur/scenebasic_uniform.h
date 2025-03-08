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

// SceneBasic_Uniform is a subclass of the Scene class
class SceneBasic_Uniform : public Scene
{
private:

    int viewportWidth, viewportHeight;

    GLSLProgram prog;

    float angle;    // enabled in lab 3.4, works with tPrev

    glm::mat4 mv;

    // lab 5.2
    
    // std::unique_ptr<ObjMesh> spot;
    // std::unique_ptr<ObjMesh> mesh;  // disabled since lab 5.1, renamed to spot in video

    Torus torus;    // enabled since lab 5.1
    Plane plane;    // enabled since lab 5.1
    Teapot teapot;  // enabled since lab 5.1

    // Cube cube;

    float rotSpeed;     // added with lab 4.7 viewer logic

    // renamed fboTex from renderTex to match the naming style since lab 5.1
    GLuint fsQuad, fboHandle, fboTex;           // basics requires for image processing (setupFBO())
    GLuint depthBuf;                            // depth logic related, also requires for edge detection
    GLuint intermediateTex, intermediateFBO;    // requires for gaussian blur

    //// Brief logic explanation ////
    // 
    // comparing Lab 4.7 and lab 4.8(And they are not related since lab 4.8 is taken from lab 4.1)
    // the GLuint started to bind to frame buffer instead of just declared to output texture
    // texture I says means those to be bind with actual image,
    // the Tex(s) stored as sampler2D in frag shader with layout (binding = ...)
    // I can't tell the meaning of binding at this moment (7/3/2025 - UK time 20:48)
    // 
    // BUT, Lab 4.8 is starting to modify with the sampler2D
    //
    // MOST IMPORTANTLY
    // Lab 4.8, is the beginning to show this transition, following Lab 5 solutions
    // are based on this transition, with even more GLuint
    // 
    //// Brief logic explanation ////

    float temp;         // declared as global for instant gauss calculate

    // lab 5.2

    float tPrev;    // spinning logic related

    void setMatrices();

    void compile();

    // lab 5.2
    
    void setupFBO();

    void byeFBO();

    // void renderToTexture();
    void pass1();   // renamed since lab 5.1

    // void renderScene();
    void pass2();   // renamed since lab 5.1

    void pass3();   // added in lab 5.2

    float gauss(float, float);  // added in lab 5.2

    // lab 5.2

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
