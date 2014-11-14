#pragma once

#include "Application.h"
#include <glm/glm.hpp>

class UIRectangle;

// derived application class that wraps up all globals neatly
class Topic_03_UserInterfaces : public Application
{
public:

	Topic_03_UserInterfaces();
	virtual ~Topic_03_UserInterfaces();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	void loadTexture(const char* a_filepath);

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	glm::mat4 m_guiProjectionMatrix;
	unsigned int m_guiShader;

	UIRectangle* m_menu;

	unsigned int m_texture;
};