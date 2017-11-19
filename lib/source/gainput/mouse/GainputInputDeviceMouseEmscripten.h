#ifndef GAINPUTINPUTDEVICEMOUSEEMSCRIPTEN_H_
#define GAINPUTINPUTDEVICEMOUSEEMSCRIPTEN_H_

#include "GainputInputDeviceMouseImpl.h"
#include <gainput/GainputHelpers.h>

#include <emscripten.h>
#include <emscripten/html5.h>
#include <iostream>

namespace gainput
{
class InputDeviceMouseImplEmscripten : public InputDeviceMouseImpl
{
	static EM_BOOL mouseCb(int eventType, const EmscriptenMouseEvent * mouseEvent, void * userData) {
		//BX_UNUSED(userData);

		if (mouseEvent) {
			switch (eventType) {
				case EMSCRIPTEN_EVENT_MOUSEMOVE:
				{
					int m_mx = mouseEvent->canvasX;
					int m_my = mouseEvent->canvasY;
					std::cout << "mousemove: " << m_mx << " " << m_my << "\n";
					//s_ctx.m_eventQueue.postMouseEvent(s_defaultWindow, s_ctx.m_mx, s_ctx.m_my, s_ctx.m_scroll);
					return true;
				}
				case EMSCRIPTEN_EVENT_MOUSEDOWN:
				case EMSCRIPTEN_EVENT_MOUSEUP:
				case EMSCRIPTEN_EVENT_DBLCLICK:
				{
					int m_mx = mouseEvent->canvasX;
					int m_my = mouseEvent->canvasY;
					MouseButton mb = (mouseEvent->button == 2) ? MouseButtonRight : ((mouseEvent->button == 1) ? MouseButtonMiddle : MouseButtonLeft);
					std::cout << "mousemove: " << m_mx << " " << m_my << " " << mb << "\n";
					//s_ctx.m_eventQueue.postMouseEvent(s_defaultWindow, s_ctx.m_mx, s_ctx.m_my, s_ctx.m_scroll, mb, (eventType != EMSCRIPTEN_EVENT_MOUSEUP));
					return true;
				}
			}
		}

		return false;
	}

public:
	InputDeviceMouseImplEmscripten(InputManager& manager, InputDevice& device, InputState& state, InputState& previousState) 
	{
		emscripten_set_mousedown_callback("#canvas", this, true, mouseCb);
		emscripten_set_mouseup_callback("#canvas", this, true, mouseCb);
		emscripten_set_mousemove_callback("#canvas", this, true, mouseCb);

	}

	InputDevice::DeviceVariant GetVariant() const
	{
		return InputDevice::DV_NULL;
	}

	void Update(InputDeltaState* /*delta*/)
	{
	}
};
}

#endif
