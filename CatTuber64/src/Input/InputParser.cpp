
#include "Input/InputParser.h"
#include"Util/Util.h"


const char* InputParser::KeyNameToBaseName(const std::string& keyName)
{
    return nullptr;
}

const char* InputParser::ParamNameToButtonBaseName(const std::string& paramName)
{
	//统一转大写
	std::string upper = util::StringToUpper(paramName);


	std::string keyName;
	if (util::IsStringStartWith(upper, "KEYBOARD_"))
		keyName = upper.substr(sizeof("KEYBOARD_") - 1);
	else if (util::IsStringStartWith(upper, "BUTTON_"))
		keyName = upper.substr(sizeof("BUTTON_") - 1);
	else if (util::IsStringStartWith(upper, "CAT_KEY_"))
		keyName = upper.substr(sizeof("CAT_KEY_") - 1);
	if (!keyName.empty())
	{
		auto& keyDesk = GetIns().keyDesk;
		auto it = keyDesk.find(keyName);
		if (it != keyDesk.end())
			return it->second;
	}


	return NULL;
}

const char* InputParser::ParamNameToAxisBaseName(const std::string& paramName)
{
	//原cattuber模型
	if (paramName == "CAT_MouseX")return "Mouse.Pos.X";
	if (paramName == "CAT_MouseY")return "Mouse.Pos.Y";
	return nullptr;
}

const char* InputParser::ButtonBaseNameToUIName(const char* buttonBaseName)
{
	auto& baseNameToScanCodeMap = GetIns().baseNameToScanCodeMap;
	auto it=baseNameToScanCodeMap.find(buttonBaseName);
	if (it != baseNameToScanCodeMap.end())
	{
		return SDL_GetKeyName(SDL_GetKeyFromScancode(it->second, SDL_KMOD_NONE, false));
	}
	else
	{
		//如果是鼠标相关
		if (0 == SDL_strncmp(buttonBaseName, "Mouse", 5))
		{
			if (0 == SDL_strcmp(buttonBaseName, "Mouse.Left"))
				return "MB1";
			if (0 == SDL_strcmp(buttonBaseName, "Mouse.Right"))
				return "MB2";
			if (0 == SDL_strcmp(buttonBaseName, "Mouse.Middle"))
				return "MB3";
			if (0 == SDL_strcmp(buttonBaseName, "Mouse.X1"))
				return "MB4";
			if (0 == SDL_strcmp(buttonBaseName, "Mouse.X2"))
				return "MB5";
		}
	}
	return nullptr;
}




InputParser::InputParser()
{
	//keyDesk
	{
		keyDesk["ESCAPE"] = "Keyboard.ESC";
		keyDesk["ESC"] = "Keyboard.ESC";

		keyDesk["1"] = "Keyboard.1";
		keyDesk["2"] = "Keyboard.2";
		keyDesk["3"] = "Keyboard.3";
		keyDesk["4"] = "Keyboard.4";
		keyDesk["5"] = "Keyboard.5";
		keyDesk["6"] = "Keyboard.6";
		keyDesk["7"] = "Keyboard.7";
		keyDesk["8"] = "Keyboard.8";
		keyDesk["9"] = "Keyboard.9";
		keyDesk["0"] = "Keyboard.0";

		keyDesk["MINUS"] = "Keyboard.MINUS";
		keyDesk["EQUALS"] = "Keyboard.EQUALS";
		keyDesk["BACKSPACE"] = "Keyboard.BACKSPACE";
		keyDesk["BACK"] = "Keyboard.BACKSPACE";
		keyDesk["TAB"] = "Keyboard.TAB";


		keyDesk["Q"] = "Keyboard.Q";
		keyDesk["W"] = "Keyboard.W";
		keyDesk["E"] = "Keyboard.E";
		keyDesk["R"] = "Keyboard.R";
		keyDesk["T"] = "Keyboard.T";
		keyDesk["Y"] = "Keyboard.Y";
		keyDesk["U"] = "Keyboard.U";
		keyDesk["I"] = "Keyboard.I";
		keyDesk["O"] = "Keyboard.O";
		keyDesk["P"] = "Keyboard.P";


#define _TOSTR(X) #X
#define PUSHDESK(X) keyDesk[#X]= _TOSTR(Keyboard.##X);
		PUSHDESK(LBRACKET);
		PUSHDESK(RBRACKET);
		keyDesk["ENTER"] = "Keyboard.RETURN";
		keyDesk["RETURN"] = "Keyboard.RETURN";
		keyDesk["LCONTROL"] = "Keyboard.LCTRL";
		keyDesk["LCTRL"] = "Keyboard.LCTRL";

		PUSHDESK(A);
		PUSHDESK(S);
		PUSHDESK(D);
		PUSHDESK(F);
		PUSHDESK(G);
		PUSHDESK(H);
		PUSHDESK(J);
		PUSHDESK(K);
		PUSHDESK(L);

		PUSHDESK(SEMICOLON);
		PUSHDESK(APOSTROPHE);
		PUSHDESK(GRAVE);
		PUSHDESK(LSHIFT);
		PUSHDESK(BACKSLASH);


		PUSHDESK(Z);
		PUSHDESK(X);
		PUSHDESK(C);
		PUSHDESK(V);
		PUSHDESK(B);
		PUSHDESK(N);
		PUSHDESK(M);

		PUSHDESK(COMMA);
		PUSHDESK(PERIOD);
		PUSHDESK(SLASH);
		PUSHDESK(RSHIFT);

		keyDesk["MULTIPLY"] = "Keyboard.KP_MULTIPLY";
		keyDesk["NUM_STAR"] = "Keyboard.KP_MULTIPLY";
		keyDesk["NUM_MULTIPLY"] = "Keyboard.KP_MULTIPLY";
		keyDesk["KP_MULTIPLY"] = "Keyboard.KP_MULTIPLY";

		keyDesk["LMENU"] = "Keyboard.LALT";
		keyDesk["LALT"] = "Keyboard.LALT";
		PUSHDESK(SPACE);
		keyDesk["CAPITAL"] = "Keyboard.CAPSLOCK";
		keyDesk["CAPSLOCK"] = "Keyboard.CAPSLOCK";
		keyDesk["CAPS"] = "Keyboard.CAPSLOCK";

		PUSHDESK(F1);
		PUSHDESK(F2);
		PUSHDESK(F3);
		PUSHDESK(F4);
		PUSHDESK(F5);
		PUSHDESK(F6);
		PUSHDESK(F7);
		PUSHDESK(F8);
		PUSHDESK(F9);
		PUSHDESK(F10);


		keyDesk["NUM"] = "Keyboard.NUMLOCK";
		keyDesk["NUMLOCK"] = "Keyboard.NUMLOCK";
		keyDesk["SCROLL"] = "Keyboard.SCROLLLOCK";
		keyDesk["SCROLLLOCK"] = "Keyboard.SCROLLLOCK";

#define PUSHDESK_NUM(X) keyDesk[_TOSTR(NUMPAD##X)]=_TOSTR(Keyboard.KP_##X);\
keyDesk[_TOSTR(KP_##X)]=_TOSTR(Keyboard.KP_##X);\
keyDesk[_TOSTR(NUM_##X)]=_TOSTR(Keyboard.KP_##X)
		keyDesk["NUMPAD7"] = "Keyboard.KP_7";
		keyDesk["KP_7"] = "Keyboard.KP_7";
		keyDesk["NUM_7"] = "Keyboard.KP_7";
		PUSHDESK_NUM(8);
		PUSHDESK_NUM(9);
		keyDesk["SUBTRACT"] = "Keyboard.KP_MINUS";
		keyDesk["NUMPADMINUS"] = "Keyboard.KP_MINUS";
		keyDesk["KP_MINUS"] = "Keyboard.KP_MINUS";
		PUSHDESK_NUM(4);
		PUSHDESK_NUM(5);
		PUSHDESK_NUM(6);
		keyDesk["ADD"] = "Keyboard.KP_PLUS";
		keyDesk["NUMPADPLUS"] = "Keyboard.KP_PLUS";
		keyDesk["NUM_PLUS"] = "Keyboard.KP_PLUS";
		keyDesk["KP_PLUS"] = "Keyboard.KP_PLUS";
		PUSHDESK_NUM(1);
		PUSHDESK_NUM(2);
		PUSHDESK_NUM(3);
		PUSHDESK_NUM(0);
		keyDesk["DECIMAL"] = "Keyboard.KP_PERIOD";
		keyDesk["NUM_DECIMAL"] = "Keyboard.KP_PERIOD";
		keyDesk["NUM_PERIOD"] = "Keyboard.KP_PERIOD";
		keyDesk["NUMPADPERIOD"] = "Keyboard.KP_PERIOD";
		keyDesk["KP_PERIOD"] = "Keyboard.KP_PERIOD";
		keyDesk["OEM_102"] = "Keyboard.NONUSBACKSLASH";
		keyDesk["NONUSBACKSLASH"] = "Keyboard.NONUSBACKSLASH";


		PUSHDESK(F11);
		PUSHDESK(F12);
		PUSHDESK(F13);
		PUSHDESK(F14);
		PUSHDESK(F15);

		PUSHDESK(KANA);
		PUSHDESK(YEN);

		keyDesk["NUMPADEQUALS"] = "Keyboard.KP_EQUALS";
		keyDesk["KP_EQUALS"] = "Keyboard.KP_EQUALS";
		keyDesk["NUM_EQUALS"] = "Keyboard.KP_EQUALS";

		keyDesk["PREVTRACK"] = "Keyboard.MEDIA_PREVTRACK";
		keyDesk["MEDIA_PREVTRACK"] = "Keyboard.MEDIA_PREVTRACK";
		keyDesk["AT"] = "Keyboard.KP_AT";
		keyDesk["KP_AT"] = "Keyboard.KP_AT";
		keyDesk["COLON"] = "Keyboard.KP_COLON";
		keyDesk["KP_COLON"] = "Keyboard.KP_COLON";
		keyDesk["STOP"] = "Keyboard.MEDIA_STOP";
		keyDesk["MEDIA_STOP"] = "Keyboard.MEDIA_STOP";
		keyDesk["NEXTTRACK"] = "Keyboard.MEDIA_NEXTTRACK";
		keyDesk["MEDIA_NEXTTRACK"] = "Keyboard.MEDIA_NEXTTRACK";

		keyDesk["NUMPADENTER"] = "Keyboard.KP_ENTER";
		keyDesk["NUM_ENTER"] = "Keyboard.KP_ENTER";
		keyDesk["KP_ENTER"] = "Keyboard.KP_ENTER";
		keyDesk["RCONTROL"] = "Keyboard.RCTRL";
		keyDesk["RCTRL"] = "Keyboard.RCTRL";

		keyDesk["MUTE"] = "Keyboard.MUTE";
		keyDesk["PLAYPAUSE"] = "Keyboard.MEDIA_PLAYPAUSE";
		keyDesk["MEDIA_PLAYPAUSE"] = "Keyboard.MEDIA_PLAYPAUSE";
		keyDesk["MEDIASTOP"] = "Keyboard.MEDIA_STOP";
		keyDesk["MEDIA_STOP"] = "Keyboard.MEDIA_STOP";

		PUSHDESK(VOLUMEDOWN);
		PUSHDESK(VOLUMEUP);
		PUSHDESK(WEBHOME);

		keyDesk["NUMPADCOMMA"] = "Keyboard.KP_COMMA";
		keyDesk["NUM_COMMA"] = "Keyboard.KP_COMMA";
		keyDesk["KP_COMMA"] = "Keyboard.KP_COMMA";
		keyDesk["DIVIDE"] = "Keyboard.KP_DIVIDE";
		keyDesk["NUMPADSLASH"] = "Keyboard.KP_DIVIDE";
		keyDesk["NUM_DIVIDE"] = "Keyboard.KP_DIVIDE";
		keyDesk["KP_DIVIDE"] = "Keyboard.KP_DIVIDE";
		keyDesk["SYSRQ"] = "Keyboard.SYSREQ";
		keyDesk["SYSREQ"] = "Keyboard.SYSREQ";
		keyDesk["RMENU"] = "Keyboard.RALT";
		keyDesk["RALT"] = "Keyboard.RALT";
		PUSHDESK(PAUSE);
		PUSHDESK(HOME);
		PUSHDESK(UP);
		keyDesk["PRIOR"] = "Keyboard.PAGEUP";
		keyDesk["PAGEUP"] = "Keyboard.PAGEUP";
		keyDesk["PGUP"] = "Keyboard.PAGEUP";
		PUSHDESK(LEFT);
		PUSHDESK(RIGHT);
		PUSHDESK(END);
		PUSHDESK(DOWN);
		keyDesk["NEXT"] = "Keyboard.PAGEDOWN";
		keyDesk["PAGEDOWN"] = "Keyboard.PAGEDOWN";
		keyDesk["PGDN"] = "Keyboard.PAGEDOWN";

		PUSHDESK(INSERT);
		PUSHDESK(DELETE);
		PUSHDESK(LWIN);
		PUSHDESK(RWIN);
		keyDesk["LCOMMAND"] = "Keyboard.LWIN";
		keyDesk["LCMD"] = "Keyboard.LWIN";
		keyDesk["RCOMMAND"] = "Keyboard.RWIN";
		keyDesk["RCMD"] = "Keyboard.RWIN";
		PUSHDESK(POWER);
		PUSHDESK(SLEEP);
		PUSHDESK(WAKE);
		PUSHDESK(WEBSEARCH);
		keyDesk["WEBFAVORITES"] = "Keyboard.BOOKMARKS";
		keyDesk["BOOKMARKS"] = "Keyboard.BOOKMARKS";
		PUSHDESK(WEBREFRESH);
		PUSHDESK(WEBSTOP);
		PUSHDESK(WEBFORWARD);
		PUSHDESK(WEBBACK);

		keyDesk["MEDIASELECT"] = "Keyboard.MEDIA_SELECT";
		keyDesk["MEDIA_SELECT"] = "Keyboard.MEDIA_SELECT";



		//鼠标按键
		keyDesk["MB1"] = "Mouse.Left";
		keyDesk["MB2"] = "Mouse.Right";
		keyDesk["MB3"] = "Mouse.Middle";
		keyDesk["MB4"] = "Mouse.X1";
		keyDesk["MB5"] = "Mouse.X2";







	}
	//BaseNameToScanCode
	baseNameToScanCodeMap["Keyboard.ESC"] = SDL_Scancode::SDL_SCANCODE_ESCAPE;
#define PUSHSCANCODE(X) baseNameToScanCodeMap[_TOSTR(Keyboard.##X)]=SDL_Scancode::SDL_SCANCODE_##X
	baseNameToScanCodeMap["Keyboard.1"] = SDL_Scancode::SDL_SCANCODE_1;
	PUSHSCANCODE(2);
	PUSHSCANCODE(3);
	PUSHSCANCODE(4);
	PUSHSCANCODE(5);
	PUSHSCANCODE(6);
	PUSHSCANCODE(7);
	PUSHSCANCODE(8);
	PUSHSCANCODE(9);
	PUSHSCANCODE(0);

	PUSHSCANCODE(MINUS);
	PUSHSCANCODE(EQUALS);
	PUSHSCANCODE(BACKSPACE);
	PUSHSCANCODE(TAB);

	PUSHSCANCODE(Q);
	PUSHSCANCODE(W);
	PUSHSCANCODE(E);
	PUSHSCANCODE(R);
	PUSHSCANCODE(T);
	PUSHSCANCODE(Y);
	PUSHSCANCODE(U);
	PUSHSCANCODE(I);
	PUSHSCANCODE(O);
	PUSHSCANCODE(P);

	PUSHSCANCODE(RETURN);
	PUSHSCANCODE(LCTRL);

	PUSHSCANCODE(A);
	PUSHSCANCODE(S);
	PUSHSCANCODE(D);
	PUSHSCANCODE(F);
	PUSHSCANCODE(G);
	PUSHSCANCODE(H);
	PUSHSCANCODE(J);
	PUSHSCANCODE(K);
	PUSHSCANCODE(L);

	PUSHSCANCODE(SEMICOLON);
	PUSHSCANCODE(APOSTROPHE);
	PUSHSCANCODE(GRAVE);
	PUSHSCANCODE(LSHIFT);
	PUSHSCANCODE(BACKSLASH);

	PUSHSCANCODE(Z);
	PUSHSCANCODE(X);
	PUSHSCANCODE(C);
	PUSHSCANCODE(V);
	PUSHSCANCODE(B);
	PUSHSCANCODE(N);
	PUSHSCANCODE(M);

	PUSHSCANCODE(COMMA);
	PUSHSCANCODE(PERIOD);
	PUSHSCANCODE(SLASH);
	PUSHSCANCODE(RSHIFT);
	PUSHSCANCODE(KP_MULTIPLY);
	PUSHSCANCODE(LALT);
	PUSHSCANCODE(SPACE);
	PUSHSCANCODE(CAPSLOCK);

	PUSHSCANCODE(F1);
	PUSHSCANCODE(F2);
	PUSHSCANCODE(F3);
	PUSHSCANCODE(F4);
	PUSHSCANCODE(F5);
	PUSHSCANCODE(F6);
	PUSHSCANCODE(F7);
	PUSHSCANCODE(F8);
	PUSHSCANCODE(F9);
	PUSHSCANCODE(F10);

	baseNameToScanCodeMap["Keyboard.NUMLOCK"] = SDL_Scancode::SDL_SCANCODE_NUMLOCKCLEAR;//num lock on PC, clear on Mac keyboards
	PUSHSCANCODE(SCROLLLOCK);
	PUSHSCANCODE(KP_7);
	PUSHSCANCODE(KP_8);
	PUSHSCANCODE(KP_9);
	PUSHSCANCODE(KP_MINUS);
	PUSHSCANCODE(KP_4);
	PUSHSCANCODE(KP_5);
	PUSHSCANCODE(KP_6);
	PUSHSCANCODE(KP_PLUS);
	PUSHSCANCODE(KP_1);
	PUSHSCANCODE(KP_2);
	PUSHSCANCODE(KP_3);
	PUSHSCANCODE(KP_0);
	PUSHSCANCODE(KP_PERIOD);
	PUSHSCANCODE(NONUSBACKSLASH);

	PUSHSCANCODE(F11);
	PUSHSCANCODE(F12);
	PUSHSCANCODE(F13);
	PUSHSCANCODE(F14);
	PUSHSCANCODE(F15);

	baseNameToScanCodeMap["Keyboard.KANA"] = SDL_Scancode::SDL_SCANCODE_LANG3;//TODO待验证
	baseNameToScanCodeMap["Keyboard.YEN"] = SDL_Scancode::SDL_SCANCODE_INTERNATIONAL3;//TODO待验证
	PUSHSCANCODE(KP_EQUALS);
	baseNameToScanCodeMap["Keyboard.MEDIA_PREVTRACK"] = SDL_Scancode::SDL_SCANCODE_MEDIA_PREVIOUS_TRACK;
	PUSHSCANCODE(KP_AT);
	PUSHSCANCODE(KP_COLON);
	PUSHSCANCODE(MEDIA_STOP);
	baseNameToScanCodeMap["Keyboard.MEDIA_NEXTTRACK"] = SDL_Scancode::SDL_SCANCODE_MEDIA_NEXT_TRACK;
	PUSHSCANCODE(KP_ENTER);
	PUSHSCANCODE(RCTRL);
	PUSHSCANCODE(MUTE);
	baseNameToScanCodeMap["Keyboard.MEDIA_PLAYPAUSE"] = SDL_Scancode::SDL_SCANCODE_MEDIA_PLAY_PAUSE;
	PUSHSCANCODE(MEDIA_STOP);
	PUSHSCANCODE(VOLUMEDOWN);
	PUSHSCANCODE(VOLUMEUP);
	baseNameToScanCodeMap["Keyboard.WEBHOME"] = SDL_Scancode::SDL_SCANCODE_AC_HOME;//TODO待验证
	PUSHSCANCODE(KP_COMMA);
	PUSHSCANCODE(KP_DIVIDE);
	PUSHSCANCODE(SYSREQ);
	PUSHSCANCODE(RALT);
	PUSHSCANCODE(PAUSE);
	PUSHSCANCODE(HOME);
	PUSHSCANCODE(UP);
	PUSHSCANCODE(PAGEUP);
	PUSHSCANCODE(LEFT);
	PUSHSCANCODE(RIGHT);
	PUSHSCANCODE(END);
	PUSHSCANCODE(DOWN);
	PUSHSCANCODE(PAGEDOWN);
	PUSHSCANCODE(INSERT);
	PUSHSCANCODE(DELETE);
	baseNameToScanCodeMap["Keyboard.LWIN"] = SDL_Scancode::SDL_SCANCODE_LGUI;
	baseNameToScanCodeMap["Keyboard.RWIN"] = SDL_Scancode::SDL_SCANCODE_RGUI;
	PUSHSCANCODE(POWER);
	PUSHSCANCODE(SLEEP);
	PUSHSCANCODE(WAKE);
	baseNameToScanCodeMap["Keyboard.WEBSEARCH"] = SDL_Scancode::SDL_SCANCODE_AC_SEARCH;//TODO待验证
	baseNameToScanCodeMap["Keyboard.BOOKMARKS"] = SDL_Scancode::SDL_SCANCODE_AC_BOOKMARKS;//TODO待验证
	baseNameToScanCodeMap["Keyboard.WEBREFRESH"] = SDL_Scancode::SDL_SCANCODE_AC_REFRESH;//TODO待验证
	baseNameToScanCodeMap["Keyboard.WEBSTOP"] = SDL_Scancode::SDL_SCANCODE_AC_STOP;//TODO待验证
	baseNameToScanCodeMap["Keyboard.WEBFORWARD"] = SDL_Scancode::SDL_SCANCODE_AC_FORWARD;//TODO待验证
	baseNameToScanCodeMap["Keyboard.WEBBACK"] = SDL_Scancode::SDL_SCANCODE_AC_BACK;//TODO待验证
	PUSHSCANCODE(MEDIA_SELECT);




	//鼠标不在此列



}
