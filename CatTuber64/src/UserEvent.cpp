#include"UserEvent.h"




void UserEvent::Init()
{
	auto& _this = GetIns();
	if (_this.eventStart != 0)return;
	_this.eventStart=SDL_RegisterEvents(EventNum);
}

bool UserEvent::PushEvent(_UserEvent event, void* data1, void* data2,  bool hightPriority)
{
	SDL_Event sdlEvent;
	sdlEvent.user.type = GetIns().eventStart + event;
	sdlEvent.user.code = event;
	sdlEvent.user.data1 = data1;
	sdlEvent.user.data2 = data2;

	return SDL_PushEvent(&sdlEvent);
}

void UserEvent::HandleUserEvent(SDL_UserEvent* event)
{
	switch (event->code)
	{
	case TASK:
	{
	//对于TASK,DATA1 为函数， data2为参数
		void(*func)(void* data)= (void(*)(void*))event->data1;
		func(event->data2);
		break;
	}
	default:
		break;
	}


}
