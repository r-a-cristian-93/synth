#ifndef NOTE_MANAGER_H
#define NOTE_MANAGER_H

#include <list>
#include <cstdint>
#include <OrganEngine/OrganOscillator.h>
#include <OrganEngine/Config.h>

#define MAX_NOTES 128

enum KeyState {
	KEY_STATE_OFF,
	KEY_STATE_ON
};

extern uint8_t keysList[MANUAL_KEYS];

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
