#pragma once

#ifdef USE_SDL
#include <SDL/SDL.h>
#endif

#ifdef USE_GEM
#include <mint/osbind.h>
#include <gem.h>
#endif

// Key definitions for SDL input handling
#ifdef USE_SDL
#define EVENT_QUIT 	SDL_QUIT
#define EVENT_KEYDOWN SDL_KEYDOWN
#define KEY_q 		SDLK_q
#define KEY_c 		SDLK_c
#define KEY_F1 		SDLK_F1
#define KEY_F2 		SDLK_F2
#define KEY_F3 		SDLK_F3
#define KEY_F4 		SDLK_F4
#define KEY_F5 		SDLK_F5
#define KEY_F6 		SDLK_F6
#define KEY_F7 		SDLK_F7
#define KEY_F8 		SDLK_F8
#define KEY_F9 		SDLK_F9
#define KEY_F10 	SDLK_F10
#define KEY_F11 	SDLK_F11
#define KEY_F12 	SDLK_F12
#define KEY_DOWN 	SDLK_DOWN
#define KEY_UP 		SDLK_UP
#define KEY_LEFT 	SDLK_LEFT
#define KEY_RIGHT 	SDLK_RIGHT
#define KEY_TAB 	SDLK_TAB
#define KEY_ESCAPE	SDLK_ESCAPE
#define KEY_RETURN	SDLK_RETURN
typedef SDLKey keypress;
typedef unsigned short input_event_type;
#endif

#ifdef USE_GEM
#define EVENT_QUIT	0xffff
#define EVENT_KEYDOWN MU_KEYBD
#define KEY_Q		0x1071
#define KEY_q		0x1051
#define KEY_c 		0x2E63
#define KEY_F1 		0x3B00
#define KEY_F2 		0x3C00
#define KEY_F3 		0x3D00
#define KEY_F4 		0x3E00
#define KEY_F5 		0x3F00
#define KEY_F6 		0x4000
#define KEY_F7 		0x4100
#define KEY_F8 		0x4200
#define KEY_F9 		0x4300
#define KEY_F10 	0x4400
#define KEY_F11 	0x5400
#define KEY_F12 	0x5500
#define KEY_DOWN 	0x5000
#define KEY_UP 		0x4800
#define KEY_LEFT 	0x4B00
#define KEY_RIGHT 	0x4D00
#define KEY_TAB 	0x0F09
#define KEY_ESCAPE	0x011b
#define KEY_RETURN	0x1C0D
typedef short keypress;
typedef int input_event_type;
#endif

#ifdef USE_ALLEGRO
#define EVENT_QUIT 	"quit"
#define EVENT_KEYDOWN "keypressed"
#define KEY_q 		KEY_Q
#define KEY_c 		KEY_C
#define KEY_RETURN	KEY_ENTER
#define KEY_ESCAPE	KEY_ESC
typedef int keypress;
typedef int input_event_type;
#endif

// Platform/library agnostic event handler data structure
typedef struct agnostic_event {
#ifdef USE_SDL
	SDL_Event event;
#endif
#ifdef USE_GEM
	int event;
	int ev_flags;
	int ev_mbclicks;
	int ev_mbmask;
	int ev_mbstate;
	int ev_mm1_flags;
	int ev_mm1_x;
	int ev_mm1_y;
	int ev_mm1_w;
	int ev_mm1_h;
	int ev_mm2_flags;
	int ev_mm2_x;
	int ev_mm2_y;
	int ev_mm2_w;
	int ev_mm2_h;
	short ev_msg;
	int ev_interval;
	short ev_mmo_x;
	short ev_mmo_y;
	short ev_mbutton;
	short ev_kbd_special;
	short ev_keycode;
	short ev_clicks;
#endif
} agnostic_event;

// Public event handling functions
void inputSleep();
int inputPollEvent(struct agnostic_event *event);
bool inputEventCheck(struct agnostic_event *event, input_event_type event_type);
keypress inputEventKeypress(struct agnostic_event *event);
bool inputEventKeypressCheck(struct agnostic_event *event, keypress key);