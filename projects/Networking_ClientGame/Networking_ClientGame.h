#pragma once

#include "Application.h"
#include <glm/glm.hpp>

#include <RakPeerInterface.h>
#include <map>

// derived application class that wraps up all globals neatly
class Networking_ClientGame : public Application
{
public:

	Networking_ClientGame();
	virtual ~Networking_ClientGame();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	int myID;
	std::map<int, glm::vec3> players;

	RakNet::SystemAddress serverAddress;
	RakNet::RakPeerInterface* raknet;

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};