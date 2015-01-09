#include "AI_Assessment.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <iostream>

#include "Agent.h"

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

/*
== Monster Behaviors ===

-- Action Behaviors --
- Eat Food (Timer)
- Seek Food (SeekTarget)
- Seek Wander Target (SeekTarget)
- Get Wander Target(random point for now (RandomizeTarget))

-- Condition Behaviors --
- At Food?
- Food Detected?
- Have Wander Target?

-- Composite Behaviors
- Root (sel)
- Eat Food (seq)
- Roam (sel)
- Have Path (seq)
*/

class RandomizeTarget : public Behavior
{
public:

	RandomizeTarget(char* a_name, float a_radius) : name(a_name), radius(a_radius) {}
	virtual ~RandomizeTarget() {}

	virtual Status execute(Agent* a_agent)
	{
		glm::vec3 target(0);

		target.xz = glm::circularRand(radius);

		a_agent->setTarget(target);
		std::printf("Random Target Set at (%f, %f)\n", target.x, target.z);
		return SUCCESS;
	}

	float radius;
	char* name;
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
		std::cout << "Seeking Target\n";
		return SUCCESS;
	}

	float speed;
};

// == Ian Code =========

Food allFood[20];

class Timer : public Behavior
{
public:

	Timer(char* a_name, float a_duration) : name(a_name), duration(a_duration) {}
	virtual ~Timer() {}

	virtual Status execute(Agent* agent)
	{
		duration -= Utility::getDeltaTime();
		if (duration <= 0)
			std::cout << "Timer Finished\n";
			return SUCCESS;
	}

	float duration;
	char* name;
};

class AtFood : public Behavior
{
public:

	AtFood(float a_range) : range2(a_range*a_range) {}
	virtual ~AtFood() {}

	virtual Status execute(Agent* agent)
	{
		// if position is in food list
		for (Food &f : allFood)
		{
			float dist2 = glm::distance2(agent->getPosition(), f.position);

			if (f.alive && dist2<= range2)
			{
				std::cout << "Is At Food\n";
				f.alive = false;
				return SUCCESS;
			}
		}
		std::cout << "Is NOT At Food\n";
		return FAILURE;
	}

	float range2;
};

class ClosestFoodDetected : public Behavior
{
public:

	ClosestFoodDetected(float a_range) : range2(a_range*a_range) {}
	virtual ~ClosestFoodDetected() {}

	virtual Status execute(Agent* agent)
	{
		glm::vec3 bestTarget;

		for (Food &f : allFood)
		{
			if (f.alive)
			{
				float dist2 = glm::distance2(agent->getPosition(), f.position);

				if (dist2 <= range2) // food is within perception range
				{
					bestTarget = f.position;
					float closestD2 = dist2;

					// check for closest
					for (Food &g : allFood)
					{
						if (g.alive)
						{
							float d2 = glm::distance2(agent->getPosition(), g.position);
							if (d2 < closestD2)
								bestTarget = g.position;
						}
					}

					agent->setTarget(bestTarget);
					std::cout << "Closest Food Detected\n";
					return SUCCESS;
				}
			}
		}
		std::cout << "Food Not Detected\n";
		return FAILURE;
	}

	float range2;
};

class AtWanderTarget : public Behavior
{
public:

	AtWanderTarget(float a_range) : range2(a_range*a_range) {}
	virtual ~AtWanderTarget() {}

	virtual Status execute(Agent* agent)
	{
		float dist2 = glm::distance2(agent->getPosition(), agent->getTarget());

		if (dist2 <= range2)
			return SUCCESS;

		return FAILURE;
	}

	float range2;
};

class HaveWanderTarget : public Behavior
{
public:

	HaveWanderTarget() {}
	virtual ~HaveWanderTarget() {}

	virtual Status execute(Agent* agent)
	{
		if (agent->getTarget() != glm::vec3(NULL))
		{
			std::cout << "Has Wander Target\n";
			return SUCCESS;
		}
		std::cout << "Has NO Wander Target\n";
		return FAILURE;
	}
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

	// Ian Code ====================

	monster = new Agent();

//--------------
	Behavior* isAtFood = new AtFood(0.5f);
	Behavior* eatFood = new Timer("eatFood", 5000);

	Sequence* eatingCheck = new Sequence();
	eatingCheck->addChild(isAtFood);
	eatingCheck->addChild(eatFood);
//--------------
	Behavior* isFoodClose = new ClosestFoodDetected(5);
	Behavior* seekFood = new SeekTarget(5);

	Sequence* towardsFood = new Sequence();
	towardsFood->addChild(isFoodClose);
	towardsFood->addChild(seekFood);
//-------------
	Behavior* isAtWanderTarget = new AtWanderTarget(0.5f);
	Behavior* getWander = new RandomizeTarget("getWander", 10);

	Sequence* waypointCheck = new Sequence();
	waypointCheck->addChild(isAtWanderTarget);
	waypointCheck->addChild(getWander);
//-------------
	Behavior* haveWander = new HaveWanderTarget();
	Behavior* seekWander = new SeekTarget(3);

	Sequence* towardsWander = new Sequence();
	towardsWander->addChild(haveWander);
	towardsWander->addChild(seekWander);
//------------
	Selector* target = new Selector();
	target->addChild(towardsWander);
	target->addChild(getWander);
//------------
	Selector* wander = new Selector();
	wander->addChild(waypointCheck);
	wander->addChild(target);
//-----------
	Selector* findFood = new Selector();
	findFood->addChild(towardsFood);
	findFood->addChild(wander);
//----------
	Selector* root = new Selector();
	root->addChild(eatingCheck);
	root->addChild(findFood);
//---------

	monsterBehavior = root;
	monster->setBehavior(monsterBehavior);

	//monster->setPosition(glm::vec3(0));

	for (Food &f : allFood)
	{
		f.position = glm::vec3(0);
		f.alive = false;
	}

	// temp food 

	Food food1 = { glm::vec3(5,0,7), true };
	Food food2 = { glm::vec3(2, 0, 4), true };
	Food food3 = { glm::vec3(9, 0, 9), true };

	allFood[1] = food1;
	allFood[2] = food2;
	allFood[3] = food3;

	// ============================

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

	monster->update(a_deltaTime);

	/*std::printf("Monster Position (post): (%f, %f)\n", monster->getPosition().x, monster->getPosition().z);
	std::printf("Monster Target (post): (%f, %f)\n", monster->getTarget().x, monster->getTarget().z);*/

	Gizmos::addAABBFilled(monster->getPosition(),
		glm::vec3(0.5f), glm::vec4(1, 1, 0, 1));

	Gizmos::addAABBFilled(monster->getTarget(),
		glm::vec3(0.1f), glm::vec4(1, 0, 0, 1));

	// Ian Code ======================

	// draw food positions
	for (int f = 1; f != 20; f++)
	{
		if (allFood[f].alive)
			Gizmos::addAABBFilled(allFood[f].position, glm::vec3(0.3f), glm::vec4(1, 0.5, 0.5, 1));
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