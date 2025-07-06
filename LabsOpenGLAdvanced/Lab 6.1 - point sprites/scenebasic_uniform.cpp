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
    // mesh = ObjMesh::load("media/spot/spot_triangulated.obj");    // moved to initScene()
}

// init(), initialization of everything in a scene happen in here
// Light intensity setting to be placed in here (Well actually that does not matter at all, so as all setting to be import to shader)
void SceneBasic_Uniform::initScene()
{
    compile();  // compile, link and use shaders

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);   // set up a background color

    glEnable(GL_DEPTH_TEST);    // enable depth test

    // model, view, projection matrix initialization and setup (for 3D scene)
    // model = mat4(1.0f); // moved to render()
    // the view is a bit different from lab cuz I like it more
    // view = glm::lookAt(vec3(1.0f, 1.25f, 1.25f), vec3(0.0f, -0.1f, 0.0f), vec3(0.0f, 1.0f, 0.0f));   // moved to render()

    projection = mat4(1.0f);

    // angle = glm::pi<float>() / 4.0f; // rotate again since lab 5.4
    angle = glm::pi<float>() / 2.0f;

    // set up 50 sprites
    numSprites = 50;
    locations = new float[numSprites * 3];  // we need an x, y, and z for each sprite
    srand( (unsigned int)time(0) );         // assign random value, used later for location

    // for each sprite we set up the location in an array
    for (int i = 0; i < numSprites; i++)
    {
        vec3 p( ((float)rand() / RAND_MAX * 2.0f) - 1.0f,   // x in the vec3
                ((float)rand() / RAND_MAX * 2.0f) - 1.0f,   // y in the vec3
                ((float)rand() / RAND_MAX * 2.0f) - 1.0f ); // z in the vec3

        locations[i * 3] = p.x;
        locations[i * 3 + 1] = p.y;
        locations[i * 3 + 2] = p.z;
    }

    // set up the buffers
    GLuint handle;
    glGenBuffers(1, &handle);

    glBindBuffer(GL_ARRAY_BUFFER, handle);
    glBufferData(GL_ARRAY_BUFFER, numSprites * 3 * sizeof(float), locations, GL_STATIC_DRAW);
    // they have the same meaning and similar structure
    // glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    // glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);

    // in here the locations[] array was stored in memory(RAM), it has been loaded to the graphic memory
    // therefor the tutorial is deleting it to free the memory
    delete[] locations;

    // Set up the vertex array object
    glGenVertexArrays(1, &sprites);    // former quad
    glBindVertexArray(sprites);

    glBindBuffer(GL_ARRAY_BUFFER, handle);

    // former glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0); instead I don't fucking know what the fuck is this ((GLubyte*)NULL + (0))
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)) );
    glEnableVertexAttribArray(0); // Vertex position

    glBindVertexArray(0);

    #pragma region (Disabled)(Light related) Light (diffuse, ambient, specular) intensity setting

    /*prog.setUniform("Light.Ld", vec3(1.0f));
    prog.setUniform("Light.La", vec3(0.05f));
    prog.setUniform("Light.Ls", vec3(1.0f));*/

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

    #pragma region Texture files linking

    // texture linking has returned after whole lab 5
    
    // load texture file
    const char* texName = "media/texture/flower.png";
    Texture::loadTexture(texName);

    #pragma endregion

    prog.setUniform("SpriteTex", 0);
    prog.setUniform("Size2", 0.15f);
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

    #pragma region (Disabled)lab 3.4 spinning logic
    
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

    glBindVertexArray(sprites);
    glDrawArrays(GL_POINTS, 0, numSprites);

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
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);

}

// to be called for rendering 3d models, unlikely to be edit very often
void SceneBasic_Uniform::setMatrices()
{
    mv = view * model; // model view matrix

    // besides of setUniform, there are also setMat4, and etc. (From COMP3016)
    // set the ModelViewMatrix uniform to mv
    prog.setUniform("ModelViewMatrix", mv);

    // set the NormalMatrix uniform to following structure
    // prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));

    // set uniform for model, view, projection (MVP) and pass in the projection matrix * model view matrix
    // prog.setUniform("MVP", projection * mv);

    prog.setUniform("ProjectionMatrix", projection); // send the projection matrix
}