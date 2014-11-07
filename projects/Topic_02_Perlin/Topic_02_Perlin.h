#pragma once

#include "Application.h"
#include <glm/glm.hpp>

// derived application class that wraps up all globals neatly
class Topic_02_Perlin : public Application
{
public:

	Topic_02_Perlin();
	virtual ~Topic_02_Perlin();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	glm::ivec2	m_dimensions;
	float*		m_height;


	unsigned int m_vao, m_vbo, m_ibo;

	unsigned int m_programID;

	unsigned int m_texture;
};