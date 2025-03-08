//////////////////// lab 4.8 is taken from lab 4.1 single texture ////////////////////

//////////////////// This lab is awesome ////////////////////
//////////////////// The function renderToTexture() will render a cow ////////////////////
//////////////////// The cow is the object to be rendered to the texture of a cube ////////////////////
//////////////////// Without summoning the cube you can see the cow in the scene ////////////////////
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

// lab 4.1

#include "helper/texture.h" // texture loading related

// lab 4.1

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

// initialization of mainly 3D models in a scene happen in here, but well I don't care
SceneBasic_Uniform::SceneBasic_Uniform() :

    tPrev(0),  // added in lab 3.4, for spinning logic
    rotSpeed(glm::pi<float>() / 8.0f)
{
    //                    relative file location in my computer            , bool center (according to the IDE)
    // mesh = ObjMesh::load("media/spot/spot_triangulated.obj");    // moved to initScene()
}

// init(), initialization of everything in a scene happen in here
// Light intensity setting to be placed in here (Well actually that does not matter at all, so as all setting to be import to shader)
void SceneBasic_Uniform::initScene()
{
    compile();

    glEnable(GL_DEPTH_TEST);

    // model, view, projection matrix initialization and setup (for 3D scene)
    // model = mat4(1.0f); // moved to render()
    // the view is a bit different from lab cuz I like it more
    // view = glm::lookAt(vec3(1.0f, 1.25f, 1.25f), vec3(0.0f, -0.1f, 0.0f), vec3(0.0f, 1.0f, 0.0f));   // moved to render()

    projection = mat4(1.0f);

    // lab 3.4, rotation logic of the light(s)
    
    // init float angle
    angle = 0.0f;

    // lab 3.4

    #pragma region (Light related) Light (diffuse, ambient, specular) intensity setting
    
    prog.setUniform("Light.Ld", vec3(1.0f));
    prog.setUniform("Light.La", vec3(0.05f));
    prog.setUniform("Light.Ls", vec3(1.0f));

    #pragma endregion

    #pragma region Texture files linking
    
    // reference : https://www.reddit.com/r/opengl/comments/1f1wizb/how_bad_is_it_to_only_use_gl_texture0_and_what_is/
    //             https://stackoverflow.com/questions/8866904/differences-and-relationship-between-glactivetexture-and-glbindtexture
    //             https://community.khronos.org/t/when-to-use-glactivetexture/64913/2

    // Order 1 :
    // Load the texture into program, in lab 4.8 it refer to the texture for Mesh mesh
    // GLuint texID = Texture::loadTexture("media/texture/brick1.jpg");
    GLuint texID = Texture::loadTexture("media/spot/spot_texture.png");   // different options from the lab materials

    // Order 1.1
    mesh = ObjMesh::load("media/spot/spot_triangulated.obj");

    setupFBO();
    
    // glActiveTexture(GL_TEXTURE0 + 1);    // this is used in lab solution
    // Order 2 :
    // pickup a texture unit that will keep affecting the whole program for to bind a texture to it
    glActiveTexture(GL_TEXTURE0 + 1);   // in here a slot 1 will be the texture to render
    // glActiveTexture(GL_TEXTURE0);   // mapped it with slot 0 will cause the texture rendered to
                                       // the cube instead of the cow

    // assume the system somehow know it will send the texture binded in this texture unit 0
    // to somewhere in whatever shader with the "binding = 0"  in vertex
    // and then just forget it, it's meant to be written in this way

    // Order 3 :
    // the type of obj you want to declare as, obj to be bind
    glBindTexture(GL_TEXTURE_2D, texID);    // I guess when a sampler2D read this in frag shader
                                            // it knows the texture to be render and their slot no.

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

    #pragma region lab 3.4 spinning logic
    
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
    // Since the 2 objects is treated as, well separate 2 object
    // their transform is totally different
    // resize the viewport could affects how they look in the scene
    // 
    // this is viewport resizing logic (I figure it out by myself)
    // now the content will stick to the window size when resizing happened
    glfwGetFramebufferSize(glfwGetCurrentContext(), &viewportWidth, &viewportHeight);
    resize(viewportWidth, viewportHeight);

    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
    renderToTexture();
    glFlush();  // flush the buffer and remove the texture loaded

    // clear frame buffer binded above
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    renderScene();
    glFlush();  // flush the buffer and remove the texture loaded
}

// this function stored the mesh declared which looks like a cow and will be handled with Phong model
// void SceneBasic_Uniform::render()    // renamed to renderToTexture()
void SceneBasic_Uniform::renderToTexture()
{
    // refer to the sampler2D Tex in frag shader
    // prog.setUniform("RenderTex", 1); // in the lab standard solution it is called RenderTex
                                        // for my own study I will rename it as Tex,
                                        // so that I can compare it with Lab 5 solutions
    prog.setUniform("Tex", 1);  // from the explanation in initScene(), this probably means
                                // called the sampler2D to render slot 1 item which contains
                                // an image to render to
                                // the obj in following logic, in this function it is the cow

    // "Tex" in the fragment shader has no layout binding -> manual setting outside frag shader
    // in the following solutions in lab 5 it will be driven by frame buffer within frag shader

    glViewport(0, 0, 512, 512);

    // clear color buffer and clear color & depth buffers
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    #pragma region (Light, viewer related) Light and camera settings section in render()

    /// Here it is single directional light, basic one
    // vec4 lightPos = vec4(0.0f, 10.0f, 0.0f, 1.0f);  // static position

    // lab 4.8 - camera logic modified since lab 4.3

    // vec4 lightPos = vec4(10.0f * cos(angle), 10.0f, 10.0f * sin(angle), 1.0f);  // spinning rotation
    // vec3 focus = vec3(1.0f * cos(angle), 1.25f, 1.25f * sin(angle));  // lab 4.8, name is changed to cameraPos in lab

    view = glm::lookAt(vec3(0.0f, 0.0f, 2.5f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));   // static camera position
    // view = glm::lookAt(focus, vec3(0.0f, -0.1f, 0.0f), vec3(0.0f, 1.0f, 0.0f));  // camera x is starring at a point in the void, focus

    projection = glm::perspective(glm::radians(50.0f), 1.0f, 0.3f, 100.0f);

    // prog.setUniform("Light.Position", vec4(view * lightPos));   // Position of light was changing dynamically
    prog.setUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));   // lab 4.8

    // lab 4.8

    #pragma endregion

    #pragma region (Material related) Multiple imported model setting section (Will be modified in numerous labs)

    // the color and shininess setting of the material that mapped to the model
    // that going to be effected by the light(s) setting in the scene soon in shader(s)



    //////////////////// First model(s) ////////////////////

    // prog.setUniform("Material.Kd", vec3(0.2f, 0.55f, 0.9f));                         // disabled in lab 4.8
    prog.setUniform("Material.Ks", vec3(0.95f, 0.95f, 0.95f));
    // prog.setUniform("Material.Ka", vec3(0.2f * 0.3f, 0.55f * 0.3f, 0.9f * 0.3f));    // disabled in lab 4.8
    prog.setUniform("Material.shininess", 100.0f);

    model = mat4(1.0f);

    model = glm::rotate(model, angle, vec3(0.0f, 1.0f, 0.0f));                          // changed in lab 4.8
    setMatrices();

    mesh->render();  // this mesh here is actually referring to the spot object, which looks like a cow
    // cube.render();

    //////////////////// First model ////////////////////



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

// this function stored the cube declared which is awaiting the spot to be rendered onto it and awaits to be rendered in scene
void SceneBasic_Uniform::renderScene()
{
    // refer to the sampler2D Tex in frag shader
    // prog.setUniform("RenderTex", 0); // in the lab standard solution it is called RenderTex
                                        // for my own study I will rename it as Tex,
                                        // so that I can compare it with Lab 5 solutions

    prog.setUniform("Tex", 0);  // from the explanation in initScene(), this probably means
                                // called the sampler2D to render slot 0 item which contains
                                // ABSOLUTELY NOTHING to
                                // the obj in following logic, in this function it is the cube
    //              declared in scene.h
    glViewport(0, 0, width, height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /// Here it is single directional light, basic one
    // vec4 lightPos = vec4(0.0f, 10.0f, 0.0f, 1.0f);  // static position

    // lab 4.8 - camera logic modified since lab 4.3

    // vec4 lightPos = vec4(10.0f * cos(angle), 10.0f, 10.0f * sin(angle), 1.0f);  // spinning rotation
    vec3 focus = vec3(2.0f * cos(angle), 1.5f, 2.0f * sin(angle));  // lab 4.8, name is changed to cameraPos in lab

    // view = glm::lookAt(vec3(0.0f, 0.0f, 2.5f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));   // static camera position
    view = glm::lookAt(focus, vec3(0.0f, -0.1f, 0.0f), vec3(0.0f, 1.0f, 0.0f));  // camera x is starring at a point in the void, focus

    // setting the aspect ratio for the cow mesh, effects is like zoom in
    //                                                  declared in scene.h
    projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.3f, 100.0f);

    // prog.setUniform("Light.Position", vec4(view * lightPos));   // Position of light was changing dynamically
    prog.setUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));   // lab 4.8

    // prog.setUniform("Material.Kd", vec3(0.2f, 0.55f, 0.9f));                         // disabled in lab 4.8
    prog.setUniform("Material.Ks", vec3(0.0f, 0.0f, 0.0f));
    // prog.setUniform("Material.Ka", vec3(0.2f * 0.3f, 0.55f * 0.3f, 0.9f * 0.3f));    // disabled in lab 4.8
    prog.setUniform("Material.shininess", 1.0f);

    model = mat4(1.0f);

    setMatrices();

    cube.render();  // the cube here has its own texture, and the cow is the object to render onto the cube
                    // without prper texture mapping they looks like they are overlapped
}

#pragma endregion

// unlikely to be edit very often
void SceneBasic_Uniform::resize(int w, int h)
{

    glViewport(0, 0, w, h);

    width = w;
    height = h;

    // setting the aspect ratio for the model according to the window size
    // without this line it will not render
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);

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

// compare the setupFBO() in lab 4.8 and all solutions in lab 5

void SceneBasic_Uniform::setupFBO()
{
    // Generate and bind the framebuffer
    glGenFramebuffers(1, &fboHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle); 

    // Create the texture object
    // GLuint renderTex;
    GLuint fboTex;                          // renamed as fboTex to match the naming style

    // glGenTextures(1, &renderTex); 
    glGenTextures(1, &fboTex);              // renamed as fboTex to match the naming style
    
    glActiveTexture(GL_TEXTURE0); // Use texture unit0, thats why avoid using the same slot when init
    
    // glBindTexture(GL_TEXTURE_2D, renderTex);
    glBindTexture(GL_TEXTURE_2D, fboTex);   // renamed as fboTex to match the naming style
    
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 512, 512);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Bind the texture to the FBO
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTex, 0); // renamed as fboTex to match the naming style


    /// Beginning of depth buffer ///
    

    // Create the depth buffer
    GLuint depthBuf;
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 512, 512);
    
    // Bind the depth buffer to the FBO 
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
                              GL_RENDERBUFFER, depthBuf);
    
    // Set the targets for the fragment output variables
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);
    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result == GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer is complete" << endl;
    }
    else
    {

        std::cout << "Framebuffer error:" << result << endl;

    }

    // Unbind the framebuffer, and revert to default framebufter
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}