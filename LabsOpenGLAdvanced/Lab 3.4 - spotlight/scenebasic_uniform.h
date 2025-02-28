#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"

// lab 1

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// lab 1

// lab 2

#include "helper/torus.h"

// lab 2

// lab 3.4

#include "helper/teapot.h"

// lab 3.4

// lab 3.3

#include "helper/plane.h"
#include "helper/objmesh.h"

// lab 3.3

// SceneBasic_Uniform is a subclass of the Scene class
class SceneBasic_Uniform : public Scene
{
private:

    int viewportWidth, viewportHeight;
    // lab 2
    
    // GLuint vaoHandle;
    
    // lab 2

    GLSLProgram prog;

    float angle;    // enabled in lab 3.4, works with tPrev

    // glm::mat4 rotationMatrix;    // useless if rotation is not needed

    glm::mat4 mv;

    // lab 2

    // lab 3.3
    
    Torus torus; // enabled since lab 3.4
    Plane plane;
    // std::unique_ptr<ObjMesh> mesh;  // disabled since lab 3.4

    // lab 3.3

    // lab 3.4
    
    Teapot teapot;

    float tPrev;    // spinning logic related

    // lab 3.4

    void setMatrices();

    // lab 2

    void compile();

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
