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
    rotSpeed(0.2f),
    shadowMapWidth(512), shadowMapHeight(512),
    // teapot(14, glm::mat4(1.0f)),             // disabled since lab 8.2
    plane(40.0f, 40.0f, 2, 2)
    // torus(0.7f * 2.0f, 0.3f * 2.0f, 50, 50)  // disabled since lab 8.2
{
    samplesU = 4;
    samplesV = 8;
    jitterMapSize = 8;
    radius = 7.0f;

    //                       relative file location in my computer            , bool center (according to the IDE)
    mesh = ObjMesh::load("media/building.obj");   // enabled since lab 8.2 for the building model
}

// init(), initialization of everything in a scene happen in here
// Light intensity setting to be placed in here (Well actually that does not matter at all, so as all setting to be import to shader)
void SceneBasic_Uniform::initScene()
{
    compile();

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);   // lab 8.1
    
    glEnable(GL_DEPTH_TEST);

    angle = glm::quarter_pi<float>();    // lab 8.1 & 8.2 setting
    // angle = glm::two_pi<float>() * 0.85f;   // lab 8.3 setting

    // set up the framebuffer object
    setupFBO();

    buildJitterTex();   // lab 8.3

    GLuint programHandle = prog.getHandle();
    pass1Index = glGetSubroutineIndex( programHandle, GL_FRAGMENT_SHADER, "recordDepth" );
    pass2Index = glGetSubroutineIndex( programHandle, GL_FRAGMENT_SHADER, "shadeWithShadow" );
    
    // was renamed to shadowScale since lab 8.3, I don't know why
    shadowBias = mat4( vec4(0.5f, 0.0f, 0.0f, 0.0f),
                       vec4(0.0f, 0.5f, 0.0f, 0.0f), 
                       vec4(0.0f, 0.0f, 0.5f, 0.0f),
                       vec4(0.5f, 0.5f, 0.5f, 1.0f) );
        
    // float c = 1.65f; // disabled since lab 8.3
    // lightPos = vec3(0.0f, c * 5.25f, c * 7.5f); // world coords, turns to global variable since lab 8.3
    lightPos = vec3(-2.5f, 2.0f, -2.5f);    // lab 8.3 setting
    lightFrustum.orient( lightPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f) );
    // lightFrustum.setPerspective(50.0f, 1.0f, 1.0f, 25.0f);   // disabled since lab 8.3
    lightFrustum.setPerspective(40.0f, 1.0f, 0.1f, 100.0f);  // lab 8.3 setting

    lightPV = shadowBias * lightFrustum.getProjectionMatrix() * lightFrustum.getViewMatrix();

    // model, view, projection matrix initialization and setup (for 3D scene)
    //model = mat4(1.0f);
    // the view is a bit different from lab cuz I like it more
    //view = glm::lookAt(vec3(1.0f, 1.25f, 1.25f), vec3(0.0f, -0.1f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    //projection = mat4(1.0f);

    #pragma region (Light related) Light (diffuse, ambient, specular) intensity setting
    
    // prog.setUniform("Light.Intensity", vec3(0.85f)); // lab 8.1 setting

    prog.setUniform("Light.Ld", vec3(1.0f));
    prog.setUniform("Light.La", vec3(0.05f));
    prog.setUniform("Light.Ls", vec3(1.0f));

    prog.setUniform("ShadowMap", 0);    // lab 8.1

    // lab 8.3
    
    prog.setUniform("OffsetTex", 1);

    prog.setUniform("Radius", radius / 512.0f);

    prog.setUniform("OffsetTexSize", vec3(jitterMapSize, jitterMapSize, samplesU * samplesV / 2.0f));

    // lab 8.3

    #pragma endregion

    #pragma region (Disabled)Texture files linking
    
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

        // used when rendering light frustum
		solidProg.compileShader("shader/solid.vert", GLSLShader::VERTEX);
        solidProg.compileShader("shader/solid.frag", GLSLShader::FRAGMENT);
        solidProg.link();
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

    // update pass
    prog.use();

    // Pass 1 (shadow map generation)
    view = lightFrustum.getViewMatrix();
    projection = lightFrustum.getProjectionMatrix();
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, shadowMapWidth, shadowMapHeight);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass1Index);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.5f, 10.0f);
    drawScene();
    
    glDisable(GL_POLYGON_OFFSET_FILL); // lab 8.3
    
    //glCullFace(GL_BACK);  // disabled since lab 8.3
    //glFlush();            // disabled since lab 8.3
    
    //spitOutDepthBuffer(); // this is just used to get an image of the depth buffer

    // Pass 2 (render)

    // float c = 2.0f;                                                              // disabled since lab 8.2
    // float c = 1.5f;                                                                  // disabled since lab 8.3
    float c = 1.8f;                                                                         // lab 8.3
    // vec3 cameraPos(c * 11.5f * cos(angle), c * 7.0f, c * 11.5f * sin(angle));    // disabled since lab 8.2
    // vec3 cameraPos(c * cos(angle), c, c * sin(angle));                               // disabled since lab 8.3
    vec3 cameraPos(c * cos(angle), 0.7f, c * sin(angle));                                  // lab 8.3
    // view = glm::lookAt(cameraPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));           // disabled since lab 8.2
    view = glm::lookAt(cameraPos, vec3(0.0f, -0.175f, 0.0f), vec3(0.0f, 1.0f, 0.0f));   // lab 8.2

    prog.setUniform( "Light.Position", view * vec4(lightFrustum.getOrigin(), 1.0f) );

    projection = glm::perspective(glm::radians(50.0f), (float)width / height, 0.1f, 100.0f);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width, height);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass2Index);

    glDisable(GL_CULL_FACE); // lab 8.3

    drawScene();

    glFinish();

    // Draw the light's frustum
    solidProg.use();
    solidProg.setUniform("Color", vec4(1.0f, 0.0f, 0.0f, 1.0f));
    mat4 mv = view * lightFrustum.getInverseViewMatrix();
    solidProg.setUniform("MVP", projection * mv);
    lightFrustum.render();

}

#pragma endregion

void SceneBasic_Uniform::drawScene()
{
    vec3 color = vec3(0.2f, 0.5f, 0.9f);
    
    /* since lab 8.2 we are using building model instead
    prog.setUniform("Material.Kd", color);
    prog.setUniform("Material.Ka", color * 0.05f);
    prog.setUniform("Material.Ks", vec3(0.9f, 0.9f, 0.9f));
    prog.setUniform("Material.shininess", 150.0f);
    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0F));
    setMatrices();
    teapot.render();

    prog.setUniform("Material.Kd", color);
    prog.setUniform("Material.Ka", color * 0.05f);
    prog.setUniform("Material.Ks", vec3(0.9f, 0.9f, 0.9f));
    prog.setUniform("Material.shininess", 150.0f);
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 2.0f, 5.0f));
    model = glm::rotate(model, glm::radians(-45.0f), vec3(1.0f, 0.0f, 0.0f));
    setMatrices();
    torus.render();
    */

    model = mat4(1.0f);
    setMatrices();
    mesh->render();

    prog.setUniform("Material.Kd", vec3(0.25f, 0.25f, 0.25f));
    prog.setUniform("Material.Ka", vec3(0.05f, 0.05f, 0.05f));
    prog.setUniform("Material.Ks", vec3(0.0f, 0.0f, 0.0f));
    prog.setUniform("Material.shininess", 1.0f);
    model = mat4(1.0f);
    setMatrices();
    plane.render();

    model = mat4(1.0f);
    model = glm::translate(model, vec3(-5.0f, 5.0f, 0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), vec3(0.0f, 0.0f, 1.0f));
    setMatrices();
    plane.render();

    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 5.0f, -5.0f));
    model = glm::rotate(model, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
    setMatrices();
    plane.render();

    model = mat4(1.0f);
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

    prog.setUniform("ShadowMatrix", lightPV * model);
}

void SceneBasic_Uniform::setupFBO()
{
    GLfloat border[] = { 1.0f, 0.0f, 0.0f, 0.0f };

    // the depth buffer texture
    GLuint depthTex;

    glGenTextures(1, &depthTex);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, shadowMapWidth, shadowMapHeight);

    /*  lab 8.1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);*/
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

    // assign the depth buffer texture to texture channel 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTex);

    // Create and set up the FBO
    glGenFramebuffers(1, &shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_2D, depthTex, 0);

    GLenum drawBuffers[] = { GL_NONE };
    glDrawBuffers(1, drawBuffers);

    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result == GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Framebuffer is complete" << endl;
    }
    else
    {
        std::cout << "Framebuffer error:" << result << endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneBasic_Uniform::buildJitterTex()
{
    int size = jitterMapSize;
    int samples = samplesU * samplesV;
    int bufSize = size * size * samples * 2;
    float* data = new float[bufSize];

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            for (int k = 0; k < samples; k += 2)
            {
                int x1, y1, x2, y2;
                x1 = k % (samplesU);
                y1 = (samples - 1 - k) / samplesU;
                x2 = (k + 1) % samplesU;
                y2 = (samples - 1 - k - 1) / samplesU;

                vec4 v;
                // Center on grid and jitter
                v.x = (x1 + 0.5f) + jitter();
                v.y = (y1 + 0.5f) + jitter();
                v.z = (x2 + 0.5f) + jitter();
                v.w = (y2 + 0.5f) + jitter();

                // Scale between 0 and 1
                v.x /= samplesU;
                v.y /= samplesV;
                v.z /= samplesU;
                v.w /= samplesV;

                // Warp to disk
                int cell = ( (k / 2) * size * size + j * size + i ) * 4;
                data[cell + 0] = sqrtf(v.y) * cosf(glm::two_pi<float>() * v.x);
                data[cell + 1] = sqrtf(v.y) * sinf(glm::two_pi<float>() * v.x);
                data[cell + 2] = sqrtf(v.w) * cosf(glm::two_pi<float>() * v.z);
                data[cell + 3] = sqrtf(v.w) * sinf(glm::two_pi<float>() * v.z);
            }
        }
    }

    glActiveTexture(GL_TEXTURE1);
    GLuint texID;
    glGenTextures(1, &texID);

    glBindTexture(GL_TEXTURE_3D, texID);
    glTexStorage3D(GL_TEXTURE_3D, 1, GL_RGBA32F, size, size, samples / 2);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, size, size, samples / 2, GL_RGBA, GL_FLOAT, data);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    delete[] data;
}

// Return random float between -0.5 and 0.5
float SceneBasic_Uniform::jitter()
{
    static std::default_random_engine generator;
    static std::uniform_real_distribution<float> distrib(-0.5f, 0.5f);
    return distrib(generator);
}