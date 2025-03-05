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

    // lab 4.6

    // angle(0.0f),
    angle(90.0f),

    // rotSpeed(glm::pi<float>() / 2.0f)
    rotSpeed(glm::pi<float>() / 8.0f),   // rotation speed decreased

    teapot(14, glm::mat4(1.0f)),    // enabled in lab 4.6

    sky(100.0f) // skybox is gen with 6 faces like a cube,
    // basically a triangle mesh just getting cubic, and then enter the size of the skybox ~ JJ in lab 4 part 6 video
{
    //                          file name, bool center (according to the IDE), bool genTangents (according to the IDE)
    // ogre = ObjMesh::load("media/bs_ears.obj", false, true);  // disabled since lab 4.5

    // lab 4.6
}

// init(), initialization of everything in a scene happen in here
// Light intensity setting to be placed in here (Well actually that does not matter at all, so as all setting to be import to shader)
void SceneBasic_Uniform::initScene()
{
    compile();

    glEnable(GL_DEPTH_TEST);

    // model, view, projection matrix initialization and setup (for 3D scene)
    // model = mat4(1.0f);  // not necessary for skybox, disabled since lab 4.5

    // the view is a bit different from lab cuz I like it more
    // view = glm::lookAt(vec3(1.0f, 1.25f, 1.25f), vec3(0.0f, -0.1f, 0.0f), vec3(0.0f, 1.0f, 0.0f));   // moved to render() since lab 4.3
    projection = mat4(1.0f);
    
    // init float angle, duplicate setting cuz it has been declared in constructor
    // angle = 0.0f;    // not necessary for skybox, disabled since lab 4.5

    #pragma region (Disabled) (Light related) Light (diffuse, ambient, specular) intensity setting
    
    // not necessary for skybox, disabled since lab 4.5
    // prog.setUniform("Light.Ld", vec3(1.0f));
    // prog.setUniform("Light.La", vec3(0.05f));
    // prog.setUniform("Light.Ls", vec3(1.0f));

    #pragma endregion

    // lab 4.5 - edited for readability

    #pragma region Texture files linking
    
    // if they are not load together, the mixing of both texture will not take place
    // GLuint diffTex = Texture::loadTexture("media/texture/ogre_diffuse.png");

    // it seems the program will sample the skybox itself
    GLuint cubeTex = Texture::loadHdrCubeMap("media/texture/cube/pisa-hdr/pisa");

    // texture 1 of multiple texture
    glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, diffTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);

    #pragma endregion

    // lab 4.
}

void SceneBasic_Uniform::compile()
{
    // compile mr prog with the desired shaders to be used in this scene
	try {
		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");

        // for skybox only
		skyProg.compileShader("shader/skybox.vert");
        skyProg.compileShader("shader/skybox.frag");

        // no need to add skyProg.use()
        skyProg.link();

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

    if (tPrev == 0.0f) deltaT = 0.0f;
    tPrev = t;
    // angle += 0.1f * deltaT;  // changed since lab 4.5
    angle += rotSpeed * deltaT;

    // lab 4.5
    if (angle > glm::two_pi<float>()) angle -= glm::two_pi<float>();

    /* lab 4.3
    if (this->m_animate)
    {
        // it is just modifing the variable angle more rapidly
        angle += rotSpeed * deltaT;

        if (angle > glm::two_pi<float>()) angle -= glm::two_pi<float>();
    }

    */

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

    #pragma region (Light, viewer related) Light and camera settings section in render()

    /// Here it is single directional light, basic one
    // vec4 lightPos = vec4(0.0f, 10.0f, 0.0f, 1.0f);  // static position
    
    // lab 4.5 - camera movement logic added since lab 4.3

    // vec4 lightPos = vec4(10.0f * cos(angle), 10.0f, 10.0f * sin(angle), 1.0f);  // spinning rotation
    // vec3 focus = vec3(6.0f * cos(angle), 1.0f, 6.0f * sin(angle));  // lab 4.3, name is changed to cameraPos in lab
    // vec3 focus = vec3(-1.0f * cos(angle), 0.25f, 2.0f * sin(angle));  // lab 4.4
    vec3 focus = vec3(7.0f * cos(angle), 2.0f, 7.0f * sin(angle));  // lab 4.5

    /// SWITCHABLE ///

    // view = glm::lookAt(vec3(-1.0f, 0.25f, 2.0f), vec3(0.0f, -0.1f, 0.0f), vec3(0.0f, 1.0f, 0.0f));   // static camera position
    // view = glm::lookAt(focus, vec3(0.0f, -0.1f, 0.0f), vec3(0.0f, 1.0f, 0.0f));  // camera x is starring at a point in the void, focus
    view = glm::lookAt(focus, vec3(0.0f, 0.2f, 0.0f), vec3(0.0f, 1.0f, 0.0f));  // modified for lab 4.6 only

    /// SWITCHABLE ///

    // prog.setUniform("Light.Position", vec4(view * lightPos));   // Position of light was changing dynamically

    // Dynamically changing position logic of light using in lab 4.4
    prog.setUniform("Light.Position", view * glm::vec4(10.0f * cos(angle), 1.0f, 10.0f * sin(angle), 1.0f));

    // prog.setUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));   // Position of light is now static

    // lab 4.5

    #pragma endregion

    #pragma region (Skybox related) Skybox setting section

    // Prog.use();  // renamed since lab 4.6
    skyProg.use();

    model = mat4(1.0f);

    // setMatrices();
    setMatrices(skyProg);

    sky.render();

    #pragma endregion

    // lab 4.6

    // using another shader program to render other objects
    prog.use();

    // WorldCameraPosition seems to be newly introduce from lab 4.6
    // prog.setUniform("WorldCameraPosition", cameraPos);    // renamed for my program
    prog.setUniform("WorldCameraPosition", focus);

    // lab 4.6

    #pragma region (Material related) Multiple imported model setting section (Will be modified in numerous labs)

    /* disabled since lab 4.5

    // the color and shininess setting of the material that mapped to the model
    // that going to be effected by the light(s) setting in the scene soon in shader(s)

    // each labs may require different mesh / model to be render, the scripts will be changed without mentioned with comments

    //////////////////// First model(s) ////////////////////

    

    // prog.setUniform("Material.Kd", vec3(0.2f, 0.55f, 0.9f));
    // prog.setUniform("Material.Ks", vec3(0.95f, 0.95f, 0.95f));
    // prog.setUniform("Material.Ka", vec3(0.2f * 0.3f, 0.55f * 0.3f, 0.9f * 0.3f));
    // prog.setUniform("Material.shininess", 100.0f);

    // model = mat4(1.0f);

    // setMatrices();

    // ogre->render();
    // cube.render();

    //////////////////// First model ////////////////////



    //////////////////// Second model ////////////////////

    /* standard template of setting up a model
    prog.setUniform("Material.Kd", vec3(?f, ?f, ?f));
    prog.setUniform("Material.Ks", vec3(?f, ?f, ?f));
    prog.setUniform("Material.Ka", vec3(?f, ?f, ?f));
    prog.setUniform("Material.shininess", ?f);

    model = mat4(1.0f);
    setMatrices();

    /// these 3 are transform related, could be most commonly used
    // translation            obj  ,along x , y , z
    // model = glm::translate(model, vec3(?f, ?f, ?f));

    // rotation            obj  , how much degree ,along  x,  y, z
    // model = glm::rotate(model, glm::radians(?f), vec3(?f, ?f, ?f));

    // scaling            obj  ,along x , y , z
    // model = glm::scale(model, vec3(?f, ?f, ?f));
    /// more examples could refer to COMP3016 CW2

    // it seems as a std::unique_ptr<ObjMesh> mesh
    // -> has to be use instead of . notation and so it could be render()
    // ?->render();
    ?.render();
    */

    //////////////////// Second model ////////////////////



    //////////////////// Model for reflection - lab 4.6 ////////////////////

    // in the tutorial video lab 4 part 6b a simple phong model and the related set of data is used
    // prog.setUnifrorm("MaterialColor", glm::vec4(0.5f, 0.5f, 0.5f, 0.5f));
    // it is still phong model for the final output is still vec4 in the end
    // no matter the complete one where you have to set Kd, Ks, Ka or this simple structure
    // bu then again, I choose to keep the complete structure by copying my past solutions

    // set color with the phong model struct
    prog.setUniform("Material.Kd", vec3(0.5f, 0.5f, 0.5f));
    prog.setUniform("Material.Ks", vec3(0.5f, 0.5f, 0.5f));
    //prog.setUniform("Material.Ka", vec3(0.5f));

    // set color in simple way
    prog.setUniform("MaterialColor", glm::vec4(0.5f, 0.5f, 0.5f, 0.5f));
    
    // not needed for reflection
    prog.setUniform("Material.shininess", 0.85f);

    prog.setUniform("ReflectFactor", 0.85f);

    model = mat4(1.0f);

    /// these 3 are transform related, could be most commonly used
    // translation            obj  ,along x , y , z
    // model = glm::translate(model, vec3(?f, ?f, ?f));

    // rotation            obj  , how much degree ,along  x,  y, z
    // model = glm::rotate(model, glm::radians(?f), vec3(?f, ?f, ?f));
    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));

    // scaling            obj  ,along x , y , z
    // model = glm::scale(model, vec3(?f, ?f, ?f));
    /// more examples could refer to COMP3016 CW2

    // an advancement to the setMatrices(), more flexible
    setMatrices(prog);

    // it seems as a std::unique_ptr<ObjMesh> mesh
    // -> has to be use instead of . notation and so it could be render()
    // ?->render();
    teapot.render();
    //

    //////////////////// Model for reflection - lab 4.6 ////////////////////

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

// lab 4.6

/* to be called for rendering 3d models, unlikely to be edit very often
void SceneBasic_Uniform::setMatrices()
{
    mv = view * model;

    // besides of setUniform, there are also setMat4, and etc. (From COMP3016)
    // set the ModelViewMatrix uniform to mv
    prog.setUniform("ModelViewMatrix", mv);

    // not using in lab 4.6
    // set the NormalMatrix uniform to following structure
    // prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));

    // set uniform for model, view, projection (MVP) and pass in the projection matrix * model view matrix
    prog.setUniform("MVP", projection * mv);
}
*/

void SceneBasic_Uniform::setMatrices(GLSLProgram &p)
{
    mv = view * model;

    // besides of setUniform, there are also setMat4, and etc. (From COMP3016)
    // set the ModelViewMatrix uniform to mv
    // p.setUniform("ModelViewMatrix", mv);

    // p.setUniform("ModelMatrix", mv);    // this provide a variation of reflection
    p.setUniform("ModelMatrix", model);    // this provide another variation of reflection

    // not using in lab 4.6
    // set the NormalMatrix uniform to following structure
    // p.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));

    // set uniform for model, view, projection (MVP) and pass in the projection matrix * model view matrix
    p.setUniform("MVP", projection * mv);
}

// lab 4.6