#ifndef NOTE_MANAGER_H
#define NOTE_MANAGER_H

#include <list>
#include <OrganEngine/Note.h>
#include <OrganEngine/Envelope.h>
#include <OrganEngine/OrganOscillator.h>

#define MAX_NOTES 128

enum KeyState {
	KEY_STATE_OFF,
	KEY_STATE_ON
};

extern Note notesList[MAX_NOTES];
extern uint8_t keysList[MANUAL_KEYS];

void note_manager_initialize();

__attribute__((always_inline)) inline
void note_on(uint8_t midiNote) {
	if (midiNote < MANUAL_KEY_FIRST || midiNote >= MANUAL_KEY_LAST)
		return;

	keysList[midiNote - MANUAL_KEY_FIRST] = KEY_STATE_ON;
}

__attribute__((always_inline)) inline
void note_off(uint8_t midiNote) {
	if (midiNote < MANUAL_KEY_FIRST || midiNote >= MANUAL_KEY_LAST)
		return;

	keysList[midiNote - MANUAL_KEY_FIRST] = KEY_STATE_OFF;
}

#endif
