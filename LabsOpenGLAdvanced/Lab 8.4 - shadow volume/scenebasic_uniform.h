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

#include "helper/frustum.h" // lab 8.1

#include "helper/random.h"  // lab 8.3

// SceneBasic_Uniform is a subclass of the Scene class
class SceneBasic_Uniform : public Scene
{
private:

    // lab 8.4

    int viewportWidth, viewportHeight;

    float angle, rotSpeed, tPrev;    // spinning logic related
    
    glm::mat4 mv;

    std::unique_ptr<ObjMesh> mesh;  // enabled since lab 8.2 for the building model, in lab 8.4 it's the cow
    
    // GLSLProgram prog, solidProg; // restructure the header file for lab 8.4
    GLSLProgram volumeProg, renderProg, compProg;    // lab 8.4

    // GLuint shadowFBO, pass1Index, pass2Index;    // restructure the header file for lab 8.4
    GLuint colorDepthFBO, fsQuad;

    GLuint spotTex, brickTex;
    
    // Teapot teapot;   // disabled since lab 8.2
    Plane plane;
    // Torus torus;     // disabled since lab 8.2
    
    glm::vec4 lightPos;

    void setMatrices(GLSLProgram &);    // lab 8.4
    
    void compile();

    void setupFBO();
    void drawScene(GLSLProgram&, bool);
    void pass1();
    void pass2();
    void pass3();
    void updateLight();

    // lab 8.4

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
