#include <mint/osbind.h>
#include <gem.h>

// Poll the input event queue
int inputPollEvent(struct agnostic_event *event){
	int r;
	return r;
}

bool inputEventCheck(struct agnostic_event *event, input_event_type event_type){
	bool r = false;
	return r;
}

// Return the actual keypress symbol from an event 
keypress inputEventKeypress(struct agnostic_event *event){
	keypress k;
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