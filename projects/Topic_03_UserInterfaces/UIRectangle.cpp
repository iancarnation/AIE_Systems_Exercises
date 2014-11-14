#include "UIRectangle.h"
#include <GL/glew.h>
#include <glm/ext.hpp>

UIRectangle::UIRectangle(unsigned int a_shaderProgram,
						const vec2& a_dimensions /* = vec2(1, 1)*/,
						const vec4& a_color /* = vec4(1)*/)
						: m_shaderProgram(a_shaderProgram),
						m_vao(0), m_vbo(0),
						m_dimensions(a_dimensions),
						m_color(a_color),
						m_texture(0)
{
	m_textureCoordinates[0] = vec2(0,0);
	m_textureCoordinates[1] = vec2(0,1);
	m_textureCoordinates[2] = vec2(1,0);
	m_textureCoordinates[3] = vec2(1,1);
		
	glm::vec3 vertices[4] = {
		glm::vec3(-0.5f, 0.5f, 1), glm::vec3(-0.5f, -0.5f, 1),
		glm::vec3(0.5f, 0.5f, 1), glm::vec3(0.5f, -0.5f, 1)
	};

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec3), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glBindVertexArray(0);
}

UIRectangle::~UIRectangle()
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
}

void UIRectangle::draw(const glm::mat4& a_projection)
{
	int program = -1;
	glGetIntegerv(GL_CURRENT_PROGRAM, &program);

	if (program != m_shaderProgram)
		glUseProgram(m_shaderProgram);

	GLuint ulProjection = glGetUniformLocation(m_shaderProgram, "uProjection");
	glUniformMatrix4fv(ulProjection, 1, GL_FALSE, glm::value_ptr(a_projection));

	mat3 transform = getTransform();
	GLuint ulTransform = glGetUniformLocation(m_shaderProgram, "uTransform");
	glUniformMatrix3fv(ulTransform, 1, GL_FALSE, glm::value_ptr(transform));

	GLuint ulDimensions = glGetUniformLocation(m_shaderProgram, "uDimensions");
	glUniform2f(ulDimensions, m_dimensions.x, m_dimensions.y);

	GLuint ulColor = glGetUniformLocation(m_shaderProgram, "uColor");
	glUniform4fv(ulColor, 1, glm::value_ptr(m_color));

	GLuint ulTexCoords = glGetUniformLocation(m_shaderProgram, "uTexCoords");
	glUniform2fv(ulTexCoords, 4, (float*)m_textureCoordinates);

	GLuint ulDiffuseMap = glGetUniformLocation(m_shaderProgram, "uDiffuseMap");
	glUniform1i(ulDiffuseMap, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	UIElement::draw(a_projection);

	if (program != m_shaderProgram)
		glUseProgram(program);
}

