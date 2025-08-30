#include "Input/InputManager.h"
#include <Util.h>



InputManager::InputManager()
{
	//383、179是个中等大小的质数,应该算是比较适合目前的情况的
	buttonMap.max_load_factor(0.7f);
	axisMap.max_load_factor(0.7f);
	buttonMap.reserve(383);
	_buttonPool.reserve(383);
	axisMap.reserve(179);
	_axisPool.reserve(179);
}


ButtonProxy InputManager::GetButton(const char* baseName)
{
	auto it = buttonMap.find(baseName);
	if (it == buttonMap.end())
	{
		buttonMap[baseName] = { _buttonPool.create(input::InputButton(baseName)) };
	}
	return buttonMap[baseName];
}


AxisProxy InputManager::GetAxis(const char* baseName)
{
	auto it = axisMap.find(baseName);
	if (it == axisMap.end())
	{
		axisMap[baseName] = { _axisPool.create(input::InputAxis(baseName)) };
	}
	return axisMap[baseName];
}

ActionProxy InputManager::GetAction(const char* actionName)
{
	if (!actionName || 0 == SDL_strcmp(actionName, ""))
		return ActionProxy();
	auto it = actionMap.find(actionName);
	if (it == actionMap.end())
	{
		actionMap[actionName] = { _actionPool.create(Action(actionName)) };
	}
	return actionMap[actionName];
}

bool InputManager::InitInput()
{
	return mouseInput.Init()&& kerboardInput.Init();
}

void InputManager::PumpDeviceOrNetworkInputEvents()
{
	mouseInput.UpdateAndPumpEvents();
	kerboardInput.UpdateAndPumpEvents();

	//设置默认鼠标值
	if (isMousePosMove)
	{
		_axis_mouse_posX->SetValue(_axis_mouse_posMoveX->GetValue());
		_axis_mouse_posY->SetValue(_axis_mouse_posMoveY->GetValue());
	}
	else
	{
		_axis_mouse_posX->SetValue(_axis_mouse_posCoordX->GetValue());
		_axis_mouse_posY->SetValue(_axis_mouse_posCoordY->GetValue());
	}

	//其他需要每帧调用的内容
	ButtonToAxisActionBinding::UpdateAxesValue();


}

ActionBindingHandle InputManager::RegisterButtonActionBinding(const char* downActionName, const char* upActionName, std::string* strArray, int buttonCount)
{
	if (0 == buttonCount)return NULL;

	ActionProxy downAction;
	ActionProxy upAction;



	downAction = GetAction(downActionName);
	upAction = GetAction(downActionName);



	//给每个按键设置绑定
	std::vector<ButtonProxy> buttonlist;
	for (int i = 0; i < buttonCount; i++)
	{
		buttonlist.push_back(GetButton(strArray[i].c_str()));
	}

	ActionBindingHandle handle = new ButtonActionBinding(downAction, upAction, buttonlist.data(), buttonCount);
	handle->InstallBinding();
	return handle;
}

ActionBindingHandle InputManager::RegisterAxisChangeActionBinding(const char* actionName, const char* axisBaseName)
{

	ActionBindingHandle handle = new AxisChangeActionBinding(
		GetAction(actionName),GetAxis(axisBaseName));
	return handle;
}

ActionBindingHandle InputManager::RegisterAxisExceedActionBinding(const char* actionName, const char* axisBaseName, float targetValue)
{
	if(!axisBaseName||0==SDL_strcmp(axisBaseName,""))
		return NULL;

	ActionProxy axisExceedAction = GetAction(actionName);
	ActionBindingHandle handle=new AxisExceedActionBinding(axisExceedAction,GetAxis(axisBaseName), targetValue);
	handle->InstallBinding();
	return handle;
}

ActionBindingHandle InputManager::RegisterAxisDroppedActionBinding(const char* actionName, const char* axisBaseName, float targetValue)
{
	if (UTIL_IS_CSTR_EMPTY(actionName)|| UTIL_IS_CSTR_EMPTY(axisBaseName))
		return NULL;

	ActionProxy axisExceedAction = GetAction(actionName);
	ActionBindingHandle handle = new AxisDroppedActionBinding(axisExceedAction, GetAxis(axisBaseName), targetValue);
	handle->InstallBinding();
	return handle;
}

ActionBindingHandle InputManager::RegisterButtonToAxisActionBinding(const char* axisActionName, const char* buttonName, float axisTargetValue)
{
	if (UTIL_IS_CSTR_EMPTY(axisActionName)||UTIL_IS_CSTR_EMPTY(buttonName))
		return NULL;
	ActionProxy axisAction = GetAction(axisActionName);
	ActionBindingHandle handle = new ButtonToAxisActionBinding(axisAction,GetButton(buttonName), axisTargetValue);

	handle->InstallBinding();
	return handle;
}

ActionBindingHandle InputManager::RegisterActionByActionBinding(const char* actionToBeTriggered, const char* origAction, float axisTargetValue)
{
	if (UTIL_IS_CSTR_EMPTY(actionToBeTriggered) || UTIL_IS_CSTR_EMPTY(origAction))
		return NULL;
	return ActionBindingHandle();
}

void InputManager::UnregisterActionBinding(ActionBindingHandle bindinghandle)
{
	//Qusetion是否有池化的必要？
	bindinghandle->UninstallBinding();
	delete bindinghandle;
}

void InputManager::RegisterActionCallback(const char* actionName, ActionCallback callback)
{
	GetAction(actionName)->AttachCallback(callback);
}

void InputManager::UnregisterActionCallback(const char* actionName, ActionCallback callback)
{
	GetAction(actionName)->DettachCallback(callback);
}






void InputManager::RiseAction(const char* actionName, float value)
{
	//不使用actionMap[XX]或GetAction()防止创建不必要map对象
	auto it = actionMap.find(actionName);
	if (it == actionMap.end())
	{
		return;
	}
	it->second->DoAction(value);
}

void InputManager::RiseAction(ActionProxy action, float value)
{
	action->DoAction(value);
}




void InputManager::SetMouseInputArea(SDL_Rect* displayRect)
{
	mouseInput.SetDisplayRect(displayRect);
}

void InputManager::ShutDown()
{
	mouseInput.Release();
	kerboardInput.Release();
	//已经进入了关闭过程了，其他东西让他们自动析构吧.
}
