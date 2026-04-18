
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

const char* InputParser::BongoCatKeyToButtonBaseName(uint32_t bongoCatKeyIndex, bool isGamepad)
{
	if (isGamepad)
	{
		SDL_assert(false);
	}
	else
		return  GetIns().bongoCatKeyNameTable[bongoCatKeyIndex];
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








	//Bongo Cat Key Name
	{
		bongoCatKeyNameTable[0x01] =("Mouse.Left");
		bongoCatKeyNameTable[0x02] =("Mouse.Right");
		bongoCatKeyNameTable[0x04] =("Mouse.Middle");
		bongoCatKeyNameTable[0x05] =("Mouse.X1");
		bongoCatKeyNameTable[0x06] =("Mouse.X2");

		bongoCatKeyNameTable[0x08] =("Keyboard.BACKSPACE");
		bongoCatKeyNameTable[0x09] =("Keyboard.TAB");

		//bongoCatKeyNameTable[0x0C] = VK_CLEAR;
		bongoCatKeyNameTable[0x0D] =("Keyboard.RETURN");

		//默认左shift
		bongoCatKeyNameTable[0x10] =("Keyboard.LSHIFT");
		bongoCatKeyNameTable[0x11] =("Keyboard.LCTRL");
		bongoCatKeyNameTable[0x12] =("Keyboard.LALT");
		bongoCatKeyNameTable[0x13] =("Keyboard.MEDIA_PLAYPAUSE");
		bongoCatKeyNameTable[0x14] =("Keyboard.CAPSLOCK");
		bongoCatKeyNameTable[0x15] =("Keyboard.KANA");
		//bongoCatKeyNameTable[0x16] = VK_IME_ON
		//bongoCatKeyNameTable[0x15] = VK_JUNJA;
		//bongoCatKeyNameTable[0x18] = VK_FINAL;
		//bongoCatKeyNameTable[0x19] = VK_KANJI;
		//bongoCatKeyNameTable[0x1A] = VK_IME_OFF;

		bongoCatKeyNameTable[0x1B] =("Keyboard.ESC");

		bongoCatKeyNameTable[0x20] =("Keyboard.SPACE");
		bongoCatKeyNameTable[0x21] =("Keyboard.PAGEUP");
		bongoCatKeyNameTable[0x22] =("Keyboard.PAGEDOWN");
		bongoCatKeyNameTable[0x23] =("Keyboard.END");
		bongoCatKeyNameTable[0x24] =("Keyboard.HOME");
		bongoCatKeyNameTable[0x25] =("Keyboard.LEFT");
		bongoCatKeyNameTable[0x26] =("Keyboard.UP");
		bongoCatKeyNameTable[0x27] =("Keyboard.RIGHT");
		bongoCatKeyNameTable[0x28] =("Keyboard.DOWN");

		bongoCatKeyNameTable[0x2C] =("Keyboard.SYSREQ");
		bongoCatKeyNameTable[0x2D] =("Keyboard.INSERT");
		bongoCatKeyNameTable[0x2E] =("Keyboard.DELETE");
		bongoCatKeyNameTable[0x2F] =("Keyboard.INSERT");

		bongoCatKeyNameTable[0x30] =("Keyboard.0");
		bongoCatKeyNameTable[0x31] =("Keyboard.1");
		bongoCatKeyNameTable[0x32] =("Keyboard.2");
		bongoCatKeyNameTable[0x33] =("Keyboard.3");
		bongoCatKeyNameTable[0x34] =("Keyboard.4");
		bongoCatKeyNameTable[0x35] =("Keyboard.5");
		bongoCatKeyNameTable[0x36] =("Keyboard.6");
		bongoCatKeyNameTable[0x37] =("Keyboard.7");
		bongoCatKeyNameTable[0x38] =("Keyboard.8");
		bongoCatKeyNameTable[0x39] =("Keyboard.9");

		bongoCatKeyNameTable[0x41] =("Keyboard.A");
		bongoCatKeyNameTable[0x42] =("Keyboard.B");
		bongoCatKeyNameTable[0x43] =("Keyboard.C");
		bongoCatKeyNameTable[0x44] =("Keyboard.D");
		bongoCatKeyNameTable[0x45] =("Keyboard.E");
		bongoCatKeyNameTable[0x46] =("Keyboard.F");
		bongoCatKeyNameTable[0x47] =("Keyboard.G");
		bongoCatKeyNameTable[0x48] =("Keyboard.H");
		bongoCatKeyNameTable[0x49] =("Keyboard.I");
		bongoCatKeyNameTable[0x4A] =("Keyboard.J");
		bongoCatKeyNameTable[0x4B] =("Keyboard.K");
		bongoCatKeyNameTable[0x4C] =("Keyboard.L");
		bongoCatKeyNameTable[0x4D] =("Keyboard.M");
		bongoCatKeyNameTable[0x4E] =("Keyboard.N");
		bongoCatKeyNameTable[0x4F] =("Keyboard.O");
		bongoCatKeyNameTable[0x50] =("Keyboard.P");
		bongoCatKeyNameTable[0x51] =("Keyboard.Q");
		bongoCatKeyNameTable[0x52] =("Keyboard.R");
		bongoCatKeyNameTable[0x53] =("Keyboard.S");
		bongoCatKeyNameTable[0x54] =("Keyboard.T");
		bongoCatKeyNameTable[0x55] =("Keyboard.U");
		bongoCatKeyNameTable[0x56] =("Keyboard.V");
		bongoCatKeyNameTable[0x57] =("Keyboard.W");
		bongoCatKeyNameTable[0x58] =("Keyboard.X");
		bongoCatKeyNameTable[0x59] =("Keyboard.Y");
		bongoCatKeyNameTable[0x5A] =("Keyboard.Z");

		bongoCatKeyNameTable[0x5B] =("Keyboard.LWIN");
		bongoCatKeyNameTable[0x5C] =("Keyboard.RWIN");
		bongoCatKeyNameTable[0x5D] =("Keyboard.RWIN");
		bongoCatKeyNameTable[0x5F] =("Keyboard.SLEEP");
		bongoCatKeyNameTable[0x60] =("Keyboard.KP_0");
		bongoCatKeyNameTable[0x61] =("Keyboard.KP_1");
		bongoCatKeyNameTable[0x62] =("Keyboard.KP_2");
		bongoCatKeyNameTable[0x63] =("Keyboard.KP_3");
		bongoCatKeyNameTable[0x64] =("Keyboard.KP_4");
		bongoCatKeyNameTable[0x65] =("Keyboard.KP_5");
		bongoCatKeyNameTable[0x66] =("Keyboard.KP_6");
		bongoCatKeyNameTable[0x67] =("Keyboard.KP_7");
		bongoCatKeyNameTable[0x68] =("Keyboard.KP_8");
		bongoCatKeyNameTable[0x69] =("Keyboard.KP_9");
		bongoCatKeyNameTable[0x6A] =("Keyboard.KP_MULTIPLY");
		bongoCatKeyNameTable[0x6B] =("Keyboard.KP_PLUS");
		//bongoCatKeyNameTable[0x6C] = SEPARATOR;
		bongoCatKeyNameTable[0x6D] =("Keyboard.KP_MINUS");
		bongoCatKeyNameTable[0x6E] =("Keyboard.KP_PERIOD");
		bongoCatKeyNameTable[0x6F] =("Keyboard.KP_DIVIDE");
		bongoCatKeyNameTable[0X70] =("Keyboard.F1");
		bongoCatKeyNameTable[0X71] =("Keyboard.F2");
		bongoCatKeyNameTable[0X72] =("Keyboard.F3");
		bongoCatKeyNameTable[0X73] =("Keyboard.F4");
		bongoCatKeyNameTable[0X74] =("Keyboard.F5");
		bongoCatKeyNameTable[0X75] =("Keyboard.F6");
		bongoCatKeyNameTable[0X76] =("Keyboard.F7");
		bongoCatKeyNameTable[0X77] =("Keyboard.F8");
		bongoCatKeyNameTable[0X78] =("Keyboard.F9");
		bongoCatKeyNameTable[0X79] =("Keyboard.F10");
		bongoCatKeyNameTable[0X7A] =("Keyboard.F11");
		bongoCatKeyNameTable[0X7B] =("Keyboard.F12");
		bongoCatKeyNameTable[0X7C] =("Keyboard.F13");
		bongoCatKeyNameTable[0X7D] =("Keyboard.F14");
		bongoCatKeyNameTable[0X7E] =("Keyboard.F15");
		//bongoCatKeyNameTable[0X7F] =("Keyboard.F16");
		//bongoCatKeyNameTable[0X80] =("Keyboard.F17");
		//bongoCatKeyNameTable[0X81] =("Keyboard.F18");
		//bongoCatKeyNameTable[0X82] =("Keyboard.F19");
		//...

		bongoCatKeyNameTable[0X90] =("Keyboard.NUMLOCK");
		bongoCatKeyNameTable[0X91] =("Keyboard.SCROLLLOCK");

		bongoCatKeyNameTable[0XA0] =("Keyboard.LSHIFT");
		bongoCatKeyNameTable[0XA1] =("Keyboard.RSHIFT");
		bongoCatKeyNameTable[0XA2] =("Keyboard.LCTRL");
		bongoCatKeyNameTable[0XA3] =("Keyboard.RCTRL");
		bongoCatKeyNameTable[0XA4] =("Keyboard.LALT");
		bongoCatKeyNameTable[0XA5] =("Keyboard.RALT");
		bongoCatKeyNameTable[0XA6] =("Keyboard.WEBBACK");
		bongoCatKeyNameTable[0XA7] =("Keyboard.WEBFORWARD");
		bongoCatKeyNameTable[0XA8] =("Keyboard.WEBREFRESH");
		bongoCatKeyNameTable[0XA9] =("Keyboard.WEBSTOP");
		bongoCatKeyNameTable[0XAA] =("Keyboard.WEBSEARCH");
		bongoCatKeyNameTable[0XAB] =("Keyboard.BOOKMARKS");
		bongoCatKeyNameTable[0XAC] =("Keyboard.WEBHOME");
		bongoCatKeyNameTable[0XAD] =("Keyboard.MUTE");
		bongoCatKeyNameTable[0XAE] =("Keyboard.VOLUMEDOWN");
		bongoCatKeyNameTable[0XAF] =("Keyboard.VOLUMEUP");
		bongoCatKeyNameTable[0XB0] =("Keyboard.MEDIA_NEXTTRACK");
		bongoCatKeyNameTable[0XB1] =("Keyboard.MEDIA_PREVTRACK");
		bongoCatKeyNameTable[0XB2] =("Keyboard.MEDIA_STOP");
		bongoCatKeyNameTable[0XB3] =("Keyboard.MEDIA_PLAYPAUSE");

		bongoCatKeyNameTable[0XB5] =("Keyboard.MEDIA_SELECT");

		bongoCatKeyNameTable[0XBA] =("Keyboard.SEMICOLON");
		bongoCatKeyNameTable[0XBB] =("Keyboard.EQUALS");
		bongoCatKeyNameTable[0XBC] =("Keyboard.COMMA");
		bongoCatKeyNameTable[0XBD] =("Keyboard.MINUS");
		bongoCatKeyNameTable[0XBE] =("Keyboard.PERIOD");
		bongoCatKeyNameTable[0XBF] =("Keyboard.SLASH");
		bongoCatKeyNameTable[0XC0] =("Keyboard.GRAVE");



		bongoCatKeyNameTable[0XC3] =("Gamepad.A");
		bongoCatKeyNameTable[0XC4] =("Gamepad.B");
		bongoCatKeyNameTable[0XC5] =("Gamepad.X");
		bongoCatKeyNameTable[0XC6] =("Gamepad.Y");
		bongoCatKeyNameTable[0XC7] =("Gamepad.RB");
		bongoCatKeyNameTable[0XC8] =("Gamepad.LB");
		bongoCatKeyNameTable[0XC9] =("Gamepad.LT");
		bongoCatKeyNameTable[0XCA] =("Gamepad.RT");
		bongoCatKeyNameTable[0XCB] =("Gamepad.DPAD_UP");
		bongoCatKeyNameTable[0XCC] =("Gamepad.DPAD_DOWN");
		bongoCatKeyNameTable[0XCD] =("Gamepad.DPAD_LEFT");
		bongoCatKeyNameTable[0XCE] =("Gamepad.DPAD_RIGHT");
		bongoCatKeyNameTable[0XCF] =("Gamepad.START");
		bongoCatKeyNameTable[0XD0] =("Gamepad.BACK");
		bongoCatKeyNameTable[0XD1] =("Gamepad.LS");
		bongoCatKeyNameTable[0XD2] =("Gamepad.RS");
		//左遥感向上，  不太方便支持...
		//bongoCatKeyNameTable[0XD3] = VK_GAMEPAD_LEFT_THUMBSTICK_UP;

		//bongoCatKeyNameTable[0XCA] =("Gamepad.RT");
		//todo :开发手柄功能时完善游戏板相关




		bongoCatKeyNameTable[0XDB] =("Keyboard.LBRACKET");
		bongoCatKeyNameTable[0XDC] =("Keyboard.BACKSLASH");
		bongoCatKeyNameTable[0XDD] =("Keyboard.RBRACKET");
		bongoCatKeyNameTable[0XDE] =("Keyboard.APOSTROPHE");
		bongoCatKeyNameTable[0XDF] =("Keyboard.RCTRL");

		bongoCatKeyNameTable[0XE2] =("Keyboard.BACKSLASH");


		bongoCatKeyNameTable[0xFA] =("Keyboard.MEDIA_PLAYPAUSE");
	
	
	}



}
