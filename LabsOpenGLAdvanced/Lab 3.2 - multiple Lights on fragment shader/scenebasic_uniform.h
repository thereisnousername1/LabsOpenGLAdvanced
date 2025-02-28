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

// SceneBasic_Uniform is a subclass of the Scene class
class SceneBasic_Uniform : public Scene
{
private:

    // lab 2
    
    // GLuint vaoHandle;
    
    // lab 2

    GLSLProgram prog;

    // lab 2

    // float angle;

    // lab 2

    glm::mat4 rotationMatrix;

    glm::mat4 mv;

    // lab 2

    Torus torus;

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
