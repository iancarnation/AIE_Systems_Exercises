#ifndef __UIBUTTON_H_
#define __UIBUTTON_H_

#include "UIRectangle.h"

class UIButton : public UIRectangle
{
public:

	UIButton(unsigned int a_shaderProgram,
		const vec2& a_dimensions = vec2(1, 1),
		const vec4& a_normalColor = vec4(1),
		const vec4& a_pressedColor = vec4(1),
		const vec4& a_hoverColor = vec4(1));
	virtual ~UIButton();

	typedef void (*OnPressCB)(UIButton* a_caller, void* a_userData);

	void setPressedCB(OnPressCB a_callback, void* a_userData) { m_pressedCB = a_callback; m_pressedUD = a_userData; }

	virtual void update(float a_deltaTime);

protected:

	vec4 m_normalColor;
	vec4 m_pressedColor;
	vec4 m_hoverColor;
	bool m_pressed;

	OnPressCB m_pressedCB;
	void* m_pressedUD;
};


#endif // __UIBUTTON_H_