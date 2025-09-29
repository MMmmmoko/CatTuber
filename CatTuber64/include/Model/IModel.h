#ifndef _IModel_h
#define _IModel_h


#include<SDL3/SDL.h>
#include<vector>
#include<iostream>




//动画区分为按钮触发、事件触发
//对于Live2D，名称前缀将用于区分动作管理器
enum ModelType
{
	ModelType_Unknown,
	ModelType_Live2D,
	ModelType_Spine
};


typedef uintptr_t ParamHandle;
typedef uintptr_t HandPosHandle;//模型使用这个标识获取手部的位置
#define INVALID_HANDHANDLE UINT64_MAX
class MixDrawList;
class Scene;
class IModel
{
public:
	virtual ~IModel() = default;

	static IModel* CreateFromFolder(const char* packPath,const char* folderInpack);
	virtual void Release()=0;

	////内部加载文件并调用LoadFromMemFile，当然也可以继承实现自己的文件读取
	//virtual bool LoadFromFile(const char* packPath, const char* filePath) { return false; };
	////让对象自己在文件夹中寻找模型文件. 如Live2D的模型后缀名为.model3.json
	//virtual bool LoadFromFolder(const char* packPath, const char* foldPath) { return false; };

	//virtual bool LoadFromMemFile(unsigned char* mem) = 0;

	virtual void Update(uint64_t deltaTicksNS) = 0;
	virtual void Draw()=0;
	virtual void DrawMix(MixDrawList* pMix) {};//不一定所有模型都支持混合绘制，所以这里不纯虚

	//xy暂时定位模型空间中的坐标
	virtual bool Hit(float x, float y) { return false; };



	//对于Live2D，组名即动画名，播放动画时在组里随机播放
	//好像暂时不需要设置动画轨道（即动画混合）
	//模型自身管理动画轨道“Track”
	virtual void PlayAnimation(const std::string& name, bool loop=false)=0;
	virtual ParamHandle GetParamHandle(const std::string& param)=0;
	virtual void SetParamValue(ParamHandle param,float value,bool normallizeValue=true,bool longTerm=false)=0;//未设置长期longTerm的话，修改只对当前帧有效
	virtual void AddParamValue(ParamHandle param,float value, bool normallizeValue = true,bool longTerm = false)=0;
	virtual void MultiplyParamValue(ParamHandle param,float value,bool longTerm = false)=0;
	
	//从模型中获取某个网格的中点的位置，要求Y坐标范围为-1~1 y轴向上
	virtual void GetHandPosFromHandle(HandPosHandle handPosHandle,float* x,float* y) {};
	virtual HandPosHandle GetHandHandle(const std::string& param) { return INVALID_HANDHANDLE; };


	//获取所有软件可控参数
	virtual std::vector<std::string> GetParamList() { return std::vector<std::string>(); };
	//获取的是CatTuber可控的动画
	virtual std::vector<std::string> GetAnimationList() { return std::vector<std::string>(); };



	//其他接口
	Scene* GetScene() { return _scene; }
	void SetScene(Scene* scene) { _scene = scene; };
protected:
	Scene* _scene;
};







#endif