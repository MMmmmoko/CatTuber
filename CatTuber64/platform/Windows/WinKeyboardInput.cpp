#include<SDL3/SDL.h>
#ifdef SDL_PLATFORM_WINDOWS



#include<dinput.h>

#include"Input/InputCommon.h"
#include"Input/KeyboardInput.h"
#include"Input/InputManager.h"




#define DINPUT_BUFFERSIZE 128


//主DInput COM对象
static LPDIRECTINPUT8 lpdi = NULL;
static LPDIRECTINPUTDEVICE8 lpdikey = NULL;//键盘
static bool inited = false;

//Dinput索引，并不是都是有效位
static ButtonProxy buttnList[256] = {NULL};


bool KeyboardInput::Init()
{

	if (inited)return true;

	//填充索引
	{
		auto& im=InputManager::GetIns();
		buttnList[DIK_ESCAPE]= im.GetButton("Keyboard.ESC");
		buttnList[DIK_1]= im.GetButton("Keyboard.1");
		buttnList[DIK_2]= im.GetButton("Keyboard.2");
		buttnList[DIK_3]= im.GetButton("Keyboard.3");
		buttnList[DIK_4]= im.GetButton("Keyboard.4");
		buttnList[DIK_5]= im.GetButton("Keyboard.5");
		buttnList[DIK_6]= im.GetButton("Keyboard.6");
		buttnList[DIK_7]= im.GetButton("Keyboard.7");
		buttnList[DIK_8]= im.GetButton("Keyboard.8");
		buttnList[DIK_9]= im.GetButton("Keyboard.9");
		buttnList[DIK_0]= im.GetButton("Keyboard.0");

		buttnList[DIK_MINUS]= im.GetButton("Keyboard.MINUS");
		buttnList[DIK_EQUALS]= im.GetButton("Keyboard.EQUALS");
		buttnList[DIK_BACKSPACE]= im.GetButton("Keyboard.BACKSPACE");
		buttnList[DIK_TAB]= im.GetButton("Keyboard.TAB");

		buttnList[DIK_Q]= im.GetButton("Keyboard.Q");
		buttnList[DIK_W]= im.GetButton("Keyboard.W");
		buttnList[DIK_E]= im.GetButton("Keyboard.E");
		buttnList[DIK_R]= im.GetButton("Keyboard.R");
		buttnList[DIK_T]= im.GetButton("Keyboard.T");
		buttnList[DIK_Y]= im.GetButton("Keyboard.Y");
		buttnList[DIK_U]= im.GetButton("Keyboard.U");
		buttnList[DIK_I]= im.GetButton("Keyboard.I");
		buttnList[DIK_O]= im.GetButton("Keyboard.O");
		buttnList[DIK_P]= im.GetButton("Keyboard.P");
		//SDL_SCANCODE_LEFTBRACKET SDL_SCANCODE_RIGHTBRACKET
		buttnList[DIK_LBRACKET]= im.GetButton("Keyboard.LBRACKET");
		buttnList[DIK_RBRACKET]= im.GetButton("Keyboard.RBRACKET");
		buttnList[DIK_RETURN]= im.GetButton("Keyboard.RETURN");//主键盘的enter键
		buttnList[DIK_LCONTROL]= im.GetButton("Keyboard.LCTRL");

		buttnList[DIK_A] = im.GetButton("Keyboard.A");
		buttnList[DIK_S] = im.GetButton("Keyboard.S");
		buttnList[DIK_D] = im.GetButton("Keyboard.D");
		buttnList[DIK_F] = im.GetButton("Keyboard.F");
		buttnList[DIK_G] = im.GetButton("Keyboard.G");
		buttnList[DIK_H] = im.GetButton("Keyboard.H");
		buttnList[DIK_J] = im.GetButton("Keyboard.J");
		buttnList[DIK_K] = im.GetButton("Keyboard.K");
		buttnList[DIK_L] = im.GetButton("Keyboard.L");

		buttnList[DIK_SEMICOLON] = im.GetButton("Keyboard.SEMICOLON");//分号键
		buttnList[DIK_APOSTROPHE] = im.GetButton("Keyboard.APOSTROPHE");//撇号（引号
		buttnList[DIK_GRAVE] = im.GetButton("Keyboard.GRAVE");
		buttnList[DIK_LSHIFT] = im.GetButton("Keyboard.LSHIFT");
		buttnList[DIK_BACKSLASH] = im.GetButton("Keyboard.BACKSLASH");//美式键盘回车上方的斜线和垂直线
	
		buttnList[DIK_Z]= im.GetButton("Keyboard.Z");
		buttnList[DIK_X]= im.GetButton("Keyboard.X");
		buttnList[DIK_C]= im.GetButton("Keyboard.C");
		buttnList[DIK_V]= im.GetButton("Keyboard.V");
		buttnList[DIK_B]= im.GetButton("Keyboard.B");
		buttnList[DIK_N]= im.GetButton("Keyboard.N");
		buttnList[DIK_M]= im.GetButton("Keyboard.M");

		buttnList[DIK_COMMA]= im.GetButton("Keyboard.COMMA");
		buttnList[DIK_PERIOD]= im.GetButton("Keyboard.PERIOD");//主键盘的"."
		buttnList[DIK_SLASH] = im.GetButton("Keyboard.SLASH");// 主键盘的"/"
		buttnList[DIK_RSHIFT] = im.GetButton("Keyboard.RSHIFT");
		buttnList[DIK_MULTIPLY] = im.GetButton("Keyboard.KP_MULTIPLY");
		buttnList[DIK_LMENU] = im.GetButton("Keyboard.LALT");//left alt
		buttnList[DIK_SPACE] = im.GetButton("Keyboard.SPACE");
		buttnList[DIK_CAPITAL] = im.GetButton("Keyboard.CAPSLOCK");//大写锁定键

		buttnList[DIK_F1] = im.GetButton("Keyboard.F1");
		buttnList[DIK_F2] = im.GetButton("Keyboard.F2");
		buttnList[DIK_F3] = im.GetButton("Keyboard.F3");
		buttnList[DIK_F4] = im.GetButton("Keyboard.F4");
		buttnList[DIK_F5] = im.GetButton("Keyboard.F5");
		buttnList[DIK_F6] = im.GetButton("Keyboard.F6");
		buttnList[DIK_F7] = im.GetButton("Keyboard.F7");
		buttnList[DIK_F8] = im.GetButton("Keyboard.F8");
		buttnList[DIK_F9] = im.GetButton("Keyboard.F9");
		buttnList[DIK_F10] = im.GetButton("Keyboard.F10");

		buttnList[DIK_NUMLOCK] = im.GetButton("Keyboard.NUMLOCK");
		buttnList[DIK_SCROLL] = im.GetButton("Keyboard.SCROLLLOCK");
		buttnList[DIK_NUMPAD7] = im.GetButton("Keyboard.KP_7");
		buttnList[DIK_NUMPAD8] = im.GetButton("Keyboard.KP_8");
		buttnList[DIK_NUMPAD9] = im.GetButton("Keyboard.KP_9");
		buttnList[DIK_SUBTRACT] = im.GetButton("Keyboard.KP_MINUS");
		buttnList[DIK_NUMPAD4] = im.GetButton("Keyboard.KP_4");
		buttnList[DIK_NUMPAD5] = im.GetButton("Keyboard.KP_5");
		buttnList[DIK_NUMPAD6] = im.GetButton("Keyboard.KP_6");
		buttnList[DIK_ADD] = im.GetButton("Keyboard.KP_PLUS");
		buttnList[DIK_NUMPAD1] = im.GetButton("Keyboard.KP_1");
		buttnList[DIK_NUMPAD2] = im.GetButton("Keyboard.KP_2");
		buttnList[DIK_NUMPAD3] = im.GetButton("Keyboard.KP_3");
		buttnList[DIK_NUMPAD0] = im.GetButton("Keyboard.KP_0");
		buttnList[DIK_DECIMAL] = im.GetButton("Keyboard.KP_PERIOD");
		buttnList[DIK_OEM_102] = im.GetButton("Keyboard.NONUSBACKSLASH");

		buttnList[DIK_F11] = im.GetButton("Keyboard.F11");
		buttnList[DIK_F12] = im.GetButton("Keyboard.F12");
		buttnList[DIK_F13] = im.GetButton("Keyboard.F13");
		buttnList[DIK_F14] = im.GetButton("Keyboard.F14");
		buttnList[DIK_F15] = im.GetButton("Keyboard.F15");
		buttnList[DIK_KANA] = im.GetButton("Keyboard.KANA");
		//buttnList[DIK_ABNT_C1] = im.GetButton("Keyboard.KANA");巴西键盘什么玩意
		//buttnList[DIK_CONVERT] = im.GetButton("Keyboard.KANA");
		//DIK_NOCONVERT
		buttnList[DIK_YEN] = im.GetButton("Keyboard.YEN");//SDL_SCANCODE_INTERNATIONAL3
		//DIK_ABNT_C2
		buttnList[DIK_NUMPADEQUALS]= im.GetButton("Keyboard.KP_EQUALS");
		buttnList[DIK_PREVTRACK]=im.GetButton("Keyboard.MEDIA_PREVTRACK");
		buttnList[DIK_AT]= im.GetButton("Keyboard.KP_AT");//不确定和SDL_SCANCODE_KP_AT的关系..
		buttnList[DIK_COLON] = im.GetButton("Keyboard.KP_COLON");
		//buttnList[DIK_KANJI]
		buttnList[DIK_STOP] = im.GetButton("Keyboard.MEDIA_STOP");
		//buttnList[DIK_AX] 
		//buttnList[DIK_UNLABELED] 
		buttnList[DIK_NEXTTRACK] = im.GetButton("Keyboard.MEDIA_NEXTTRACK");
		buttnList[DIK_NUMPADENTER] = im.GetButton("Keyboard.KP_ENTER");
		buttnList[DIK_RCONTROL] = im.GetButton("Keyboard.RCTRL");
		buttnList[DIK_MUTE] = im.GetButton("Keyboard.MUTE");//SDL_SCANCODE_MUTE
		//buttnList[DIK_CALCULATOR]
		buttnList[DIK_PLAYPAUSE]=im.GetButton("Keyboard.MEDIA_PLAYPAUSE");//SDL_SCANCODE_MEDIA_PLAY_PAUSE
		buttnList[DIK_MEDIASTOP]=im.GetButton("Keyboard.MEDIA_STOP");
		buttnList[DIK_VOLUMEDOWN]=im.GetButton("Keyboard.VOLUMEDOWN");
		buttnList[DIK_VOLUMEUP]=im.GetButton("Keyboard.VOLUMEUP");
		buttnList[DIK_WEBHOME]= im.GetButton("Keyboard.WEBHOME");//不确定SDL_SCANCODE_AC_HOME
		buttnList[DIK_NUMPADCOMMA]= im.GetButton("Keyboard.KP_COMMA");
		buttnList[DIK_DIVIDE]= im.GetButton("Keyboard.KP_DIVIDE");
		buttnList[DIK_SYSRQ]= im.GetButton("Keyboard.SYSREQ");//prinrt screen一个键？
		buttnList[DIK_RMENU] = im.GetButton("Keyboard.RALT");
		buttnList[DIK_PAUSE] = im.GetButton("Keyboard.PAUSE");
		buttnList[DIK_HOME] = im.GetButton("Keyboard.HOME");
		buttnList[DIK_UP] = im.GetButton("Keyboard.UP");//SDL_SCANCODE_UP
		buttnList[DIK_PRIOR] = im.GetButton("Keyboard.PAGEUP");//SDL_SCANCODE_PAGEUP
		buttnList[DIK_LEFT] = im.GetButton("Keyboard.LEFT");
		buttnList[DIK_RIGHT] = im.GetButton("Keyboard.RIGHT");
		buttnList[DIK_END] = im.GetButton("Keyboard.END");// SDL_SCANCODE_END
		buttnList[DIK_DOWN] = im.GetButton("Keyboard.DOWN");
		buttnList[DIK_NEXT] = im.GetButton("Keyboard.PAGEDOWN");
		buttnList[DIK_INSERT] = im.GetButton("Keyboard.INSERT");//PC上的insert，mac上的help键。
		buttnList[DIK_DELETE] = im.GetButton("Keyboard.DELETE");
		buttnList[DIK_LWIN] = im.GetButton("Keyboard.LWIN");//SDL_SCANCODE_LGUI MAC上是COMMAND
		buttnList[DIK_RWIN] = im.GetButton("Keyboard.RWIN");
		//buttnList[DIK_APPS] = im.GetButton("Keyboard.RWIN");
		buttnList[DIK_POWER] = im.GetButton("Keyboard.POWER");
		buttnList[DIK_SLEEP] = im.GetButton("Keyboard.SLEEP");
		buttnList[DIK_WAKE] = im.GetButton("Keyboard.WAKE");
		buttnList[DIK_WEBSEARCH]= im.GetButton("Keyboard.WEBSEARCH");//不确定SDL_SCANCODE_AC_SEARCH
		buttnList[DIK_WEBFAVORITES] = im.GetButton("Keyboard.BOOKMARKS");//不确定SDL_SCANCODE_AC_BOOKMARKS
		buttnList[DIK_WEBREFRESH] = im.GetButton("Keyboard.WEBREFRESH");
		buttnList[DIK_WEBSTOP] = im.GetButton("Keyboard.WEBSTOP");//SDL_SCANCODE_AC_STOP
		buttnList[DIK_WEBFORWARD] = im.GetButton("Keyboard.WEBFORWARD");//SDL_SCANCODE_AC_FORWARD
		buttnList[DIK_WEBBACK] = im.GetButton("Keyboard.WEBBACK");//SDL_SCANCODE_AC_FORWARD
		//buttnList[DIK_MYCOMPUTER] ;
		//buttnList[DIK_MAIL];
		buttnList[DIK_MEDIASELECT]= im.GetButton("Keyboard.MEDIA_SELECT");//SDL_SCANCODE_MEDIA_SELECT

		//很多语言相关的按钮没法找到
		//todo 买个日本键盘（躺
	}




	//初始化DINPUT
	do
	{
		//创建键盘设备
		if (FAILED(::DirectInput8Create(GetModuleHandle(0), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&lpdi, NULL)))
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "KeyboardInput: Init Failed.");
			break;
		}
		if (FAILED(lpdi->CreateDevice(GUID_SysKeyboard, &lpdikey, NULL)))
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "KeyboardInput: Init Failed.");
			break;
		}
		//设置键盘协作等级，设置为后台/非独占模式
		if (FAILED(lpdikey->SetCooperativeLevel(0, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "KeyboardInput: Init Failed.");
			break;
		}



		//设置缓冲区大小
		DIPROPDWORD     property;
		property.diph.dwSize = sizeof(DIPROPDWORD);
		property.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		property.diph.dwObj = 0;
		property.diph.dwHow = DIPH_DEVICE;
		property.dwData = DINPUT_BUFFERSIZE;
		if (FAILED(lpdikey->SetProperty(DIPROP_BUFFERSIZE, &property.diph)))
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "KeyboardInput: Init Failed.");
			break;
		}




		//设置键盘的数据格式，键盘对应的数据格式为c_dfDIKeyboard
		if (FAILED(lpdikey->SetDataFormat(&c_dfDIKeyboard)))
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "KeyboardInput: Init Failed.");
			break;
		}
		//获取键盘
		if (FAILED(lpdikey->Acquire()))
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "KeyboardInput: Init Failed.");
			break;
		}
		inited = true;
		return true;
	} while (false);


	Release();
	return false;

}



void KeyboardInput::UpdateAndPumpEvents()
{
	if (!lpdikey)return;



	DIDEVICEOBJECTDATA  didod[DINPUT_BUFFERSIZE];  // Receives buffered data
	DWORD               dwElements;
	HRESULT             hr;

	hr = DIERR_INPUTLOST;

	while (hr != DI_OK)
	{
		dwElements = DINPUT_BUFFERSIZE;
		hr = lpdikey->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), didod, &dwElements, 0);
		if (hr != DI_OK)
		{
			// hr != DI_OK表示发生了一个错误
			// 这个错误有可能是 DI_BUFFEROVERFLOW 缓冲区溢出错误
			// 但不管是哪种错误，都意味着同输入设备的联系被丢失了

			// 这种错误引起的最严重的后果就是如果你按下一个键后还未松开时
			// 发生了错误，就会丢失后面松开该键的消息。这样一来，你的程序
			// 就可能以为该键尚未被松开，从而发生一些意想不到的情况

			// 现在这段代码并未处理该错误

			// 解决该问题的一个办法是，在出现这种错误时，就去调用一次
			// GetDeviceState()，然后把结果同程序最后所记录的状态进行
			// 比较，从而修正可能发生的错误

			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "KeyboardInput ErrorCode: HRESULT %d",hr);
			hr = lpdikey->Acquire();
			if (FAILED(hr))
				break;
		}
	}

	if (FAILED(hr))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "KeyboardInput ErrorCode: HRESULT %d", hr);
		ResetData();
		return;
	}


	for (DWORD i = 0; i < dwElements; i++)
	{
		// 此处放入处理代码
		// didod[i].dwOfs 表示那个键被按下或松开
		// didod[i].dwData 记录此键的状态，低字节最高位是 1 表示按下，0 表示松开
		// 一般用 didod[i].dwData&0x80 来测试

		auto curKey = didod[i].dwOfs;
		auto curData = didod[i].dwData;

		//keyboardStates[curKey] = curData;
		if (curData & 0x80)
		{
			//按下
			if(buttnList[curKey])
				buttnList[curKey]->ButtonDown();
		}
		else
		{
			if (buttnList[curKey])
				buttnList[curKey]->ButtonUp();
		}


	}


}

void KeyboardInput::Release()
{
	if (lpdikey)
	{
		//获取后，归还
		lpdikey->Unacquire();
		//释放
		lpdikey->Release();
		lpdikey = NULL;
	}
	if (lpdi)
	{
		lpdi->Release();
		lpdi = NULL;
	}
	inited = false;
}

void KeyboardInput::ResetData()
{
	for (int i = 0; i < 256; i++)
	{
		if (buttnList[i])
		{
			while (buttnList[i]->IsDown())
				buttnList[i]->ButtonUp();
		}
	}
}




































#endif