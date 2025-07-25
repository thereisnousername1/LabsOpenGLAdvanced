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

#include "helper/noisetex.h"  // lab 9.1

// SceneBasic_Uniform is a subclass of the Scene class
class SceneBasic_Uniform : public Scene
{
private:

    int viewportWidth, viewportHeight;

    float angle, rotSpeed, tPrev;    // spinning logic related
    
    glm::mat4 mv;

    // std::unique_ptr<ObjMesh> mesh;  // disabled since lab 9.1
    
    GLSLProgram prog; // restructure the header file for lab 9.1

    GLuint fsQuad;  // lab 9.1, yay we are setting fbo again(and again then again......)

    // Teapot teapot;   // disabled since lab 8.2
    // Plane plane;     // disabled since lab 9.1
    // Torus torus;     // disabled since lab 8.2
    
    glm::vec3 lightPos; // only in lab 8.4 it is a vec4 value

    // void setMatrices(GLSLProgram &); // lab 8.4
    void setMatrices(); // revert to default since lab 9.1
    
    void compile();

    // void drawScene(GLSLProgram&, bool);  // lab 8.4
    void drawScene();   // revert to default since lab 9.1

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
