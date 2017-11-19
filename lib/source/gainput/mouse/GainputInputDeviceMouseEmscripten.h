#ifndef GAINPUTINPUTDEVICEMOUSEEMSCRIPTEN_H_
#define GAINPUTINPUTDEVICEMOUSEEMSCRIPTEN_H_

namespace gainput
{
class InputDeviceMouseImplEmscripten : public InputDeviceMouseImpl
{
public:
	InputDeviceMouseImplEmscripten(InputManager& manager, InputDevice& device, InputState& state, InputState& previousState) 
	{
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
