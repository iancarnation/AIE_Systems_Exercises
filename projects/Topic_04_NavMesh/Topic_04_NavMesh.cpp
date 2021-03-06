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

	m_navMesh = new FBXFile();
	m_navMesh->load("./resources/models/SponzaSimpleNavMesh.fbx", FBXFile::UNITS_CENTIMETER);
	// createOpenGLBuffers(m_navMesh);

	buildNavMesh(m_navMesh->getMeshByIndex(0), m_graph);

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

	for (auto node : m_graph)
	{
		Gizmos::addAABBFilled(node->position, glm::vec3(0.05f), glm::vec4(1, 0, 0, 1));

		// if there is actually connection, draw yellow line
		if (node->adjacentNodes[0] != nullptr)
			Gizmos::addLine(node->position + glm::vec3(0, 0.05, 0), 
							node->adjacentNodes[0]->position + glm::vec3(0, 0.05, 0), 
							glm::vec4(1, 1, 0, 1));

		if (node->adjacentNodes[1] != nullptr)
			Gizmos::addLine(node->position + glm::vec3(0, 0.05, 0),
			node->adjacentNodes[1]->position + glm::vec3(0, 0.05, 0),
			glm::vec4(1, 1, 0, 1));

		if (node->adjacentNodes[2] != nullptr)
			Gizmos::addLine(node->position + glm::vec3(0, 0.05, 0),
			node->adjacentNodes[2]->position + glm::vec3(0, 0.05, 0),
			glm::vec4(1, 1, 0, 1));
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
	//cleanupOpenGLBuffers(m_navMesh);

	delete m_navMesh;
	delete m_sponza;

	glDeleteProgram(m_shaderProgram);

	// clean up anything we created
	Gizmos::destroy();
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new Topic_04_NavMesh();

	if (app->create("AIE - Topic_04_NavMesh", DEFAULT_SCREENWIDTH, DEFAULT_SCREENHEIGHT, argc, argv) == true)
		app->run();

	// explicitly control the destruction of our application
	delete app;

	return 0;
}

void Topic_04_NavMesh::createOpenGLBuffers(FBXFile* a_fbx)
{
	// create the GL VAO/VBO/IBO data for meshes
	for (unsigned int i = 0; i < a_fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = a_fbx->getMeshByIndex(i);

		// storage for the opengl data in 3 unsigned int
		GLData* glData = new GLData();

		glGenVertexArrays(1, &glData->vao);
		glBindVertexArray(glData->vao);

		glGenBuffers(1, &glData->vbo);
		glGenBuffers(1, &glData->ibo);

		glBindBuffer(GL_ARRAY_BUFFER, glData->vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData->ibo);

		glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size() * sizeof(FBXVertex), mesh->m_vertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_indices.size() * sizeof(unsigned int), mesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); // position
		glEnableVertexAttribArray(1); // normal
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char*)FBXVertex::PositionOffset);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char*)FBXVertex::NormalOffset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		mesh->m_userData = glData;
	}
}

void Topic_04_NavMesh::cleanupOpenGLBuffers(FBXFile* a_fbx)
{
	// bind our vertex array object and draw the mesh
	for (unsigned int i = 0; i < a_fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = a_fbx->getMeshByIndex(i);

		GLData* glData = (GLData*)mesh->m_userData;

		glDeleteVertexArrays(1, &glData->vao);
		glDeleteBuffers(1, &glData->vbo);
		glDeleteBuffers(1, &glData->ibo);

		delete[] glData;
	}
}

void Topic_04_NavMesh::buildNavMesh(FBXMeshNode* a_mesh, std::vector<NavNodeTri*>& a_graph)
{
	unsigned int triCount = a_mesh->m_indices.size() / 3;

	for (unsigned int tri = 0; tri < triCount; ++tri)
	{
		NavNodeTri* node = new NavNodeTri();

		node->f = 0;
		node->g = 0;
		node->h = 0;
		node->parent = nullptr;

		// edge [ABC]
		// [AB] = 0, [BC] = 1, [CA] = 2

		node->adjacentNodes[0] = nullptr;
		node->adjacentNodes[1] = nullptr;
		node->adjacentNodes[2] = nullptr;

		node->vertices[0] = a_mesh->m_vertices[a_mesh->m_indices[tri * 3 + 0]].position.xyz;
		node->vertices[1] = a_mesh->m_vertices[a_mesh->m_indices[tri * 3 + 1]].position.xyz;
		node->vertices[2] = a_mesh->m_vertices[a_mesh->m_indices[tri * 3 + 2]].position.xyz;

		// get center of triangle (position) by averaging vertices
		node->position = (node->vertices[0] + node->vertices[1] + node->vertices[2]) / 3.0f;

		a_graph.push_back(node);
	}

	for (auto start : a_graph)
	{
		for (auto end : a_graph)
		{
			if (start == end)
				continue;

			// compare triangle ABC to XYZ

			// AB == XY || AB == YZ || AB == ZX
			// AB == YX || AB == ZY || AB == XZ

			if ((start->vertices[0] == end->vertices[0] && start->vertices[1] == end->vertices[1]) ||
				(start->vertices[0] == end->vertices[1] && start->vertices[1] == end->vertices[2]) ||
				(start->vertices[0] == end->vertices[2] && start->vertices[1] == end->vertices[0]) ||
				(start->vertices[0] == end->vertices[1] && start->vertices[1] == end->vertices[0]) ||
				(start->vertices[0] == end->vertices[2] && start->vertices[1] == end->vertices[1]) ||
				(start->vertices[0] == end->vertices[0] && start->vertices[1] == end->vertices[2]))
			{
				start->adjacentNodes[0] = end;
			}

			// BC == XY || BC == YZ || BC == ZX
			// BC == YX || BC == ZY || BC == XZ

			if ((start->vertices[1] == end->vertices[0] && start->vertices[2] == end->vertices[1]) ||
				(start->vertices[1] == end->vertices[1] && start->vertices[2] == end->vertices[2]) ||
				(start->vertices[1] == end->vertices[2] && start->vertices[2] == end->vertices[0]) ||
				(start->vertices[1] == end->vertices[1] && start->vertices[2] == end->vertices[0]) ||
				(start->vertices[1] == end->vertices[2] && start->vertices[2] == end->vertices[1]) ||
				(start->vertices[1] == end->vertices[0] && start->vertices[2] == end->vertices[2]))
			{
				start->adjacentNodes[1] = end;
			}

			// CA == XY || CA == YZ || CA == ZX
			// CA == YX || CA == ZY || CA == XZ

			if ((start->vertices[2] == end->vertices[0] && start->vertices[0] == end->vertices[1]) ||
				(start->vertices[2] == end->vertices[1] && start->vertices[0] == end->vertices[2]) ||
				(start->vertices[2] == end->vertices[2] && start->vertices[0] == end->vertices[0]) ||
				(start->vertices[2] == end->vertices[1] && start->vertices[0] == end->vertices[0]) ||
				(start->vertices[2] == end->vertices[2] && start->vertices[0] == end->vertices[1]) ||
				(start->vertices[2] == end->vertices[0] && start->vertices[0] == end->vertices[2]))
			{
				start->adjacentNodes[2] = end;
			}
		}

	}
	

}