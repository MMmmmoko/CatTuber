#ifndef _InputCommon_h
#define _InputCommon_h


#include<iostream>
#include<vector>
#include"Util/ObjectPool.h"
//这里选择使用传统函数指针而不使用Weakcallback，Weakcallback的性能适合于UI场景
//这里比较高频且性能敏感，所以选择用C风格函数指针
//#include"WeakSupport.h"









//
//typedef struct SDL_UserEvent
//{
//    Uint32 type;        /**< SDL_EVENT_USER through SDL_EVENT_LAST-1, Uint32 because these are not in the SDL_EventType enumeration */
//    Uint32 reserved;
//    Uint64 timestamp;   /**< In nanoseconds, populated using SDL_GetTicksNS() */
//    SDL_WindowID windowID; /**< The associated window if any */
//    Sint32 code;        /**< User defined event code */
//    void* data1;        /**< User defined data pointer */
//    void* data2;        /**< User defined data pointer */
//} SDL_UserEvent;




//code 低16位为索引，高16位为
namespace input
{

    ////无法转换则返回空
    //const char* ButtonNameToBaseName(const std::string &);
    //const char* AxisNameToBaseName(const std::string &);


    //仅在输入相关的线程使用
    //UI中可以将轴视为按键，可以设置为正向或负向sin(22.5度)的点视为按键触发点
    //有需要将按键视为轴的情况吗？
    //有。比如对于摇杆模型或者方向盘绑定到WASD键
    //所以模型绑定要再做一层封装？？
    //


    class InputButton;
    class InputAxis;
    struct ButtonInputCallback
    {
        void(*callback)(InputButton* pSender, float value, void* userData);
        void* userData;
    };
    struct AxisInputCallback
    {
        void(*callback)(InputAxis* pSender, float value, void* userData);
        void* userData;
    };
    class InputButton
    {

        std::string name;
        int _refCount = 0;
        int keyPressCount = 0;
        bool downCurframe = false;
        bool upCurframe = false;

        friend class InputManager;
    public:
        InputButton(const char* name) :name(name) {};
        InputButton() {};
        void _SetName(std::string _name) { name = _name; };

        bool IsDown() { return keyPressCount > 0; };
        bool IsDown_Curframe() { return downCurframe; };
        bool IsUp_Curframe() { return upCurframe; };

        void ButtonDown();
        void ButtonUp();
        void AddRef(int refCount);


        void AttachKeyUpCallback(ButtonInputCallback callback);
        void DetachKeyUpCallback(ButtonInputCallback callback);
        void AttachKeyDownCallback(ButtonInputCallback callback);
        void DetachKeyDownCallback(ButtonInputCallback callback);
        void AttachKeyStateChange(ButtonInputCallback callback);
        void DetachKeyStateChange(ButtonInputCallback callback);
    private:

        //void _OnKeyUp();
        //void _OnKeyDown();
        std::vector<ButtonInputCallback> keyDownCallbackList;
        std::vector<ButtonInputCallback> keyUpCallbackList;
        std::vector<ButtonInputCallback> keyStateChangeCallbackList;
    };


    class InputAxis
    {
        friend class InputManager;
        std::string name;
        int _refCount = 0;
        float lastvalue = 0;//用于判断运动方向
        float value = 0;
        bool _valueChangeCurFrame = false;

    public:
        InputAxis(const char* name) :name(name) {};
        void SetValue(float value);
        float GetLastValue() { return  lastvalue; };
        float GetValue() { return  value; };
        void AddRef(int refCount);
        //一般用于各种绘制对象在Update甚至Draw时调用。在当前帧数据源SetValue前调用可能无法得出预期结果
        bool ValueChangeCurFrame() { return _valueChangeCurFrame; };


        void AttachValueChangeCallback(AxisInputCallback callback);
        void DettachValueChangeCallback(AxisInputCallback callback);
    private:
        std::vector<AxisInputCallback> axisValueChangeCallbackList;

        ////防止轴采样过高导致过于频繁调用轴的callback，记录下一帧中有变化的对象以每帧只调用一次callback？
        // 采样率过高的问题应该交给各类设备Input去过滤解决，不需要在这里重复解决
        //static std::vector<InputAxis*> _valueChangeList;
    };
}






//ActionName eg:   Table.Button.1.Down Character.Animation.Jump
//看一下其他库的action设计
//区分用于模型的Action和设置界面中的CommandAction

struct ActionCallback
{
    void(*callback)(const char* actionName,float value, void* userData, uint64_t userData2) = NULL;
    void* userData=NULL;
    uint64_t userData2 = NULL;//作为data1的参数，减少actionName的解析压力，
};

class Action
{
public:
    std::string name;
    Action(const char* name) :name(name) {};
    void AttachCallback(ActionCallback callback);
    void DettachCallback(ActionCallback callback);
    void DoAction(float value=0.f);

private:
    std::vector<ActionCallback> callbackList;
};


typedef ObjectPool<Action>::Handle ActionProxy;
typedef ObjectPool<input::InputButton>::Handle ButtonProxy;
typedef ObjectPool<input::InputAxis>::Handle AxisProxy;






class IActionBinding
{
public:
    virtual ~IActionBinding() = default;

    //virtual bool TestTrigger() = 0;

    virtual void InstallBinding() = 0;
    virtual void UninstallBinding() = 0;

    //void* listener;
};
typedef IActionBinding* ActionBindingHandle;



class ButtonActionBinding :public IActionBinding
{
public:
    ButtonActionBinding(ActionProxy downAction, ActionProxy upAction, ButtonProxy* buttonArray,int count=1);
    //为Vec的每个键设置绑定
    virtual void InstallBinding()override;
    virtual void UninstallBinding()override;

    //bool TestTrigger();
    //这里和下方的TryTrigger函数是按钮或轴的CALLBACK,
    //用于在按钮被按下抬起的时候或者轴数值变化的时候，检查是否可以发送事件，如果可以，则发起事件
    static void TryDownTrigger(input::InputButton* button,float value,void* pThisBinding);
    static void TryUpTrigger(input::InputButton* button,float value,void* pThisBinding);
private:
    std::vector<ButtonProxy> buttonVec;
    ActionProxy downAction;
    ActionProxy upAction;
    bool isDown=false;
};

class AxisChangeActionBinding :public IActionBinding
{
public:
    AxisChangeActionBinding(ActionProxy action, AxisProxy axis) :action(action), axis(axis) {};
    //为Vec的每个键设置绑定
    virtual void InstallBinding()override;
    virtual void UninstallBinding()override;


    static void TryTrigger(input::InputAxis* axis,float value,void* pThisBinding);
private:
    AxisProxy axis;
    ActionProxy action;
};

//Axis数值从越过某值时触发
class AxisExceedActionBinding :public IActionBinding
{
public:
    AxisExceedActionBinding(ActionProxy action, AxisProxy axis, float targetValue)
        :action(action), axis(axis),targetValue(targetValue) {};
    //为Vec的每个键设置绑定
    virtual void InstallBinding()override;
    virtual void UninstallBinding()override;

    static void TryTrigger(input::InputAxis* axis,float value,void* pThisBinding);
private:
    AxisProxy axis;
    float targetValue;
    ActionProxy action;
    //QUESTION 后续需要给这个设置“冷却时间”吗
};

//Axis数值跌破某数值时触发
class AxisDroppedActionBinding :public IActionBinding
{
public:
    AxisDroppedActionBinding(ActionProxy action, AxisProxy axis, float targetValue)
        :action(action), axis(axis), targetValue(targetValue) {
    };
    //为Vec的每个键设置绑定
    virtual void InstallBinding()override;
    virtual void UninstallBinding()override;

    static void TryTrigger(input::InputAxis* axis,float value,void* pThisBinding);
private:
    AxisProxy axis;
    float targetValue;
    ActionProxy action;
};


class ButtonToAxisActionBinding :public IActionBinding
{
public:
    ButtonToAxisActionBinding(ActionProxy axisAction, ButtonProxy button, float axisTargetValue)
        :axisAction(axisAction), button(button), targetValue(axisTargetValue)
    {
    };
    virtual void InstallBinding()override;
    virtual void UninstallBinding()override;


    static void TryDownTrigger(input::InputButton* button, float value, void* pThisBinding);
    static void TryUpTrigger(input::InputButton* button, float value, void* pThisBinding);


    //这个需要在inputmanager里每帧调用一次
    static void UpdateAxesValue();

private:
    ButtonProxy button;
    ActionProxy axisAction;
    float targetValue;
    bool isDown = false;

    struct BindingInfo
    {
        int motion = 0;//1按下0抬起
        uint64_t motionTriggeredTick = 0;//触发的时刻
        float value;
        ActionProxy targetAxisAction;
    };
    ObjectPool<BindingInfo>::Handle bindingInfoHandle;
    static ObjectPool<BindingInfo> bindingList;
};

class ActionByActionBinding :public IActionBinding
{
public:
    ActionByActionBinding(ActionProxy actionToBeTriggered, ActionProxy origAction, float paramValue = 0.f)
        :actionToBeTriggered(actionToBeTriggered), origAction(origAction), actionParam(paramValue)
    {
    };

    virtual void InstallBinding()override;
    virtual void UninstallBinding()override;

private:
    static void ActionCallbackFunc(const char* actionName, float value, void* userData, uint64_t userData2);
    ActionProxy actionToBeTriggered;
    ActionProxy origAction;
    float actionParam;
};







#endif