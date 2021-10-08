
#include <gainput/gainput.h>

#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>
#include <iostream>


// Define your user buttons
enum Button
{
    ButtonMenu,
    ButtonConfirm,
    MouseX,
    MouseY
};

static Window create_win(Display *dpy)
{
    XIEventMask mask;

    Window        win;
    unsigned long    screen;

    screen = DefaultScreen(dpy);
    win = RootWindow(dpy, screen);

    XSelectInput(dpy, win, ExposureMask);
    mask.deviceid = XIAllDevices;
    mask.mask_len = XIMaskLen(XI_RawMotion);
    mask.mask = (unsigned char*)calloc(mask.mask_len, sizeof(char));
    memset(mask.mask, 0, mask.mask_len);

    XISetMask(mask.mask, XI_RawMotion);
    XISetMask(mask.mask, XI_Enter);
    XISetMask(mask.mask, XI_Leave);
    XISetMask(mask.mask, XI_ButtonPress);
    XISetMask(mask.mask, XI_ButtonRelease);
    XISetMask(mask.mask, XI_KeyPress);
    XISetMask(mask.mask, XI_KeyRelease);
    XISetMask(mask.mask, XI_RawKeyPress);
    XISetMask(mask.mask, XI_RawKeyRelease);
    XISetMask(mask.mask, XI_RawButtonPress);
    XISetMask(mask.mask, XI_RawButtonRelease);

    XISelectEvents(dpy, win, &mask, 1);

    free(mask.mask);
    XMapWindow(dpy, win);
    XSync(dpy, True);
    return win;
}

int main(int argc, char** argv)
{
    // Setup Gainput
    gainput::InputManager manager;
    const gainput::DeviceId mouseId = manager.CreateDevice<gainput::InputDeviceMouse>(gainput::InputDevice::AutoIndex, gainput::InputDevice::DV_RAW);
    const gainput::DeviceId keyboardId = manager.CreateDevice<gainput::InputDeviceKeyboard>(gainput::InputDevice::AutoIndex, gainput::InputDevice::DV_RAW);
    const gainput::DeviceId padId = manager.CreateDevice<gainput::InputDevicePad>(gainput::InputDevice::AutoIndex);

    gainput::InputMap map(manager);
    map.MapBool(ButtonMenu, keyboardId, gainput::KeyEscape);
    map.MapBool(ButtonConfirm, mouseId, gainput::MouseButtonLeft);
    map.MapFloat(MouseX, mouseId, gainput::MouseAxisX);
    map.MapFloat(MouseY, mouseId, gainput::MouseAxisY);
    map.MapBool(ButtonConfirm, padId, gainput::PadButtonA);

    Display *dpy;
    int xi_opcode, event, error;
    Window win;
    XEvent ev;

    dpy = XOpenDisplay(NULL);

    if (!dpy) {
        fprintf(stderr, "Failed to open display.\n");
        return -1;
    }

    if (!XQueryExtension(dpy, "XInputExtension", &xi_opcode, &event, &error)) {
        printf("X Input extension not available.\n");
        return -1;
    }

    win = create_win(dpy);

    for (;;)
    {
        // Update Gainput
        manager.Update();

        XGenericEventCookie *cookie = &ev.xcookie;

        XNextEvent(dpy, &ev);
        if (cookie->type != GenericEvent ||
            cookie->extension != xi_opcode ||
            !XGetEventData(dpy, cookie)) {
            continue;
        }

        manager.HandleEvent(ev);
        XFreeEventData(dpy, cookie);

        // Check button states
        if (map.GetBoolWasDown(ButtonMenu))
        {
            std::cout << "Open menu!!" << std::endl;
        }
        if (map.GetBoolWasDown(ButtonConfirm))
        {
            std::cout << "Confirmed!!" << std::endl;
        }

        if (map.GetFloatDelta(MouseX) != 0.0f || map.GetFloatDelta(MouseY) != 0.0f)
        {
            std::cout << "Mouse: " << map.GetFloat(MouseX) << ", " << map.GetFloat(MouseY) << std::endl;
        }
    }

    XCloseDisplay(dpy);

    return 0;
}

