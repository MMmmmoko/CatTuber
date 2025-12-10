#ifndef _HandheldItemObject_h
#define _HandheldItemObject_h





#include"json/json.h"
#include"Model/IModel.h"
#include"Item/ModelControl.h"
#include"MeshMapping.h"
class MixDrawList;



class HandheldItemObject
{
	friend class ClassicItem;

public:
	bool LoadFromPath(const char* u8PackPath, const Json::Value& bindingJson = Json::Value());//入参应该是个文件夹或者资源包
	const char* GetPackPath() { return resourcePath.c_str(); };
	IModel* GetModel() { return _model; };


	void Update(uint64_t deltaTicksNS);
	void Draw(MixDrawList* drawList);
	void OnLoopEnd();//一些清理和状态重置工作


	//有时希望不显示模型让模型不工作，但暂时不希望删除模型（如预览其他模式时），
// 所以使用这个方法设置停止工作的标识
	void SetWorkingFlag(bool bWorking) { working = bWorking; };


	virtual Json::Value GenerateAttributes();
	//如果出现问题返回空
	static HandheldItemObject* CreateFromAttributes(const Json::Value& applyJson);
	static void ReleaseObj(HandheldItemObject*);//Release需要和Create搭配，免得其他人不知道怎么正确释放资源

	const std::vector<ModelButtonControl>& GetModelButtons() { return modelButtonVec; };
	const std::vector<ModelAxisControl>& GetModelAxes() { return modelAxisVec; };
	const std::vector<ModelAnimationControl>& GetModelAnimations() { return modelAnimationVec; };


	bool LoadBindingByName(const char* bindingName);//读取手动保存的绑定
	void LoadBinding();//先尝试读取被App保存的用户设置的绑定，然后尝试读取Pack文件中缓存的绑定，如果没有则根据模型参数名生成绑定
	void ClearBinding();//模型卸载的时候移除绑定的回调//预览其他模型时不需要这个，因为有个working判断，仅在删除此对象时调用

	//与角色模型需要设置手的位置对应、手持物模型也需要来自桌子的数据
	void SetPosition(float x, float y);
	bool IsAntButtonPushed();
private:


	//注册按钮、轴等数据，即将具体函数绑定到对应Action上
	void RegisterAllActionFunc(bool falseToUnregister = true);
	void UnregisterAllActionFunc();//Action与实际成员函数的绑定


	void OnButtonDown(int btnIndex);//控制参数的同时还要触发按钮上绑定的动画//发送一个TableButtonDown的Action（当Event用）
	void OnButtonUp(int btnIndex);//发送一个TableButtonUp的Action（当Event用）
	void OnAxisValueChange(int axisGroupIndex, int axisIndex, float value);//控制对应参数
	void OnAnimationPlay(int animationIndex);
	void _UpdateAxisVec();

	//模型一些软件参数
	float offsetX = 0.f;
	float offsetY = 0.f;
	float scale = 1.f;

	bool working = false;
	IModel* _model = NULL;
	ClassicItem* _pParentItem = NULL;
	std::string resourcePath;

	//模型控件
	std::vector<ModelButtonControl> modelButtonVec;
	std::vector<ModelAxisControl> modelAxisVec;
	std::vector<ModelAnimationControl> modelAnimationVec;







};














#endif
