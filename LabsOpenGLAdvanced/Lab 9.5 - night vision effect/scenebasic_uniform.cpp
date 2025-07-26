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
// Lab 9.5 settings of the scene is partially taken from previous labs, lab 5.1, lab 5.2, lab 9.2
////

// initialization of mainly 3D models in a scene happen in here, but well I don't care
SceneBasic_Uniform::SceneBasic_Uniform() :

    tPrev(0),
    angle(0.0f),
    rotSpeed(glm::pi<float>() / 2.0f),
    // rotSpeed(0.001f),  // lab 9.5
    teapot(14, glm::mat4(1.0f)),

    plane(50.0f, 50.0f, 1, 1),              // lab 9.3
    torus(0.7f * 1.5f, 0.3f * 1.5f, 50, 50) // lab 9.3
{
    //                       relative file location in my computer            , bool center (according to the IDE)
    // mesh = ObjMesh::load("../Lab 3.3 - pig mesh/media/pig_triangulated.obj", true);
}

// init(), initialization of everything in a scene happen in here
// Light intensity setting to be placed in here (Well actually that does not matter at all, so as all setting to be import to shader)
void SceneBasic_Uniform::initScene()
{
    compile();

    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);   // lab 9.3

    glEnable(GL_DEPTH_TEST);

    // model, view, projection matrix initialization and setup (for 3D scene)
    model = mat4(1.0f);
    // the view is a bit different from lab cuz I like it more
    // view = glm::lookAt(vec3(1.0f, 1.25f, 1.25f), vec3(0.0f, -0.1f, 0.0f), vec3(0.0f, 1.0f, 0.0f));   // moved to render() since lab 4.3
    projection = mat4(1.0f);
    
    // init float angle
    // angle = 0.0f;
    angle = glm::pi<float>() / 4.0f;    // lab 9.5

    // lab 9.5

    #pragma region Taken and modified from Image Processing Techniques lab

    // Array for full-screen quad
    GLfloat verts[] =
    {
        -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f
    };
    GLfloat tc[] =
    {
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
    };

    // set up the buffers
    unsigned int handle[2];
    glGenBuffers(2, handle);
    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

    // Set up the vertex array object
    glGenVertexArrays(1, &fsQuad);
    glBindVertexArray(fsQuad);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    // glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, ( (GLubyte*)NULL + (0) ));   // lab 9.5
    glEnableVertexAttribArray(0); // Vertex position

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    // glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));   // lab 9.5
    glEnableVertexAttribArray(2); // Texture coordinates

    glBindVertexArray(0);

    #pragma endregion

    #pragma region (Disabled)(Light related) Light (diffuse, ambient, specular) intensity setting
    
    /*prog.setUniform("Light.Ld", vec3(1.0f));
    prog.setUniform("Light.La", vec3(0.05f));
    prog.setUniform("Light.Ls", vec3(1.0f));*/

    #pragma endregion

    /* moved to resize()
    setupFBO(); // WARNING! IN THE LAB 9.5 IT DIDN"T MENTIONED TO CALL setupFBO() AT AL!!! REMEMBER TO CALL IT!!! 

    // Set up the subroutine indices
    GLuint programHandle = prog.getHandle();
    pass1Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "pass1");
    pass2Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "pass2");

    prog.setUniform("Width", width);
    prog.setUniform("Height", height);
    prog.setUniform("Radius", width / 3.5f);
    //prog.setUniform("Light.Intensity", vec3(1.0f, 1.0f, 1.0f));
    prog.setUniform("Light.Ld", vec3(1.0f));
    prog.setUniform("Light.La", vec3(0.05f));
    prog.setUniform("Light.Ls", vec3(1.0f));

    noiseTex = NoiseTex::generatePeriodic2DTex(200.0f, 0.5f, 512, 512);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, noiseTex);

    prog.setUniform("RenderTex", 0);
    prog.setUniform("NoiseTex", 1);
    */

    // lab 9.5

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
        // angle += 0.1f * deltaT;  // the logic is piece of shit in the video, rearranged for better spinning logic

        // it is just modifing the variable angle more rapidly
        angle += rotSpeed * deltaT;

        if (angle > glm::two_pi<float>())
            angle -= glm::two_pi<float>();
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
    // glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    view = mat4(1.0f);

    pass1();
    glFlush();
    pass2();
}

#pragma endregion

void SceneBasic_Uniform::pass1()
{
    glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);

    // clear color buffer and clear color & depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass1Index);

    #pragma region (Light, viewer related) Light and camera settings section in render()

    /// Here it is single directional light, basic one
    // vec4 lightPos = vec4(0.0f, 10.0f, 0.0f, 1.0f);  // static position

    // vec4 lightPos = vec4(10.0f * cos(angle), 10.0f, 10.0f * sin(angle), 1.0f);  // spinning rotation
    vec3 focus = vec3(7.0f * cos(angle), 4.0f, 7.0f * sin(angle));

    // view = glm::lookAt(vec3(0.0f, 0.0f, 2.5f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));   // static camera position
    view = glm::lookAt(focus, vec3(0.0f, -0.1f, 0.0f), vec3(0.0f, 1.0f, 0.0f));  // camera x is starring at a point in the void, focus

    //                                                  declared in scene.h
    projection = glm::perspective(glm::radians(60.0f), (float)width / height, 0.3f, 100.0f);

    // prog.setUniform("Light.Position", vec4(view * lightPos));   // Position of light was changing dynamically
    prog.setUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));

    #pragma endregion

    #pragma region (Material related) Multiple imported model setting section (Will be modified in numerous labs)

    // the color and shininess setting of the material that mapped to the model
    // that going to be effected by the light(s) setting in the scene soon in shader(s)



    //////////////////// First model(s) ////////////////////

    prog.setUniform("Material.Kd", vec3(0.9f, 0.9f, 0.9f));
    prog.setUniform("Material.Ks", vec3(0.95f, 0.95f, 0.95f));
    prog.setUniform("Material.Ka", vec3(0.1f, 0.1f, 0.1f));
    prog.setUniform("Material.shininess", 100.0f);

    model = mat4(1.0f);

    model = glm::translate(model, vec3(0.0f, 0.0f, 0.0f));

    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    setMatrices();

    // mesh->render();
    teapot.render();

    //////////////////// First model ////////////////////



    //////////////////// Second model ////////////////////

    prog.setUniform("Material.Kd", vec3(0.4f, 0.4f, 0.4f));
    prog.setUniform("Material.Ks", vec3(0.0f, 0.0f, 0.0f));
    prog.setUniform("Material.Ka", vec3(0.1f, 0.1f, 0.1f));
    prog.setUniform("Material.shininess", 1.0f);

    model = mat4(1.0f);

    model = glm::translate(model, vec3(0.0f, -0.75f, 0.0f));
    setMatrices();

    plane.render();

    //////////////////// Second model ////////////////////

    prog.setUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));

    //////////////////// Third model ////////////////////

    prog.setUniform("Material.Kd", vec3(0.9f, 0.5f, 0.2f));
    prog.setUniform("Material.Ks", vec3(0.95f, 0.95f, 0.95f));
    prog.setUniform("Material.Ka", vec3(0.1f, 0.1f, 0.1f));
    prog.setUniform("Material.shininess", 100.0f);

    model = mat4(1.0f);

    // it has to execute before rotate or the torus gone nowhere (probably blocked by something else)
    model = glm::translate(model, vec3(1.0f, 1.0f, 3.0f));
    model = glm::rotate(model, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));

    setMatrices();

    torus.render();

    //////////////////// Third model ////////////////////



    //////////////////// Model render template ////////////////////

    /* standard template of setting up a model
    prog.setUniform("Material.Kd", vec3(?f, ?f, ?f));
    prog.setUniform("Material.Ks", vec3(?f, ?f, ?f));
    prog.setUniform("Material.Ka", vec3(?f, ?f, ?f));
    prog.setUniform("Material.shininess", ?f);

    model = mat4(1.0f);

    /// these 3 are transform related, could be most commonly used
    // translation            obj  ,along x , y , z
    // model = glm::translate(model, vec3(?f, ?f, ?f));

    // rotation            obj  , how much degree ,along  x,  y, z
    // model = glm::rotate(model, glm::radians(?f), vec3(?f, ?f, ?f));

    // scaling            obj  ,along x , y , z
    // model = glm::scale(model, vec3(?f, ?f, ?f));
    /// more examples could refer to COMP3016 CW2

    setMatrices();

    // it seems as a std::unique_ptr<ObjMesh> mesh
    // -> has to be use instead of . notation and so it could be render()
    // ?->render();
    ?.render();
    */

    //////////////////// Model render template ////////////////////

    #pragma endregion
}

void SceneBasic_Uniform::pass2()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderTex);

    glDisable(GL_DEPTH_TEST);

    // clear color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass2Index);

    model = mat4(1.0f);
    view = mat4(1.0f);
    projection = mat4(1.0f);

    setMatrices();

    // Render the full-screen quad
    glBindVertexArray(fsQuad);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    //glBindVertexArray(0);
}

// unlikely to be edit very often
void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);

    width = w;
    height = h;

    // setting the aspect ratio for the model according to the window size
    // without this line it will not render
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);

    setupFBO();  // WARNING! IN THE LAB 9.5 IT DIDN"T MENTIONED TO CALL setupFBO() AT AL!!! REMEMBER TO CALL IT!!! 

    // Set up the subroutine indices
    GLuint programHandle = prog.getHandle();
    pass1Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "pass1");
    pass2Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "pass2");

    prog.setUniform("Width", width);
    prog.setUniform("Height", height);
    prog.setUniform("Radius", width / 3.5f);
    //prog.setUniform("Light.Intensity", vec3(1.0f, 1.0f, 1.0f));
    prog.setUniform("Light.Ld", vec3(1.0f));
    prog.setUniform("Light.La", vec3(0.05f));
    prog.setUniform("Light.Ls", vec3(1.0f));

    noiseTex = NoiseTex::generatePeriodic2DTex(200.0f, 0.5f, 512, 512);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, noiseTex);

    prog.setUniform("RenderTex", 0);
    prog.setUniform("NoiseTex", 1);
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

    // set uniform for model, view, projection (MVP) and pass in the projection matrix * model view matrix
    prog.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::setupFBO()
{
    // Generate and bind the framebuffer
    glGenFramebuffers(1, &renderFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);

    // Create the texture object
    glGenTextures(1, &renderTex);
    glBindTexture(GL_TEXTURE_2D, renderTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Bind the texture to the FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex, 0);

    // Create the depth buffer
    GLuint depthBuf;
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    // Bind the depth buffer to the FBO
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, depthBuf);

    // Set the targets for the fragment output variables
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    // Unbind the framebuffer, and revert to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}