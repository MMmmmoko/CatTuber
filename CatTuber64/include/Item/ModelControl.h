#ifndef _ModelControl_h
#define _ModelControl_h

//提供一些Object间共用的代码
//模型中抽象出一些控制相关的内容
//此文件中主要是模型的与物理按键相关的控制项目


#include"json/json.h"
#include"Model/IModel.h"
#include"Input/InputCommon.h"

struct BindingInfo
{
	enum Type
	{
		Undefined = 0,
		Button_ActualButton,
		Button_ActualAxisToButton,

		Axis_ActualAxis,
		Axis_ActualButtonToAxis,

		Animation_ActualButton,
		Animation_ActualAxisActive,
		Animation_ActualAxisInactive,
		Animation_Action,
		TypeMax
	};//添加项目的时候要修改GetTypeJsonStr
	Type type = Undefined;
	std::vector<std::string> controllList;//如果是按钮相关则表示这个绑定相关的按钮列表，轴相关则表示的是轴列表
	float controlValue = 0.38268343236508f;//某些特殊类型的绑定需要用到一些参数//可能得使用union//数值为sin22.5

	void RegisterBinding(int index,int index2=0);//参数为按钮、轴、动画的索引？//轴可用到index2
	void UnRegisterBinding();
	static const char* GetTypeJsonStr(Type type);
private:
	std::vector<ActionBindingHandle> _bindingHandleList;
};


struct HandControl
{
	enum HAND
	{
		UNDEFINED,
		LEFT,
		RIGHT
	};

	//Axis用的
	enum MOVETYPE
	{
		FIXPOS,
		MOUSE,
		STICK,
		MOVING,
		ABSOLUTEBALL,
		INERTIA_ABSOLUTEBALL,
	};

	HandPosHandle handPosHandle=INVALID_HANDHANDLE;//模型需要依据这个东西计算手的位置
	float handWeight=1.f;
	HAND handIndex = UNDEFINED;
	MOVETYPE moveType= MOUSE;
	//用于计算的数据
	union _moveCalcData
	{
		struct {
			bool mouseDataActive;//对于鼠标,为鼠标是否有效
			bool requiredHandheldItem;//如果为true，则要求必须存在handheld item 才生效
		}mouseData;
		uint64_t stickDataZeroTimeMs;//上次数据为0时的时间戳
		uint64_t movingDataStopTimeMs;//上次数据变化的时间戳
	}moveCalcData = {};
	//仅提供给MOUSE运动的运动标识
	//bool mouseDataActive=false;
};


//抽象出模型按钮信息
struct ModelButtonControl
{
	std::string uiName;
	std::string paramID;
	//std::string iconPath;
	std::string downAnimation;
	std::string upAnimation;
	//std::string downAction;//注意这个是触发按钮时产生的Action，不是物理按键产生的aciton
	//std::string upAction;

	//不允许用户多重绑定吧
	BindingInfo defaultBinding;//默认绑定
	BindingInfo binding;//当前应用的绑定



	//上方是用于UI的
	//下方是用于程序控制的
	ParamHandle paramHandle;

	HandControl handControl;


	ActionProxy downAction;
	ActionProxy upAction;
	bool isDown = false;
	float recoveryDuration = 0.25F;//按键弹起需要的时间
	Uint64 upTickMs = 0;//抬起的毫秒时间戳



	//std::vector <BindingInfo> defaultBinding;//默认绑定
	//std::vector<BindingInfo> bindings;//当前应用的绑定
};


//多轴应妥善处理防止多次触发
struct ModelAxisControl
{
	std::string uiName;

	//轴支持多维度
	struct AxisInfo
	{
		std::string paramID;

		BindingInfo defaultBinding;//默认绑定
		BindingInfo binding;//当前应用的绑定


		ParamHandle paramHandle;
		float value = 0.f;
	};
	std::vector<AxisInfo> axisVec;


	std::string activeAnimation;
	std::string inactiveAnimation;

	std::string changeAction;


	float actionActiveValue = 1.f;
	float groupValue = 0.f;//对于单个轴的情况，直接为轴的数值，对位多轴的组，为模长



	//上方是用于UI的
//下方是用于程序控制的


	HandControl handControl;

	ActionProxy activeAction;
	ActionProxy inactiveAction;

	bool valueChangedCurFrame = false;
};



struct ModelAnimationControl
{
	std::string uiName;//用于UI展示的名称
	std::string controlName;//用于软件控制的名称
	std::string imageFile;//用于UI展示的方形图片在资源包中的路径
	//int track = 0;//用于混合动画  TRACK直接由模型进行管理。


	//std::string endAction;//动画结束时的action

	//动画允许绑定多种触发方式
	std::vector<BindingInfo> defaultBinding;//默认绑定
	std::vector<BindingInfo> binding;//当前应用的绑定
};


class ModelControl
{
public:
	static bool LoadBindingByName(const std::string& packPath,const char* bindingName, 
		std::vector<ModelButtonControl>* modelButtonVec, std::vector<ModelAxisControl>* modelAxisVec, std::vector<ModelAnimationControl>* modelAnimationVec);
	
	//通过json设置binding (不是defaultbinding)
	static void SetUpBindingByJson(const Json::Value& bindingJson, std::vector<ModelButtonControl>* modelButtonVec, std::vector<ModelAxisControl>* modelAxisVec, std::vector<ModelAnimationControl>* modelAnimationVec);

	static Json::Value GenerateJsonBinding(std::vector<ModelButtonControl>* modelButtonVec, std::vector<ModelAxisControl>* modelAxisVec, std::vector<ModelAnimationControl>* modelAnimationVec);

	//从模型的desc文件中加载默认控件信息，如无则从模型参数加载
	static void SetUpDefaultControl(const Json::Value& descItemInfo, IModel* model,std::vector<ModelButtonControl>* modelButtonVec, std::vector<ModelAxisControl>* modelAxisVec, std::vector<ModelAnimationControl>* modelAnimationVec);

	static void ApplyControlBindings(std::vector<ModelButtonControl>* modelButtonVec, std::vector<ModelAxisControl>* modelAxisVec, std::vector<ModelAnimationControl>* modelAnimationVec);
};


















#endif

