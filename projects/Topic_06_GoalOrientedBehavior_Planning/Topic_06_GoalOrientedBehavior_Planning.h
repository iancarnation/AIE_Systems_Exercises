#pragma once

#include "Application.h"
#include <glm/glm.hpp>

#include "Agent.h"

// derived application class that wraps up all globals neatly
class Topic_06_GoalOrientedBehavior_Planning : public Application
{
public:

	Topic_06_GoalOrientedBehavior_Planning();
	virtual ~Topic_06_GoalOrientedBehavior_Planning();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	Agent* m_agent;
	Task* m_task;

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};