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
#define KEY_F10 		SDLK_F10
#define KEY_F11 		SDLK_F11
#define KEY_F12 		SDLK_F12
#define KEY_DOWN 	SDLK_DOWN
#define KEY_UP 		SDLK_UP
#define KEY_LEFT 	SDLK_LEFT
#define KEY_RIGHT 	SDLK_RIGHT
#define KEY_TAB 		SDLK_TAB
#define KEY_ESCAPE	SDLK_ESCAPE
#define KEY_RETURN	SDLK_RETURN
#endif

typedef struct agnostic_event {
#ifdef USE_SDL
	SDL_Event event;
#endif
} agnostic_event;

#ifdef USE_SDL
	typedef SDLKey keypress;
	typedef unsigned short input_event_type;
#endif

int inputPollEvent(struct agnostic_event *event);
bool inputEventCheck(struct agnostic_event *event, input_event_type event_type);
keypress inputEventKeypress(struct agnostic_event *event);
bool inputEventKeypressCheck(struct agnostic_event *event, keypress key);