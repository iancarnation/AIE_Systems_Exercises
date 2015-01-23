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

	struct Player
	{
		glm::vec3 position;
		glm::vec4 color;

		Player() : position(glm::vec3(0)), color(glm::vec4(1, 1, 0, 1)) {}
	};

	struct Fireball
	{
		glm::vec3 position;
		float radius;
		float speed;
		bool alive;

		Fireball() :  position(glm::vec3(0)), radius(0.1f), speed(2.0f), alive(false) {}	
	};

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	int myID;
	//std::map<int, glm::vec3> players;
	std::map<int, Player*> players;

	Fireball* fireballs;

	RakNet::SystemAddress serverAddress;
	RakNet::RakPeerInterface* raknet;

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};