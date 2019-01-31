#include <stdbool.h>
#include <SDL/SDL.h>

// Poll the SDL input event queue
int inputPollEvent(struct agnostic_event *event){
	int r;
	r = SDL_PollEvent(&event->event);
	return r;
}

bool inputEventCheck(struct agnostic_event *event, input_event_type event_type){
	bool r = false;
	if (event_type == SDL_KEYDOWN){
		if (event->event.type == SDL_KEYDOWN){
			r = true;
		}
	}
	if (event_type == SDL_QUIT){
		if (event->event.type == SDL_QUIT){
			r = true;
		}
	}
	return r;
}

// Return the actual keypress symbol from an event 
keypress inputEventKeypress(struct agnostic_event *event){
	keypress k;
	if (inputEventCheck(event, SDL_KEYDOWN)){
		k = event->event.key.keysym.sym;
	}
	return k;
}

// Check the keypress from the event against a known key symbol
bool inputEventKeypressCheck(struct agnostic_event *event, keypress key){
	bool r = false;
	keypress k;
	k = inputEventKeypress(event);
	if (k == key){
		r = true;
	}
	return r;
}