#pragma once

#include "Application.h"
#include <glm/glm.hpp>

#include <FBXFile.h>

// derived application class that wraps up all globals neatly
class Topic_04_NavMesh : public Application
{
public:

	Topic_04_NavMesh();
	virtual ~Topic_04_NavMesh();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	struct NavNodeTri
	{
		glm::vec3 position;
		glm::vec3 vertices[3];
		NavNodeTri* edgeTarget[3];
	};

	std::vector<NavNodeTri*> m_graph;

	void buildNavMesh(FBXMeshNode* a_mesh,
					  std::vector<NavNodeTri*>& a_graph);

	void createOpenGLBuffers(FBXFile* a_fbx);
	void cleanupOpenGLBuffers(FBXFile* a_fbx);

	struct GLData
	{
		unsigned int vao, vbo, ibo;
	};

	FBXFile* m_sponza;
	FBXFile* m_navMesh;

	unsigned int m_shaderProgram;

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};
