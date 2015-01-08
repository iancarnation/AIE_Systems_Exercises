#include "AI_Assessment.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include "Agent.h"

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

/*
-- Action Behaviors --
- Go To Food (SeekTarget)
- Get Path (random point for now (RandomizeTarget))
- Go To Next Waypoint (SeekTarget)

-- Condition Behaviors --
- Food Detected?
- Need New Path?

-- Composite Behaviors
- Root (sel)
- Eat Food (seq)
- Roam (sel)
- Have Path (seq)
*/

class RandomizeTarget : public Behavior
{
public:

	RandomizeTarget(float a_radius) : radius(a_radius) {}
	virtual ~RandomizeTarget() {}

	virtual Status execute(Agent* a_agent)
	{
		glm::vec3 target(0);

		target.xz = glm::circularRand(radius);

		a_agent->setTarget(target);
		return SUCCESS;
	}

	float radius;
};

class SeekTarget : public Behavior
{
public: 

	SeekTarget(float a_speed) : speed(a_speed) {}
	virtual ~SeekTarget() {}

	virtual Status execute(Agent* agent)
	{
		glm::vec3 pos = agent->getPosition();
		glm::vec3 dir = glm::normalize(agent->getTarget() - pos);

		agent->setPosition(pos + dir * speed * Utility::getDeltaTime());
		return SUCCESS;
	}

	float speed;
};

// == Ian Code =========

std::vector<glm::vec3*> foodPositions;

class FoodDetected : public Behavior
{
public:

	FoodDetected(float a_range) : range2(a_range*a_range) {}
	virtual ~FoodDetected() {}

	virtual Status execute(Agent* agent)
	{
		for (auto foodPos = foodPositions.begin(); foodPos != foodPositions.end(); foodPos++)
		{
			glm::vec3 **fPvalue = &*foodPos; // dereferenced value of food position vector
			float dist2 = glm::distance2(agent->getPosition(), fPvalue);

			if (dist2 <= range2)
				return SUCCESS;
		}

		return FAILURE;
	}

	float range2;
};

// =====================



AI_Assessment::AI_Assessment()
{

}

AI_Assessment::~AI_Assessment()
{

}

bool AI_Assessment::onCreate(int a_argc, char* a_argv[]) 
{
	// initialise the Gizmos helper class
	Gizmos::create();

	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	
	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// --------------------------

	monster = new Agent();



	// --------------------------

	return true;
}

void AI_Assessment::onUpdate(float a_deltaTime) 
{
	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	// clear all gizmos from last frame
	Gizmos::clear();
	
	// add an identity matrix gizmo
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );

	// add a 20x20 grid on the XZ-plane
	for ( int i = 0 ; i < 21 ; ++i )
	{
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10), 
						 i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
		
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i), 
						 i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	// quit our application when escape is pressed
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void AI_Assessment::onDraw() 
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	// draw the gizmos from this frame
	Gizmos::draw(m_projectionMatrix, viewMatrix);
}

void AI_Assessment::onDestroy()
{
	// clean up anything we created
	Gizmos::destroy();
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new AI_Assessment();
	
	if (app->create("AIE - AI_Assessment",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}