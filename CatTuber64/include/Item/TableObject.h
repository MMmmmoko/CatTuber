#ifndef _TableObject_h
#define _TableObject_h


//桌子对象除了自身之外还将为笔和角色提供运动数据


#include"json/json.h"
#include"Model/IModel.h"
#include"Input/InputCommon.h"
class MixDrawList;

struct BindingInfo
{
	enum Type
	{
		Undefined=0,
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
	Type type= Undefined;
	std::vector<std::string> controllList;//如果是按钮相关则表示这个绑定相关的按钮列表，轴相关则表示的是轴列表
	float controlValue = 0.38268343236508f;//某些特殊类型的绑定需要用到一些参数//可能得使用union//数值为sin22.5

	void RegisterBinding(int index);//参数为按钮、轴、动画的索引？
	void UnRegisterBinding();
	static const char* GetTypeJsonStr(Type type);
private:
	std::vector<ActionBindingHandle> _bindingHandleList;
};


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
	ActionProxy downAction;
	ActionProxy upAction;
	bool isDown=false;
	float recoveryDuration=0.4f;//按键弹起需要的时间
	Uint64 upTickMs=0;//按下的毫秒时间戳

	//std::vector <BindingInfo> defaultBinding;//默认绑定
	//std::vector<BindingInfo> bindings;//当前应用的绑定
}; 
struct ModelAxisControl
{
	std::string uiName;
	std::string paramID;
	std::string activeAnimation;
	std::string inactiveAnimation;
	//std::string activeAction;
	//std::string inactiveAction;

	std::string changeAction;


	float actionActiveValue=1.f;

	BindingInfo defaultBinding;//默认绑定
	BindingInfo binding;//当前应用的绑定

	//上方是用于UI的
//下方是用于程序控制的


	ParamHandle paramHandle;
	ActionProxy activeAction;
	ActionProxy inactiveAction;
	float value = 0.f;

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




class TableObject
{


public:

	//Question:创意工坊路径和preferPath是有很小的可能会变化的，需要对路径进行一下包装防止路径过于绝对吗？
	//包装也可能反而导致寻找不到文件(切换路径而不搬运文件).
	bool LoadFromPath(const char* u8PackPath,const Json::Value& bindingJson= Json::Value());//入参应该是个文件夹或者资源包
	const char* GetPackPath() { return resourcePath.c_str(); };
	//
	//static const char* GetType();



	void Update(uint64_t deltaTicksNS);
	//其实是将模型推送到DrawList
	void Draw(MixDrawList* drawList);

	//有时希望不显示模型让模型不工作，但暂时不希望删除模型（如预览其他模式时），
	// 所以使用这个方法设置停止工作的标识
	void SetWorkingFlag(bool bWorking) { working = bWorking; };




	virtual Json::Value GenerateAttributes();
	//如果出现问题返回空
	static TableObject* CreateFromAttributes(const Json::Value& applyJson);
	static void ReleaseObj(TableObject*);//Release需要和Create搭配，免得其他人不知道怎么正确释放资源




	const std::vector<ModelButtonControl>& GetModelButtons() { return modelButtonVec; };
	const std::vector<ModelAxisControl>& GetModelAxes() { return modelAxisVec; };
	const std::vector<ModelAnimationControl>& GetModelAnimations() { return modelAnimationVec; };





	//物理按键到模型按钮（索引）与action的绑定？
	//物理按键到actionName的绑定？
	//物理按键到模型按钮actionName的绑定？
	//需要添加模型参数吗？
	//JsonRoot[PackPath]= [AppBasePath]/... [AppPrefPath]/...  [WorkshopPath]/..  创意工坊路径需要在软件退出时保存以便在离线时读取
	//JsonRoot[PackName]//模型名需要提供两份？一份提供给软件验证？一份多语言提供给UI显示？
	//JsonRoot[BindingName]="测试中文"
	//参考_SetUpControlAndAnimation函数，但不完全一致
	//直接按索引来吗？
	//JsonRoot[Buttons][0][Type]="ButtonToAction"
	//JsonRoot[Buttons][0][Button][0]="Keyborad.A"//WinKeyboardInput.cpp
	//JsonRoot[Buttons][0][DownAction]="Table.Button.0.Down"
	//JsonRoot[Buttons][0][UpAction]="Table.Button.0.Up"
	//Action
	


	bool LoadBindingByName(const char* bindingName);//读取手动保存的绑定
	void LoadBinding();//先尝试读取被App保存的用户设置的绑定，然后尝试读取Pack文件中缓存的绑定，如果没有则根据模型参数名生成绑定
	//void ClearBinding();//模型卸载、其他模型预览的时候移除绑定的回调//目前应该是不需要这个，因为有个working判断

private:
	//这个函数设置好模型的按钮、轴、动画信息以及默认的绑定信息，可以传入空json
	void _SetUpControlAndAnimation(const Json::Value& descItemInfo);
	//所有control（按钮轴动画）的设置已经完毕，向InputManager进行注册
	void _ApplyControlBindings();

	//从JSON中加载绑定覆写ModelButtonControl.Binding
	void _SetUpJsonBinding(const Json::Value&);
	Json::Value _GenerateJsonBinding();

	//注册按钮、轴等数据，即将具体函数绑定到对应Action上
	void RegisterAllActionFunc(bool falseToUnregister=true);
	void UnregisterAllActionFunc();

	void OnButtonDown(int btnIndex);//控制参数的同时还要触发按钮上绑定的动画//发送一个TableButtonDown的Action（当Event用）
	void OnButtonUp(int btnIndex);//发送一个TableButtonUp的Action（当Event用）
	void OnAxisValueChange(int axisIndex,float value);//控制对应参数
	void OnAnimationPlay(int animationIndex);


	bool working = false;
	IModel* _model=NULL;
	std::string resourcePath;

	//模型控件
	std::vector<ModelButtonControl> modelButtonVec;
	std::vector<ModelAxisControl> modelAxisVec;
	std::vector<ModelAnimationControl> modelAnimationVec;



};





#endif