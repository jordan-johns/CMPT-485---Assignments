
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

#include "../GL3/gl3w.h"
#include <GL/glfw.h>
#include <cassert>
#include <cstdio>
#include <cwchar>

#include "ui.h"
#include "../glUtils.h"

namespace UI
{

static Callbacks *_callbacks = 0;
static int _windowWidth, _windowHeight;
static bool _keepRunning = true;
static bool _repaint = false;
static int _mouseButtonsDown = 0;
static float _targetFPS = 30.0f;

Callbacks::Callbacks() {}
Callbacks::~Callbacks() {}
void Callbacks::repaint() {}
void Callbacks::windowResize(int width, int height) {}
void Callbacks::mouseEvent(MouseButton_t button, ButtonState_t state, int x, int y) {}
void Callbacks::mouseDrag(int x, int y) {}
void Callbacks::charKeyboard(wchar_t ch) {}
void Callbacks::specialKeyboard(KeySpecial_t key, ButtonState_t state) {}
void Callbacks::idle() {}

static void GLFWCALL onResize(int width, int height);
static void GLFWCALL mouseEvent(int button, int state);
static void GLFWCALL mouseDrag(int x, int y);
static void GLFWCALL specialKeypress(int key, int state);
static void GLFWCALL charKeypress(wchar_t key, int state);
static void GLFWCALL draw();
static void idle();

bool init(int windowWidth, int windowHeight)
{
	// Initialize GLFW -- this is the library
	// that creates the window, and handles input
	if (glfwInit() != GL_TRUE)
	{
		fprintf(stderr, "ERROR: Could not initialize GLFW\n");
		return false;
	}

	// Tell GLFW that we want an OpenGL 3.3+ core profile rendering context
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3); // Set OpenGL version to 3.3+
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Request core profile
	glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Disable legacy

	// Initialize the window & create
	if (glfwOpenWindow(windowWidth, windowHeight,
			8, 8, 8, 8, // RGBA bits
			24, 0, // depth, stencil bits
			GLFW_WINDOW) != GL_TRUE)
	{
		fprintf(stderr, "ERROR: Could not open window\n");
		glfwTerminate();
		return false;
	}

	// Initialize extensions
	//  This finds the function pointers for all OpenGL functions
	//  from the OpenGL context.
	// When an OpenGL context is first created, all OpenGL
	// functions will actually be null pointers.
	if ( gl3wInit() != 0 )
	{
		fprintf(stderr, "Error: Failed to initialize GL3\n");
		return false;
	}
	if (isGLError())
	{
		return false;
	}
	if ( !gl3wIsSupported(3,3) )
	{
		fprintf(stderr, "ERROR: OpenGL 3.3+ is not supported\n");
		return false;
	}

	// Set up callbacks
	glfwSetWindowRefreshCallback(draw);
	glfwSetWindowSizeCallback(onResize);
	glfwSetKeyCallback(specialKeypress);
	glfwSetCharCallback((void(*)(int,int))charKeypress);
	glfwSetMouseButtonCallback(mouseEvent);
	glfwSetMousePosCallback(mouseDrag);

	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
	return true;
}

void setWindowTitle(const char *title)
{
	glfwSetWindowTitle(title);
}

void shutdown()
{
	glfwTerminate();
}

void setCallbacks(Callbacks *callbacks)
{
	_callbacks = callbacks;
	onResize(_windowWidth, _windowHeight);
}

void getMousePos(int *x, int *y)
{
	assert(x != 0 && y != 0);
	glfwGetMousePos(x,y);
}

double getTime()
{
	return glfwGetTime();
}

void setTargetFPS(const float fps)
{
	_targetFPS = fps;
}

void mainLoop()
{
	float lastRepaintTime = 0;
	const float repaintTime = 1.0f / _targetFPS;
	while (_keepRunning)
	{
		float currTime = getTime();
		// Poll for a keypress
		if (_repaint && (currTime - lastRepaintTime > repaintTime))
		{
			draw();
			_repaint = false;
			lastRepaintTime = currTime;
			glfwSwapBuffers();
		}
		else
		{
			glfwSleep( 0.5f * (currTime - lastRepaintTime) );
			idle();
			glfwPollEvents();
		}
		// Allow the window to close when the user clicks the close window button
		_keepRunning = _keepRunning && glfwGetWindowParam( GLFW_OPENED );
	}
}

void exitMainLoop()
{
	_keepRunning = false;
}

static void GLFWCALL onResize(int width, int height)
{
	glViewport(0,0,width,height);
	_windowWidth = width;
	_windowHeight = height;
	if (_callbacks)
	{
		_callbacks->windowResize(width,height);
		_repaint = true;
	}
}
static void GLFWCALL mouseEvent(int button, int state)
{
	if (_callbacks)
	{
		ButtonState_t _state;
		if (state == GLFW_PRESS)
		{
			_state = BUTTON_DOWN;
		}
		else
		{
			_state = BUTTON_UP;
		}

		MouseButton_t _button;
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:
			_button = MOUSE_LEFT;
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			_button = MOUSE_RIGHT;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			_button = MOUSE_MIDDLE;
			break;
		default:
			return;
		}

		_mouseButtonsDown ^= _button;

		int x, y;
		glfwGetMousePos(&x,&y);

		_callbacks->mouseEvent(_button,_state, x, _windowHeight - y);

		_repaint = true;
	}
}
static void GLFWCALL mouseDrag(int x, int y)
{
	if (_callbacks && _mouseButtonsDown)
	{
		_callbacks->mouseDrag(x, _windowHeight - y);
		_repaint = true;
	}
}
static void GLFWCALL specialKeypress(int key, int state)
{
	if (_callbacks)
	{
		ButtonState_t _state;
		if (state == GLFW_PRESS)
		{
			_state = BUTTON_DOWN;
		}
		else
		{
			_state = BUTTON_UP;
		}
		switch (key)
		{
		case GLFW_KEY_F1:    _callbacks->specialKeyboard(KEY_F1,_state); break;
		case GLFW_KEY_F2:    _callbacks->specialKeyboard(KEY_F2,_state); break;
		case GLFW_KEY_F3:    _callbacks->specialKeyboard(KEY_F3,_state); break;
		case GLFW_KEY_F4:    _callbacks->specialKeyboard(KEY_F4,_state); break;
		case GLFW_KEY_F5:    _callbacks->specialKeyboard(KEY_F5,_state); break;
		case GLFW_KEY_F6:    _callbacks->specialKeyboard(KEY_F6,_state); break;
		case GLFW_KEY_F7:    _callbacks->specialKeyboard(KEY_F7,_state); break;
		case GLFW_KEY_F8:    _callbacks->specialKeyboard(KEY_F8,_state); break;
		case GLFW_KEY_F9:    _callbacks->specialKeyboard(KEY_F9,_state); break;
		case GLFW_KEY_F10:   _callbacks->specialKeyboard(KEY_F10,_state); break;
		case GLFW_KEY_F11:   _callbacks->specialKeyboard(KEY_F11,_state); break;
		case GLFW_KEY_F12:   _callbacks->specialKeyboard(KEY_F12,_state); break;

		case GLFW_KEY_UP:    _callbacks->specialKeyboard(KEY_UP,_state); break;
		case GLFW_KEY_DOWN:  _callbacks->specialKeyboard(KEY_DOWN,_state); break;
		case GLFW_KEY_LEFT:  _callbacks->specialKeyboard(KEY_LEFT,_state); break;
		case GLFW_KEY_RIGHT: _callbacks->specialKeyboard(KEY_RIGHT,_state); break;

		case GLFW_KEY_LSHIFT:  _callbacks->specialKeyboard(KEY_LSHIFT,_state); break;
		case GLFW_KEY_RSHIFT:  _callbacks->specialKeyboard(KEY_RSHIFT,_state); break;
		case GLFW_KEY_LCTRL:   _callbacks->specialKeyboard(KEY_LCTRL,_state); break;
		case GLFW_KEY_RCTRL:   _callbacks->specialKeyboard(KEY_RCTRL,_state); break;
		case GLFW_KEY_LALT:    _callbacks->specialKeyboard(KEY_LALT,_state); break;
		case GLFW_KEY_RALT:    _callbacks->specialKeyboard(KEY_RALT,_state); break;

		case GLFW_KEY_INSERT:  _callbacks->specialKeyboard(KEY_INSERT,_state); break;
		case GLFW_KEY_DEL:     _callbacks->specialKeyboard(KEY_DELETE,_state); break;
		case GLFW_KEY_HOME:    _callbacks->specialKeyboard(KEY_HOME,_state); break;
		case GLFW_KEY_END:     _callbacks->specialKeyboard(KEY_END,_state); break;
		case GLFW_KEY_PAGEUP:  _callbacks->specialKeyboard(KEY_PAGEUP,_state); break;
		case GLFW_KEY_PAGEDOWN:_callbacks->specialKeyboard(KEY_PAGEDOWN,_state); break;

		case GLFW_KEY_KP_0:    _callbacks->specialKeyboard(KEY_KP_0,_state); break;
		case GLFW_KEY_KP_1:    _callbacks->specialKeyboard(KEY_KP_1,_state); break;
		case GLFW_KEY_KP_2:    _callbacks->specialKeyboard(KEY_KP_2,_state); break;
		case GLFW_KEY_KP_3:    _callbacks->specialKeyboard(KEY_KP_3,_state); break;
		case GLFW_KEY_KP_4:    _callbacks->specialKeyboard(KEY_KP_4,_state); break;
		case GLFW_KEY_KP_5:    _callbacks->specialKeyboard(KEY_KP_5,_state); break;
		case GLFW_KEY_KP_6:    _callbacks->specialKeyboard(KEY_KP_6,_state); break;
		case GLFW_KEY_KP_7:    _callbacks->specialKeyboard(KEY_KP_7,_state); break;
		case GLFW_KEY_KP_8:    _callbacks->specialKeyboard(KEY_KP_8,_state); break;
		case GLFW_KEY_KP_9:    _callbacks->specialKeyboard(KEY_KP_9,_state); break;
		case GLFW_KEY_KP_DIVIDE:    _callbacks->specialKeyboard(KEY_KP_DIV,_state); break;
		case GLFW_KEY_KP_MULTIPLY:  _callbacks->specialKeyboard(KEY_KP_MULT,_state); break;
		case GLFW_KEY_KP_SUBTRACT:  _callbacks->specialKeyboard(KEY_KP_SUB,_state); break;
		case GLFW_KEY_KP_ADD:       _callbacks->specialKeyboard(KEY_KP_ADD,_state); break;
		case GLFW_KEY_KP_DECIMAL:   _callbacks->specialKeyboard(KEY_KP_DECIMAL,_state); break;
		case GLFW_KEY_KP_EQUAL:     _callbacks->specialKeyboard(KEY_KP_EQUAL,_state); break;
		case GLFW_KEY_KP_ENTER:     _callbacks->specialKeyboard(KEY_KP_ENTER,_state); break;

		case GLFW_KEY_KP_NUM_LOCK:  _callbacks->specialKeyboard(KEY_NUMLOCK,_state); break;
		case GLFW_KEY_CAPS_LOCK:    _callbacks->specialKeyboard(KEY_CAPSLOCK,_state); break;
		case GLFW_KEY_SCROLL_LOCK:  _callbacks->specialKeyboard(KEY_SCROLLLOCK,_state); break;

		case GLFW_KEY_PAUSE:     _callbacks->specialKeyboard(KEY_PAUSE,_state); break;
		case GLFW_KEY_BACKSPACE: _callbacks->specialKeyboard(KEY_BACKSPACE,_state); break;
		case GLFW_KEY_TAB:       _callbacks->specialKeyboard(KEY_TAB,_state); break;
		case GLFW_KEY_ENTER:     _callbacks->specialKeyboard(KEY_ENTER,_state); break;
		case GLFW_KEY_ESC:       _callbacks->specialKeyboard(KEY_ESC,_state); break;
		case GLFW_KEY_SPACE:     _callbacks->specialKeyboard(KEY_SPACE,_state); break;

		default:
			// key is a printable character
			_callbacks->specialKeyboard((KeySpecial_t)key,_state);
			break;
		}
		_repaint = true;
	}
}

static void GLFWCALL charKeypress(wchar_t key, int state)
{
	if (state == GLFW_RELEASE)
		return;
	if (_callbacks)
	{
		_callbacks->charKeyboard(key);
		_repaint = true;
	}
}

static void GLFWCALL draw()
{
	if (_callbacks)
	{
		_callbacks->repaint();
	}
}
static void idle()
{
	if (_callbacks)
	{
		_callbacks->idle();
		_repaint = true;
	}
}

} // namespace
