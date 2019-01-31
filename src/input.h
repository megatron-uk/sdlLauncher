#pragma once

#ifdef USE_SDL
#include <SDL/SDL.h>
#endif

#ifdef USE_ALLEGRO
#include <allegro.h>
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
#define EVENT_QUIT	"QUIT"
#define EVENT_KEYDOWN MU_KEYBD
#define KEY_q		0x15
#define KEY_c 		0x22
#define KEY_F1 		59
#define KEY_F2 		60
#define KEY_F3 		61
#define KEY_F4 		62
#define KEY_F5 		63
#define KEY_F6 		64
#define KEY_F7 		65
#define KEY_F8 		66
#define KEY_F9 		67
#define KEY_F10 	0x4f
#define KEY_F11 	0x56
#define KEY_F12 	0x5e
#define KEY_DOWN 	0x60
#define KEY_UP 		0x63
#define KEY_LEFT 	0x61
#define KEY_RIGHT 	0x6a
#define KEY_TAB 	0x0d
#define KEY_ESCAPE	0x08
#define KEY_RETURN	0x5c
typedef short keypress;
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
int inputPollEvent(struct agnostic_event *event);
bool inputEventCheck(struct agnostic_event *event, input_event_type event_type);
keypress inputEventKeypress(struct agnostic_event *event);
bool inputEventKeypressCheck(struct agnostic_event *event, keypress key);