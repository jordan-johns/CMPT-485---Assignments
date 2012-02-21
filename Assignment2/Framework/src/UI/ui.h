
/*
 * Copyright:
 * Daniel D. Neilson (ddneilson@ieee.org)
 * University of Saskatchewan
 * All rights reserved
 *
 * Permission granted to use for use in assignments and
 * projects for CMPT 485 & CMPT 829 at the University
 * of Saskatchewan.
 */

#pragma once
#ifndef __INC_UI_H_
#define __INC_UI_H_

namespace UI
{

// What type of event was a key/mousebutton event?
// UP = button released
// DOWN = button pressed
typedef enum _ButtonState
{
	BUTTON_UP=0x00,
	BUTTON_DOWN=0x01
} ButtonState_t;

// Which mouse button generated an event
typedef enum _MouseButton
{
	MOUSE_LEFT=0x01,   // enum values can be combined in a bitfield, if needed
	MOUSE_MIDDLE=0x02,
	MOUSE_RIGHT=0x04
} MouseButton_t;

// Which keyboard key generated an event.
// This is used by the specialKeyboard() callback
// There should be an enum for every key on the keyboard
// except print screen, and the windows keys
typedef enum _KeySpecial
{
	// Character keys (in order for a US101 keyboard)
	KEY_BACKTICK=0x60,
	KEY_1=0x31,
	KEY_2=0x32,
	KEY_3=0x33,
	KEY_4=0x34,
	KEY_5=0x35,
	KEY_6=0x36,
	KEY_7=0x37,
	KEY_8=0x38,
	KEY_9=0x39,
	KEY_0=0x30,
	KEY_MINUS=0x2D,
	KEY_EQUAL=0x3D,
	KEY_Q=0x51,
	KEY_W=0x57,
	KEY_E=0x45,
	KEY_R=0x52,
	KEY_T=0x54,
	KEY_Y=0x59,
	KEY_U=0x55,
	KEY_I=0x49,
	KEY_O=0x4f,
	KEY_P=0x50,
	KEY_OPENSQUARE=0x5b,
	KEY_CLOSESQUARE=0x5d,
	KEY_BACKSLASH=0x5c,
	KEY_A=0x41,
	KEY_S=0x53,
	KEY_D=0x44,
	KEY_F=0x46,
	KEY_G=0x47,
	KEY_H=0x48,
	KEY_J=0x4a,
	KEY_K=0x4b,
	KEY_L=0x4c,
	KEY_SEMICOLON=0x3b,
	KEY_QUOTE=0x27,
	KEY_Z=0x5a,
	KEY_X=0x58,
	KEY_C=0x43,
	KEY_V=0x56,
	KEY_B=0x42,
	KEY_N=0x4e,
	KEY_M=0x4d,
	KEY_COMMA=0x2c,
	KEY_PERIOD=0x2e,
	KEY_FRONTSLASH=0x2f,
	// Function keys
	KEY_F1=0x0100, // start outside of the range of an 8-bit ASCII character
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	// Directional arrows
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	// Modifier keys
	KEY_LSHIFT, KEY_RSHIFT,
	KEY_LCTRL, KEY_RCTRL,
	KEY_LALT, KEY_RALT,
	// Insert, PGUP, etc.
	KEY_INSERT,
	KEY_DELETE,
	KEY_HOME,
	KEY_END,
	KEY_PAGEUP,
	KEY_PAGEDOWN,
	// Keypad
	KEY_KP_0, KEY_KP_1,	KEY_KP_2,
	KEY_KP_3, KEY_KP_4,	KEY_KP_5,
	KEY_KP_6, KEY_KP_7,	KEY_KP_8, KEY_KP_9,
	KEY_KP_DIV, KEY_KP_MULT,
	KEY_KP_SUB, KEY_KP_ADD,
	KEY_KP_DECIMAL, KEY_KP_EQUAL,
	KEY_KP_ENTER,
	// Locks
	KEY_NUMLOCK,
	KEY_CAPSLOCK,
	KEY_SCROLLLOCK,
	// Misc
	KEY_PAUSE,
	KEY_BACKSPACE,
	KEY_TAB,
	KEY_ENTER,
	KEY_ESC,
	KEY_SPACE
} KeySpecial_t;


/*
 * Base class for all OpenGL "program objects" in this framework.
 * Provides virtual functions that can be overwritten to handle
 * each of the types of events that the UI will generate
 */
class Callbacks
{
protected:
public:
	Callbacks();
	virtual ~Callbacks();

	// Called to repaint the screen
	virtual void repaint();

	// Called when the window is resized. (width,height) is the new size
	// Should always be called when the main loop is first entered
	// Note: This callback is automatically called by UI::setCallbacks()
	// with the current window width & height when this Callback object is set.
	virtual void windowResize(int width, int height);

	// Called when a mouse button event occurs.
	//   button: MOUSE_{LEFT,MIDDLE_RIGHT}
	//   state: MOUSE_{UP,DOWN}
	//   (x,y): mouse position. (0,0) is lower-left
	virtual void mouseEvent(MouseButton_t button, ButtonState_t state, int x, int y);

	// Called when a mouse drag event occurs
	//  i.e. mouse button is down, and the mouse moved.
	//   (x,y): mouse position. (0,0) is lower-left
	virtual void mouseDrag(int x, int y);

	// Called when a character is typed
	//   key: ASCII code of character typed
	virtual void charKeyboard(wchar_t key);

	// Called when a special key on the keyboard is pressed.
	virtual void specialKeyboard(KeySpecial_t key, ButtonState_t state);

	// Called on idle event loop iterations.
	virtual void idle();
};

// Set up the UI
bool init(int windowWidth, int windowHeight);
// Set up the callbacks
void setCallbacks(Callbacks *callbacks);
// Shutdown the UI
void shutdown();

// Misc
void setWindowTitle(const char *title);
void getMousePos(int *x, int *y);
double getTime(); // Value of high-precision timer in seconds.
void setTargetFPS(const float fps);

// Run the main loop.
//  Calling exitMainLoop() will cause this function to exit
void mainLoop();
void exitMainLoop();

} // namespace

#endif
