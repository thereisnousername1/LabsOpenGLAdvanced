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

    // lab 5.5

    GLSLProgram prog;

    float angle;    // rotate again since lab 5.4

    glm::mat4 mv;
    
    // std::unique_ptr<ObjMesh> spot;
    // std::unique_ptr<ObjMesh> mesh;  // disabled since lab 5.1, renamed to spot in video

    Torus torus;    // lab 5.5
    // Sphere sphere;  // disabled since lab 5.5

    Plane plane;    // enabled since lab 5.1
    Teapot teapot;  

    // Cube cube;

    float rotSpeed; // rotate again since lab 5.4

    /// from lab 5.5 deferred shading the image processing things is kind of completed
    /// starting from this lab the GLuint will be modified to match the name as shown in the video
    /// next lab and so on the comments will be removed for better readability
    GLuint deferredFBO, quad;
    
    /*
    // renamed to match the naming style from prvious labs
    GLuint fsQuad, fboHandle, fboTex;           // basics requires for image processing (setupFBO())
    */

    GLuint depthBuf;                            // depth logic related, also requires for edge detection
    
    /*
    // GLuint intermediateTex, intermediateFBO; // requires for gaussian blur (from lab 5.2), modified as next line
    GLuint intermediateTex1, intermediateTex2, intermediateFBO; // they are Tex1, Tex2, blurFbo in lab sheet 5 p.36
    
    GLuint linearSampler, nearestSampler;       // requires for bloom effect with gamma correction
    int bloomBufWidth, bloomBufHeight;          // requires for bloom effect with gamma correction

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
    */

    // gaussian calculating logic and related variables are removed

    float tPrev;    // rotate again since lab 5.4

    void setMatrices();

    void compile();
    
    void setupFBO();

    void byeFBO();

    // 2 pass for lab 5.5 only
    
    // void renderToTexture();
    void pass1();   // renamed since lab 5.1

    // void renderScene();
    void pass2();   // renamed since lab 5.1

    // also no need to use gauss() and computeLogAveLuminance()

    void drawScene();

    // creategeometry buffer (labsheet 5 p.44)
    void createGBufTex(GLenum, GLenum, GLuint&);
    // lab 5.5

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
