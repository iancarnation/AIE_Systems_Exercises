#include "UIButton.h"
#include <GLFW/glfw3.h>

UIButton::UIButton(unsigned int a_shaderProgram, const vec2& a_dimensions,
					const vec4& a_normalColor, const vec4& a_pressedColor, const vec4& a_hoverColor)
					: UIRectangle(a_shaderProgram, a_dimensions),
					m_normalColor(a_normalColor),
					m_pressedColor(a_pressedColor),
					m_hoverColor(a_hoverColor),
					m_pressed(false), m_pressedCB(nullptr), m_pressedUD(nullptr)
{

}

UIButton::~UIButton()
{

}

void UIButton::update(float a_deltaTime)
{
	// get window
	GLFWwindow* window = glfwGetCurrentContext();
	
	int w = 0, h = 0;
	glfwGetWindowSize(window, &w, &h);

	// get cursor
	double x = 0, y = 0;
	glfwGetCursorPos(window, &x, &y);

	// flip cursor's Y
	glm::vec3 cursor = glm::vec3(x, h - y, 1);

	// transform by inverse
	cursor = glm::inverse(getTransform()) * cursor;

	// compare if inside
	vec2 scaledHalfDimensions = m_dimensions * m_scale * 0.5f;

	if (cursor.x >= -scaledHalfDimensions.x &&
		cursor.x <= scaledHalfDimensions.x &&
		cursor.y >= -scaledHalfDimensions.y &&
		cursor.y <= scaledHalfDimensions.y)
	{
		// test for pressed/unpressed
		if (glfwGetMouseButton(window, 0) == GLFW_PRESS)
		{
			m_color = m_pressedColor;
			m_pressed = true;
		}
		else
		{
			// if released?
			if (m_pressed == true)
			{
				m_pressed = false;
				if (m_pressedCB != nullptr)
					m_pressedCB(this, m_pressedUD);
			}
			// hovering
			m_color = m_hoverColor;
		}
	}
	// not over button
	else
	{
		m_pressed = false;
		m_color = m_normalColor;
	}

	UIRectangle::update(a_deltaTime);
}