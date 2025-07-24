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

// self note: Lab 8.1 is taken from lab 4.1, therefore the commenting and lines could look naive

#include "helper/texture.h" // texture loading related

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

// initialization of mainly 3D models in a scene happen in here, but well I don't care
SceneBasic_Uniform::SceneBasic_Uniform() :

    tPrev(0),
    rotSpeed(0.1f),                         // lab 8.4 setting
    plane(10.0f, 10.0f, 2, 2, 5.0f, 5.0f)   // lab 8.4 setting
{

    //                       relative file location in my computer            , bool center (according to the IDE)
    mesh = ObjMesh::loadWithAdjacency("media/spot/spot_triangulated.obj");  // enabled since lab 8.2 for the building model
}

// init(), initialization of everything in a scene happen in here
// Light intensity setting to be placed in here (Well actually that does not matter at all, so as all setting to be import to shader)
void SceneBasic_Uniform::initScene()
{
    compile();

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);   // lab 8.1
    
    glClearStencil(0);

    glEnable(GL_DEPTH_TEST);

    angle = 0.0f;       // lab 8.4 setting

    // set up the framebuffer object
    setupFBO();

    renderProg.use();
    renderProg.setUniform("LightIntensity", vec3(1.0f));

    #pragma region Taken from Image Processing Techniques lab

    // Set up a VAO for the full-screen quad
    GLfloat verts[] =
    {
        -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 
        1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f
    };

    GLuint bufHandle;

    glGenBuffers(1, &bufHandle);
    glBindBuffer(GL_ARRAY_BUFFER, bufHandle);
    glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), verts, GL_STATIC_DRAW);

    // Set up the vertex array object
    glGenVertexArrays(1, &fsQuad);
    glBindVertexArray(fsQuad);

    glBindBuffer(GL_ARRAY_BUFFER, bufHandle);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0); // Vertex position

    glBindVertexArray(0);

    #pragma endregion

    // model, view, projection matrix initialization and setup (for 3D scene)
    //model = mat4(1.0f);
    // the view is a bit different from lab cuz I like it more
    //view = glm::lookAt(vec3(1.0f, 1.25f, 1.25f), vec3(0.0f, -0.1f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    //projection = mat4(1.0f);

    #pragma region Texture files linking
    
    // reference : https://www.reddit.com/r/opengl/comments/1f1wizb/how_bad_is_it_to_only_use_gl_texture0_and_what_is/
    //             https://stackoverflow.com/questions/8866904/differences-and-relationship-between-glactivetexture-and-glbindtexture
    //             https://community.khronos.org/t/when-to-use-glactivetexture/64913/2

    // Order 1 :
    // Load the texture into program
    // GLuint texID = Texture::loadTexture("media/texture/brick1.jpg");
    // GLuint texID = Texture::loadTexture("media/texture/fire.png");   // different options from the lab materials

    // Order 2 :
    // pickup a texture unit that will keep affecting the whole program for to bind a texture to it
    // glActiveTexture(GL_TEXTURE0);   // not really necessary, OpenGL can still bind without active anything
                                    // that brought GL_TEXTURE0 texture unit (by default)

    // assume the system somehow know it will send the texture binded in this texture unit 0
    // to somewhere in whatever shader with the "binding = 0"  in vertex
    // and then just forget it, it's meant to be written in this way

    // Order 3 :
    // the type of obj you want to declare as, obj to be bind
    // glBindTexture(GL_TEXTURE_2D, texID);

    // Load textures
    glActiveTexture(GL_TEXTURE2);

    spotTex = Texture::loadTexture("media/spot/spot_texture.png");
    brickTex = Texture::loadTexture("media/texture/brick1.jpg");

    updateLight();
    
    renderProg.use();
    renderProg.setUniform("Tex", 2);
    
    compProg.use();
    compProg.setUniform("DiffSpecTex", 0);

    this->animate(true);

    #pragma endregion

}

void SceneBasic_Uniform::updateLight()
{
    #pragma region (Light related) Light (diffuse, ambient, specular) intensity setting
    
    // prog.setUniform("Light.Intensity", vec3(0.85f)); // lab 8.1 setting

    /*prog.setUniform("Light.Ld", vec3(1.0f));
    prog.setUniform("Light.La", vec3(0.05f));
    prog.setUniform("Light.Ls", vec3(1.0f));*/

    lightPos = vec4( 5.0f * vec3(cosf(angle) * 7.5f, 1.5f, sinf(angle) * 7.5f), 1.0f );  // World coords

    #pragma endregion
}

void SceneBasic_Uniform::compile()
{
    // compile mr prog with the desired shaders to be used in this scene
	try {
		
        // The shader for the volumes
        volumeProg.compileShader("shader/basic_uniform.vert");
        volumeProg.compileShader("shader/basic_uniform.frag");
        volumeProg.compileShader("shader/basic_uniform.gs");
        volumeProg.link();

        // The shader for rendering and compositing
        renderProg.compileShader("shader/shadow_volume_render.vert");
        renderProg.compileShader("shader/shadow_volume_render.frag");
        renderProg.link();

        // The final composite shader
        compProg.compileShader("shader/shadow_volume_comp.vert");
        compProg.compileShader("shader/shadow_volume_comp.frag");
        compProg.link();

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
        angle += rotSpeed * deltaT;

        if (angle > glm::two_pi<float>())
            angle -= glm::two_pi<float>();

        updateLight();  // lab 8.4
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

    pass1();
    glFlush();
    pass2();
    glFlush();
    pass3();

}

#pragma endregion

// Just renders the geometry normally with shading. The ambient component is rendered to one buffer,
// and the diffuse and specular components are written to a texture.
void SceneBasic_Uniform::pass1()
{
    glDepthMask(GL_TRUE);
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);
    projection = glm::infinitePerspective(glm::radians(50.0f), (float)width / height, 0.5f);
    view = glm::lookAt(vec3(5.0f, 5.0f, 5.0f), vec3(0, 2, 0), vec3(0, 1, 0));

    renderProg.use();
    renderProg.setUniform("LightPosition", view * lightPos);

    glBindFramebuffer(GL_FRAMEBUFFER, colorDepthFBO);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    drawScene(renderProg, false);
}

// This is the pass that generates the shadow volumes using the geometry shader
void SceneBasic_Uniform::pass2()
{
    volumeProg.use();
    volumeProg.setUniform("LightPosition", view * lightPos);

    // Copy the depth and color buffers from the FBO into the default FBO
    // The color buffer should contain the ambient component.
    glBindFramebuffer(GL_READ_FRAMEBUFFER, colorDepthFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, width - 1, height - 1, 0, 0, width - 1, height - 1,
        GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // Disable writing to the color buffer and depth buffer
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);

    // Re-bind to the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Set up the stencil test so that it always succeeds, 
    // increments for front faces, and decrements for back faces.
    glClear(GL_STENCIL_BUFFER_BIT);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0, 0xffff);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP);
    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_DECR_WRAP);

    // Draw only the shadow casters
    drawScene(volumeProg, true);

    // Enable writing to the color buffer
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

// In this pass, we read the diffuse and specular component from a texture
// and combine it with the ambient component if the stencil test succeeds.
void SceneBasic_Uniform::pass3()
{
    // We don't need the depth test
    glDisable(GL_DEPTH_TEST);

    // We want to just sum the ambient component and the diffuse + specular when the stencil test succeeds,
    // so we'll use this simple blend function.
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    // We want to only render those pixels that have a stencil value equal to zero.
    glStencilFunc(GL_EQUAL, 0, 0xffff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    compProg.use();

    // Just draw a screen filling quad
    model = mat4(1.0f);
    projection = model;
    view = model;
    setMatrices(compProg);

    glBindVertexArray(fsQuad);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);

    // Restore some state
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void SceneBasic_Uniform::drawScene(GLSLProgram& prog, bool onlyShadowCasters)
{
    // vec3 color = vec3(0.2f, 0.5f, 0.9f); // reset in lab 8.4
    vec3 color;

    if (!onlyShadowCasters)
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, spotTex);
        color = vec3(1.0f);

        prog.setUniform("Ka", color * 0.1f);
        prog.setUniform("Kd", color);
        prog.setUniform("Ks", vec3(0.9f));
        prog.setUniform("Shininess", 150.0f);
    }

    model = mat4(1.0f);
    model = glm::translate(model, vec3(-2.3f, 1.0f, 0.2f));
    model = glm::rotate(model, glm::radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, vec3(1.5f));
    setMatrices(prog);
    mesh->render();

    model = mat4(1.0f);
    model = glm::translate(model, vec3(2.5f, 1.0f, -1.2f));
    model = glm::rotate(model, glm::radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, vec3(1.5f));
    setMatrices(prog);
    mesh->render();
    
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.5f, 1.0f, 2.7f));
    model = glm::rotate(model, glm::radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, vec3(1.5f));
    setMatrices(prog);
    mesh->render();

    if (!onlyShadowCasters)
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, brickTex);
        
        color = vec3(0.5f);

        prog.setUniform("Kd", color);
        prog.setUniform("Ks", vec3(0.0f));
        prog.setUniform("Ka", vec3(0.1f));
        prog.setUniform("Shininess", 1.0f);
        
        model = mat4(1.0f);
        setMatrices(prog);
        plane.render();

        model = glm::translate(model, vec3(-5.0f, 5.0f, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f), vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(-90.0f), vec3(0.0f, 0.0f, 1.0f));
        setMatrices(prog);
        plane.render();

        model = mat4(1.0f);
        model = glm::translate(model, vec3(0.0f, 5.0f, -5.0f));
        model = glm::rotate(model, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
        setMatrices(prog);
        plane.render();

        model = mat4(1.0f);
    }
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

}

// to be called for rendering 3d models, unlikely to be edit very often
void SceneBasic_Uniform::setMatrices(GLSLProgram& prog)
{
    mv = view * model;

    // besides of setUniform, there are also setMat4, and etc. (From COMP3016)
    // set the ModelViewMatrix uniform to mv
    prog.setUniform("ModelViewMatrix", mv);

    // set the NormalMatrix uniform to following structure
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));

    // set uniform for model, view, projection (MVP) and pass in the projection matrix * model view matrix
    prog.setUniform("MVP", projection * mv);

    prog.setUniform("ProjectionMatrix", projection);
}

void SceneBasic_Uniform::setupFBO()
{
    // The depth buffer
    GLuint depthBuf;
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    // The ambient buffer
    GLuint ambBuf;
    glGenRenderbuffers(1, &ambBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, ambBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height);

    // The diffuse+specular component
    glActiveTexture(GL_TEXTURE0);
    GLuint diffSpecTex;
    glGenTextures(1, &diffSpecTex);
    glBindTexture(GL_TEXTURE_2D, diffSpecTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Create and set up the FBO
    glGenFramebuffers(1, &colorDepthFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, colorDepthFBO);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, ambBuf);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, diffSpecTex, 0);

    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, drawBuffers);

    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result == GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Framebuffer is complete. \n");
    }
    else
    {
        printf("Framebuffer is not complete. \n");
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}