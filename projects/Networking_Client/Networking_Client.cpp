#include "Networking_Client.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include <MessageIdentifiers.h>
#include <BitStream.h>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

#define SERVER_PORT 12001

enum MESSAGE_HEADER
{
	HEADER_HELLO_WORLD = ID_USER_PACKET_ENUM + 1,
	HEADER_HELLO_WORLD_RESPONSE,
};

Networking_Client::Networking_Client()
{

}

Networking_Client::~Networking_Client()
{

}

bool Networking_Client::onCreate(int a_argc, char* a_argv[]) 
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

	Interface = RakNet::RakPeerInterface::GetInstance();

	RakNet::SocketDescriptor sd;
	Interface->Startup(10, &sd, 1, 0);

	Interface->Connect("127.0.0.1", SERVER_PORT, 0, 0);

	RakNet::Packet* packet = nullptr;

	while (Utility::getTotalTime() < 2.f)
	{
		Utility::tickTimer();
		printf("Total Time = %f\n", Utility::getTotalTime());
		packet = Interface->Receive();

		if (packet != nullptr)
		{
			if (packet->data[0] == ID_CONNECTION_REQUEST_ACCEPTED)
			{
				ServerAddress = packet->systemAddress;
				printf("Connection to server successful!\n");
				return true;
			}
		}
	}

	printf("Connection to server UNsuccessful :( TIMEOUT\n");
	return false;
}

void Networking_Client::onUpdate(float a_deltaTime) 
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

	RakNet::BitStream outputStream;
	outputStream.Write((unsigned char)HEADER_HELLO_WORLD);
	outputStream.Write("Hello WOrld");

	Interface->Send(&outputStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, ServerAddress, false);

	RakNet::Packet* packet = nullptr;

	for (packet = Interface->Receive(); packet != nullptr; Interface->DeallocatePacket(packet), packet = Interface->Receive())
	{
		if (packet->data[0] == HEADER_HELLO_WORLD_RESPONSE)
		{
			printf("Recieved Message From Other Client!\n");
		}
	}

	// quit our application when escape is pressed
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void Networking_Client::onDraw() 
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	// draw the gizmos from this frame
	Gizmos::draw(m_projectionMatrix, viewMatrix);
}

void Networking_Client::onDestroy()
{
	// clean up anything we created
	Gizmos::destroy();
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new Networking_Client();
	
	if (app->create("AIE - Networking_Client",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}