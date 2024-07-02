#include <OrganEngine/NoteManager.h>

Note notesList[MAX_NOTES] = {Note{}};
uint8_t keysList[MANUAL_KEYS] = {KEY_STATE_OFF};


void note_manager_initialize() {
	for (uint8_t noteIndex = 0; noteIndex < MAX_NOTES; noteIndex++)
	{
		notesList[noteIndex].midiNote = noteIndex;
	}
}