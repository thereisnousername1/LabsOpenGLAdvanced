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

    float temp;         // declared as global for instant gauss calculate

    // GLuint fboHandle;   // replaced since lab 5.1
    // rename Tex1 from RenderTex since lab 5.1
    //                   depthBuf declared as global var -> resizing logic(different from lab 5.1)
    GLuint fsQuad, Tex1, depthBuf;                      // requires for image processing
    // GLuint fboHandle;                                // requires for edge detection
    GLuint intermediateTex, renderFBO, intermediateFBO; // requires for gaussian blur

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
