#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>

#include <string>
using std::string;

#include <iostream>
using std::cerr;
using std::endl;

#include "helper/glutils.h"

using glm::vec3;

SceneBasic_Uniform::SceneBasic_Uniform() : angle(0.0f) {}

void SceneBasic_Uniform::initScene()
{
    // linking desired shaders
    compile();

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
	//update your angle here

    // lab 1
    // try to interact with spacebar
    if (m_animate) {

        angle += 0.1f;

        if (angle >= 360.0f)

            angle -= 360.0f;

    }

    // lab 1
}

void SceneBasic_Uniform::render()
{
    // clear last frame
    glClear(GL_COLOR_BUFFER_BIT);
    
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
}

void SceneBasic_Uniform::resize(int w, int h)
{
    width = w;
    height = h;
    glViewport(0,0,w,h);
}
