#pragma once

#include "Application.h"
#include <glm/glm.hpp>

class Agent;
class Behavior;

// derived application class that wraps up all globals neatly
class Topic_05_BehaviorTrees : public Application
{
public:

	Topic_05_BehaviorTrees();
	virtual ~Topic_05_BehaviorTrees();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	Agent* m_agent;
	Behavior* m_behavior;

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};