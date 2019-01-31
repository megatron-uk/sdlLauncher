//#include <mint/osbind.h>
#include <gem.h>
#include <mt_gem.h>

// Include our generic input handling headers
#include "input.h"

// Poll the input event queue
int inputPollEvent(struct agnostic_event *event){
	int r = 0;
	event->ev_flags = MU_KEYBD|MU_BUTTON;
	
	//printf("checking event\n");
	event->event = evnt_multi(event->ev_flags,
		event->ev_mbclicks, event->ev_mbmask, event->ev_mbstate,
		event->ev_mm1_flags, event->ev_mm1_x, event->ev_mm1_y, event->ev_mm1_w, event->ev_mm1_h,
		event->ev_mm2_flags, event->ev_mm2_x, event->ev_mm2_y, event->ev_mm2_w, event->ev_mm2_h,
		&event->ev_msg,
		event->ev_interval,
		&event->ev_mmo_x, &event->ev_mmo_y, &event->ev_mbutton, &event->ev_kbd_special, &event->ev_keycode, &event->ev_clicks
	);
	//printf("got event\n");
	return r;
}

bool inputEventCheck(struct agnostic_event *event, input_event_type event_type){
	bool r = false;
	
	// Test if keyboard event bit is set
	if (event->event & event_type){
		r = true;
		printf("Keyboard event\n");
	}
	return r;
}

// Return the actual keypress symbol from an event 
keypress inputEventKeypress(struct agnostic_event *event){
	keypress k;
	k = event->ev_keycode;
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