#ifndef SHARE_RESOURCES_H
#define SHARE_RESOURCES_H

#include <list>
#include <mutex>
#include <OrganEngine/Note.h>

extern std::list<Note> notesList;
extern std::mutex notesMutex;

#endif
