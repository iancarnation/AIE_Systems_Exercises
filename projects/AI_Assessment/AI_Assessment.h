#pragma once

#include "Application.h"
#include <glm/glm.hpp>

class Agent;
class Behavior;

struct Food
{
	glm::vec3 position;
	bool alive;
};


// derived application class that wraps up all globals neatly
class AI_Assessment : public Application
{
public:

	AI_Assessment();
	virtual ~AI_Assessment();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	Agent * monster, * baiter;
	Behavior * monsterBehavior, * baiterBehavior;

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};