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

    // lab 5.3

    // tPrev(0),                            // rotate no more since lab 5.3
    // angle(0.0f),                         // rotate no more since lab 5.3
    // rotSpeed(glm::pi<float>() / 8.0f),   // rotate no more since lab 5.3
    plane(20.0f, 50.0f, 1, 1),              // modified since lab 5.3
    teapot(14, mat4(1.0f)),
    // torus(0.7f * 1.5f, 0.3f * 1.5f, 50, 50)  // replaced with sphere since lab 5.3
    sphere(2.0f, 50, 50)

    // lab 5.3
{
    //                    relative file location in my computer            , bool center (according to the IDE)
    // mesh = ObjMesh::load("media/spot/spot_triangulated.obj");    // moved to initScene()
}

// init(), initialization of everything in a scene happen in here
// Light intensity setting to be placed in here (Well actually that does not matter at all, so as all setting to be import to shader)
void SceneBasic_Uniform::initScene()
{
    compile();

    // lab 5.3

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);   // modified since lab 5.2

    glEnable(GL_DEPTH_TEST);

    // model, view, projection matrix initialization and setup (for 3D scene)
    // model = mat4(1.0f); // moved to render()
    // the view is a bit different from lab cuz I like it more
    // view = glm::lookAt(vec3(1.0f, 1.25f, 1.25f), vec3(0.0f, -0.1f, 0.0f), vec3(0.0f, 1.0f, 0.0f));   // moved to render()

    projection = mat4(1.0f);

    // angle = glm::pi<float>() / 4.0f; // rotate no more since lab 5.3

    #pragma region Image Processing Techniques - HDR with Tone Mapping

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
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0); // Vertex position

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2); // Texture coordinates

    glBindVertexArray(0);

    //// HDR with Tone Mapping ////

    // I dunno seems there is nothing

    //// HDR with Tone Mapping ////

    #pragma endregion

    #pragma region (Light related) Light (diffuse, ambient, specular) intensity setting
    
    // turn to multiple directional lights[3] since lab 5.3

    /// Disabled since lab 5.3
    // prog.setUniform("Light.Ld", vec3(1.0f));
    // prog.setUniform("Light.La", vec3(0.05f));
    // prog.setUniform("Light.Ls", vec3(1.0f));

    vec3 intense = vec3(5.0f);

    prog.setUniform("lights[0].Ld", intense);
    prog.setUniform("lights[0].Ls", intense);
    prog.setUniform("lights[1].Ld", intense);
    prog.setUniform("lights[1].Ls", intense);
    prog.setUniform("lights[2].Ld", intense);
    prog.setUniform("lights[2].Ls", intense);

    intense = vec3(0.2f);

    prog.setUniform("lights[0].La", intense);
    prog.setUniform("lights[1].La", intense);
    prog.setUniform("lights[2].La", intense);

    #pragma endregion

    // since resize() is called in scenerunner run() after initScene(),
    // only 1 setupFBO() is necessary to appears in initScene() and resize()
    // put it in resize() with proper deallocate function could perform a cool ass
    // viewport resize feature
    // setupFBO();

    #pragma region (Disabled) Texture files linking
    
    /* Disabled since lab 5.1
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
    glActiveTexture(GL_TEXTURE0 + 1);
    // glActiveTexture(GL_TEXTURE0);   // not really necessary, OpenGL can still bind without active anything
                                       // that brought GL_TEXTURE0 texture unit (by default)

    // assume the system somehow know it will send the texture binded in this texture unit 0
    // to somewhere in whatever shader with the "binding = 0"  in vertex
    // and then just forget it, it's meant to be written in this way

    // Order 3 :
    // the type of obj you want to declare as, obj to be bind
    glBindTexture(GL_TEXTURE_2D, texID);
    */

    #pragma endregion
    
    // lab 5.3
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
    
    /* rotate no more since lab 5.3
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
    */

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
        byeFBO();
        // setupFBO();
        resize(viewportWidth, viewportHeight);
    }
    
    pass1();
    // glFlush();  // flush the buffer and remove the texture loaded    // disabled since lab 5.2

    computeLogAveLuminance();   // added since lab 5.3

    pass2();
    // glFlush();  // flush the buffer and remove the texture loaded    // disabled since lab 5.1
}

//// Try to compare each solution of Lab 5
//// Their pass() functions looks similar

// this function stored the model / mesh / object declared and will be handled with Phong model
// void SceneBasic_Uniform::renderToTexture()   // renamed since lab 5.1
void SceneBasic_Uniform::pass1()
{
    prog.setUniform("Pass", 1); // the pass() function will handle the sampler2D Tex in frag shader
    
    // lab 5.3
    
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);   // added since lab 5.3, may conflict with line 51

    glViewport(0, 0, width, height); // enabled since lab 5.3

    // glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);   // in the lab it is called hdrFBO
                                                    // for my own study I will rename it as fboHandle
    glEnable(GL_DEPTH_TEST);

    // lab 5.3

    // clear color buffer and clear color & depth buffers
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    #pragma region (Light, viewer related) Light and camera settings section in render()

    /// Here it is single directional light, basic one
    // vec4 lightPos = vec4(0.0f, 10.0f, 0.0f, 1.0f);  // static position

    // lab 4.8 - camera logic modified since lab 4.3

    // vec4 lightPos = vec4(10.0f * cos(angle), 10.0f, 10.0f * sin(angle), 1.0f);  // spinning rotation
    // vec3 focus = vec3(7.0f * cos(angle), 4.0f, 7.0f * sin(angle));  // lab 5.1, name is changed to cameraPos in lab

    view = glm::lookAt(vec3(2.0f, 0.0f, 14.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));   // static camera position
    // view = glm::lookAt(focus, vec3(0.0f, -0.1f, 0.0f), vec3(0.0f, 1.0f, 0.0f));  // camera x is starring at a point in the void, focus

    //                                                  declared in scene.h
    projection = glm::perspective(glm::radians(60.0f), (float)width / height, 0.3f, 100.0f);

    // prog.setUniform("Light.Position", vec4(view * lightPos));   // Position of light was changing dynamically
    // prog.setUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));   // disabled since multiple lights, lab 5.3

    // lab 4.8

    #pragma endregion

    drawScene();

    #pragma region (Disabled) (Material related) Multiple imported model setting section (Will be modified in numerous labs)

    /* replaced with the drawScene()

    // the color and shininess setting of the material that mapped to the model
    // that going to be effected by the light(s) setting in the scene soon in shader(s)



    //////////////////// First model(s) ////////////////////

    prog.setUniform("Material.Kd", vec3(0.9f, 0.9f, 0.9f));
    prog.setUniform("Material.Ks", vec3(0.95f, 0.95f, 0.95f));
    prog.setUniform("Material.Ka", vec3(0.1f, 0.1f, 0.1f));
    prog.setUniform("Material.shininess", 100.0f);

    model = mat4(1.0f);

    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    setMatrices();
    
    // mesh->render();
    teapot.render();
    
    //////////////////// First model ////////////////////



    //////////////////// Second model ////////////////////

    prog.setUniform("Material.Kd", vec3(0.4f, 0.4f, 0.4f));
    prog.setUniform("Material.Ks", vec3(0.0f, 0.0f, 0.0f));
    prog.setUniform("Material.Ka", vec3(0.1f, 0.1f, 0.1f));
    prog.setUniform("Material.shininess", 100.0f);

    model = mat4(1.0f);

    model = glm::translate(model, vec3(0.0f, -0.75f, 0.0f));
    setMatrices();

    plane.render();

    //////////////////// Second model ////////////////////



    //////////////////// Third model ////////////////////

    prog.setUniform("Material.Kd", vec3(0.2f, 0.5f, 0.9f));     // modified since lab 5.2 to create difference
    prog.setUniform("Material.Ks", vec3(0.0f, 0.0f, 0.0f));
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

// I don't know what is the specific duty of this function
// void SceneBasic_Uniform::renderScene()   // renamed since lab 5.1
void SceneBasic_Uniform::pass2()
{
    // lab 5.3
    
    prog.setUniform("Pass", 2);

    // revert to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // clear color buffer and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);   // not sure what does it do, still necessary in the content

    model = mat4(1.0f);
    view = mat4(1.0f);
    projection = mat4(1.0f);

    setMatrices();

    // Render the full-screen quad
    glBindVertexArray(fsQuad);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // lab 5.3
}

void SceneBasic_Uniform::drawScene()
{
    vec3 intense = vec3(1.0f);

    prog.setUniform("lights[0].Ld", intense);
    prog.setUniform("lights[0].Ls", intense);
    prog.setUniform("lights[1].Ld", intense);
    prog.setUniform("lights[1].Ls", intense);
    prog.setUniform("lights[2].Ld", intense);
    prog.setUniform("lights[2].Ls", intense);

    vec4 lightPos = vec4(0.0f, 4.0f, 2.5f, 1.0f);

    lightPos.x = -7.0f;
    prog.setUniform("lights[0].Position", view * lightPos);

    lightPos.x = 0.0f;
    prog.setUniform("lights[1].Position", view * lightPos);

    lightPos.x = 7.0f;
    prog.setUniform("lights[2].Position", view * lightPos);

    prog.setUniform("Material.Kd", vec3(0.9f, 0.3f, 0.2f));
    prog.setUniform("Material.Ks", vec3(1.0f, 1.0f, 1.0f));
    prog.setUniform("Material.Ka", vec3(0.2f, 0.2f, 0.2f));
    prog.setUniform("Material.shininess", 100.0f);

    model = mat4(1.0f);

    // The backdrop plane
    model = glm::rotate(mat4(1.0f), glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
    setMatrices();
    plane.render();

    // The bottom plane
    model = glm::translate(mat4(1.0f), vec3(0.0f, -5.0f, 0.0f));
    setMatrices();
    plane.render();

    // Top plane
    model = glm::translate(mat4(1.0f), vec3(0.0f, 5.0f, 0.0f));
    model = glm::rotate(model, glm::radians(180.0f), vec3(1.0f, 0.0f, 0.0f));
    setMatrices();
    plane.render();

    // Sphere
    prog.setUniform("Material.Kd", vec3(0.4f, 0.9f, 0.4f));
    model = glm::translate(mat4(1.0f), vec3(-3.0f, -3.0f, 2.0f));
    setMatrices();
    sphere.render();

    // Teapot
    prog.setUniform("Material.Kd", vec3(0.4f, 0.4f, 0.9f));
    model = glm::translate(mat4(1.0f), vec3(3.0f, -5.0f, 1.5f));
    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    setMatrices();
    teapot.render();
}

#pragma endregion

// unlikely to be edit very often
void SceneBasic_Uniform::resize(int w, int h)
{

    glViewport(0, 0, w, h);

    width = w;
    height = h;

    // it replace the one in initScene()
    setupFBO();

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

// width, height are massively used in all lab 5 solutions
// since setupFBO() is called in initScene()
// the viewport settled always has fixed size, but you can always do setupFBO()
// so now the viewport is adjustable
void SceneBasic_Uniform::setupFBO() // each image processing technique has unique setupFBO()
{
    ///// Necessary part /////

    // Generate and bind the framebuffer
    // glDeleteFramebuffers(1, &hdrFBO);            // Deallocate to free the slot
    // glGenFramebuffers(1, &hdrFBO);
    glGenFramebuffers(1, &fboHandle);               // in lab 5.3 it is called hdrFBO
                                                    // for my own study I will rename it as fboHandle

    // glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);   // in lab 5.3 it is called hdrFBO
                                                    // for my own study I will rename it as fboHandle

    // Create the texture object
    // glGenTextures(1, &hdrTex);
    // glDeleteTextures(1, &hdrTex);                // Deallocate to free the slot
    glGenTextures(1, &fboTex);                      // in lab 5.3 it is called hdrTex
                                                    // for my own study I will rename it as fboTex

    // enabled since lab 5.3
    glActiveTexture(GL_TEXTURE0); // Use texture unit0, thats why avoid using the same slot when init
    
    // glBindTexture(GL_TEXTURE_2D, renderTex);
    glBindTexture(GL_TEXTURE_2D, fboTex);   // renamed as fboTex to match the naming style

    // glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);   // replaced since lab 5.3
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, width, height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);     // disabled since lab 5.3
    
    // Bind the texture to the FBO
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdrTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTex, 0); // renamed as fboTex to match the naming style
    

    /// Beginning of depth buffer ///


    // Create the depth buffer
    // GLuint depthBuf; // declared as global variable, go to header file
    // glDeleteRenderbuffers(1, &depthBuf);         // Deallocate to free the slot
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);

    // replaced since lab 5.1
    // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 512, 512);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    
    // Bind the depth buffer to the FBO 
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
                              GL_RENDERBUFFER, depthBuf);
    
    ///// Necessary part /////

    // Set the targets for the fragment output variables
    // GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 }; // replaced since lab 5.3 
    GLenum drawBuffers[] = { GL_NONE, GL_COLOR_ATTACHMENT0 };
    
    // glDrawBuffers(1, drawBuffers);                   // replaced since lab 5.3
    glDrawBuffers(2, drawBuffers);

    // Unbind the framebuffer, and revert to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// lab 5.3

// this is self-made deallocator, it serve for one reason
// to free the slots and save some memory (at least I hope it can)
// it is called when you need to resize the viewport
void SceneBasic_Uniform::byeFBO()   // each glDelete... correspond to each glGen...
{
    // glDeleteFramebuffers(1, &hdrFBO);    // in the lab it is called hdrFBO
                                            // for my own study I will rename it as fboHandle
    glDeleteFramebuffers(1, &fboHandle);

    // glDeleteTextures(1, &hdrTex);        // in the lab it is called hdrTex
                                            // for my own study I will rename it as fboTex
    glDeleteTextures(1, &fboTex);
    glDeleteRenderbuffers(1, &depthBuf);
}

void SceneBasic_Uniform::computeLogAveLuminance()
{
    int size = width * height;
    std::vector<GLfloat> texData(size * 3);
    glActiveTexture(GL_TEXTURE0);

    // glBindTexture(GL_TEXTURE_2D, hdrTex);
    glBindTexture(GL_TEXTURE_2D, fboTex);   // renamed in current context

    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, texData.data());
    
    float sum = 0.0f;
    for (int i = 0; i < size; i++) {

        float lum = glm::dot(vec3(texData[i * 3 + 0],
                                  texData[i * 3 + 1],
                                  texData[i * 3 + 2]),
                             vec3(0.2126f, 0.7152f, 0.0722f));
        sum += logf(lum + 0.00001f);
    }

    prog.setUniform("AveLum", expf(sum / size));
}

// lab 5.3