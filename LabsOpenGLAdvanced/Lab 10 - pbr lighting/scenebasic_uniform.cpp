#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>

#include <string>
using std::string;

#include <sstream>
#include <iostream>
using std::cerr;
using std::endl;

#include "helper/glutils.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

#include "helper/texture.h" // texture loading related

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

////
// Lab 10 intends to introduce different looking textures by super complicated mathematics magic in fragment shader
////

// initialization of mainly 3D models in a scene happen in here, but well I don't care
SceneBasic_Uniform::SceneBasic_Uniform() :

    tPrev(0),
    angle(0.0f),
    rotSpeed(glm::pi<float>() / 2.0f),
    teapot(5, glm::mat4(1.0f)),         // lab 10 setting
    plane(20, 20, 1, 1),                // lab 10 setting
    lightPos(5.0f, 5.0f, 5.0f, 1.0f)    // lab 10 setting
{
    //                       relative file location in my computer            , bool center (according to the IDE)
    // mesh = ObjMesh::load("../Lab 3.3 - pig mesh/media/pig_triangulated.obj", true);
    mesh = ObjMesh::load("media/spot/spot_triangulated.obj");
}

// init(), initialization of everything in a scene happen in here
// Light intensity setting to be placed in here (Well actually that does not matter at all, so as all setting to be import to shader)
void SceneBasic_Uniform::initScene()
{
    compile();

    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    // model, view, projection matrix initialization and setup (for 3D scene)
    
    model = mat4(1.0f);

    view = mat4(1.0f);

    //                                                  declared in scene.h
    // projection = glm::perspective(glm::radians(60.0f), (float)width / height, 0.3f, 100.0f);
    projection = glm::perspective(glm::radians(50.0f), (float)width / height, 0.5f, 100.0f); // lab 10 setting
    
    // init float angle
    // angle = 0.0f;
    angle = glm::pi<float>() / 4.0f;

    #pragma region (Light related) Light (diffuse, ambient, specular) intensity setting
    
    /*prog.setUniform("Light.Ld", vec3(1.0f));
    prog.setUniform("Light.La", vec3(0.05f));
    prog.setUniform("Light.Ls", vec3(1.0f));*/

    lightAngle = 0.0f;
    lightRotationSpeed = 1.5f;

    #pragma endregion
}

void SceneBasic_Uniform::compile()
{
    // compile mr prog with the desired shaders to be used in this scene
	try {
		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();
		prog.use();
	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

#pragma region Program mainloop() functions, where everything that has to be updated take place according to program lifespan

// update should be a while(true) loop as always, or in this case the loop is defined in the scenerunner class function mainloop
// it seems float t can be used as a time factor, restriction to the program lifespan
void SceneBasic_Uniform::update( float t )
{

    #pragma region Spinning logic
    
    float deltaT = t - tPrev;

    if (tPrev == 0.0f)
        deltaT = 0.0f;
    tPrev = t;

    if (m_animate)
    {
        angle += 0.1f * deltaT;  // the logic is piece of shit in the video, rearranged for better spinning logic

        // it is just modifing the variable angle more rapidly
        // angle += rotSpeed * deltaT;

        if (angle > glm::two_pi<float>())
            angle -= glm::two_pi<float>();

        lightAngle = glm::mod(lightAngle + deltaT * lightRotationSpeed, glm::two_pi<float>());
        lightPos.x = glm::cos(lightAngle) * 7.0f;
        lightPos.y = 3.0f;
        lightPos.z = glm::sin(lightAngle) * 7.0f;
    }

    #pragma endregion

}

// this function stored model / mesh / object declared and awaits to be rendered in scene
void SceneBasic_Uniform::render()
{
    // resizing will work this time, but still not ideal
    // it regen the FBO when its necessary to
    // 
    // this is viewport resizing logic (I figure it out by myself)
    // now the content will stick to the window size when resizing happened
    glfwGetFramebufferSize(glfwGetCurrentContext(), &viewportWidth, &viewportHeight);
    if (viewportWidth != width || viewportHeight != height)
    {
        resize(viewportWidth, viewportHeight);
    }

    // clear color buffer and clear color & depth buffers
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    prog.setUniform("lights[0].Ld", vec3(45.0f));
    prog.setUniform("lights[0].La", vec3(45.0f));
    prog.setUniform("lights[0].Ls", vec3(45.0f));
    prog.setUniform("lights[0].L", vec3(45.0f));
    prog.setUniform("lights[0].Position", view * lightPos);

    prog.setUniform("lights[1].Ld", vec3(0.3f));
    prog.setUniform("lights[1].La", vec3(0.3f));
    prog.setUniform("lights[1].Ls", vec3(0.3f));
    prog.setUniform("lights[1].L", vec3(0.3f));
    prog.setUniform("lights[1].Position", glm::vec4(0, 0.15f, -1.0f, 0));

    prog.setUniform("lights[2].Ld", vec3(45.0f));
    prog.setUniform("lights[2].La", vec3(45.0f));
    prog.setUniform("lights[2].Ls", vec3(45.0f));
    prog.setUniform("lights[2].L", vec3(45.0f));
    prog.setUniform("lights[2].Position", view * glm::vec4(-7, 3, 7, 1));

    vec3 focus = vec3(7.0f * cos(angle), 4.0f, 7.0f * sin(angle));

    // view = glm::lookAt(vec3(0.0f, 4.0f, 7.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));   // static camera position
    view = glm::lookAt(focus, vec3(0.0f, -0.1f, 0.0f), vec3(0.0f, 1.0f, 0.0f));  // camera x is starring at a point in the void, focus

    drawScene();
}

#pragma endregion

void SceneBasic_Uniform::drawScene()
{
    drawFloor();

    // Draw dielectric cows with varying roughness
    int numCows = 9;                               // use hot reload to see any difference!
    glm::vec3 cowBaseColor(0.1f, 0.33f, 0.97f);
    for (int i = 0; i < numCows; i++)
    {
        float cowX = i * (10.0f / (numCows - 1)) - 5.0f;
        float rough = (i + 1) * (1.0f / numCows);
        drawSpot(glm::vec3(cowX, 0, 0), rough, 0, cowBaseColor);
    }

    // Draw metal cows
    float metalRough = 0.43f;

    // Gold
    drawSpot(glm::vec3(-3.0f, 0.0f, 3.0f), metalRough, 1, glm::vec3(1, 0.71f, 0.29f));

    // Copper
    drawSpot(glm::vec3(-1.5f, 0.0f, 3.0f), metalRough, 1, glm::vec3(0.95f, 0.64f, 0.54f));

    // Aluminum
    drawSpot(glm::vec3(-0.0f, 0.0f, 3.0f), metalRough, 1, glm::vec3(0.91f, 0.92f, 0.92f));

    // Titanium
    drawSpot(glm::vec3(1.5f, 0.0f, 3.0f), metalRough, 1, glm::vec3(0.542f, 0.497f, 0.449f));

    // Silver
    drawSpot(glm::vec3(3.0f, 0.0f, 3.0f), metalRough, 1, glm::vec3(0.95f, 0.93f, 0.88f));
}

void SceneBasic_Uniform::drawFloor()
{
    model = glm::mat4(1.0f);

    prog.setUniform("Material.Rough", 0.9f);
    prog.setUniform("Material.Metal", 0);
    prog.setUniform("Material.Color", glm::vec3(0.2f));
    
    model = glm::translate(model, glm::vec3(0.0f, -0.75f, 0.0f));

    setMatrices();
    plane.render();
}

void SceneBasic_Uniform::drawSpot(const glm::vec3& pos, float rough, int metal, const glm::vec3& color)
{
    model = glm::mat4(1.0f);
    
    prog.setUniform("Material.Rough", rough);
    prog.setUniform("Material.Metal", metal);
    prog.setUniform("Material.Color", color);
    
    model = glm::translate(model, pos);
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
    setMatrices();
    mesh->render();
}

// unlikely to be edit very often
void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);

    width = w;
    height = h;

    // setting the aspect ratio for the model according to the window size
    // without this line it will not render
    // projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
    projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.3f, 100.0f);
}

// to be called for rendering 3d models, unlikely to be edit very often
void SceneBasic_Uniform::setMatrices()
{
    mv = view * model;

    // besides of setUniform, there are also setMat4, and etc. (From COMP3016)
    // set the ModelViewMatrix uniform to mv
    prog.setUniform("ModelViewMatrix", mv);

    // set the NormalMatrix uniform to following structure
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    // prog.setUniform("NormalMatrix", glm::mat3(mv));  // they are the same, I don't understand why they aren't written the same way

    // set uniform for model, view, projection (MVP) and pass in the projection matrix * model view matrix
    prog.setUniform("MVP", projection * mv);
}