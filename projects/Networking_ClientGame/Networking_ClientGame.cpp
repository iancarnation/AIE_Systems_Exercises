#include "Networking_ClientGame.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include <BitStream.h>
#include <MessageIdentifiers.h>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

enum MESSAGE_ID
{
	ID_USER_NEW_CLIENT = ID_USER_PACKET_ENUM,
	ID_USER_CLIENT_DISCONNECTED,
	ID_USER_ID,

	ID_USER_CUSTOM_DATA, 

	ID_USER_POSITION = ID_USER_CUSTOM_DATA,
};

const int SERVER_PORT = 12001;

Networking_ClientGame::Networking_ClientGame()
{

}

Networking_ClientGame::~Networking_ClientGame()
{

}

bool Networking_ClientGame::onCreate(int a_argc, char* a_argv[]) 
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

	raknet = RakNet::RakPeerInterface::GetInstance();

	RakNet::SocketDescriptor desc;
	raknet->Startup(1, &desc, 1);
	raknet->Connect("127.0.0.1", SERVER_PORT, nullptr, 0);

	myID = -1;

	return true;
}

void Networking_ClientGame::onUpdate(float a_deltaTime) 
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

	RakNet::Packet* packet = raknet->Receive();
	while (packet != nullptr)
	{
		// process
		switch (packet->data[0])
		{
		case ID_CONNECTION_REQUEST_ACCEPTED:
		{
			printf("Connected\n");
			serverAddress = packet->systemAddress;
			break;
		}
		case ID_USER_ID:
		{
			RakNet::BitStream input(packet->data, packet->length, true);
			input.IgnoreBytes(1);

			input.Read(myID);
			//players[myID] = glm::vec3(0);
			players[myID] = new Player{ glm::vec3(0), glm::vec4(1, 1, 0, 1) };

			printf("My ID: %i\n", myID);

			int id = -1;
			while (input.Read(id))
			{
				//players[id] = glm::vec3(0);
				players[id] = new Player{ glm::vec3(0), glm::vec4(1, 1, 0, 1) };
			}
			break;
		}
		case ID_USER_NEW_CLIENT:
		{
			RakNet::BitStream input(packet->data, packet->length, true);
			input.IgnoreBytes(1);
			int id = -1;
			input.Read(id);
			printf("New player connected: %i\n", id);
			//players[id] = glm::vec3(0);
			players[id] = new Player{ glm::vec3(0), glm::vec4(1, 1, 0, 1) };
			break;
		}
		case ID_USER_CLIENT_DISCONNECTED:
		{
			RakNet::BitStream input(packet->data, packet->length, true);
			input.IgnoreBytes(1);
			int id = -1;
			input.Read(id);
			printf("player disconnected: %i\n", id);
			
			players.erase(players.find(id));

			break;
		}
		case ID_USER_POSITION:
		{
			RakNet::BitStream input(packet->data, packet->length, true);
			input.IgnoreBytes(1);
			int id = -1;
			input.Read(id);
			glm::vec3 pos(0);
			input.Read(pos);
			players[id]->position = pos;
			break;
		}
		};


		// get next
		raknet->DeallocatePacket(packet);
		packet = raknet->Receive();
	}

	if (myID != -1)
	{
		glm::vec3 movement(0);
		if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS)
			movement.z -= 1;
		if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS)
			movement.z += 1;
		if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS)
			movement.x -= 1;
		if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			movement.x += 1;

		if (glm::length2(movement) > 0)
		{
			players[myID]->position = players[myID]->position + glm::normalize(movement) * a_deltaTime * 5.0f;

			RakNet::BitStream output;
			output.Write((unsigned char)ID_USER_POSITION);
			output.Write(myID);
			output.Write(players[myID]->position);

			raknet->Send(&output, HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverAddress, false);
		}

		// if space is pressed
		// fireball at other player
		// if id=0, get vector towards 1
		// otherwise, get vector towards 0
		//if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)



		// if fireball hits, change color

	}



	for (auto player : players)
		Gizmos::addAABBFilled(player.second->position, glm::vec3(0.5f), glm::vec4(1, 1, 0, 1));


	// quit our application when escape is pressed
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void Networking_ClientGame::onDraw() 
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	// draw the gizmos from this frame
	Gizmos::draw(m_projectionMatrix, viewMatrix);
}

void Networking_ClientGame::onDestroy()
{
	raknet->Shutdown(100);
	RakNet::RakPeerInterface::DestroyInstance(raknet);

	// clean up anything we created
	Gizmos::destroy();
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new Networking_ClientGame();
	
	if (app->create("AIE - Networking_ClientGame",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}