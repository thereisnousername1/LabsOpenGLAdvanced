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

    float angle, rotSpeed, tPrev;    // spinning logic related
    
    GLSLProgram prog;
    
    glm::mat4 mv;
    
    Plane plane;
    std::unique_ptr<ObjMesh> mesh;
    Teapot teapot;
    
    // Torus torus;

    // GLuint brick, moss;

    // lab 10

    float lightAngle, lightRotationSpeed;

    glm::vec4 lightPos;

    // lab 10

    // Cube cube;

    void setMatrices();

    void compile();

    // lab 10

    void drawScene();

    void drawFloor();

    void drawSpot(const glm::vec3& pos, float rough, int metal, const glm::vec3& color);

    // lab 10

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
