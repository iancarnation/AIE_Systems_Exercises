#ifndef __UIRECTANGLE_H_
#define __UIRECTANGLE_H_

#include "UIElement.h"

using glm::vec4;

class UIRectangle : public UIElement
{
public:

	UIRectangle(unsigned int a_shaderProgram,
				const vec2& a_dimensions = vec2(1, 1),
				const vec4& a_color = vec4(1));
	virtual ~UIRectangle();

	virtual void draw(const glm::mat4& a_projection);

	void setTexture(unsigned int a_texture) { m_texture = a_texture; }

protected:

	unsigned int m_shaderProgram;
	unsigned int m_vao, m_vbo;

	vec2 m_dimensions;
	vec4 m_color;

	unsigned int m_texture;
	vec2 m_textureCoordinates[4];

};

#endif __UIRECTANGLE_H_