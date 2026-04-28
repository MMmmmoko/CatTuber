#ifndef _DecorationObject_h
#define _DecorationObject_h


//角色对象
//需要接收面捕输入

#include"json/json.h"
#include"Model/IModel.h"
#include"Item/ModelControl.h"
#include"Item/StraightMovingPoint.h"
#include"MeshMapping.h"
class MixDrawList;

class DecorationObject
{

public:
	bool LoadFromPath(const char* u8PackPath, const Json::Value& bindingJson = Json::Value());//入参应该是个文件夹或者资源包
	const char* GetPackPath() { return resourcePath.c_str(); };
	IModel* GetModel() { return _model; };


	void Update(uint64_t deltaTicksNS);
	void Draw(MixDrawList* drawList);


	//有时希望不显示模型让模型不工作，但暂时不希望删除模型（如预览其他模式时），
	// 所以使用这个方法设置停止工作的标识
	void SetWorkingFlag(bool bWorking) { working = bWorking; };


	virtual Json::Value GenerateAttributes();
	//如果出现问题返回空
	static DecorationObject* CreateFromAttributes(const Json::Value& applyJson);
	static DecorationObject* CreateFromPath(const char* packPath);
	static void ReleaseObj(DecorationObject*);//Release需要和Create搭配，免得其他人不知道怎么正确释放资源



	const std::vector<ModelAnimationControl>& GetModelAnimations() { return modelAnimationVec; };

	bool LoadBindingByName(const char* bindingName);//读取手动保存的绑定
	void LoadBinding();//先尝试读取被App保存的用户设置的绑定，然后尝试读取Pack文件中缓存的绑定，如果没有则根据模型参数名生成绑定
	void ClearBinding();//模型卸载的时候移除绑定的回调//预览其他模型时不需要这个，因为有个working判断，仅在删除此对象时调用


	void SetHandPosition(int handindex,bool bPress, float x, float y);//bPress为false时表示没有数据，手应该抬起了

	
private:


	//注册按钮、轴等数据，即将具体函数绑定到对应Action上
	void RegisterAllActionFunc(bool falseToUnregister = true);
	void UnregisterAllActionFunc();//Action与实际成员函数的绑定

	void OnAnimationPlay(int animationIndex);



	//模型一些软件参数
	float offsetX = 0.f;
	float offsetY = 0.f;
	float scale = 1.f;

	bool working = false;
	IModel* _model = NULL;
	class DecorationItem* _pParentItem = NULL;
	std::string resourcePath;

	//模型控件
	std::vector<ModelAnimationControl> modelAnimationVec;



};










#endif










