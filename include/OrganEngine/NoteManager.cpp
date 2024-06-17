#include "NoteManager.h"

Note notesList[MAX_NOTES] = {Note{}};

void note_manager_initialize() {
	for (uint8_t noteIndex = 0; noteIndex < MAX_NOTES; noteIndex++)
	{
		notesList[noteIndex].midiNote = noteIndex;
	}
}