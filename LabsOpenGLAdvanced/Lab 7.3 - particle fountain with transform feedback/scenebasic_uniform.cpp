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
#include "helper/particleutils.h"   // added in lab 7.3

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

// initialization of mainly 3D models in a scene happen in here, but well I don't care
SceneBasic_Uniform::SceneBasic_Uniform() :

    // rotation logic related
    tPrev(0), 
    angle(0.0f), 
    drawBuf(1), // lab 7.3
    time(0),    // lab 7.1
    deltaT(0),  // lab 7.3
    rotSpeed(glm::pi<float>() / 8.0f),
    // plane(13.0, 10.0f, 200, 2)  // lab 7.1
    // particleLifeTime(5.5f),
    particleLifeTime(6.0f), // lab 7.3
    // nParticles(8000),
    nParticles(4000),       // lab 7.3
    emitterPos(1, 0, 0),
    emitterDir(-1, 2, 0)
{
    //                    relative file location in my computer            , bool center (according to the IDE)
    // mesh = ObjMesh::load("media/bs_ears.obj");
    // mesh = ObjMesh::loadWithAdjacency("media/bs_ears.obj");    // disabled in lab 6.4
}

// init(), initialization of everything in a scene happen in here
// Light intensity setting to be placed in here (Well actually that does not matter at all, so as all setting to be import to shader)
void SceneBasic_Uniform::initScene()
{
    compile();  // compile, link and use shaders

    //glClearColor(0.5f, 0.5f, 0.5f, 1.0f);   // set up a background color
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);   // set up a background color

    // lab 7.2
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);    // enable depth test

    angle = glm::pi<float>() / 2.0f;

    // angle = 0
    initBuffers();

    // lab 7.2

    #pragma region (Light related) Light (diffuse, ambient, specular) intensity setting

    ///////////// Uniforms ////////////////////

    /*  moved to render()
    prog.setUniform("Material.Kd", 0.7f, 0.7f, 0.7f);
    prog.setUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
    prog.setUniform("Material.Ks", 0.8f, 0.8f, 0.8f);
    prog.setUniform("Material.shininess", 100.0f);*/

    /*prog.setUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));
    
    prog.setUniform("Light.Ld", vec3(1.0f));
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

    // lab 7.3

    prog.use();
    prog.setUniform("ParticleTex", 0);
    prog.setUniform("ParticleLifetime", particleLifeTime);
    prog.setUniform("ParticleSize", 0.05f);
    // prog.setUniform("Gravity", vec3(0.0f, -0.2f, 0.0f)); // disabled in lab 7.3
    prog.setUniform("EmitterPos", emitterPos);

    prog.setUniform("RandomTex", 1);
    prog.setUniform("Accel", vec3(0.0f, -0.5f, 0.0f));
    prog.setUniform("EmitterBasis", ParticleUtils::makeArbitraryBasis(emitterDir));

    flatProg.use();
    // it is flatProg.setUniform( "Color", glm::vec4(0.3f, 0.3f, 0.3f, 1.0f) ); in the lab, but forget it
    flatProg.setUniform( "Color", glm::vec4(0.4f, 0.4f, 0.4f, 1.0f) );

    // lab 7.3
    #pragma endregion

    #pragma region Texture files linking
    
    // load texture file
    // const char* texName = "media/texture/flower.png";
    // Texture::loadTexture(texName);

    glActiveTexture(GL_TEXTURE0);
    // Texture::loadTexture("media/texture/flower.png");
    Texture::loadTexture("media/texture/fire.png");

    // lab 7.3

    glActiveTexture(GL_TEXTURE1);
    ParticleUtils::createRandomTex1D(nParticles * 3);

    // lab 7.3
    #pragma endregion
}

void SceneBasic_Uniform::compile()
{
    // compile mr prog with the desired shaders to be used in this scene
	try {

        // lab 7.3

		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");

        //////////////////////////////////////////////////////
        // Setup the transform feedback (must be done before linking the program)
        GLuint progHandle = prog.getHandle();
        const char* outputNames[] = { "Position", "Velocity", "Age" };
        glTransformFeedbackVaryings( progHandle, 3, outputNames, GL_SEPARATE_ATTRIBS );
        ///////////////////////////////////////////////////////

		prog.link();
        prog.use();

		flatProg.compileShader("shader/flat.vert");
        flatProg.compileShader("shader/flat.frag");
        flatProg.link();

        // lab 7.3

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
    deltaT = t - time;

    time = t;   // added since lab 7.1

    #pragma region Spinning logic

    /*float deltaT = t - tPrev;

    if (tPrev == 0.0f)
        deltaT = 0.0f;
    tPrev = t;*/

    if (m_animate)
    {
        // it was               + 0.01f in the lab, I like it slower
        angle = std::fmod(angle + 0.005f, glm::two_pi<float>());

        // angle += rotSpeed * deltaT;  // been replaced in lab 7.2

        if (angle > glm::two_pi<float>())
            angle -= glm::two_pi<float>();
    }

    #pragma endregion
}

void SceneBasic_Uniform::initBuffers()
{
    // Generate the buffers for initial velocity and start (birth) time
    //glGenBuffers(1, &initVel);      // Initial velocity buffer
    //glGenBuffers(1, &startTime);    // Start time buffer

    // Generate the buffers
    glGenBuffers(2, posBuf);    // position buffers
    glGenBuffers(2, velBuf);    // velocity buffers
    glGenBuffers(2, age);       // age buffers

    // Allocate space for all buffers
    /*int size = nParticles * sizeof(float);
    glBindBuffer(GL_ARRAY_BUFFER, initVel);
    glBufferData(GL_ARRAY_BUFFER, size * 3, 0, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, startTime);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_STATIC_DRAW);*/

    // Allocate space for all buffers
    int size = nParticles * 3 * sizeof(GLfloat);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, age[0]);
    glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, age[1]);
    glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), 0, GL_DYNAMIC_COPY);

    /*  lab 7.2
    // Fill the first velocity buffer with random velocities
    glm::mat3 emitterBasis = ParticleUtils::makeArbitraryBasis(emitterDir);
    vec3 v(0.0f);
    float velocity, theta, phi;
    std::vector<GLfloat> data(nParticles * 3);
    for (uint32_t i = 0; i < nParticles; i++)
    {
        // Pick the direction of the velocity
        theta = glm::mix(0.0f, glm::pi<float>() / 20.0f, randFloat());
        phi = glm::mix(0.0f, glm::two_pi<float>(), randFloat());

        v.x = sinf(theta) * cosf(phi);
        v.y = cosf(theta);
        v.z = sinf(theta) * sinf(phi);

        // scale to set the magnitude of the velocity
        velocity = glm::mix(1.25f, 1.5f, randFloat());
        v = glm::normalize(emitterBasis * v) * velocity;

        data[3 * i] = v.x;
        data[3 * i + 1] = v.y;
        data[3 * i + 2] = v.z;
    }

    glBindBuffer(GL_ARRAY_BUFFER, initVel);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size * 3, data.data());

    // Fill the start time buffer
    float rate = particleLifeTime / nParticles;
    for (int i = 0; i < nParticles; i++)
    {
        data[i] = rate * i;
    }

    glBindBuffer(GL_ARRAY_BUFFER, startTime);
    glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), data.data());

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &particles);
    glBindVertexArray(particles);

    glBindBuffer(GL_ARRAY_BUFFER, initVel);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, startTime);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glVertexAttribDivisor(0, 1);
    glVertexAttribDivisor(1, 1);
    */

    // Fill the first age buffer
    std::vector<GLfloat> tempData(nParticles);  // former data from above

    float rate = particleLifeTime / nParticles;

    for (int i = 0; i < nParticles; i++)
    {
        tempData[i] = rate * (i - nParticles);
    }

    glBindBuffer(GL_ARRAY_BUFFER, age[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), tempData.data());

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Create vertex arrays for each set of buffers
    glGenVertexArrays(2, particleArray);   // was &particles, but in lab 7.3 we use array instead

    // Set up particle array 0
    glBindVertexArray(particleArray[0]);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, age[0]);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    // Set up particle array 1
    glBindVertexArray(particleArray[1]);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, age[1]);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // Set up the feedback objects
    glGenTransformFeedbacks(2, feedback);

    // Transform feedback 0
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[0]);

    // Transform feedback 1
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[1]);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
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
    // vec3 cameraPos(2.0f * sin(angle), 0.0f, 2.0f * cos(angle));   // spinning logic applied
    //vec3 cameraPos(4.25f * cos(angle), 3.0f, 4.25f * sin(angle));   // lab 6.6
    //vec3 cameraPos(10.0f * cos(angle), 4.0f, 10.0f * sin(angle));   // lab 7.1
    vec3 cameraPos(3.0f * cos(angle), 1.5f, 3.0f * sin(angle));   // lab 7.2
    // view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    view = glm::lookAt(cameraPos, vec3(0.0f, 1.5f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    model = mat4(1.0f);

    flatProg.use();
    //// model = glm::translate( model, vec3(0.0f, -1.5f, 0.0f) );    // disabled since lab 7.1
    //model = glm::rotate( model, glm::radians(-10.0f), vec3(0.0f, 0.0f, 1.0f) );
    //model = glm::rotate( model, glm::radians(50.0f), vec3(1.0f, 0.0f, 0.0f) );

    //prog.setUniform("Time", time);  // added since lab 7.1
    //
    //prog.setUniform("Material.Kd", 0.2f, 0.5f, 0.9f);
    //prog.setUniform("Material.Ka", 0.2f, 0.5f, 0.9f);
    //prog.setUniform("Material.Ks", 0.8f, 0.8f, 0.8f);
    //prog.setUniform("Material.shininess", 100.0f);

    setMatrices(flatProg);  //set up your matrices and send to shaders

    grid.render();

    /* lab 7.2
    glDepthMask(GL_FALSE);
    prog.use();
    setMatrices(prog);
    prog.setUniform("Time", time);
    glBindVertexArray(particles);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticles);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);*/

    prog.use();
    prog.setUniform("Time", time);
    prog.setUniform("DeltaT", deltaT);

    // Update pass
    prog.setUniform("Pass", 1);

    glEnable(GL_RASTERIZER_DISCARD);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);
    glBeginTransformFeedback(GL_POINTS);

    glBindVertexArray( particleArray[1 - drawBuf] );
    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 0);
    glVertexAttribDivisor(2, 0);
    glDrawArrays(GL_POINTS, 0, nParticles);
    glBindVertexArray(0);

    glEndTransformFeedback();
    glDisable(GL_RASTERIZER_DISCARD);

    // Render pass
    prog.setUniform("Pass", 2);

    setMatrices(prog);

    glDepthMask(GL_FALSE);

    glBindVertexArray( particleArray[drawBuf] );
    glVertexAttribDivisor(0, 1);
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticles);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);

    // Swap buffers
    drawBuf = 1 - drawBuf;

}

#pragma endregion

// unlikely to be edit very often
void SceneBasic_Uniform::resize(int w, int h)
{

    glViewport(0, 0, w, h);

    width = w;
    height = h;

    // remember to put the   vao/fbo/whatever setup logic here instead of init() to keep the resizing logic functional
    // setupFBO();

    // setting the aspect ratio for the model according to the window size
    // without this line it will not render
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);

    float w2 = w / 2.0f;
    float h2 = h / 2.0f;

    /*viewport = mat4( vec4(w2, 0.0f, 0.0f, 0.0f),
                     vec4(0.0f, h2, 0.0f, 0.0f),
                     vec4(0.0f, 0.0f, 1.0f, 0.0f),
                     vec4(w2 + 0, h2 + 0, 0.0f, 1.0f) );*/  // since lab 7.1

}

// to be called for rendering 3d models, unlikely to be edit very often
void SceneBasic_Uniform::setMatrices()
{
    mv = view * model; // model view matrix

    prog.use(); // lab 6.4

    // besides of setUniform, there are also setMat4, and etc. (From COMP3016)
    // set the ModelViewMatrix uniform to mv
    prog.setUniform("ModelViewMatrix", mv);  // lab 6.6

    // set the NormalMatrix uniform to following structure
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));   // lab 6.6

    // set uniform for model, view, projection (MVP) and pass in the projection matrix * model view matrix
    prog.setUniform("MVP", projection * mv);

    // prog.setUniform("ProjectionMatrix", projection); // send the projection matrix

    //prog.setUniform("ViewportMatrix", viewport); // lab 6.5
}

void SceneBasic_Uniform::setMatrices(GLSLProgram& p)
{
    mv = view * model; // model view matrix

    p.setUniform("ModelViewMatrix", mv);

    p.setUniform("MVP", projection * mv);

    p.setUniform("ProjectionMatrix", projection);
}