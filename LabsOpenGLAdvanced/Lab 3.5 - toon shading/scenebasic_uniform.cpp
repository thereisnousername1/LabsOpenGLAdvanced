#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>

#include <string>
using std::string;

// lab 3.1

#include <sstream>

// lab 3.1

#include <iostream>
using std::cerr;
using std::endl;

#include "helper/glutils.h"

// lab 2

#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

// lab 2

using glm::vec3;

// lab 2

// lab 3.4

using glm::vec4;

using glm::mat3;

// lab 3.4

using glm::mat4;

/* disabled since lab 3.4

// SceneBasic_Uniform::SceneBasic_Uniform() : angle(0.0f) {}
// provided in lab 2
// SceneBasic_Uniform::SceneBasic_Uniform() : torus(0.7f, 0.3f, 30, 30) {}

// updated for phong model
//                                                              30, 30 // the no. of sides and no. of rings
// SceneBasic_Uniform::SceneBasic_Uniform() : torus(0.7f, 0.3f, 50, 50) {}  // disabled since lab 3.3

SceneBasic_Uniform::SceneBasic_Uniform() : plane(10.0f, 10.0f, 100, 100) 
{
    //                    relative file location in my computer            , bool center (according to the IDE)
    mesh = ObjMesh::load("../Lab 3.3 - pig mesh/media/pig_triangulated.obj", true);
}

*/

// lab 3.4

SceneBasic_Uniform::SceneBasic_Uniform() :

    tPrev(0),   // added in lab 3.4, for sinning logic
    plane(50.0f, 50.0f, 1, 1),
    teapot(14, glm::mat4(1.0f)),
    torus(1.75f * 0.75f, 1.75f * 0.75f, 50, 50)
{
    //                       relative file location in my computer            , bool center (according to the IDE)
    // mesh = ObjMesh::load("../Lab 3.3 - pig mesh/media/pig_triangulated.obj", true);
}

// lab 3.4

// lab 2

void SceneBasic_Uniform::initScene()
{
    // linking desired shaders
    compile();

    #pragma region Disabled in lab 2

    /*
    std::cout << std::endl;

    prog.printActiveUniforms();

    /////////////////// Create the VBO ////////////////////
    float positionData[] = {
        -0.8f, -0.8f, 0.0f,
         0.8f, -0.8f, 0.0f,
         0.0f,  0.8f, 0.0f };

    // RGB
    float colorData[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f };

    // Create and populate the buffer objects
    GLuint vboHandles[2];   // when it is a global parameter, it could be draw directly, but only single item in it, thats where VAO take place
    glGenBuffers(2, vboHandles);                                                        // order 1 : Gen buffer array which stored raw data(relatively speaking to
                                                                                        //           VAOs structure as it store different buffers in its attrib)

    // according to https://stackoverflow.com/questions/12102864/assist-me-to-understand-opengl-glgenbuffers, it said
    // "It just returns a list of integers that are not currently used as buffer names"
    // which means, glGenBuffers serve as creating a list for to store buffers to be draw later in the program

    // once a buffer object is created, it is also mapped to the corresponding slot in the vertex array
    // this method simplify the concept between buffer object, different slots in vertex array, and idea of binding
    GLuint positionBufferHandle = vboHandles[0];    // in this case a GLuint positionBufferHandle is connected to vboHandles[0]
    GLuint colorBufferHandle = vboHandles[1];       // GLuint colorBufferHandle is bind to vboHandles[1]

    // once a buffer object is binded the data can be edit afterwards
    glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);                                // order 2 : Bind a buffer object (in this case position is binded)
                                                                                        //           that is already stored/link to the buffer array
                                                                                        //           generated above, to declare afterwards
                                                                                        //
                                                                                        // alt method : in COMP3016, there is another tutorial
                                                                                        //              created by different person, the way is
                                                                                        //              differ from this set of lab.
                                                                                        //              the vertex array to store different buffers
                                                                                        //              is created first then the buffers to be
                                                                                        //              stored is generated and mapped.
                                                                                        //              
                                                                                        //              In current set of labs it seems doing the same
                                                                                        //              but actually,
                                                                                        //              

    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), positionData, GL_STATIC_DRAW);     // order 3 : declare that the buffer just binded
                                                                                        //           (in this case positionBufferHandle)
                                                                                        //           has the size of 9, and the related data
                                                                                        //           set is positionData, the way to draw is
                                                                                        //           GL_STATIC_DRAW

    glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);                                   // order 2 : Bind a buffer object (in this case color is binded)
                                                                                        //           that is already stored/link to the buffer array
                                                                                        //           generated above, to declare afterwards
                                                                                        //

    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), colorData, GL_STATIC_DRAW);        // order 3 : declare that the buffer just binded
                                                                                        //           (in this case colorBufferHandle)
                                                                                        //           has the size of 9, and the related data
                                                                                        //           set is colorData, the way to draw is
                                                                                        //           GL_STATIC_DRAW

    // Create and set-up the vertex array object
    glGenVertexArrays( 1, &vaoHandle );                                                 // order 1 : generate a vertex array object namely vaoHandle
    glBindVertexArray(vaoHandle);                                                       // order 2 : binding this object to modify afterwards

    // knowledge pre-requisites:
    // 
    // VAO, unlike VBO just being created as set of data array that store, of course data
    // by default it already has something called attributes(position, texture vertex, color data, normals etc.)
    // while you need to define the same data with glBufferData for VBO
    // Buffer objects with settled data is required for VAO to bind into the attrib array
    // 
    // which means, you need to set up the buffer with specific type of data sets(i.e. position, texture vertex, color data, normals etc.)
    // and then use glVertexAttribPointer(in COMP3016) or in this set of labs(COMP3015), glBindVertexBuffer and glVertexAttribBinding is used instead
    //
    glEnableVertexAttribArray(0);  // Vertex position                                   // order 3 : enable the attrib 0 (refer to position) in the attrib array 
                                                                                        //           of binded VAO (in this case it is vaoHandle)

    glEnableVertexAttribArray(1);  // Vertex color                                      // order 3 : enable the attrib 1 (refer to color) in the attrib array 
                                                                                        //           of binded VAO (in this case it is vaoHandle)

    #ifdef __APPLE__    // for mac user
        glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

        glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
        glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );
    #else
            //             index, buffer to be bind   , offset, stride(I guess it means size?)
    		glBindVertexBuffer(0, positionBufferHandle, 0, sizeof(GLfloat)*3);          // order 4 : connect buffer to binding index 0
    		glBindVertexBuffer(1, colorBufferHandle, 0, sizeof(GLfloat)*3);             // order 4 : connect buffer to binding index 1

            // setting up attrib index to bind with the relative buffer binding index
    		glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);                          // order 5

            //         <vaobject>, attrib index, binding index
            // glVertexArrayAttribBinding(vaoHandle, 0, 0);
    		glVertexAttribBinding(0, 0);                                                // order 6

            // setting up attrib index to bind with the relative buffer binding index
    		glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 0);                          // order 5

            //         <vaobject>, attrib index, binding index
            // glVertexArrayAttribBinding(vaoHandle, 1, 1);
    	    glVertexAttribBinding(1, 1);                                                // order 6
    #endif

    // unbind
    glBindVertexArray(0);
    */

    #pragma endregion

    // lab 2 - diffuse
    
    glEnable(GL_DEPTH_TEST);

    // init model matrix
    model = mat4(1.0f);

    // to make the model itself rotate, along z-axis
    //                                                       z,    y, x(?)
    // model = glm::rotate(model, glm::radians(-45.0f), vec3(1.0f, 0.0f, 0.0f));   // disabled in since lab 3.3, no longer needed since it is for the torus

    // to make the model itself rotate, along y-axis
    //                                                       z,    y, x(?)
    // model = glm::rotate(model, glm::radians(0.0f), vec3(0.0f, 1.0f, 0.0f));  // no longer needed since it is for the torus

    // init view matrix
    // view = glm::lookAt(vec3(0.0f, 0.0f, 2.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)); // changed in lab 3.3
    // view = glm::lookAt(vec3(0.5f, 0.75f, 0.75f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)); // changed in lab 3.4
    view = glm::lookAt(vec3(5.0f, 5.0f, 7.5f), vec3(0.0f, 0.75f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    // init projection matrix
    projection = mat4(1.0f);

    // lab 3.4
    
    // init float angle
    angle = 0.0f;

    // lab 3.4

    #pragma region (Disabled)lab 3.1 - the data of all lights intensity are adjusted since multiple light is added to replace what is in lab 2
    
    /* Disabled since lab 3.4
    
    // Material parts are moved to render()
    // by disable all of the diffuse intensity, 3 lights with different color settings could be observe more obviously
    
    // the diffuse light (how bright it is?) settled
    // prog.setUniform("Ld", vec3(1.0f, 1.0f, 1.0f));
    // prog.setUniform("Light.Ld", vec3(1.0f, 1.0f, 1.0f));
    prog.setUniform("lights[0].Ld", vec3(0.0f, 0.0f, 0.8f));                           // extended to a class in the vertex shader
    prog.setUniform("lights[1].Ld", vec3(0.0f, 0.8f, 0.0f));
    prog.setUniform("lights[2].Ld", vec3(0.8f, 0.0f, 0.0f));

    // the position of the light in the scene settled
    // prog.setUniform("LightPosition", view* glm::vec4(5.0f, 5.0f, 2.0f, 1.0f));
    // prog.setUniform("Light.Position", view* glm::vec4(5.0f, 5.0f, 2.0f, 1.0f));     // extended to a class in the vertex shader

    #pragma region multiple lights position setting (method to adjust variables in vertex shader)

    float x, z; // y-axis (height) remains the same

    // for all of the lights
    for (int i = 0; i < 3; i++)
    {
        std::stringstream name;
        name << "lights[" << i << "].Position";
        x = 2.0f * cosf((glm::two_pi<float>() / 3) * i);
        z = 2.0f * sinf((glm::two_pi<float>() / 3) * i);

        // set the x and z for the position of each of the lights
        prog.setUniform(name.str().c_str(), view * glm::vec4(x, 1.2f, z + 1.0f, 1.0f));
    }

    #pragma endregion

    // the ambient light (how bright it is?) settled
    // prog.setUniform("Light.La", vec3(0.4f, 0.4f, 0.4f));
    prog.setUniform("lights[0].La", vec3(0.0f, 0.0f, 0.2f));
    prog.setUniform("lights[1].La", vec3(0.0f, 0.2f, 0.0f));
    prog.setUniform("lights[2].La", vec3(0.2f, 0.0f, 0.0f));

    // the specular light (how bright it is?) settled
    // prog.setUniform("Light.Ls", vec3(1.0f, 1.0f, 1.0f));
    prog.setUniform("lights[0].Ls", vec3(0.0f, 0.0f, 0.8f));
    prog.setUniform("lights[1].Ls", vec3(0.0f, 0.8f, 0.0f));
    prog.setUniform("lights[2].Ls", vec3(0.8f, 0.0f, 0.0f));

    */

    #pragma endregion

    // Spotlight intensity settled
    // prog.setUniform("Spot.Ld", vec3(0.9f));
    prog.setUniform("Spot.Ld", vec3(0.09f));    // dreate diversity from other labs
    prog.setUniform("Spot.Ls", vec3(0.9f));
    // prog.setUniform("Spot.La", vec3(0.5f));
    prog.setUniform("Spot.La", vec3(0.3f));
    prog.setUniform("Spot.Exponent", 50.0f);
    prog.setUniform("Spot.Cutoff", glm::radians(15.0f));

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

// update should be a while(true) loop as always, or in this case the loop is defined in the scenerunner class function mainloop
// it seems float t can be used as a time factor, restriction to the program lifespan
void SceneBasic_Uniform::update( float t )
{
    #pragma region Disabled in lab 2
    /*
	//update your angle here

    // lab 1
    // try to interact with spacebar
    if (m_animate) {

        angle += 0.1f;

        if (angle >= 360.0f)

            angle -= 360.0f;

    }

    // lab 1
    */
    #pragma endregion

    // lab 3.4 spinning logic
    
    float deltaT = t - tPrev;
    if (tPrev == 0.0f) deltaT = 0.0f;
    tPrev = t;
    angle += 0.25f * deltaT;
    if (angle > glm::two_pi<float>()) angle -= glm::two_pi<float>();

    // lab 3.4

}

// this function stored model / mesh / object declared and awaits to be rendered in scene
void SceneBasic_Uniform::render()
{
    // this is viewport resizing logic (I figure it out by myself)
    // now the content will stick to the window size when resizing happened
    glfwGetFramebufferSize(glfwGetCurrentContext(), &viewportWidth, &viewportHeight);
    resize(viewportWidth, viewportHeight);

    // clear last frame
    // glClear(GL_COLOR_BUFFER_BIT);
    // lab 2
    // clear color buffer and clear color & depth buffers
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    #pragma region Disabled in lab 2 VAO / VBO setting with rotation
    /*
    // lab 1
    rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, vec3(0.0f, 0.0f, 1.0f));

    GLuint programHandle = prog.getHandle();

    GLuint location = glGetUniformLocation(programHandle, "RotationMatrix");

    glUniformMatrix4fv(location, 1, GL_FALSE, &rotationMatrix[0][0]);
    // lab 1

    //create the rotation matrix here and update the uniform in the shader 

    // draw the whole vaoHandle (vaoHandle[0] and vaoHandle[1])
    glBindVertexArray(vaoHandle);
    glDrawArrays(GL_TRIANGLES, 0, 3 );

    // unbind
    glBindVertexArray(0);
    */
    #pragma endregion

    #pragma region Disabled in lab 3.3 models settings for single directional light
    /* lab 3.2
    
    // the diffuse material settled
    // prog.setUniform("Kd", vec3(0.2f, 0.55f, 0.9f));
    prog.setUniform("Material.Kd", vec3(0.2f, 0.55f, 0.9f));                       // extended to a class in the vertex shader

    // the ambient material settled
    prog.setUniform("Material.Ka", vec3(0.2f, 0.55f, 0.9f));

    // the specular material settled
    prog.setUniform("Material.Ks", vec3(0.8f, 0.8f, 0.8f));

    // the shininess of the specular lighting settled
    prog.setUniform("Material.shininess", 100.0f);

    setMatrices();

    // lab 3.3

    // torus.render();  // disabled since lab 3.3

    plane.render();

    // it seems as a std::unique_ptr<ObjMesh> mesh
    // -> has to be use instead of . notation and so it could be render()
    mesh->render();

    // lab 3.3
    
    */
    #pragma endregion
    
    #pragma region Disabled in lab 3.4 models settings for multiple directional light
    /* lab 3.3

    // the diffuse material settled
    prog.setUniform("Material.Kd", vec3(0.4f, 0.4f, 0.4f));
    // the ambient material settled
    prog.setUniform("Material.Ka", vec3(0.5f, 0.5f, 0.5f));
    // the specular material settled
    prog.setUniform("Material.Ks", vec3(0.9f, 0.9f, 0.9f));
    // the shininess of the specular lighting settled
    prog.setUniform("Material.shininess", 180.0f);

    // modify transform.position in the scene per mesh I guess?
    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
    setMatrices();

    // plane.render();

    // it seems as a std::unique_ptr<ObjMesh> mesh
    // -> has to be use instead of . notation and so it could be render()
    // mesh->render();  // disabled since lab 3.4

    prog.setUniform("Material.Kd", vec3(0.1f, 0.1f, 0.1f));
    prog.setUniform("Material.Ka", vec3(0.7f, 0.7f, 0.7f));
    prog.setUniform("Material.Ks", vec3(0.9f, 0.9f, 0.9f));
    prog.setUniform("Material.shininess", 180.0f);

    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -0.45f, 0.0f));
    setMatrices();

    // plane.render();
    plane.render();

    */
    #pragma endregion

    // lab 3.4

    //////////////////// First model ////////////////////
    
    //Spotlight position settled (in lab 3.4 there is only one spotlight)
    // vec4 lightPos = vec4(0.0f, 10.0f, 0.0f, 1.0f);  // static position
    vec4 lightPos = vec4(10.0f * cos(angle), 10.0f, 10.0f * sin(angle), 1.0f);  // spinning rotation
    prog.setUniform("Spot.Position", vec3(view * lightPos));

    mat3 normalMatrix = mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));
    prog.setUniform("Spot.Direction", normalMatrix * vec3(-lightPos));

    // the diffuse material settled
    prog.setUniform("Material.Kd", vec3(0.2f, 0.55f, 0.9f));
    // the specular material settled
    prog.setUniform("Material.Ks", vec3(0.95f, 0.95f, 0.95f));
    // the ambient material settled
    prog.setUniform("Material.Ka", vec3(0.2f * 0.3f, 0.55f * 0.3f, 0.9f * 0.3f));
    // the shininess of the specular lighting settled
    prog.setUniform("Material.shininess", 100.0f);

    // modify transform.position in the scene per mesh I guess?
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 0.0f, -2.0f));

    //                  obj  , how much degree    ,along    x,  y  , z
    model = glm::rotate(model, glm::radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    setMatrices();

    // it seems as a std::unique_ptr<ObjMesh> mesh
    // -> has to be use instead of . notation and so it could be render()
    // mesh->render();  // disabled since lab 3.4
    teapot.render();

    //////////////////// First model ////////////////////



    //////////////////// Second model ////////////////////

    prog.setUniform("Material.Kd", vec3(0.2f, 0.55f, 0.9f));
    prog.setUniform("Material.Ks", vec3(0.95f, 0.95f, 0.95f));
    prog.setUniform("Material.Ka", vec3(0.2f * 0.3f, 0.55f * 0.3f, 0.9f * 0.3f));
    prog.setUniform("Material.shininess", 100.0f);

    model = mat4(1.0f);
    model = glm::translate(model, vec3(-1.0f, 0.75f, 3.0f));
    //                  obj  , how much degree    ,along    x,  y  , z
    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    setMatrices();

    torus.render();

    //////////////////// Second model ////////////////////



    //////////////////// Third model ////////////////////

    prog.setUniform("Material.Kd", vec3(0.7f, 0.7f, 0.7f));
    prog.setUniform("Material.Ks", vec3(0.9f, 0.9f, 0.9f));
    prog.setUniform("Material.Ka", vec3(0.2f, 0.2f, 0.2f));
    prog.setUniform("Material.shininess", 180.0f);

    model = mat4(1.0f);
    setMatrices();

    plane.render();

    //////////////////// Third model ////////////////////
}

void SceneBasic_Uniform::resize(int w, int h)
{
    // lab 2

    glViewport(0, 0, w, h);

    // lab 2

    width = w;
    height = h;

    // lab 2

    // relocated in lab 2
    // glViewport(0,0,w,h);

    // setting the aspect ratio for the model according to the window size
    // without this line it will not render
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);

    // lab 2
}

// lab 2

void SceneBasic_Uniform::setMatrices()
{
    mv = view * model;

    // besides of setUniform, there are also setMat4, and etc. (From COMP3016)
    // set the ModelViewMatrix uniform to mv
    prog.setUniform("ModelViewMatrix", mv);

    // set the NormalMatrix uniform to follwing structure
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));

    // set uniform for model, view, projection (MVP) and pass in the projection matrix * model view matrix
    prog.setUniform("MVP", projection * mv);
}

// lab 2
