#ifndef _UserEvent_H
#define _UserEvent_H

#include<SDL3/SDL.h>


class UserEvent
{
public:
	static UserEvent& GetIns() { static UserEvent ref; return ref; }

	
	
	static void Init();





	enum _UserEvent
	{
		TASK,
		EventNum
	};
	static bool PushEvent(_UserEvent, void* data1=NULL,void* data2 = NULL,bool hightPriority=false);

	static void HandleUserEvent(SDL_UserEvent* event);

private:
	Uint32 eventStart = 0;
};
























#endif