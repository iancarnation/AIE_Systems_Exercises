#include "Topic_03_UserInterfaces.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include "UIButton.h"
#include <stb_image.h>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

Topic_03_UserInterfaces::Topic_03_UserInterfaces()
{

}

Topic_03_UserInterfaces::~Topic_03_UserInterfaces()
{

}

void buttonPress(UIButton* a_caller, void* a_userData)
{
	UIRectangle* parent = (UIRectangle*)a_userData;

	parent->setPosition(vec2(glm::linearRand<float>(200, DEFAULT_SCREENWIDTH - 200), DEFAULT_SCREENHEIGHT * 0.5f));
}

bool Topic_03_UserInterfaces::onCreate(int a_argc, char* a_argv[]) 
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

	m_guiProjectionMatrix = glm::ortho<float>(0, DEFAULT_SCREENWIDTH, 0, DEFAULT_SCREENHEIGHT);

	// shader setup -----------------------------
	unsigned int vertShader = Utility::loadShader("./resources/shaders/ui.vert", GL_VERTEX_SHADER);
	unsigned int fragShader = Utility::loadShader("./resources/shaders/ui.frag", GL_FRAGMENT_SHADER);
	m_guiShader = Utility::createProgram(vertShader, 0, 0, 0, fragShader);
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	// texture ----------------------------------
	loadTexture("./resources/textures/crate.png");

	// buttons ----------------------------------
	m_menu = new UIRectangle(m_guiShader, vec2(300, 500));
	m_menu->setPosition(vec2(DEFAULT_SCREENWIDTH * 0.5f, DEFAULT_SCREENHEIGHT * 0.5f));
	m_menu->setTexture(m_texture);

	UIButton* button = new UIButton(m_guiShader, vec2(100, 50), vec4(1), vec4(0,0,1,1), vec4(1,0,0,1));
	button->setPosition(vec2(0, 100));
	button->setTexture(m_texture);
	button->setPressedCB(buttonPress, m_menu);
	m_menu->addElement(button);

	button = new UIButton(m_guiShader, vec2(100, 50), vec4(1), vec4(0, 0, 1, 1), vec4(1, 0, 0, 1));
	button->setTexture(m_texture);
	button->setPressedCB(buttonPress, m_menu);
	m_menu->addElement(button);

	button = new UIButton(m_guiShader, vec2(100, 50), vec4(1), vec4(0, 0, 1, 1), vec4(1, 0, 0, 1));
	button->setPosition(vec2(0, -100));
	button->setTexture(m_texture);
	button->setPressedCB(buttonPress, m_menu);
	m_menu->addElement(button);

	return true;
}

void Topic_03_UserInterfaces::onUpdate(float a_deltaTime) 
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

	m_menu->update(a_deltaTime);

	// quit our application when escape is pressed
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void Topic_03_UserInterfaces::onDraw() 
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	// draw the gizmos from this frame
	Gizmos::draw(m_projectionMatrix, viewMatrix);

	// draw GUI -----------------------------------------------------------------
	int depthFunc = GL_LESS;
	glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);

	glDepthFunc(GL_LEQUAL);

	m_menu->draw(m_guiProjectionMatrix);

	glDepthFunc(depthFunc);
}

void Topic_03_UserInterfaces::onDestroy()
{
	delete m_menu;

	glDeleteProgram(m_guiShader);


	// clean up anything we created
	Gizmos::destroy();
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new Topic_03_UserInterfaces();
	
	if (app->create("AIE - Topic_03_UserInterfaces",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}

void Topic_03_UserInterfaces::loadTexture(const char* a_filepath)
{
	// load image data
	int width = 0;
	int height = 0;
	int format = 0;
	unsigned char* pixelData = stbi_load(a_filepath,
		&width, &height, &format, STBI_default);

	printf("Width: %i Height: %i Format: %i\n", width, height, format);

	// convert the stbi format to the actual GL code
	switch (format)
	{
	case STBI_grey: format = GL_LUMINANCE; break;
	case STBI_grey_alpha: format = GL_LUMINANCE_ALPHA; break;
	case STBI_rgb: format = GL_RGB; break;
	case STBI_rgb_alpha: format = GL_RGBA; break;
	};

	printf("Width: %i Height: %i Format: %i\n", width, height, format);

	// create OpenGL texture handle
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	// set pixel data for texture
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixelData);

	// set filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// clear bound texture state so we don't accidentally change it
	glBindTexture(GL_TEXTURE_2D, 0);

	delete[] pixelData;
}