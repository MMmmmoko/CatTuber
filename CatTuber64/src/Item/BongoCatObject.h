#ifndef _BongoCatObject_h
#define _BongoCatObject_h



#if 0

#include"json/json.h"
#include"Model/IModel.h"
#include"Item/ModelControl.h"
#include"Item/StraightMovingPoint.h"
#include"MeshMapping.h"
#include"BongoCatSprite.h"
#include"BongoCatHand.h"
#include"Pack/Pack.h" 
#include"Model/BongoCatModel.h"


class MixDrawList;



class BongoCatObject
{
	friend class ClassicItem;

public:

	//Question:创意工坊路径和preferPath是有很小的可能会变化的，需要对路径进行一下包装防止路径过于绝对吗？
	//包装也可能反而导致寻找不到文件(切换路径而不搬运文件).
	bool LoadFromPath(const char* u8PackPath, const Json::Value& bindingJson = Json::Value());//入参应该是个文件夹或者资源包
	const char* GetPackPath() { return resourcePath.c_str(); };
	IModel* GetModel() { return _model; };



	void Update(uint64_t deltaTicksNS);
	void Draw(MixDrawList* drawList);
	void Draw();
	void OnLoopEnd();//一些清理和状态重置工作

	//有时希望不显示模型让模型不工作，但暂时不希望删除模型（如预览其他模式时），
	// 所以使用这个方法设置停止工作的标识
	void SetWorkingFlag(bool bWorking) { working = bWorking; };




	virtual Json::Value GenerateAttributes();
	//如果出现问题返回空
	static BongoCatObject* CreateFromAttributes(const Json::Value& applyJson);
	static void ReleaseObj(BongoCatObject*);//Release需要和Create搭配，免得其他人不知道怎么正确释放资源




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
	//JsonRoot[Buttons][0][DownAction]="Desk.Button.0.Down"
	//JsonRoot[Buttons][0][UpAction]="Desk.Button.0.Up"
	//Action



	bool LoadBindingByName(const char* bindingName);//读取手动保存的绑定
	void LoadBinding();//先尝试读取被App保存的用户设置的绑定，然后尝试读取Pack文件中缓存的绑定，如果没有则根据模型参数名生成绑定
	void ClearBinding();//模型卸载的时候移除绑定的回调//预览其他模型时不需要这个，因为有个working判断，仅在删除此对象时调用

private:
	void SetUpDefaultControl(Json::Value& desc, Json::Value& config);

	//注册按钮、轴等数据，即将具体函数绑定到对应Action上
	void RegisterAllActionFunc(bool falseToUnregister = true);
	void UnregisterAllActionFunc();//Action与实际成员函数的绑定


	void OnButtonDown(int btnIndex);//控制参数的同时还要触发按钮上绑定的动画//发送一个DeskButtonDown的Action（当Event用）
	void OnButtonUp(int btnIndex);//发送一个DeskButtonUp的Action（当Event用）
	void OnAxisValueChange(int axisGroupIndex, int axisIndex, float value);//控制对应参数
	void OnAnimationPlay(int animationIndex);

	void _UpdateAxisVec();


	//Update
	void _Update_Standard(uint64_t dtNS);
	void _Update_Keyboard(uint64_t dtNS) { return; };
	void _Update_Gamepad(uint64_t dtNS) { return; };


	//模型一些软件参数
	float offsetX = 0.f;
	float offsetY = 0.f;
	float scale = 1.f;


	//模型自身参数
	bool working = false;
	class BongoCatModel* _model = NULL;
	class BongoCatItem* _pParentItem = NULL;
	std::string resourcePath;

	//模型控件
	std::vector<ModelButtonControl> modelButtonVec;
	std::vector<ModelAxisControl> modelAxisVec;
	std::vector<ModelAnimationControl> modelAnimationVec;

	//辅助数据
	//用于计算当前帧按下的按钮数设立一个缓存防止反复申请内存
	std::vector<ModelButtonControl*> _pushedButtnVec;
	std::vector<struct ModelAxisControl*> _noMoveMouseAxisControls_L;//未移动但是被标注了active的鼠标轴
	std::vector<struct ModelAxisControl*> _noMoveMouseAxisControls_R;//未移动但是被标注了active的鼠标轴



	Pack pack;
};



#else
//角色对象
//需要接收面捕输入

#include"json/json.h"
#include"Model/IModel.h"
#include"Item/ModelControl.h"
#include"Item/StraightMovingPoint.h"
#include"MeshMapping.h"
#include"BongoCatSprite.h"
#include"BongoCatHand.h"
#include"Pack/Pack.h" 

class MixDrawList;

class BongoCatObject
{

public:
	~BongoCatObject();



	bool LoadFromPath(const char* u8PackPath, const Json::Value& bindingJson = Json::Value());//入参应该是个文件夹或者资源包
	const char* GetPackPath() { return pack.GetPath(); };
	IModel* GetModel() { return _model; };


	void Update(uint64_t deltaTicksNS);
	void Draw(MixDrawList* drawList);
	void Draw();


	//有时希望不显示模型让模型不工作，但暂时不希望删除模型（如预览其他模式时），
	// 所以使用这个方法设置停止工作的标识
	void SetWorkingFlag(bool bWorking) { working = bWorking; };


	virtual Json::Value GenerateAttributes();
	//如果出现问题返回空
	static BongoCatObject* CreateFromAttributes(const Json::Value& applyJson);
	static void ReleaseObj(BongoCatObject*);//Release需要和Create搭配，免得其他人不知道怎么正确释放资源



	const std::vector<ModelAnimationControl>& GetModelAnimations() { return modelAnimationVec; };

	bool LoadBindingByName(const char* bindingName);//读取手动保存的绑定
	void LoadBinding();//先尝试读取被App保存的用户设置的绑定，然后尝试读取Pack文件中缓存的绑定，如果没有则根据模型参数名生成绑定
	void ClearBinding();//模型卸载的时候移除绑定的回调//预览其他模型时不需要这个，因为有个working判断，仅在删除此对象时调用


	void SetHandPosition(int handindex, bool bPress, float x, float y);//bPress为false时表示没有数据，手应该抬起了





private:
	void SetUpBindingByJson();
	void ApplyControlBindings();




	//注册按钮、轴等数据，即将具体函数绑定到对应Action上
	void RegisterAllActionFunc(bool falseToUnregister = true);
	void UnregisterAllActionFunc();//Action与实际成员函数的绑定

	void OnAnimationPlay(int animationIndex);



	//void ButtonDown(unsigned char bcmvk);




	//资源加载
	bool _LoadResource_Standard(Json::Value& config);
	bool _LoadResource_Keyboard(Json::Value& config) { return false; };
	bool _LoadResource_Gamepad(Json::Value& config) { return false; };
	bool _LoadSprite(const char* pathInPack, BongoCatSprite& sprite);


	//Update
	void _Update_Standard(uint64_t dtNS);
	void _Update_Keyboard(uint64_t dtNS) { return; };
	void _Update_Gamepad(uint64_t dtNS) { return; };
	//Update
	void _Draw_Standard();
	void _Draw_Keyboard() { return; };
	void _Draw_Gamepad() { return; };
	inline void _DRAW(BongoCatSprite& s) { s.Draw(); };







	void StartL2DExpression(int index);
	void StartL2DMotion(int index);
	void StartL2DMotion_HideHand(int index);
	void StopL2DExpression();
	void StopL2DMotion();




	void ClearSound();
	void ClearEmoticon();
	void Play_Sound(int index);
	void SetEmoticon(int index);
	void CancelEmoticon(int index);
	void SetLeftHandState(int index, bool bdown);
	void SetRightHandState(int index, bool bdown);
	void KeyboardActive(int index, bool bactive);




	void _ReadKeysFromJsonArray(const Json::Value& arr, std::vector<std::vector<unsigned char>>& vec);









	//模型一些软件参数
	float offsetX = 0.f;
	float offsetY = 0.f;
	float scale = 1.f;

	bool working = false;
	IModel* _model = NULL;//2D模型






	struct
	{
		int emoticonIndex = -1;
		int l2dExpressionIndex = -1;
		std::vector<int> leftHandStateStack;//绘制时使用这个栈的栈顶
		std::vector<int> rightHandStateStack;//绘制时使用这个栈的栈顶
		std::vector<bool> keyboardStates;
		bool mouseButtonStates[3] = { 0 };
		//float mousePos[2] = { 0 };
		float rightHandPos[2] = { 0 };
		float leftHandPos[2] = { 0 };
		bool useRightHandPos=false;
		bool useLeftHandPos=false;

		bool isLockingHand = false;

	}currentStates;









	struct
	{
		BongoCatSprite cat;
		BongoCatSprite leftUp;
		BongoCatSprite rightUp;
		BongoCatSprite mouse;
		BongoCatSprite mouse_left;
		BongoCatSprite mouse_right;
		BongoCatSprite mouse_side;
		BongoCatSprite mousebg;
		//BongoCatSprite pen;
		//BongoCatSprite pen_left;
		//BongoCatSprite pen_right;
		//BongoCatSprite pen_side;
		//BongoCatSprite penbg;

		//SDL_GPUTexture* armL = nullptr;
		//SDL_GPUTexture* armR = nullptr;

		std::vector<BongoCatSprite>facevec;//vector扩展的时候会触发析构导致出错？
		std::vector<BongoCatSprite>LHandvec;
		std::vector<BongoCatSprite>RHandvec;
		std::vector<BongoCatSprite>keyboardvec;



	}pngResource = {};
	std::vector<struct MIX_Track*>audioTrackResource;
	std::vector<struct MIX_Audio*>audioSoundResource;
	MIX_Track* mainTrack=nullptr;


	BongoCatHand lefthand;
	BongoCatHand righthand;



	struct
	{
		unsigned char armColor[3] = { 0 };
		float l2dScale = 1.0;
		float l2dOffset[2] = { 0,-0.005f };
		bool usel2dDecoration=false;
		float handOffset[2] = { 0,0 };
		float mousePNGOffset[2] = { 10,-10 };
		float penPNGOffset[2] = { 11,-65 };
		float mousePNGscale = 1.f;
		float penPNGscale = 1.f;
		bool soundKeep = true;//可以考虑改名为多音轨 //true时 同时多次按键同时播放音频，false后按键的音频会打断正在播放的音频
		bool emoticonKeep = true;//true时 取消表情为再次按表情键，false松手取消按键
		std::vector<unsigned char> soundClearKey;//停止音频播放的按键
		std::vector<unsigned char> emoticonClearKey;//停止音频播放的按键
		int initialSize[2] = { 612,352 };//初始画布大小，数据来自Config.json
		//CubismMatrix44 projection;
	}decoration;



	std::vector<std::vector<unsigned char>> faceKeyVec;
	std::vector<std::vector<unsigned char>> leftHandKeyVec;
	std::vector<std::vector<unsigned char>> rightHandKeyVec;
	std::vector<std::vector<unsigned char>> keyboardKeyVec;
	std::vector<std::vector<unsigned char>> soundsKeyVec;
	std::vector<std::vector<unsigned char>> l2dExpressionKeyVec;
	std::vector<std::vector<unsigned char>> l2dMotionKeyVec;
	std::vector<std::vector<unsigned char>> l2dMotionKeyVec_LockHand;

	std::vector<unsigned char> mouseLeft_KeyVec;
	std::vector<unsigned char> mouseRight_KeyVec;
	std::vector<unsigned char> mouseSide_KeyVec;


	std::vector<unsigned char> stopSound_KeyVec;
	std::vector<unsigned char> stopEmotion_KeyVec;


	//BongoCat运行模式
	enum BongoCatMverMode
	{
		BongoCatMverMode_Standard,
		BongoCatMverMode_Keyboard,
		BongoCatMverMode_Gamepad,
		BongoCatMverMode_MaxCount,
	}mode = BongoCatMverMode_Standard;
	bool isUsingLive2D = false;
	bool isUsingLive2DHand = false;
	bool isUsingLive2DDesk = false;
	bool isUsingPen = false;











	class DecorationItem* _pParentItem = NULL;
	//std::string resourcePath;
	Pack pack;
	//模型控件
	//std::vector<ModelAnimationControl> modelAnimationVec;


	std::vector<ModelButtonControl> leftHandButtonVec;
	std::vector<ModelButtonControl> rightHandButtonVec;
	std::vector<ModelButtonControl> keyboardButtonVec;
	std::vector<ModelButtonControl> soundsButtonVec;//音频触发按钮绑定
	std::vector<ModelButtonControl> emotionButtonVec;//角色表情

	//一些功能绑定？
	ModelButtonControl controlClearSound;
	ModelButtonControl controlClearEmotion;


	std::vector<ModelAxisControl> modelAxisVec;
	std::vector<ModelAnimationControl> modelAnimationVec;


	//渲染状态
	struct
	{






	}renderStates;
};










#endif
#endif










