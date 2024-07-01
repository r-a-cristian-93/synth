#ifndef NOTE_MANAGER_H
#define NOTE_MANAGER_H

#include <list>
#include <OrganEngine/Note.h>
#include <OrganEngine/Envelope.h>
#include <OrganEngine/OrganOscillator.h>

#define MAX_NOTES 128
#define TONEWHEEL_KEY_INCREMENT (800)

extern Note notesList[MAX_NOTES];

void note_manager_initialize();

__attribute__((always_inline)) inline
void note_on(uint8_t midiNote) {
	// notesList[midiNote].envelope.noteOn();

	if (midiNote < MANUL_KEY_FIRST || midiNote >= MANUAL_KEY_LAST)
		return;

	for (uint8_t drawbarIndex = 0; drawbarIndex < DRAWBARS_COUNT; drawbarIndex++)
	{
		tonewheelAmplitude[tonewheelMap[midiNote - MANUL_KEY_FIRST][drawbarIndex]] += TONEWHEEL_KEY_INCREMENT;
	}
}

__attribute__((always_inline)) inline
void note_off(uint8_t midiNote) {
	// notesList[midiNote].envelope.noteOff();

	if (midiNote < MANUL_KEY_FIRST || midiNote >= MANUAL_KEY_LAST)
		return;

	for (uint8_t drawbarIndex = 0; drawbarIndex < DRAWBARS_COUNT; drawbarIndex++)
	{
		if (tonewheelAmplitude[tonewheelMap[midiNote - MANUL_KEY_FIRST][drawbarIndex]] > 0)
			tonewheelAmplitude[tonewheelMap[midiNote - MANUL_KEY_FIRST][drawbarIndex]] -= TONEWHEEL_KEY_INCREMENT;
	}
}

#endif
