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

// initialization of mainly 3D models in a scene happen in here, but well I don't care
SceneBasic_Uniform::SceneBasic_Uniform() :

    // rotation logic related
    tPrev(0), angle(0.0f), rotSpeed(glm::pi<float>() / 8.0f)
{
    //                    relative file location in my computer            , bool center (according to the IDE)
    // mesh = ObjMesh::load("media/bs_ears.obj");
    mesh = ObjMesh::loadWithAdjacency("media/bs_ears.obj");    // lab 6.3
}

// init(), initialization of everything in a scene happen in here
// Light intensity setting to be placed in here (Well actually that does not matter at all, so as all setting to be import to shader)
void SceneBasic_Uniform::initScene()
{
    compile();  // compile, link and use shaders

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);   // set up a background color

    glEnable(GL_DEPTH_TEST);    // enable depth test

    // lab 6.2

    float c = 1.5f;

    projection = glm::ortho(-0.4f * c, 0.4f * c, -0.3f * c, 0.3f * c, 0.1f, 100.0f); // orthographic view is used in lab 6.2
    // projection = mat4(1.0f);

    // lab 6.2

    angle = glm::pi<float>() / 2.0f;

    #pragma region (Light related) Light (diffuse, ambient, specular) intensity setting

    ///////////// Uniforms ////////////////////
    // prog.setUniform("Line.Width", 0.75f);   // lab 6.2

    prog.setUniform("EdegWidth", 0.015f);   // lab 6.3
    prog.setUniform("PctExtend", 0.25f);   // lab 6.3

    prog.setUniform("Line.Color", vec4(0.05f, 0.0f, 0.05f, 1.0f));

    prog.setUniform("Material.Kd", 0.7f, 0.5f, 0.2f);
    prog.setUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
    prog.setUniform("Material.Ks", 0.8f, 0.8f, 0.8f);
    prog.setUniform("Material.shininess", 100.0f);

    prog.setUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));
    
    prog.setUniform("Light.Ld", vec3(1.0f));
    prog.setUniform("Light.La", vec3(0.05f));
    prog.setUniform("Light.Ls", vec3(1.0f));

    /*vec3 intense = vec3(0.6f);

    prog.setUniform("lights[0].Ld", intense);
    prog.setUniform("lights[0].Ls", intense);
    prog.setUniform("lights[1].Ld", intense);
    prog.setUniform("lights[1].Ls", intense);
    prog.setUniform("lights[2].Ld", intense);
    prog.setUniform("lights[2].Ls", intense);

    intense = vec3(0.2f);

    prog.setUniform("lights[0].La", intense);
    prog.setUniform("lights[1].La", intense);
    prog.setUniform("lights[2].La", intense);*/

    #pragma endregion

    #pragma region (Disabled)Texture files linking

    // texture linking has returned after whole lab 5
    
    // load texture file
    // const char* texName = "media/texture/flower.png";
    // Texture::loadTexture(texName);

    #pragma endregion
}

void SceneBasic_Uniform::compile()
{
    // compile mr prog with the desired shaders to be used in this scene
	try {
		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.compileShader("shader/basic_uniform.gs");
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

    #pragma region lab 3.4 spinning logic
    
    // rotate again since lab 5.4
    float deltaT = t - tPrev;
    if (tPrev == 0.0f) deltaT = 0.0f;
    tPrev = t;
    angle += 0.1f * deltaT; // this is modified in lab video, angle += rotSpeed * deltaT;

    // lab 4.3

    // if (angle > glm::two_pi<float>()) angle -= glm::two_pi<float>();
    if (this->m_animate)
    {
        // it is just modifing the variable angle more rapidly
        angle += rotSpeed * deltaT;

        if (angle > glm::two_pi<float>()) angle -= glm::two_pi<float>();
    }

    // lab 4.3

    #pragma endregion
}

// this function now call other function(s) awaits to be executed in scene
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
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we clear color and depth buffer

    // set up camera(it was done in mainly pass1() in previous lab 5, since everything is reset, it is now render() to set up camera)
    // vec3 cameraPos(0.0f, 0.0f, 3.0f);
    vec3 cameraPos(2.0f * sin(angle), 0.0f, 2.0f * cos(angle));   // spinning logic applied
    view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    model = mat4(1.0f);
    setMatrices();  //set up your matrices and send to shaders

    mesh->render();

    glFinish();
}

#pragma endregion

// unlikely to be edit very often
void SceneBasic_Uniform::resize(int w, int h)
{

    glViewport(0, 0, w, h);

    width = w;
    height = h;

    // remember to put the vao/fbo/whatever setup logic here instead of init() to keep the resizing logic functional
    // setupFBO();

    // setting the aspect ratio for the model according to the window size
    // without this line it will not render
    // projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);

    /*float w2 = w / 2.0f;
    float h2 = h / 2.0f;

    viewport = mat4( vec4(w2, 0.0f, 0.0f, 0.0f),
                     vec4(0.0f, h2, 0.0f, 0.0f),
                     vec4(0.0f, 0.0f, 1.0f, 0.0f),
                     vec4(w2 + 0, h2 + 0, 0.0f, 1.0f) );*/

}

// to be called for rendering 3d models, unlikely to be edit very often
void SceneBasic_Uniform::setMatrices()
{
    mv = view * model; // model view matrix

    // besides of setUniform, there are also setMat4, and etc. (From COMP3016)
    // set the ModelViewMatrix uniform to mv
    prog.setUniform("ModelViewMatrix", mv);

    // set the NormalMatrix uniform to following structure
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));

    // set uniform for model, view, projection (MVP) and pass in the projection matrix * model view matrix
    prog.setUniform("MVP", projection * mv);

    // prog.setUniform("ProjectionMatrix", projection); // send the projection matrix

    // prog.setUniform("ViewportMatrix", viewport); // lab 6.2
}