#pragma once

#include <glm/glm.hpp>

/// <summary>
/// 
/// class Scene define what is a scene(Bullshit)
/// most importantly, this structure prevent a messy pile of codes
/// 
/// in this class it only define what is inside the scene
/// nothing less, nothing more
/// as init of the window is done in the scenerunner class
/// details is defined in its subclass, scenebasic_uniform class
/// 
/// </summary>
class Scene
{
protected:
	glm::mat4 model, view, projection;

public:
    int width;
    int height;
    float sigma2 = 8.0f;    // declared as global for instant gauss calculate
                            // declared in scene.h so that scenerunner can modify it

    // by default 800 and 600 will be the value once every scene object is created and constructor is executed
	Scene() : m_animate(true), width(800), height(600) { }
	virtual ~Scene() {}

	void setDimensions( int w, int h ) {
	    width = w;
	    height = h;
	}
	
    /**
      Load textures, initialize shaders, etc.
      */
    virtual void initScene() = 0;

    /**
      This is called prior to every frame.  Use this
      to update your animation.
      */
    virtual void update( float t ) = 0;

    /**
      Draw your scene.
      */
    virtual void render() = 0;

    /**
      Called when screen is resized
      */
    virtual void resize(int, int) = 0;
    
    void animate( bool value ) { m_animate = value; }
    bool animating() { return m_animate; }
    
protected:
	bool m_animate;
};
