#include "Topic_02_Perlin.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

Topic_02_Perlin::Topic_02_Perlin()
{

}

Topic_02_Perlin::~Topic_02_Perlin()
{

}

bool Topic_02_Perlin::onCreate(int a_argc, char* a_argv[])
{
	// initialise the Gizmos helper class
	Gizmos::create();

	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse(glm::lookAt(glm::vec3(10, 10, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));

	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH / (float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f, 0.25f, 0.25f, 1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// plane ------------------------------------------------------------
	Utility::build3DPlane(50, m_vao, m_vbo, m_ibo);

	unsigned int vs = Utility::loadShader("./resources/shaders/3dQuad.vert", GL_VERTEX_SHADER);
	unsigned int fs = Utility::loadShader("./resources/shaders/redOnly.frag", GL_FRAGMENT_SHADER);
	unsigned int cs = Utility::loadShader("./resources/shaders/displace.cont", GL_TESS_CONTROL_SHADER);
	unsigned int es = Utility::loadShader("./resources/shaders/displace.eval", GL_TESS_EVALUATION_SHADER);
	m_programID = Utility::createProgram(vs, cs, es, 0, fs);

	glDeleteShader(vs);
	glDeleteShader(fs);
	glDeleteShader(cs);
	glDeleteShader(es);

	// noise ---------------------------------------------------------
	m_dimensions = glm::ivec2(256, 256);
	m_height = new float[m_dimensions.x * m_dimensions.y];

	int layers = 8;
	float amplitude = 0.8f;
	glm::vec2 size = 1.0f / glm::vec2(m_dimensions - 1);

	for (int x = 0; x < m_dimensions.x; ++x)
	{
		for (int y = 0; y < m_dimensions.y; ++y)
		{
			float value = 0;

			for (int i = 0; i < layers; ++i)
			{
				float freq = (float)glm::pow(2, i);
				float amp = glm::pow(amplitude, (float)i);

				value += glm::perlin(glm::vec2(x, y) * size * freq) * amp;
			}

			m_height[x * m_dimensions.y + y] = value * 0.5f + 0.5f;
		}
	}

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_dimensions.x, m_dimensions.y, 0, GL_RED, GL_FLOAT, m_height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);


	return true;
}

void Topic_02_Perlin::onUpdate(float a_deltaTime)
{
	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement(m_cameraMatrix, a_deltaTime, 10);

	// clear all gizmos from last frame
	Gizmos::clear();

	// add an identity matrix gizmo
	//Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );

	//// add a 20x20 grid on the XZ-plane
	//for ( int i = 0 ; i < 21 ; ++i )
	//{
	//	Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10), 
	//					 i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	//	
	//	Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i), 
	//					 i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	//}

	// quit our application when escape is pressed
	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void Topic_02_Perlin::onDraw()
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse(m_cameraMatrix);

	// draw the gizmos from this frame
	Gizmos::draw(m_projectionMatrix, viewMatrix);

	glUseProgram(m_programID);

	GLuint lViewProjectionModel = glGetUniformLocation(m_programID, "uViewProjectionModel");
	glUniformMatrix4fv(lViewProjectionModel, 1, GL_FALSE, glm::value_ptr(m_projectionMatrix * viewMatrix));

	GLuint lDiffuseMap = glGetUniformLocation(m_programID, "uDiffuseMap");
	glUniform1i(lDiffuseMap, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glBindVertexArray(m_vao);
	glPatchParameteri(GL_PATCH_VERTICES, 3);
	glDrawElements(GL_PATCHES, 6, GL_UNSIGNED_INT, nullptr);
}

void Topic_02_Perlin::onDestroy()
{
	// clean up anything we created
	Gizmos::destroy();

	glDeleteProgram(m_programID);

	glDeleteVertexArrays(1, &m_vao);
	glDeleteVertexArrays(1, &m_vbo);
	glDeleteVertexArrays(1, &m_ibo);
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new Topic_02_Perlin();

	if (app->create("AIE - Topic_02_Perlin", DEFAULT_SCREENWIDTH, DEFAULT_SCREENHEIGHT, argc, argv) == true)
		app->run();

	// explicitly control the destruction of our application
	delete app;

	return 0;
}