#ifndef ___BongoCatModel_h
#define ___BongoCatModel_h
#if 0
#include<glm/glm.hpp>
#include<unordered_map>
#include <CubismFramework.hpp>
#include"IModel.h"
#include"Model/CubismUserModel.hpp"
#include"Pack/Pack.h"


#include"Item/BongoCatHand.h"
#include"Item/BongoCatSprite.h"




class BongoCatModel:public IModel
{
	friend class BongoCatObject;
public:
	enum BongoCatMverMode
	{
		BongoCatMverMode_Standard,
		BongoCatMverMode_Keyboard,
		BongoCatMverMode_Gamepad,
		BongoCatMverMode_MaxCount,
	};



	virtual ~BongoCatModel() { if (l2dmodel)l2dmodel->Release(); l2dmodel=nullptr; };

	//static BongoCatModel* LoadFromFile(const char* packPath,const char* filepath);
	static BongoCatModel* LoadFromPack(const char* packPath);
	virtual void Release()override { delete this; };

	//virtual bool LoadFromMemFile(unsigned char* mem)override;

	virtual void Update(uint64_t deltaTicksNS)override;
	virtual void Draw()override;
	virtual void DrawMix(MixDrawList* pMix)override;
	


	//后续再实现hit
	//virtual void Hit(float x, float y)override {};
	virtual std::vector<std::string> GetParamList()override;
	virtual std::vector<std::string> GetAnimationList()override;



	virtual void PlayAnimation(const char* name, bool loop = false)override;
	virtual void PlayAnimationEX(const char* name, int index = 0, FinishedAnimationCallback finishedCall = nullptr, void* finishedCallUserData = nullptr,
		BeganAnimationCallback beganCall = nullptr, void* beganCallUserData = nullptr)override;

	virtual void SetExpression(const char* expressionID)override;
	virtual void SetExpression(int expressionIndex)override;


	virtual ParamHandle GetParamHandle(const std::string& param)override;
	virtual HandPosHandle GetHandHandle(const std::string& param)override;
	virtual void GetHandPosFromHandle(HandPosHandle handPosHandle, float* x, float* y)override;
	virtual void SetParamValue(ParamHandle param,float value, bool normallizeValue=true,bool longTerm=false)override;
	virtual void AddParamValue(ParamHandle param,float value, bool normallizeValue = true, bool longTerm = false)override;
	virtual void MultiplyParamValue(ParamHandle param,float value,bool longTerm)override;



	BongoCatMverMode GetBongoCatMode(){ return mode; }

private:
	enum ParamHandleType
	{
		ParamHandleType_Unknown,
		ParamHandleType_Key,
		ParamHandleType_HandL,
		ParamHandleType_HandR,

	};
	//中间参数
	std::vector<float>keyStates;

	uint32_t lHandDownIndex=UINT32_MAX;
	uint32_t rHandDownIndex= UINT32_MAX;



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



	void _ReadKeysFromJsonArray(const Json::Value& arr, std::vector<std::vector<unsigned char>>& vec);
	inline void _DRAW(BongoCatSprite& s) { s.Draw(); };
protected:
	//BongoCat运行模式
	BongoCatMverMode mode = BongoCatMverMode_Standard;
	struct
	{
		int emoticonIndex = -1;
		int l2dExpressionIndex = -1;
		std::vector<int> leftHandStateStack;//绘制时使用这个栈的栈顶
		std::vector<bool> keyboardStates;
		bool mouseButtonStates[3] = { 0 };
		float mousePos[2] = { 0 };


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
	BongoCatHand lefthand;
	BongoCatHand righthand;

	struct
	{
		unsigned char armColor[3] = { 0 };
		float l2dScale = 1.0;
		float l2dOffset[2] = { 0,-0.005f };
		float handOffset[2] = { 0,0 };
		float mousePNGOffset[2] = { 10,-10 };
		float penPNGOffset[2] = { 11,-65 };
		float mousePNGscale = 1.f;
		float penPNGscale = 1.f;
		bool soundKeep = true;//true时 同时多次按键同时播放音频，false后按键的音频会打断正在播放的音频
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
	std::vector<std::vector<unsigned char>> l2dExpressionKeyVec;
	std::vector<std::vector<unsigned char>> l2dMotionKeyVec;
	std::vector<std::vector<unsigned char>> l2dMotionKeyVec_LockHand;

	std::vector<unsigned char> mouseLeft_KeyVec;
	std::vector<unsigned char> mouseRight_KeyVec;
	std::vector<unsigned char> mouseSide_KeyVec;
	std::vector<std::vector<unsigned char>> sounds_KeyVec;



	Pack pack;




	IModel* l2dmodel = nullptr;
	bool isUsingLive2D = false;
	bool isUsingLive2DHand = false;
	bool isUsingLive2DDesk = false;
	bool isUsingPen = false;

};









#endif
#endif