#pragma once
#ifndef _CATSTEAM_H_
#define _CATSTEAM_H_

//#include <windows.h>
#include<iostream>

namespace cat::steam
{
	bool CheckSteam(bool isRunAsAdmin);
	std::string GetSteamUserLanguage();
	std::string GetSteamUILanguage();
	void ShutDownAPI();
	bool IsSteamInited();


};





#endif // !_CATUTIL_H_



