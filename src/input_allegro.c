#include <stdbool.h>

void inputSleep(){
	
}

// Poll the SDL input event queue
int inputPollEvent(struct agnostic_event *event){
	int r = 0;
	keypress k;
	event->key = NULL;
	event->event = NULL;
	
	poll_joystick();
	poll_keyboard();
	
	if (keypressed()) {
		k = readkey();
		event->key = k >> 8;
		event->event = EVENT_KEYDOWN;
		//printf("Event [%d][%d]\n", event->key, EVENT_KEYDOWN);
		return 1;
	}
	
	// if joystick input
	// ...
	// Emulate keypresses...
	// Put KEY_DOWN in the keypress buffer
	// Put KEY_UP in the keypress buffer
	// Put KEY_LEFT in the keypress buffer
	// Put KEY_RIGHT in the keypress buffer
	// Put KEY_ENTER in the keypress buffer
	
	// if mouse input
	// ...
	
	return r;
}

bool inputEventCheck(struct agnostic_event *event, input_event_type event_type){
	bool r = false;
	
	if (event->event){
		//printf("Checking for event [%d]\n", event_type);
		if (event->event == event_type){
			//printf("Matching\n");
			r = true;
		}
	}
	
	return r;
}

// Return the actual keypress symbol from an event 
keypress inputEventKeypress(struct agnostic_event *event){
	keypress k;

	if (inputEventCheck(event, EVENT_KEYDOWN)){
		// Read keyboard scancode, as per:
		// https://www.allegro.cc/manual/4/api/keyboard-routines/readkey
		//printf("Last key was %d\n", event->key);
		return event->key;
	}
	return NULL;
}

// Check the keypress from the event against a known key symbol
bool inputEventKeypressCheck(struct agnostic_event *event, keypress key){
	bool r = false;
	keypress k;
	
	k = inputEventKeypress(event);
	if (k == key){
		//printf("Matched key\n");
		r = true;
	}
	return r;
}