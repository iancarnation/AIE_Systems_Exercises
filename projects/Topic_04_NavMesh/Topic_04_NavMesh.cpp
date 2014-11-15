#include "Topic_04_NavMesh.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

Topic_04_NavMesh::Topic_04_NavMesh()
{

}

Topic_04_NavMesh::~Topic_04_NavMesh()
{

}

bool Topic_04_NavMesh::onCreate(int a_argc, char* a_argv[]) 
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

	m_sponza = new FBXFile();
	m_sponza->load("./resources/models/SponzaSimple.fbx", FBXFile::UNITS_CENTIMETER);
	createOpenGLBuffers(m_sponza);

	m_navMesh - new FBXFile();
	m_navMesh->load("./resources/models/SponzaSimpleNavMesh.fbx", FBXFile::UNITS_CENTIMETER);
	// createOpenGLBuffers(m_navMesh);

	unsigned int vertShader = Utility::loadShader("./resources/shaders/sponza.vert", GL_VERTEX_SHADER);
	unsigned int fragShader = Utility::loadShader("./resources/shaders/sponza.frag", GL_FRAGMENT_SHADER);
	m_shaderProgram = Utility::createProgram(vertShader, 0, 0, 0, fragShader);
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	return true;
}

void Topic_04_NavMesh::onUpdate(float a_deltaTime) 
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

void Topic_04_NavMesh::onDraw() 
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );

	glUseProgram(m_shaderProgram);

	GLint ulViewProjection = glGetUniformLocation(m_shaderProgram, "uViewProjection");
	glUniformMatrix4fv(ulViewProjection, 1, GL_FALSE, glm::value_ptr(m_projectionMatrix * viewMatrix));

	unsigned int count = m_sponza->getMeshCount();
	for (unsigned int i = 0; i < count; ++i)
	{
		FBXMeshNode* mesh = m_sponza->getMeshByIndex(i);

		GLData* data = (GLData*)mesh->m_userData;

		GLint ulModel = glGetUniformLocation(m_shaderProgram, "uModel");
		glUniformMatrix4fv(ulModel, 1, GL_FALSE, glm::value_ptr(mesh->m_globalTransform));

		GLint ulInvTransposeModel = glGetUniformLocation(m_shaderProgram, "uInvTransposeModel");
		glUniformMatrix4fv(ulInvTransposeModel, 1, GL_TRUE, glm::value_ptr(glm::inverse(mesh->m_globalTransform)));

		glBindVertexArray(data->vao);
		glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, nullptr);
	}
	
	// draw the gizmos from this frame
	Gizmos::draw(m_projectionMatrix, viewMatrix);
}

void Topic_04_NavMesh::onDestroy()
{
	cleanupOpenGLBuffers(m_sponza);
	// cleanupOpenGLBuffers(m_ ************************************************************

	// clean up anything we created
	Gizmos::destroy();
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new Topic_04_NavMesh();
	
	if (app->create("AIE - Topic_04_NavMesh",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}