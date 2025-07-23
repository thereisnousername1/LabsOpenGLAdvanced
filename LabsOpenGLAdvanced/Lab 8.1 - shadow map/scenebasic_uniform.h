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

// SceneBasic_Uniform is a subclass of the Scene class
class SceneBasic_Uniform : public Scene
{
private:

    int viewportWidth, viewportHeight;

    float angle;
    
    glm::mat4 mv;

    float rotSpeed;
        
    // std::unique_ptr<ObjMesh> mesh;
    
    GLSLProgram prog, solidProg;

    GLuint shadowFBO, pass1Index, pass2Index;
    
    Teapot teapot;
    Plane plane;
    Torus torus;

    int shadowMapWidth, shadowMapHeight;
    float tPrev;    // spinning logic related
    
    glm::mat4 lightPV, shadowBias;
    
    Frustum lightFrustum;

    void setMatrices();
    
    void compile();

    void setupFBO();
    void drawScene();
    void spitOutDepthBuffer();

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
