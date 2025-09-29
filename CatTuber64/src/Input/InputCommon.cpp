#include<SDL3/SDL.h>
#include"Input/InputCommon.h"


//静态变量
ObjectPool<ButtonToAxisActionBinding::BindingInfo> ButtonToAxisActionBinding::bindingList;



void input::InputButton::ButtonDown()
{
	keyPressCount++;
	for (auto& callbackData : keyDownCallbackList)
	{
		callbackData.callback(this, 1.f, callbackData.userData);
	}
	if(keyPressCount==1)//0到1，状态改变了
	for (auto& callbackData : keyStateChangeCallbackList)
	{
		callbackData.callback(this, 1.f, callbackData.userData);
	}
}

void input::InputButton::ButtonUp()
{
	keyPressCount--;
	if (keyPressCount < 0)
	{
		keyPressCount = 0; return;
	}

	for (auto& callbackData : keyUpCallbackList)
	{
		callbackData.callback(this, 0.f, callbackData.userData);
	}

	if (keyPressCount == 0)//1到0，状态变了
	{
		for (auto& callbackData : keyStateChangeCallbackList)
		{
			callbackData.callback(this, 0.f, callbackData.userData);
		}
	}
}

void input::InputButton::AddRef(int refCount)
{
	_refCount += refCount;
	if (_refCount <= 0)
	{
		_refCount = 0;
		//引用数归0时,说明没有任何项目引用这个按键，重置回弹起的状态:
		while (keyPressCount > 0)
		{
			ButtonUp();
		}
	}
}

void input::InputButton::AttachKeyUpCallback(ButtonInputCallback callback)
{
	keyUpCallbackList.push_back(callback);
}

void input::InputButton::DetachKeyUpCallback(ButtonInputCallback callback)
{
	for (auto it = keyUpCallbackList.begin(); it != keyUpCallbackList.end(); it++)
	{
		if (it->callback == callback.callback && it->userData == callback.userData)
		{
			keyUpCallbackList.erase(it);
			return;
		}
	}
}

void input::InputButton::AttachKeyDownCallback(ButtonInputCallback callback)
{
	keyDownCallbackList.push_back(callback);
}
void input::InputButton::DetachKeyDownCallback(ButtonInputCallback callback)
{
	for (auto it = keyDownCallbackList.begin(); it != keyDownCallbackList.end(); it++)
	{
		if (it->callback == callback.callback && it->userData == callback.userData)
		{
			keyDownCallbackList.erase(it);
			return;
		}
	}
}

void input::InputButton::AttachKeyStateChange(ButtonInputCallback callback)
{
	keyStateChangeCallbackList.push_back(callback);
}
void input::InputButton::DetachKeyStateChange(ButtonInputCallback callback)
{
	for (auto it = keyStateChangeCallbackList.begin(); it != keyStateChangeCallbackList.end(); it++)
	{
		if (it->callback == callback.callback && it->userData == callback.userData)
		{
			keyStateChangeCallbackList.erase(it);
			return;
		}
	}
}



//AXIS
//AXIS
//AXIS
//AXIS
//AXIS


void input::InputAxis::SetValue(float _value)
{
	lastvalue = value;
	if (value== _value)
	{
		return;
	}



	value = _value;
	for (auto& callbackData : axisValueChangeCallbackList)
	{
		callbackData.callback(this,value, callbackData.userData);
	}
}

void input::InputAxis::AttachValueChangeCallback(AxisInputCallback callback)
{
	axisValueChangeCallbackList.push_back(callback);
}

void input::InputAxis::DettachValueChangeCallback(AxisInputCallback callback)
{
	for (auto it = axisValueChangeCallbackList.begin(); it != axisValueChangeCallbackList.end(); it++)
	{
		if (it->callback == callback.callback && it->userData == callback.userData)
		{
			axisValueChangeCallbackList.erase(it);
			return;
		}
	}
}


//ACTION
//ACTION
//ACTION
//ACTION
//ACTION
//ACTION
//ACTION
//ACTION


void Action::AttachCallback(ActionCallback callback)
{
	callbackList.push_back(callback);
}

void Action::DettachCallback(ActionCallback callback)
{
	for (auto it = callbackList.begin(); it != callbackList.end(); it++)
	{
		if (it->callback == callback.callback && it->userData == callback.userData && it->userData2 == callback.userData2)
		{
			callbackList.erase(it);
			return;
		}
	}
}

void Action::DoAction(float value)
{
	for (auto& callback : callbackList)
	{
		callback.callback(name.c_str(), value, callback.userData, callback.userData2);
	}
}


//BINDING
//BINDING
//BINDING
//BINDING
//BINDING
//BINDING
//BINDING


ButtonActionBinding::ButtonActionBinding(ActionProxy downAction, ActionProxy upAction, ButtonProxy* buttonArray, int count)
	:downAction(downAction), upAction(upAction)
{
	for (int i = 0; i < count; i++)
	{
		buttonVec.push_back(buttonArray[i]);
	}
}

void ButtonActionBinding::InstallBinding()
{
	input::ButtonInputCallback downCallback;
	downCallback.callback = TryDownTrigger;
	downCallback.userData = this;
	input::ButtonInputCallback upCallback;
	upCallback.callback = TryUpTrigger;
	upCallback.userData = this;

	//给每个键都加上callback来检测
	for (auto& button : buttonVec)
	{
		if(downAction)
		button->AttachKeyDownCallback(downCallback);
		if (upAction)
		button->AttachKeyUpCallback(upCallback);
	}



}

void ButtonActionBinding::UninstallBinding()
{
	input::ButtonInputCallback downCallback;
	downCallback.callback = TryDownTrigger;
	downCallback.userData = this;
	input::ButtonInputCallback upCallback;
	upCallback.callback = TryUpTrigger;
	upCallback.userData = this;
	for (auto& button : buttonVec)
	{
		if (downAction)
		button->DetachKeyDownCallback(downCallback);
		if (upAction)
		button->DetachKeyUpCallback(upCallback);
	}
}


void ButtonToAxisActionBinding::InstallBinding()
{
	input::ButtonInputCallback downCallback;
	downCallback.callback = TryDownTrigger;
	downCallback.userData = this;
	input::ButtonInputCallback upCallback;
	upCallback.callback = TryUpTrigger;
	upCallback.userData = this;


	button->AttachKeyDownCallback(downCallback);
	button->AttachKeyUpCallback(upCallback);

	BindingInfo bi;
	bi.targetAxisAction = axisAction;
	bindingInfoHandle=bindingList.create(bi);
}

void ButtonToAxisActionBinding::UninstallBinding()
{
	input::ButtonInputCallback downCallback;
	downCallback.callback = TryDownTrigger;
	downCallback.userData = this;
	input::ButtonInputCallback upCallback;
	upCallback.callback = TryUpTrigger;
	upCallback.userData = this;
	button->DetachKeyDownCallback(downCallback);
	button->DetachKeyUpCallback(upCallback);

	bindingList.destroy(bindingInfoHandle);
	bindingInfoHandle = ObjectPool<BindingInfo>::Handle();
}
void ButtonToAxisActionBinding::TryDownTrigger(input::InputButton* button, float value, void* pThisBinding)
{
	//按下时
	ButtonToAxisActionBinding* pThis = (ButtonToAxisActionBinding*)pThisBinding;
	pThis->bindingInfoHandle->motion = 1;
	pThis->bindingInfoHandle->motionTriggeredTick = SDL_GetTicks();
}
void ButtonToAxisActionBinding::TryUpTrigger(input::InputButton* button, float value, void* pThisBinding)
{
	ButtonToAxisActionBinding* pThis = (ButtonToAxisActionBinding*)pThisBinding;
	pThis->bindingInfoHandle->motion = 0;
	pThis->bindingInfoHandle->motionTriggeredTick = SDL_GetTicks();
}
void ButtonToAxisActionBinding::UpdateAxesValue()
{
	uint64_t curTicks=SDL_GetTicks();
	for (auto& x : bindingList)
	{
		float newValue = 0.f;
		if (x.motion == 0)
		{
			uint64_t deltaTick = curTicks - x.motionTriggeredTick;
			//在0.1s执行玩动画  todo/fix me?让这个0.1s可控
			if(deltaTick>100)newValue = 0.f;
			else
			{
				newValue = 1.f-deltaTick / 100.f;
			}
		}
		else
		{
			uint64_t deltaTick = curTicks - x.motionTriggeredTick;
			//在0.1s执行玩动画  todo/fix me?让这个0.1s可控
			if (deltaTick > 100)newValue = 1.f;
			else
			{
				newValue = deltaTick / 100.f;
			}
		}
		if (newValue != x.value)
		{
			x.value = newValue;
			x.targetAxisAction->DoAction(x.value);
		}

	}
}
void ButtonActionBinding::TryDownTrigger(input::InputButton* button, float value, void* pThisBinding)
{
	//检测数组中所有按键的状态
	ButtonActionBinding* pThis = (ButtonActionBinding*)pThisBinding;
	std::vector<ButtonProxy>& vec = pThis->buttonVec;
	for (auto& button : vec)
	{
		if (!button->IsDown())
			return;
	}
	//所有键都是Down状态，发起Action
	pThis->isDown = true;
	pThis->downAction->DoAction(1.f);
}

void ButtonActionBinding::TryUpTrigger(input::InputButton* button, float value, void* pThisBinding)
{


	//检测数组中所有按键的状态
	ButtonActionBinding* pThis = (ButtonActionBinding*)pThisBinding;
	if (pThis->isDown == false)
	{
		//只在按下的时候出发抬起
		return;
	}

	
	//当前按键up
	pThis->isDown = false;
	pThis->upAction->DoAction(0.f);
}

void AxisChangeActionBinding::InstallBinding()
{
	input::AxisInputCallback callback;
	callback.callback = TryTrigger;
	callback.userData = this;
	axis->AttachValueChangeCallback(callback);
}

void AxisChangeActionBinding::UninstallBinding()
{
	input::AxisInputCallback callback;
	callback.callback = TryTrigger;
	callback.userData = this;
	axis->DettachValueChangeCallback(callback);
}

void AxisChangeActionBinding::TryTrigger(input::InputAxis* axis, float value, void* pThisBinding)
{
	//这个函数绑定到ValueChange上，被调用说明数值一定已经改变了，直接触发Action即可
	((AxisChangeActionBinding*)pThisBinding)->action->DoAction(value);
}


void AxisExceedActionBinding::InstallBinding()
{
	input::AxisInputCallback callback;
	callback.callback = TryTrigger;
	callback.userData = this;
	axis->AttachValueChangeCallback(callback);
}

void AxisExceedActionBinding::UninstallBinding()
{
	input::AxisInputCallback callback;
	callback.callback = TryTrigger;
	callback.userData = this;
	axis->DettachValueChangeCallback(callback);
}

void AxisExceedActionBinding::TryTrigger(input::InputAxis* axis, float value, void* pThisBinding)
{
	//检测数值变化的方向，如果超越了targetValue则触发Action
	float lastValue=axis->GetLastValue();
	AxisExceedActionBinding* pThis = (AxisExceedActionBinding*)pThisBinding;
	if (lastValue<= pThis->targetValue && value> pThis->targetValue)
	{
		pThis->action->DoAction(value);
	}
}

void AxisDroppedActionBinding::InstallBinding()
{
	input::AxisInputCallback callback;
	callback.callback = TryTrigger;
	callback.userData = this;
	axis->AttachValueChangeCallback(callback);
}

void AxisDroppedActionBinding::UninstallBinding()
{
	input::AxisInputCallback callback;
	callback.callback = TryTrigger;
	callback.userData = this;
	axis->DettachValueChangeCallback(callback);
}

void AxisDroppedActionBinding::TryTrigger(input::InputAxis* axis, float value, void* pThisBinding)
{
	//检测数值变化的方向，如果跌破了targetValue则触发Action
	float lastValue = axis->GetLastValue();
	AxisDroppedActionBinding* pThis = (AxisDroppedActionBinding*)pThisBinding;
	if (lastValue >= pThis->targetValue && value < pThis->targetValue)
	{
		pThis->action->DoAction(value);
	}
}


void ActionByActionBinding::ActionCallbackFunc(const char* actionName, float value, void* userData, uint64_t userData2)
{
	ActionByActionBinding* _pthis =(ActionByActionBinding*) userData;
	_pthis->actionToBeTriggered->DoAction(_pthis->actionParam);
}

void ActionByActionBinding::InstallBinding()
{
	ActionCallback callback;
	callback.callback = ActionCallbackFunc;
	callback.userData = this;
	origAction->AttachCallback(callback);
}

void ActionByActionBinding::UninstallBinding()
{
	ActionCallback callback;
	callback.callback = ActionCallbackFunc;
	callback.userData = this;
	origAction->DettachCallback(callback);
}
