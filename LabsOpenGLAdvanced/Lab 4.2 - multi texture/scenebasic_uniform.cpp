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

    tPrev(0)  // added in lab 3.4, for spinning logic
{
    //                       relative file location in my computer            , bool center (according to the IDE)
    // mesh = ObjMesh::load("../Lab 3.3 - pig mesh/media/pig_triangulated.obj", true);
}

// init(), initialization of everything in a scene happen in here
// Light intensity setting to be placed in here (Well actually that does not matter at all, so as all setting to be import to shader)
void SceneBasic_Uniform::initScene()
{
    compile();

    glEnable(GL_DEPTH_TEST);

    // model, view, projection matrix initialization and setup (for 3D scene)
    model = mat4(1.0f);
    // the view is a bit different from lab cuz I like it more
    view = glm::lookAt(vec3(1.0f, 1.25f, 1.25f), vec3(0.0f, -0.1f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
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

    // lab 4.2

    #pragma region Texture files linking
    
    /*
    // reference : https://www.reddit.com/r/opengl/comments/1f1wizb/how_bad_is_it_to_only_use_gl_texture0_and_what_is/
    //             https://stackoverflow.com/questions/8866904/differences-and-relationship-between-glactivetexture-and-glbindtexture
    //             https://community.khronos.org/t/when-to-use-glactivetexture/64913/2

    // Order 1 :
    // Load the texture into program
    GLuint texID = Texture::loadTexture("media/texture/brick1.jpg");
    // GLuint texID = Texture::loadTexture("media/texture/fire.png");   // different options from the lab materials

    // Order 2 :
    // pickup a texture unit that will keep affecting the whole program for to bind a texture to it
    glActiveTexture(GL_TEXTURE0);   // not really necessary, OpenGL can still bind without active anything
                                    // that brought GL_TEXTURE0 texture unit (by default)

    // assume the system somehow know it will send the texture binded in this texture unit 0
    // to somewhere in whatever shader with the "binding = 0"  in vertex
    // and then just forget it, it's meant to be written in this way

    // Order 3 :
    // the type of obj you want to declare as, obj to be bind
    glBindTexture(GL_TEXTURE_2D, texID);
    */

    // if they are not load together, the mixing of both texture will not take place
    GLuint brick = Texture::loadTexture("media/texture/brick1.jpg");
    GLuint moss = Texture::loadTexture("media/texture/moss.png");

    // texture 1 of multiple texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, brick);

    // texture 2 of multiple texture
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, moss);

    #pragma endregion

    // lab 4.2

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
    angle += 0.1f * deltaT;
    if (angle > glm::two_pi<float>()) angle -= glm::two_pi<float>();

    #pragma endregion

}

// this function stored model / mesh / object declared and awaits to be rendered in scene
void SceneBasic_Uniform::render()
{
    // this is viewport resizing logic (I figure it out by myself)
    // now the content will stick to the window size when resizing happened
    glfwGetFramebufferSize(glfwGetCurrentContext(), &viewportWidth, &viewportHeight);
    resize(viewportWidth, viewportHeight);

    // clear color buffer and clear color & depth buffers
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    #pragma region (Light related) Light settings section in render()

    /// Here it is single directional light, basic one
    // vec4 lightPos = vec4(0.0f, 10.0f, 0.0f, 1.0f);  // static position
    vec4 lightPos = vec4(10.0f * cos(angle), 10.0f, 10.0f * sin(angle), 1.0f);  // spinning rotation
    prog.setUniform("Light.Position", vec4(view * lightPos));   // Position type could varies in different type of lights

    #pragma endregion

    #pragma region (Material related) Multiple imported model setting section (Will be modified in numerous labs)

    // the color and shininess setting of the material that mapped to the model
    // that going to be effected by the light(s) setting in the scene soon in shader(s)



    //////////////////// First model(s) ////////////////////

    prog.setUniform("Material.Kd", vec3(0.2f, 0.55f, 0.9f));
    prog.setUniform("Material.Ks", vec3(0.95f, 0.95f, 0.95f));
    prog.setUniform("Material.Ka", vec3(0.2f * 0.3f, 0.55f * 0.3f, 0.9f * 0.3f));
    prog.setUniform("Material.shininess", 100.0f);

    model = mat4(1.0f);

    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    setMatrices();

    // mesh->render();  // disabled since lab 3.4
    cube.render();

    //////////////////// First model ////////////////////



    //////////////////// Second model ////////////////////

    /* standard template of setting up a model
    prog.setUniform("Material.Kd", vec3(?f, ?f, ?f));
    prog.setUniform("Material.Ks", vec3(?f, ?f, ?f));
    prog.setUniform("Material.Ka", vec3(?f, ?f, ?f));
    prog.setUniform("Material.shininess", ?f);

    /// these 3 are transform related, could be most commonly used
    // translation            obj  ,along x , y , z
    // model = glm::translate(model, vec3(?f, ?f, ?f));

    // rotation            obj  , how much degree ,along  x,  y, z
    // model = glm::rotate(model, glm::radians(?f), vec3(?f, ?f, ?f));

    // scaling            obj  ,along x , y , z
    // model = glm::scale(model, vec3(?f, ?f, ?f));
    /// more examples could refer to COMP3016 CW2

    model = mat4(1.0f);
    setMatrices();

    // it seems as a std::unique_ptr<ObjMesh> mesh
    // -> has to be use instead of . notation and so it could be render()
    // ?->render();
    ?.render();
    */

    //////////////////// Second model ////////////////////

    #pragma endregion
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
