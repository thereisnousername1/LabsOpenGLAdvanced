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

    // lab 5.1

    tPrev(0),  // added in lab 3.4, for spinning logic
    angle(0.0f),
    rotSpeed(glm::pi<float>() / 8.0f),
    plane(50.0f, 50.0f, 1, 1),
    teapot(14, mat4(1.0f)),
    torus(0.7f * 1.5f, 0.3f * 1.5f, 50, 50)

    // lab 5.1
{
    //                    relative file location in my computer            , bool center (according to the IDE)
    // mesh = ObjMesh::load("media/spot/spot_triangulated.obj");    // moved to initScene()
}

// init(), initialization of everything in a scene happen in here
// Light intensity setting to be placed in here (Well actually that does not matter at all, so as all setting to be import to shader)
void SceneBasic_Uniform::initScene()
{
    compile();

    // lab 5.1

    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);   // added since lab 5.1

    glEnable(GL_DEPTH_TEST);

    // model, view, projection matrix initialization and setup (for 3D scene)
    // model = mat4(1.0f); // moved to render()
    // the view is a bit different from lab cuz I like it more
    // view = glm::lookAt(vec3(1.0f, 1.25f, 1.25f), vec3(0.0f, -0.1f, 0.0f), vec3(0.0f, 1.0f, 0.0f));   // moved to render()

    projection = mat4(1.0f);

    angle = glm::pi<float>() / 4.0f;

    #pragma region (Light related) Light (diffuse, ambient, specular) intensity setting
    
    prog.setUniform("Light.Ld", vec3(1.0f));
    prog.setUniform("Light.La", vec3(0.05f));
    prog.setUniform("Light.Ls", vec3(1.0f));

    #pragma endregion

    setupFBO();

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

    prog.setUniform("EdgeThreshold", 0.05f);


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
    
    // lab 5.1
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
    // resizing won't work for this lab, because the area to render
    // is defined in the beginning, initScene()
    // 
    // this is viewport resizing logic (I figure it out by myself)
    // now the content will stick to the window size when resizing happened
    glfwGetFramebufferSize(glfwGetCurrentContext(), &viewportWidth, &viewportHeight);
    resize(viewportWidth, viewportHeight);

    // glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);    // disabled since lab 5.1
    // renderToTexture();                               // disabled since lab 5.1
    pass1();
    glFlush();  // flush the buffer and remove the texture loaded

    // glBindFramebuffer(GL_FRAMEBUFFER, 0);            // disabled since lab 5.1
    // renderScene();                                   // disabled since lab 5.1
    pass2();
    // glFlush();  // flush the buffer and remove the texture loaded    // disabled since lab 5.1
}

// this function stored the model / mesh / object declared and awaits to be detect with their edge
// void SceneBasic_Uniform::render()    // renamed to renderToTexture()
// void SceneBasic_Uniform::renderToTexture()   // renamed since lab 5.1
void SceneBasic_Uniform::pass1()
{
    // prog.setUniform("RenderTex", 1); // replaced since lab 5.1
    prog.setUniform("Pass", 1);
    // glViewport(0, 0, 512, 512);      // disabled since lab 5.1

    // added since lab 5.1
    
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
    glEnable(GL_DEPTH_TEST);

    // added since lab 5.1

    // clear color buffer and clear color & depth buffers
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    #pragma region (Light, viewer related) Light and camera settings section in render()

    /// Here it is single directional light, basic one
    // vec4 lightPos = vec4(0.0f, 10.0f, 0.0f, 1.0f);  // static position

    // lab 4.8 - camera logic modified since lab 4.3

    // vec4 lightPos = vec4(10.0f * cos(angle), 10.0f, 10.0f * sin(angle), 1.0f);  // spinning rotation
    vec3 focus = vec3(7.0f * cos(angle), 4.0f, 7.0f * sin(angle));  // lab 5.1, name is changed to cameraPos in lab

    // view = glm::lookAt(vec3(0.0f, 0.0f, 2.5f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));   // static camera position
    view = glm::lookAt(focus, vec3(0.0f, -0.1f, 0.0f), vec3(0.0f, 1.0f, 0.0f));  // camera x is starring at a point in the void, focus

    // setting the aspect ratio for the cow mesh, effects is like zoom in
    //                                                  declared in scene.h
    projection = glm::perspective(glm::radians(60.0f), (float)width / height, 0.3f, 100.0f);

    // prog.setUniform("Light.Position", vec4(view * lightPos));   // Position of light was changing dynamically
    prog.setUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));   // lab 4.8

    // lab 4.8

    #pragma endregion

    #pragma region (Material related) Multiple imported model setting section (Will be modified in numerous labs)

    // the color and shininess setting of the material that mapped to the model
    // that going to be effected by the light(s) setting in the scene soon in shader(s)



    //////////////////// First model(s) ////////////////////

    prog.setUniform("Material.Kd", vec3(0.9f, 0.9f, 0.9f));     // enabled in lab 5.1
    prog.setUniform("Material.Ks", vec3(0.95f, 0.95f, 0.95f));
    prog.setUniform("Material.Ka", vec3(0.1f, 0.1f, 0.1f));     // enabled in lab 5.1
    prog.setUniform("Material.shininess", 100.0f);

    model = mat4(1.0f);

    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));  // changed in lab 5.1
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

    prog.setUniform("Material.Kd", vec3(0.9f, 0.5f, 0.2f));
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

// since this is edge detection, nothing except the edge is needed to draw in here
// void SceneBasic_Uniform::renderScene()   // renamed since lab 5.1
void SceneBasic_Uniform::pass2()
{
    // lab 5.1

    // prog.setUniform("RenderTex", 1); // replaced since lab 5.1
    prog.setUniform("Pass", 2);

    // Disabled since lab 5.1
    // glViewport(0, 0, width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, renderTex); // modified from lab sheet 5
    glBindTexture(GL_TEXTURE_2D, Tex1);

    glDisable(GL_DEPTH_TEST);

    // clear color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    model = mat4(1.0f);
    view = mat4(1.0f);
    projection = mat4(1.0f);

    setMatrices();

    glBindVertexArray(fsQuad);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    /* Disabled since lab 5.1
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
    */

    // lab 5.1
}

#pragma endregion

// unlikely to be edit very often
void SceneBasic_Uniform::resize(int w, int h)
{

    glViewport(0, 0, w, h);

    width = w;
    height = h;

    //////////////////// USING THIS METHOD TO UPDATE VIEWPORT SIZE IS EXTREMLY EXPENSIVE ////////////////////
    // setupFBO();

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

// width, height are massively used in all lab 5 solutions
// since setupFBO() is called in initScene()
// the viewport settled always has fixed size
void SceneBasic_Uniform::setupFBO()
{
    // Generate and bind the framebuffer
    glGenFramebuffers(1, &fboHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle); 

    // Create the texture object
    // GLuint renderTex;    // disabled since lab 5.1
    // glGenTextures(1, &renderTex);            // renamed as Tex1
    glGenTextures(1, &Tex1);

    // disabled since lab 5.1
    // glActiveTexture(GL_TEXTURE0); // Use texture unit0, thats why avoid using the same slot when init
    
    // glBindTexture(GL_TEXTURE_2D, renderTex); // renamed as Tex1
    glBindTexture(GL_TEXTURE_2D, Tex1);

    // glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 512, 512);    // replaced since lab 5.1
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);   // added since lab 5.1
    
    // Bind the texture to the FBO
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex, 0);   // renamed as Tex1
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Tex1, 0);

    // depth section
    
    // Create the depth buffer

    GLuint depthBuf;
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);

    // replaced since lab 5.1
    // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 512, 512);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    
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