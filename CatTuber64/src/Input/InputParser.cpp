
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
		auto& keyTable = GetIns().keyTable;
		auto it = keyTable.find(keyName);
		if (it != keyTable.end())
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
	//keyTable
	{
		keyTable["ESCAPE"] = "Keyboard.ESC";
		keyTable["ESC"] = "Keyboard.ESC";

		keyTable["1"] = "Keyboard.1";
		keyTable["2"] = "Keyboard.2";
		keyTable["3"] = "Keyboard.3";
		keyTable["4"] = "Keyboard.4";
		keyTable["5"] = "Keyboard.5";
		keyTable["6"] = "Keyboard.6";
		keyTable["7"] = "Keyboard.7";
		keyTable["8"] = "Keyboard.8";
		keyTable["9"] = "Keyboard.9";
		keyTable["0"] = "Keyboard.0";

		keyTable["MINUS"] = "Keyboard.MINUS";
		keyTable["EQUALS"] = "Keyboard.EQUALS";
		keyTable["BACKSPACE"] = "Keyboard.BACKSPACE";
		keyTable["BACK"] = "Keyboard.BACKSPACE";
		keyTable["TAB"] = "Keyboard.TAB";


		keyTable["Q"] = "Keyboard.Q";
		keyTable["W"] = "Keyboard.W";
		keyTable["E"] = "Keyboard.E";
		keyTable["R"] = "Keyboard.R";
		keyTable["T"] = "Keyboard.T";
		keyTable["Y"] = "Keyboard.Y";
		keyTable["U"] = "Keyboard.U";
		keyTable["I"] = "Keyboard.I";
		keyTable["O"] = "Keyboard.O";
		keyTable["P"] = "Keyboard.P";


#define _TOSTR(X) #X
#define PUSHTABLE(X) keyTable[#X]= _TOSTR(Keyboard.##X);
		PUSHTABLE(LBRACKET);
		PUSHTABLE(RBRACKET);
		keyTable["ENTER"] = "Keyboard.RETURN";
		keyTable["RETURN"] = "Keyboard.RETURN";
		keyTable["LCONTROL"] = "Keyboard.LCTRL";
		keyTable["LCTRL"] = "Keyboard.LCTRL";

		PUSHTABLE(A);
		PUSHTABLE(S);
		PUSHTABLE(D);
		PUSHTABLE(F);
		PUSHTABLE(G);
		PUSHTABLE(H);
		PUSHTABLE(J);
		PUSHTABLE(K);
		PUSHTABLE(L);

		PUSHTABLE(SEMICOLON);
		PUSHTABLE(APOSTROPHE);
		PUSHTABLE(GRAVE);
		PUSHTABLE(LSHIFT);
		PUSHTABLE(BACKSLASH);


		PUSHTABLE(Z);
		PUSHTABLE(X);
		PUSHTABLE(C);
		PUSHTABLE(V);
		PUSHTABLE(B);
		PUSHTABLE(N);
		PUSHTABLE(M);

		PUSHTABLE(COMMA);
		PUSHTABLE(PERIOD);
		PUSHTABLE(SLASH);
		PUSHTABLE(RSHIFT);

		keyTable["MULTIPLY"] = "Keyboard.KP_MULTIPLY";
		keyTable["NUM_STAR"] = "Keyboard.KP_MULTIPLY";
		keyTable["NUM_MULTIPLY"] = "Keyboard.KP_MULTIPLY";
		keyTable["KP_MULTIPLY"] = "Keyboard.KP_MULTIPLY";

		keyTable["LMENU"] = "Keyboard.LALT";
		keyTable["LALT"] = "Keyboard.LALT";
		PUSHTABLE(SPACE);
		keyTable["CAPITAL"] = "Keyboard.CAPSLOCK";
		keyTable["CAPSLOCK"] = "Keyboard.CAPSLOCK";
		keyTable["CAPS"] = "Keyboard.CAPSLOCK";

		PUSHTABLE(F1);
		PUSHTABLE(F2);
		PUSHTABLE(F3);
		PUSHTABLE(F4);
		PUSHTABLE(F5);
		PUSHTABLE(F6);
		PUSHTABLE(F7);
		PUSHTABLE(F8);
		PUSHTABLE(F9);
		PUSHTABLE(F10);


		keyTable["NUM"] = "Keyboard.NUMLOCK";
		keyTable["NUMLOCK"] = "Keyboard.NUMLOCK";
		keyTable["SCROLL"] = "Keyboard.SCROLLLOCK";
		keyTable["SCROLLLOCK"] = "Keyboard.SCROLLLOCK";

#define PUSHTABLE_NUM(X) keyTable[_TOSTR(NUMPAD##X)]=_TOSTR(Keyboard.KP_##X);\
keyTable[_TOSTR(KP_##X)]=_TOSTR(Keyboard.KP_##X);\
keyTable[_TOSTR(NUM_##X)]=_TOSTR(Keyboard.KP_##X)
		keyTable["NUMPAD7"] = "Keyboard.KP_7";
		keyTable["KP_7"] = "Keyboard.KP_7";
		keyTable["NUM_7"] = "Keyboard.KP_7";
		PUSHTABLE_NUM(8);
		PUSHTABLE_NUM(9);
		keyTable["SUBTRACT"] = "Keyboard.KP_MINUS";
		keyTable["NUMPADMINUS"] = "Keyboard.KP_MINUS";
		keyTable["KP_MINUS"] = "Keyboard.KP_MINUS";
		PUSHTABLE_NUM(4);
		PUSHTABLE_NUM(5);
		PUSHTABLE_NUM(6);
		keyTable["ADD"] = "Keyboard.KP_PLUS";
		keyTable["NUMPADPLUS"] = "Keyboard.KP_PLUS";
		keyTable["NUM_PLUS"] = "Keyboard.KP_PLUS";
		keyTable["KP_PLUS"] = "Keyboard.KP_PLUS";
		PUSHTABLE_NUM(1);
		PUSHTABLE_NUM(2);
		PUSHTABLE_NUM(3);
		PUSHTABLE_NUM(0);
		keyTable["DECIMAL"] = "Keyboard.KP_PERIOD";
		keyTable["NUM_DECIMAL"] = "Keyboard.KP_PERIOD";
		keyTable["NUM_PERIOD"] = "Keyboard.KP_PERIOD";
		keyTable["NUMPADPERIOD"] = "Keyboard.KP_PERIOD";
		keyTable["KP_PERIOD"] = "Keyboard.KP_PERIOD";
		keyTable["OEM_102"] = "Keyboard.NONUSBACKSLASH";
		keyTable["NONUSBACKSLASH"] = "Keyboard.NONUSBACKSLASH";


		PUSHTABLE(F11);
		PUSHTABLE(F12);
		PUSHTABLE(F13);
		PUSHTABLE(F14);
		PUSHTABLE(F15);

		PUSHTABLE(KANA);
		PUSHTABLE(YEN);

		keyTable["NUMPADEQUALS"] = "Keyboard.KP_EQUALS";
		keyTable["KP_EQUALS"] = "Keyboard.KP_EQUALS";
		keyTable["NUM_EQUALS"] = "Keyboard.KP_EQUALS";

		keyTable["PREVTRACK"] = "Keyboard.MEDIA_PREVTRACK";
		keyTable["MEDIA_PREVTRACK"] = "Keyboard.MEDIA_PREVTRACK";
		keyTable["AT"] = "Keyboard.KP_AT";
		keyTable["KP_AT"] = "Keyboard.KP_AT";
		keyTable["COLON"] = "Keyboard.KP_COLON";
		keyTable["KP_COLON"] = "Keyboard.KP_COLON";
		keyTable["STOP"] = "Keyboard.MEDIA_STOP";
		keyTable["MEDIA_STOP"] = "Keyboard.MEDIA_STOP";
		keyTable["NEXTTRACK"] = "Keyboard.MEDIA_NEXTTRACK";
		keyTable["MEDIA_NEXTTRACK"] = "Keyboard.MEDIA_NEXTTRACK";

		keyTable["NUMPADENTER"] = "Keyboard.KP_ENTER";
		keyTable["NUM_ENTER"] = "Keyboard.KP_ENTER";
		keyTable["KP_ENTER"] = "Keyboard.KP_ENTER";
		keyTable["RCONTROL"] = "Keyboard.RCTRL";
		keyTable["RCTRL"] = "Keyboard.RCTRL";

		keyTable["MUTE"] = "Keyboard.MUTE";
		keyTable["PLAYPAUSE"] = "Keyboard.MEDIA_PLAYPAUSE";
		keyTable["MEDIA_PLAYPAUSE"] = "Keyboard.MEDIA_PLAYPAUSE";
		keyTable["MEDIASTOP"] = "Keyboard.MEDIA_STOP";
		keyTable["MEDIA_STOP"] = "Keyboard.MEDIA_STOP";

		PUSHTABLE(VOLUMEDOWN);
		PUSHTABLE(VOLUMEUP);
		PUSHTABLE(WEBHOME);

		keyTable["NUMPADCOMMA"] = "Keyboard.KP_COMMA";
		keyTable["NUM_COMMA"] = "Keyboard.KP_COMMA";
		keyTable["KP_COMMA"] = "Keyboard.KP_COMMA";
		keyTable["DIVIDE"] = "Keyboard.KP_DIVIDE";
		keyTable["NUMPADSLASH"] = "Keyboard.KP_DIVIDE";
		keyTable["NUM_DIVIDE"] = "Keyboard.KP_DIVIDE";
		keyTable["KP_DIVIDE"] = "Keyboard.KP_DIVIDE";
		keyTable["SYSRQ"] = "Keyboard.SYSREQ";
		keyTable["SYSREQ"] = "Keyboard.SYSREQ";
		keyTable["RMENU"] = "Keyboard.RALT";
		keyTable["RALT"] = "Keyboard.RALT";
		PUSHTABLE(PAUSE);
		PUSHTABLE(HOME);
		PUSHTABLE(UP);
		keyTable["PRIOR"] = "Keyboard.PAGEUP";
		keyTable["PAGEUP"] = "Keyboard.PAGEUP";
		keyTable["PGUP"] = "Keyboard.PAGEUP";
		PUSHTABLE(LEFT);
		PUSHTABLE(RIGHT);
		PUSHTABLE(END);
		PUSHTABLE(DOWN);
		keyTable["NEXT"] = "Keyboard.PAGEDOWN";
		keyTable["PAGEDOWN"] = "Keyboard.PAGEDOWN";
		keyTable["PGDN"] = "Keyboard.PAGEDOWN";

		PUSHTABLE(INSERT);
		PUSHTABLE(DELETE);
		PUSHTABLE(LWIN);
		PUSHTABLE(RWIN);
		keyTable["LCOMMAND"] = "Keyboard.LWIN";
		keyTable["LCMD"] = "Keyboard.LWIN";
		keyTable["RCOMMAND"] = "Keyboard.RWIN";
		keyTable["RCMD"] = "Keyboard.RWIN";
		PUSHTABLE(POWER);
		PUSHTABLE(SLEEP);
		PUSHTABLE(WAKE);
		PUSHTABLE(WEBSEARCH);
		keyTable["WEBFAVORITES"] = "Keyboard.BOOKMARKS";
		keyTable["BOOKMARKS"] = "Keyboard.BOOKMARKS";
		PUSHTABLE(WEBREFRESH);
		PUSHTABLE(WEBSTOP);
		PUSHTABLE(WEBFORWARD);
		PUSHTABLE(WEBBACK);

		keyTable["MEDIASELECT"] = "Keyboard.MEDIA_SELECT";
		keyTable["MEDIA_SELECT"] = "Keyboard.MEDIA_SELECT";



		//鼠标按键
		keyTable["MB1"] = "Mouse.Left";
		keyTable["MB2"] = "Mouse.Right";
		keyTable["MB3"] = "Mouse.Middle";
		keyTable["MB4"] = "Mouse.X1";
		keyTable["MB5"] = "Mouse.X2";







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
