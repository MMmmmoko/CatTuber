#include<SDL3/SDL.h>
#ifdef SDL_PLATFORM_WINDOWS



#include<dinput.h>

#include"Input/InputCommon.h"
#include"Input/MouseInput.h"
#include"Input/InputManager.h"





//静态变量


		//获取的鼠标数据放在此结构中
static DIMOUSESTATE2 mouse_state_d7;
static LPDIRECTINPUT8 lpdi = NULL;
static LPDIRECTINPUTDEVICE8 lpdimouse = NULL;
static bool inited = false;


//目前打算只支持五个鼠标按键

//static input::InputAxis* mousePosY = NULL;//与软件相关
//static input::InputAxis* mousePosY = NULL;


#define DINPUT_BUFFERSIZE 128

//8是DInput提供的数量， CatTuber暂时只开放5个按键给用户
BYTE MouseButtonStates[8] = { 0 };
BYTE MouseButtonStates_LastFrame[8] = { 0 };//<物理机器上的鼠标按键 和网络无关鼠标按键
bool MouseWheel_LastFrame[2] = {0};

bool MouseInput::Init()
{
	
	if (inited)return true;

	//获取输入
	auto& im=InputManager::GetIns();
	//0左1右2中34侧
	mouseButtonList[0]=im.GetButton("Mouse.Left");
	mouseButtonList[1]=im.GetButton("Mouse.Right");
	mouseButtonList[2]=im.GetButton("Mouse.Middle");
	mouseButtonList[3]=im.GetButton("Mouse.X1");
	mouseButtonList[4]=im.GetButton("Mouse.X2");
	mouseWheelUp = im.GetButton("Mouse.WheelUp");
	mouseWheelDown = im.GetButton("Mouse.WheelDown");
	mouseCoordinateX = im.GetAxis("Mouse.Pos.CoordinateX");
	mouseCoordinateY = im.GetAxis("Mouse.Pos.CoordinateY");
	mouseMoveX = im.GetAxis("Mouse.Pos.MoveX");
	mouseMoveY = im.GetAxis("Mouse.Pos.MoveY");
	//mousePosX = im.GetAxis("Mouse.Pos");
	

	//获取主显示器分辨率,提供移动参考
	do
	{
		SDL_DisplayID primaryDisplay = SDL_GetPrimaryDisplay();
		if (primaryDisplay == 0)break;
		SDL_Rect rect;

		if (!SDL_GetDisplayBounds(primaryDisplay, &rect))break;

		primaryScreenW=rect.w;
		primaryScreenH= rect.h;
	} while (false);



	//初始化directinput
	do
	{
		HRESULT hr;
		if (FAILED(::DirectInput8Create(GetModuleHandle(0), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&lpdi, NULL)))
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"MouseInput: Init Failed.");
			break;
		}

		//创建鼠标设备
		if (lpdi->CreateDevice(GUID_SysMouse, &lpdimouse, NULL) != DI_OK)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "MouseInput: Init Failed.");
			break;
		}
		//设置鼠标协作等级，设置为后台/非独占模式
		if (lpdimouse->SetCooperativeLevel(0, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND) != DI_OK)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "MouseInput: Init Failed.");
			break;
		}
		DIPROPDWORD     property;

		property.diph.dwSize = sizeof(DIPROPDWORD);
		property.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		property.diph.dwObj = 0;
		property.diph.dwHow = DIPH_DEVICE;
		property.dwData = DINPUT_BUFFERSIZE;


		//设置数据格式,鼠标对应的数据格式为c_dfDIMouse
		if (lpdimouse->SetDataFormat(&c_dfDIMouse2) != DI_OK)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "MouseInput: Init Failed.");
			break;
		}
		hr = lpdimouse->SetProperty(DIPROP_BUFFERSIZE, &property.diph);

		if FAILED(hr)
		{
			// 失败
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "MouseInput: Init Failed.");
			break;
		}


		//获取鼠标
		if (lpdimouse->Acquire() != DI_OK)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"MouseInput: Init Failed.");
			break;
		}
		inited = true;
		return true;
	} while (false);

	
	Release();
	return false;
}
void MouseInput::Release()
{
	if (lpdimouse)
	{
		lpdimouse->Unacquire();
		lpdimouse->Release();
		lpdimouse = NULL;
	}
	if (lpdi)
	{
		lpdi->Release();
		lpdi = NULL;
	}
	inited = false;
}

//使用缓存模式会因为鼠标回报率过大导致极高CPU占用（也许）
//这里采用立即模式
void MouseInput::UpdateAndPumpEvents()
{


	//读取鼠标数据
	if (lpdimouse)
	{
		if (lpdimouse->GetDeviceState(sizeof(DIMOUSESTATE2), (LPVOID)&mouse_state_d7) != DI_OK)
			return;
	}
	else
	{
		//如果没有插入鼠标，则将鼠标数据结构置为0
		memset(&mouse_state_d7, 0, sizeof(mouse_state_d7));
		return;
	}





	//CatTuber暂时只处理五个鼠标按键
	//for (int i = 0; i < 8; i++)
	for (int i = 0; i < 5; i++)
	{
		if (MouseButtonStates_LastFrame[i] == 0 && mouse_state_d7.rgbButtons[i] & 0x80)
		{
			mouseButtonList[i]->ButtonDown();
			MouseButtonStates_LastFrame[i] = mouse_state_d7.rgbButtons[i];
		}
		else if (MouseButtonStates_LastFrame[i] & 0x80 && mouse_state_d7.rgbButtons[i] == 0)
		{
			mouseButtonList[i]->ButtonUp();
			MouseButtonStates_LastFrame[i] = 0;
		}
	}









	//将mouse_state_d7中的数据转写到我们需要保存数据的位置
	//float MousePoint_Model[2];		//<鼠标位置


	//*2:[0,1]映射到[-1,1]

	bool localUpdate = false;//本地是否发生了数据更新

	float dx = mouse_state_d7.lX  * 2 * mouseSpeed * (isInvertX ? -1 : 1)/(float)primaryScreenW;
	float dy = mouse_state_d7.lY  * -2 * mouseSpeed / (float)primaryScreenH;
	if (dx != 0 || dy != 0)
	{


		localUpdate = true;


		relatePosX += dx;
		relatePosX = SDL_clamp(relatePosX, -1.f, 1.f);
		relatePosY +=dy;
		relatePosY = SDL_clamp(relatePosY,-1.f,1.f);
		mouseMoveX->SetValue(relatePosX);
		mouseMoveY->SetValue(relatePosY);

	}
	else
	{
		if (localUpdate)
		{
			localUpdate = false;
			//axis_MoveOutput[2].SetValue(0);
			//axis_MoveOutput[3].SetValue(0);
		}



	}



	//仅在鼠标有移动的情况下更新坐标
	if (localUpdate)
	{
		POINT p;
		if (GetCursorPos(&p))
		{
			p.x = p.x - mouseArea.x;
			p.y = p.y - mouseArea.y;

			float newCoordinateValueX = p.x * 2.f/ mouseArea.w - 1.f;
			float newCoordinateValueY = 1.f - p.y * 2.f/ mouseArea.h;
			if (newCoordinateValueX > 1.f)newCoordinateValueX = 1.f;
			else if (newCoordinateValueX < -1.f)newCoordinateValueX = -1.f;
			if (newCoordinateValueY > 1.f)newCoordinateValueY = 1.f;
			else if (newCoordinateValueY < -1.f)newCoordinateValueY = -1.f;

			if (isInvertX)
			{
				newCoordinateValueX *= -1.f;
			}

			//CATLOG_DEBUG_("px:" << newCoordinateValueX << ",py:" << newCoordinateValueY);

			mouseCoordinateX->SetValue(newCoordinateValueX);
			mouseCoordinateY->SetValue(newCoordinateValueY);
		}
	}



	//滚轮
	//Todo需要测试方向
	if (mouse_state_d7.lZ > 0)
	{
		if (MouseWheel_LastFrame[0] == false)
		{
			MouseWheel_LastFrame[0] = true;
			mouseWheelUp->ButtonDown();
		}
	}
	else
	{
		if (MouseWheel_LastFrame[0] == true)
		{
			MouseWheel_LastFrame[0] = false;
			mouseWheelUp->ButtonUp();
		}
	}
	if (mouse_state_d7.lZ < 0)
	{
		if (MouseWheel_LastFrame[1] == false)
		{
			MouseWheel_LastFrame[1] = true;
			mouseWheelDown->ButtonDown();
		}
	}
	else
	{
		if (MouseWheel_LastFrame[1] == true)
		{
			MouseWheel_LastFrame[1] = false;
			mouseWheelDown->ButtonUp();
		}
	}











}

































#endif
