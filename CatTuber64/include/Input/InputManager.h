#ifndef _InputManager_h
#define _InputManager_h




//输入管理器
//简化设置，直接与设备相关，所以应该是单例
//CatTuber获取与处理后台输入应该与SDL那边的窗口内的输入区分开。
//CatTuber的体量应该不需要将游戏逻辑与渲染线程区分开。


#include<unordered_map>
#include<iostream>
#include"Util/ObjectPool.h"
#include"InputCommon.h"
#include"MouseInput.h"
#include"KeyboardInput.h"










//事件相关
class InputManager
{
	friend class AppSettings;
public:
	static InputManager& GetIns() { static InputManager ref;return ref; }



	ButtonProxy GetButton(const char* baseName);
	AxisProxy GetAxis(const char* baseName);
	ActionProxy GetAction(const char* actionName);

	bool InitInput();

	//每帧处理事件（update）时调用
	//读取来自设备或网络的输入事件
	void PumpDeviceOrNetworkInputEvents();
	




	//注册由按键触发的Action// EG actionName: Table.Button.1
	//为每个键设置监听？
	//可能需要将buttonBaseName的格式换为string
	ActionBindingHandle RegisterButtonActionBinding(const char* downActionName, const char* upActionName, std::string* strArray, int buttonCount = 1);
	ActionBindingHandle RegisterAxisChangeActionBinding(const char* actionName,const char* axisBaseName);

	ActionBindingHandle RegisterAxisExceedActionBinding(const char* actionName, const char* axisBaseName,float targetValue);
	ActionBindingHandle RegisterAxisDroppedActionBinding(const char* actionName, const char* axisBaseName,float targetValue);

	ActionBindingHandle RegisterButtonToAxisActionBinding(const char* axisActionName, const char* buttonName,float axisTargetValue);
	//注册后，origAction触发时，顺带触发actionToBeTriggered
	ActionBindingHandle RegisterActionByActionBinding(const char* actionToBeTriggered, const char* origAction,float paramValue=0.f);


	void UnregisterActionBinding(ActionBindingHandle bindinghandle);//执行后Handle不再可用


	void RegisterActionCallback(const char* actionName, ActionCallback callback);
	void UnregisterActionCallback(const char* actionName, ActionCallback callback);




	//条件绑定，如某个值大于0.5的时候触发
	//void RegisterActionConditionalBinding
	//序列绑定，比如格斗游戏里搓技能
	//void RegisterActionSequenceBinding


	//触发一个Action
	//注册的action不打算从池里删除，但使用actionName触发未注册的action不会导致新增action实例
	// （因为未来可能有与弹幕相关的内容或者实现一个远程action发起功能）
	void RiseAction(const char* actionName, float value=0.f);//对于模拟事件（如轴相关的事件）用第二个参数传递数值
	void RiseAction(ActionProxy action, float value=0.f);//对于模拟事件（如轴相关的事件）用第二个参数传递数值





	//App设置项
	//XXXXXX   AllDisplays或者显示器名
	//XXXXXX    使用DisplayID
	//SDL_GetDisplays最好在主线程使用，所以直接在AppSetting里计算屏幕范围
	//然后这个函数就在输入线程调用
	void SetMouseInputArea(SDL_Rect* displayRect);

	void ShutDown();
private:
	InputManager();
	//需要根据软件设置单独进行处理
	bool isMousePosMove=true;
	AxisProxy _axis_mouse_posX;
	AxisProxy _axis_mouse_posY;
	AxisProxy _axis_mouse_posMoveX;
	AxisProxy _axis_mouse_posMoveY;
	AxisProxy _axis_mouse_posCoordX;
	AxisProxy _axis_mouse_posCoordY;

	//std::unordered_map<const char* , std::vector<IActionBinding*>> actionBinding;//<buttonBaseName, >
	std::unordered_map<std::string, ActionProxy> actionMap;//<actionName, action>


	std::unordered_map<std::string, ButtonProxy> buttonMap;
	std::unordered_map<std::string, AxisProxy> axisMap;

	//为了紧凑内存，使用池
	ObjectPool<input::InputButton> _buttonPool;
	ObjectPool<input::InputAxis> _axisPool;
	ObjectPool<Action> _actionPool;


	//输入对象
	MouseInput mouseInput;
	KeyboardInput kerboardInput;

};











#endif


