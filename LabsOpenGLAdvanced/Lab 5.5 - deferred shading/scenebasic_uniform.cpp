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

    // lab 5.5

    tPrev(0),                           // rotate again since lab 5.4
    angle(0.0f),                        // rotate again since lab 5.4
    rotSpeed(glm::pi<float>() / 8.0f),  // rotate again since lab 5.4
    plane(20.0f, 50.0f, 1, 1),              // modified since lab 5.3
    teapot(14, mat4(1.0f)),
    torus(0.7f * 1.5f, 0.3f * 1.5f, 50, 50)  
    // sphere(2.0f, 50, 50)             // replaced with torus since lab 5.5

    // lab 5.5
{
    //                    relative file location in my computer            , bool center (according to the IDE)
    // mesh = ObjMesh::load("media/spot/spot_triangulated.obj");    // moved to initScene()
}

// init(), initialization of everything in a scene happen in here
// Light intensity setting to be placed in here (Well actually that does not matter at all, so as all setting to be import to shader)
void SceneBasic_Uniform::initScene()
{
    compile();

    // lab 5.4

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);   // added since lab 5.3

    glEnable(GL_DEPTH_TEST);

    // model, view, projection matrix initialization and setup (for 3D scene)
    // model = mat4(1.0f); // moved to render()
    // the view is a bit different from lab cuz I like it more
    // view = glm::lookAt(vec3(1.0f, 1.25f, 1.25f), vec3(0.0f, -0.1f, 0.0f), vec3(0.0f, 1.0f, 0.0f));   // moved to render()

    projection = mat4(1.0f);

    // angle = glm::pi<float>() / 4.0f; // rotate again since lab 5.4
    angle = glm::pi<float>() / 2.0f;

    #pragma region Image Processing Techniques - Deferred Shading

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
    glGenVertexArrays(1, &quad);    // former fsQuad
    glBindVertexArray(quad);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0); // Vertex position

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2); // Texture coordinates

    glBindVertexArray(0);

    #pragma endregion

    #pragma region (Light related) Light (diffuse, ambient, specular) intensity setting
    
    // turn to multiple directional lights[3] since lab 5.3

    // enabled for lab 5.5
    prog.setUniform("Light.Ld", vec3(1.0f));
    prog.setUniform("Light.La", vec3(0.05f));
    prog.setUniform("Light.Ls", vec3(1.0f));

    // vec3 intense = vec3(5.0f);   // modified from lab 5.3
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
    
    // lab 5.4

    setupFBO();
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

    // Gamma changeable (Key O -> ++, L -> --)
    prog.setUniform("Gamma", Scene::Gamma);

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

    pass2();
    // glFlush();  // flush the buffer and remove the texture loaded    // disabled since lab 5.1
}

//// Try to compare each solution of Lab 5
//// Their pass() functions looks similar

// this function stored the model / mesh / object declared and will be handled with Phong model
// void SceneBasic_Uniform::renderToTexture()   // renamed since lab 5.1
void SceneBasic_Uniform::pass1()    // Render pass function
{
    prog.setUniform("Pass", 1);
    
    // lab 5.4

    // glViewport(0, 0, width, height); // enabled since lab 5.3

    // glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, deferredFBO);   // former fboHandle
    glEnable(GL_DEPTH_TEST);

    // lab 5.4

    // clear color buffer and clear color & depth buffers
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    #pragma region (Light, viewer related) Light and camera settings section in render()

    /// Here it is single directional light, basic one
    // vec4 lightPos = vec4(0.0f, 10.0f, 0.0f, 1.0f);  // static position

    // lab 4.8 - camera logic modified since lab 4.3

    // vec4 lightPos = vec4(10.0f * cos(angle), 10.0f, 10.0f * sin(angle), 1.0f);  // spinning rotation
    vec3 focus = vec3(7.0f * cos(angle), 4.0f, 7.0f * sin(angle));  // lab 5.1, name is changed to cameraPos in lab

    // view = glm::lookAt(vec3(2.0f, 0.0f, 14.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));   // static camera position
    view = glm::lookAt(focus, vec3(0.0f, -0.1f, 0.0f), vec3(0.0f, 1.0f, 0.0f));  // camera x is starring at a point in the void, focus

    //                                                  declared in scene.h
    projection = glm::perspective(glm::radians(60.0f), (float)width / height, 0.3f, 100.0f);

    // prog.setUniform("Light.Position", vec4(view * lightPos));   // Position of light was changing dynamically
    // prog.setUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));   // disabled since multiple lights, lab 5.3

    // lab 4.8

    #pragma endregion

    // drawScene(); // replace with several other lines since lab 5.5

    vec3 intense = vec3(1.0f);

    prog.setUniform("Light.Ld", intense);
    prog.setUniform("Light.Ls", intense);

    prog.setUniform("Light.Position", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    prog.setUniform("Material.Kd", 0.9f, 0.9f, 0.9f);
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    setMatrices();
    teapot.render();
    prog.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -0.75f, 0.0f));
    setMatrices();
    plane.render();
    prog.setUniform("Light.Position", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    prog.setUniform("Material.Kd", 0.2f, 0.5f, 0.9f);
    model = mat4(1.0f);
    model = glm::translate(model, vec3(1.0f, 1.0f, 3.0f));
    model = glm::rotate(model, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
    setMatrices();
    torus.render();
    glFinish();

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

// lab 5.5
void SceneBasic_Uniform::pass2()    // Bright pass function
{
    prog.setUniform("Pass", 2);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);         // renamed to match current context

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);   // not sure what does it do, still necessary in the content

    model = mat4(1.0f);
    view = mat4(1.0f);
    projection = mat4(1.0f);

    setMatrices();

    // Render the full-screen quad
    glBindVertexArray(quad);    //former fsQuad
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // glBindVertexArray(0);    // disabled since lab 5.2, it doesn't seems have any difference
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
    // lab 5.5

    GLuint posTex, normTex, colorTex;

    // lab 5.5

    // Generate and bind the framebuffer
    // glDeleteFramebuffers(1, &hdrFBO);            // Deallocate to free the slot
    // glGenFramebuffers(1, &hdrFBO);
    glGenFramebuffers(1, &deferredFBO);             // former fboHandle
                                                    // for my own study I will rename it as fboHandle

    // glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, deferredFBO); // former fboHandle
                                                    // for my own study I will rename it as fboHandle

    /// Beginning of depth buffer ///

    // Create the depth buffer
    // GLuint depthBuf; // declared as global variable, go to header file
    // glDeleteRenderbuffers(1, &depthBuf);         // Deallocate to free the slot
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);

    // replaced since lab 5.1
    // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 512, 512);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    
    // Create the textures for position, normal and color
    createGBufTex(GL_TEXTURE0, GL_RGB32F, posTex); // Position
    createGBufTex(GL_TEXTURE1, GL_RGB32F, normTex); // Normal
    createGBufTex(GL_TEXTURE2, GL_RGB8, colorTex); // Color

    // Attach the textures to the framebuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
                              GL_RENDERBUFFER, depthBuf);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                           GL_TEXTURE_2D, posTex, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 
                           GL_TEXTURE_2D, normTex, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 
                           GL_TEXTURE_2D, colorTex, 0);

    GLenum drawBuffers[] = { GL_NONE, GL_COLOR_ATTACHMENT0, 
                             GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    
    glDrawBuffers(4, drawBuffers);

    // Unbind the framebuffer, and revert to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// lab 5.5

// this is self-made deallocator, it serve for one reason
// to free the slots and save some memory (at least I hope it can)
// it is called when you need to resize the viewport
void SceneBasic_Uniform::byeFBO()   // each glDelete... correspond to each glGen...
{
    glDeleteFramebuffers(1, &deferredFBO);  // former fboHandle

    glDeleteRenderbuffers(1, &depthBuf);
}

// lab 5.5

void SceneBasic_Uniform::createGBufTex(GLenum texUnit, GLenum format, GLuint& texid)
{
    glActiveTexture(texUnit);
    
    glGenTextures(1, &texid);

    glBindTexture(GL_TEXTURE_2D, texid);

    glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
}