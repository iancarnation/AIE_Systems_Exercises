#pragma once

#include "Application.h"
#include <glm/glm.hpp>

#include "RakPeerInterface.h"
#include <NetworkIDObject.h>

// derived application class that wraps up all globals neatly
class Networking_Server : public Application
{
public:

	Networking_Server();
	virtual ~Networking_Server();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	RakNet::RakPeerInterface* Interface;

#pragma pack(push, 1)
	struct Thing : public RakNet::NetworkIDObject
	{
		unsigned char useTimeStamp;		// assign ID_TIMESTAMP to this
		RakNet::Time timeStamp;			// put the system time in here returned by rn::GetTime()
		unsigned char typeId;			// an enum defined after the last on e in MessageIdentifiers.h e.g. ID_SET_TIMED_MINE
		glm::vec3 position;
		RakNet::NetworkID networkID;
		RakNet::SystemAddress systemAddress;	// address of the client that owns thing
	};
#pragma pack(pop)
};