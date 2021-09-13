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
		InputDeviceMouseImplEmscripten * that = reinterpret_cast<InputDeviceMouseImplEmscripten*>(userData);
		
		DeviceButtonId buttonId = 0;
		
		bool pressed = false;
	
		if (mouseEvent) {
			switch (eventType) {
				case EMSCRIPTEN_EVENT_MOUSEMOVE:
				{
					int m_mx = mouseEvent->canvasX;
					int m_my = mouseEvent->canvasY;
					
					const float x = float(m_mx)/float(that->manager_.GetDisplayWidth());
					const float y = float(m_my)/float(that->manager_.GetDisplayHeight());
					
					std::cout << "mousemove: " << x << " " << y << "\n";
					HandleAxis(that->device_, that->nextState_, that->delta_, MouseAxisX, x);
				    HandleAxis(that->device_, that->nextState_, that->delta_, MouseAxisY, y);
					//s_ctx.m_eventQueue.postMouseEvent(s_defaultWindow, s_ctx.m_mx, s_ctx.m_my, s_ctx.m_scroll);
					return true;
				}
				case EMSCRIPTEN_EVENT_MOUSEDOWN:
				case EMSCRIPTEN_EVENT_DBLCLICK:
					pressed = true; //falltrough
				case EMSCRIPTEN_EVENT_MOUSEUP:
				{
					int m_mx = mouseEvent->canvasX;
					int m_my = mouseEvent->canvasY;
					MouseButton mb = (mouseEvent->button == 2) ? MouseButtonRight : ((mouseEvent->button == 1) ? MouseButtonMiddle : MouseButtonLeft);
					std::cout << "mousemove: " << m_mx << " " << m_my << " " << mb << " " << pressed << "\n";
					HandleButton(that->device_, that->nextState_, that->delta_, mb, pressed);
					//s_ctx.m_eventQueue.postMouseEvent(s_defaultWindow, s_ctx.m_mx, s_ctx.m_my, s_ctx.m_scroll, mb, (eventType != EMSCRIPTEN_EVENT_MOUSEUP));
					return true;
				}
			}
		}

		return false;
	}

public:
	InputDeviceMouseImplEmscripten(InputManager& manager, InputDevice& device, InputState& state, InputState& previousState) :
		manager_(manager),
		device_(device),
		state_(&state),
		previousState_(&previousState),
		nextState_(manager.GetAllocator(), MouseButtonCount + MouseAxisCount),
		delta_(0)
	{
		emscripten_set_mousedown_callback("#canvas", this, true, mouseCb);
		emscripten_set_mouseup_callback("#canvas", this, true, mouseCb);
		emscripten_set_mousemove_callback("#canvas", this, true, mouseCb);
	}

	InputDevice::DeviceVariant GetVariant() const
	{
		return InputDevice::DV_NULL;
	}

	void Update(InputDeltaState* delta)
	{
		delta_ = delta;

		*state_ = nextState_;
	}
	
private:
	InputManager& manager_;
	InputDevice& device_;
	bool* isWheel_;
	bool* pressedThisFrame_;
	InputState* state_;
	InputState* previousState_;
	InputState nextState_;
	InputDeltaState* delta_;
};
}

#endif
