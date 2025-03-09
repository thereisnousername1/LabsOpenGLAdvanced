// order = 0, first item in the include
#include <glad/glad.h>

#include "scene.h"

// order > 0
#include <GLFW/glfw3.h>

// order > 1
#include "glutils.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#include <map>
#include <string>
#include <fstream>
#include <iostream>

class SceneRunner {
private:
    GLFWwindow * window;  // the window for displaying

    int fbw, fbh;         // by default both fbw and fbh contains nothing
                          // but then they will receive value from glfwGetFramebufferSize(window, &fbw, &fbh);

	bool debug;           // Set true to enable debug messages

public:
    // logic inside this constructor is considered done in the first frame
    SceneRunner(const std::string & windowTitle, int width = WIN_WIDTH, int height = WIN_HEIGHT, int samples = 0) : debug(true) {
        // Initialize GLFW
        if( !glfwInit() ) exit( EXIT_FAILURE );

#ifdef __APPLE__
        // Select OpenGL 4.1
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );
#else
        // Select OpenGL 4.6
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
#endif
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        // glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
        if(debug) 
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
        if(samples > 0) {
            glfwWindowHint(GLFW_SAMPLES, samples);
        }

        // Open the window
        window = glfwCreateWindow( WIN_WIDTH, WIN_HEIGHT, windowTitle.c_str(), NULL, NULL );
        if( ! window ) {
			std::cerr << "Unable to create OpenGL context." << std::endl;
            glfwTerminate();
            exit( EXIT_FAILURE );
        }
        glfwMakeContextCurrent(window);

        // Get framebuffer size, put them into fbw and fbh (&fbw, &fbh)
        // now fbw = 800, fbh = 600
        std::cout << "Before : " << fbw << " , " << fbh << std::endl;
        glfwGetFramebufferSize(window, &fbw, &fbh);
        std::cout << "After : " << fbw << " , " << fbh << std::endl;

        // Load the OpenGL functions.
        if(!gladLoadGL()) { exit(-1); }

        GLUtils::dumpGLInfo();

        // Initialization
        // according to https://registry.khronos.org/OpenGL-Refpages/gl4/html/glClearColor.xhtml, it said
        // "Specify the red, green, blue, and alpha values used when the color buffers are cleared."
        // now background color is grey(0.5f,0.5f,0.5f,1.0f) even though the screen is clear with glClear()
        // to be more specific, glClearColor means clear the screen with color on it
        glClearColor(0.5f,0.5f,0.5f,1.0f);
#ifndef __APPLE__
		if (debug) {
			glDebugMessageCallback(GLUtils::debugCallback, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
			glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0,
				GL_DEBUG_SEVERITY_NOTIFICATION, -1, "Start debugging");
		}
#endif
    }

    // logic inside this run function is considered done in the second frame and so on
    // the window is defined then the scene object with its display size and everything is also defined
    // i.e. the window size and scene display size are different things
    int run(Scene & scene) {
        scene.setDimensions(fbw, fbh);
        scene.initScene();
        scene.resize(fbw, fbh);

        // Enter the main loop
        mainLoop(window, scene);

#ifndef __APPLE__
		if( debug )
			glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 1,
				GL_DEBUG_SEVERITY_NOTIFICATION, -1, "End debug");
#endif

		// Close window and terminate GLFW
		glfwTerminate();

        // Exit program
        return EXIT_SUCCESS;
    }

    static std::string parseCLArgs(int argc, char ** argv, std::map<std::string, std::string> & sceneData) {
        if( argc < 2 ) {
            printHelpInfo(argv[0], sceneData);
            exit(EXIT_FAILURE);
        }

        std::string recipeName = argv[1];
        auto it = sceneData.find(recipeName);
        if( it == sceneData.end() ) {
            printf("Unknown recipe: %s\n\n", recipeName.c_str());
            printHelpInfo(argv[0], sceneData);
            exit(EXIT_FAILURE);
        }

        return recipeName;
    }

private:
    static void printHelpInfo(const char * exeFile,  std::map<std::string, std::string> & sceneData) {
        printf("Usage: %s recipe-name\n\n", exeFile);
        printf("Recipe names: \n");
        for( auto it : sceneData ) {
            printf("  %11s : %s\n", it.first.c_str(), it.second.c_str());
        }
    }

    void mainLoop(GLFWwindow * window, Scene & scene) {
        while( ! glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE) ) {
            GLUtils::checkForOpenGLError(__FILE__,__LINE__);
			
            // if an input() method is created somewhere, it should not be called in here, instead call it in update() method
            scene.update(float(glfwGetTime()));
            // geez it is always 800 and 600, what is the point?
            // std::cout << "Update : " << fbw << " , " << fbh << std::endl;
            scene.render();
            glfwSwapBuffers(window);

            glfwPollEvents();
			int state = glfwGetKey(window, GLFW_KEY_SPACE);
			if (state == GLFW_PRESS)
				scene.animate(!scene.animating());

            // added for lab 5.2 & 5.4 only
            Input(window, &scene.sigma2, &scene.Gamma);
        }
    }

    // input logic added for lab 5.2 to adjust gaussian value sigma2
    void Input(GLFWwindow* WindowIn, float* num, float* num2)
    {
        if (glfwGetKey(WindowIn, GLFW_KEY_I) == GLFW_PRESS)
        {
            // std::cout << "Hello" << std::endl;
            *num += 1.0f;
            std::cout << "New sigma2 value: " << *num << std::endl;
        }

        if (glfwGetKey(WindowIn, GLFW_KEY_K) == GLFW_PRESS)
        {
            // std::cout << "Hello" << std::endl;
            *num -= 1.0f;
            std::cout << "New sigma2 value: " << *num << std::endl;
        }

        if (glfwGetKey(WindowIn, GLFW_KEY_O) == GLFW_PRESS)
        {
            // std::cout << "Hello" << std::endl;
            *num2 += 0.1f;
            std::cout << "New gamma value: " << *num2 << std::endl;
        }

        if (glfwGetKey(WindowIn, GLFW_KEY_L) == GLFW_PRESS)
        {
            // std::cout << "Hello" << std::endl;
            *num2 -= 0.1f;
            std::cout << "New gamma value: " << *num2 << std::endl;
        }
    }
};
