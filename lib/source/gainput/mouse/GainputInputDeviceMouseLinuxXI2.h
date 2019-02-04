
#ifndef GAINPUTINPUTDEVICEMOUSELINUXXI2_H_
#define GAINPUTINPUTDEVICEMOUSELINUXXI2_H_

#if defined(GAINPUT_ENABLE_XI2)
#include <X11/Xlib.h>
#include <X11/extensions/XI2.h>
#include <X11/extensions/XInput2.h>

#include "GainputInputDeviceMouseImpl.h"
#include <gainput/GainputHelpers.h>

namespace gainput
{

class InputDeviceMouseImplLinuxXI2 : public InputDeviceMouseImpl
{
public:
    InputDeviceMouseImplLinuxXI2(InputManager& manager, InputDevice& device, InputState& state, InputState& previousState) :
		manager_(manager),
		device_(device),
		state_(&state),
		previousState_(&previousState),
		nextState_(manager.GetAllocator(), MouseButtonCount + MouseAxisCount),
		delta_(0)
	{
		const size_t size = sizeof(bool)*MouseButtonCount;
		isWheel_ = static_cast<bool*>(manager_.GetAllocator().Allocate(size));
		GAINPUT_ASSERT(isWheel_);
		memset(isWheel_, 0, size);
		pressedThisFrame_ = static_cast<bool*>(manager_.GetAllocator().Allocate(size));
		GAINPUT_ASSERT(pressedThisFrame_);
	}

	~InputDeviceMouseImplLinuxXI2()
	{
		manager_.GetAllocator().Deallocate(isWheel_);
		manager_.GetAllocator().Deallocate(pressedThisFrame_);
	}

	InputDevice::DeviceVariant GetVariant() const
	{
		return InputDevice::DV_RAW;
	}

	void Update(InputDeltaState* delta)
	{
		delta_ = delta;

		// Reset mouse wheel buttons
		for (unsigned i = 0; i < MouseButtonCount; ++i)
		{
			const DeviceButtonId buttonId = i;
			const bool oldValue = previousState_->GetBool(buttonId);
			if (isWheel_[i] && oldValue)
			{
				const bool pressed = false;
				HandleButton(device_, nextState_, delta_, buttonId, pressed);
			}
		}

		*state_ = nextState_;

		memset(pressedThisFrame_, 0, sizeof(bool) * MouseButtonCount);
	}

	void HandleEvent(XEvent& event)
	{
		GAINPUT_ASSERT(state_);
		GAINPUT_ASSERT(previousState_);

        XGenericEventCookie *cookie = &event.xcookie;

        switch(cookie->evtype)
        {
            case XI_RawMotion:
            case XI_RawButtonPress:
            case XI_RawButtonRelease:
                HandleRawEvent((XIRawEvent*)cookie->data);
                break;

            case XI_Motion:
            case XI_ButtonPress:
            case XI_ButtonRelease:
                HandleDeviceEvent((XIDeviceEvent*)cookie->data);
                break;

            default:
                break;
        }
	}

    void HandleRawEvent(XIRawEvent* event)
    {
        if (event->detail != 0) {
            GAINPUT_ASSERT(event->detail > 0);
            const DeviceButtonId buttonId = event->detail - 1;
            GAINPUT_ASSERT(buttonId <= MouseButtonMax);
            const bool pressed = event->evtype == XI_RawButtonPress;

            if (!pressed && pressedThisFrame_[buttonId]) {
                // This is a mouse wheel button. Ignore release now, reset next frame.
                isWheel_[buttonId] = true;
            }
            else if (event->detail < MouseButtonCount) {
                HandleButton(device_, nextState_, delta_, buttonId, pressed);
            }

            if (pressed) {
                pressedThisFrame_[buttonId] = true;
            }
        }
        else {
            double x = 0.0;
            double y = 0.0;

            double *raw_valuator = event->raw_values,
                    *valuator = event->valuators.values;

            if (XIMaskIsSet(event->valuators.mask, 0)) {
                x = valuator[0] - raw_valuator[0];
            }

            if (XIMaskIsSet(event->valuators.mask, 1)) {
                y = valuator[1] - raw_valuator[1];
            }

            HandleAxis(device_, nextState_, delta_, MouseAxisX, x);
            HandleAxis(device_, nextState_, delta_, MouseAxisY, y);
        }
    }

    void HandleDeviceEvent(XIDeviceEvent* event)
    {
        if (event->detail == 0) {
            const float x = float(event->event_x)/float(manager_.GetDisplayWidth());
            const float y = float(event->event_y)/float(manager_.GetDisplayHeight());
            HandleAxis(device_, nextState_, delta_, MouseAxisX, x);
            HandleAxis(device_, nextState_, delta_, MouseAxisY, y);
        }
        else  {
            GAINPUT_ASSERT(event->detail > 0);
            const DeviceButtonId buttonId = event->detail - 1;
            GAINPUT_ASSERT(buttonId <= MouseButtonMax);
            const bool pressed = event->evtype == XI_ButtonPress;

            if (!pressed && pressedThisFrame_[buttonId]) {
                // This is a mouse wheel button. Ignore release now, reset next frame.
                isWheel_[buttonId] = true;
            }
            else if (event->detail < MouseButtonCount) {
                HandleButton(device_, nextState_, delta_, buttonId, pressed);
            }

            if (pressed) {
                pressedThisFrame_[buttonId] = true;
            }
        }
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
#endif

