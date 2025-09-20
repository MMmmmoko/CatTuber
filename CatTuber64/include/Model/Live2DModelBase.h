#ifndef _Live2DModelBase_h
#define _Live2DModelBase_h

#include<glm/glm.hpp>
#include<unordered_map>
#include <CubismFramework.hpp>
#include"IModel.h"
#include"Model/CubismUserModel.hpp"
#include"Pack/Pack.h"


//动作的优先级
namespace Live2D
{
	namespace Cubism
	{
		namespace Framework
		{
			enum Live2DMitonPriority
			{
				Live2DMitonPriority_None = 0,
				Live2DMitonPriority_Idle = 1,
				Live2DMitonPriority_Normal = 2,
				Live2DMitonPriority_Force = 3
			};
		}
	}
}

//为了控制部分无访问权限的变量
class CubismLive2DModel :public Csm::CubismUserModel
{
public:
	virtual ~CubismLive2DModel();

	bool LoadFromFile(const char* packPath, const char* pathInPack);


	void Update(float time);

	void Draw();
	void DrawMix(MixDrawList* pMix,glm::mat4x4& view_projMat);

	//因为采用从文件夹加载的策略，所以使用了一个寻找文件夹中模型的函数
	//static std::vector<std::string> FindModelFileInDir(Pack* pPack, const char* dirPath);


	//返回一个动作标识，给IsFinished()调用来确定动作是否结束。如果动作无法开始，返回-1
	Csm::CubismMotionQueueEntryHandle StartMotion(const Csm::csmChar* group, Csm::csmInt32 no, Csm::csmInt32 priority,
		Csm::ACubismMotion::FinishedMotionCallback onFinishedMotionHandler = NULL, Csm::ACubismMotion::BeganMotionCallback onBeganMotionHandler = NULL
		);
	//CatTuber采用组内随机的方式播放动画，组名即动画名
	Csm::CubismMotionQueueEntryHandle StartRandomMotion(const Csm::csmChar* group,Csm::csmInt32 priority, 
		Csm::ACubismMotion::FinishedMotionCallback onFinishedMotionHandler = NULL, Csm::ACubismMotion::BeganMotionCallback onBeganMotionHandler = NULL
		);


	//获取的是CatTuber可控的参数和动画
	std::vector<std::string> GetParamList();
	std::vector<std::string> GetAnimationList();
	
	
	void ReleaseMotions();
	void ReleaseExpressions();
	void ReleaseMotionGroup(const Csm::csmChar* group)const;



	void SetParamValue(ParamHandle param, float value, bool longTerm);
	void AddParamValue(ParamHandle param, float value, bool longTerm);


private:
	uint8_t* CreateBuffer(const char* pathInPack,size_t* size);
	void DeleteBuffer(uint8_t* mem,const char* LogPath=NULL);

	void SetupModel(Csm::ICubismModelSetting* setting);
	void SetupTexture();


	void _PreloadMotionGroup(const Csm::csmChar* group);



	Csm::ICubismModelSetting* _modelSetting = NULL;
	Csm::csmString _modelHomeDir;//这个末尾包含有"/"字符
	Pack _pack;


	//模型中所设置的眨眼和嘴唇同步的参数
	Csm::csmVector<Csm::CubismIdHandle> _eyeBlinkIds;
	Csm::csmVector<Csm::CubismIdHandle> _lipSyncIds;


	//保存动作和表情列表，供调用和数据清理
	Csm::csmMap<Csm::csmString, Csm::ACubismMotion*>   _motions; 
	Csm::csmMap<Csm::csmString, Csm::ACubismMotion*>   _expressions;

	//Csm::csmVector<Csm::csmUint64> _bindTextureId; ///< 纹理ID
	Csm::csmVector<SDL_GPUTexture*> _bindTexture; ///< 纹理


	//创建更多的motionmanager用于实现多轨同时播放动画
	//Csm::csmMap的查询效率有问题..
	std::unordered_map<Csm::csmInt32, Csm::CubismMotionManager*>   _motionManagers;
	std::unordered_map<std::string, int>   _animationTrackMap;
	int _idleMotionTrack = 0;//待机动作的轨道
	//std::unordered_map<>


	struct _ParamInfo
	{
		ParamHandle param;
		float value;
		bool isAdd;
	};
	std::vector<_ParamInfo> _paramSetCache_longterm;
	std::vector<_ParamInfo> _paramSetCache_curFrame;
};




class Live2DModelBase:public IModel
{
public:
	virtual ~Live2DModelBase() = default;

	static Live2DModelBase* LoadFromFile(const char* packPath,const char* filepath);
	static Live2DModelBase* LoadFromFolder(const char* packPath,const char* foldPath);
	virtual void Release()override { delete this; };

	//virtual bool LoadFromMemFile(unsigned char* mem)override;

	virtual void Update(uint64_t deltaTicksNS)override;
	virtual void Draw()override;
	virtual void DrawMix(MixDrawList* pMix)override;
	


	//后续再实现hit
	//virtual void Hit(float x, float y)override {};




	virtual void PlayAnimation(const std::string& name, bool loop = false)override;
	virtual ParamHandle GetParamHandle(const std::string& param)override;
	virtual void SetParamValue(ParamHandle param,float value,bool longTerm)override;
	virtual void AddParamValue(ParamHandle param,float value,bool longTerm)override;
protected:
	
	CubismLive2DModel l2dmodel;


};









#endif