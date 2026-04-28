#ifndef _DuiCommon_h
#define _DuiCommon_h




//SDL
#include<SDL3/SDL.h>

//duilib
#include"duilib/duilib.h"


#define GETDUISTRING(strid) ui::GlobalManager::Instance().Lang().GetStringViaID(strid)

#endif