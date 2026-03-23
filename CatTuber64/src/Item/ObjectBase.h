#ifndef _ObjectBase_h
#define _ObjectBase_h



#include"json/json.h"
#include"Model/IModel.h"
#include"Item/ModelControl.h"
#include"Pack/Pack.h"
class ObjectBase
{
public:
	virtual bool LoadFromPath();
	const char* GetPackPath() { return resourcePack.GetPath(); };
	class IModel* GetModel() { return _model; };


	virtual void Update(uint64_t deltaTicksNS);
	virtual void Draw(MixDrawList* drawList);

	//有时希望不显示模型让模型不工作，但暂时不希望删除模型（如预览其他模式时），
// 所以使用这个方法设置停止工作的标识
	void SetWorkingFlag(bool bWorking) { working = bWorking; };


	virtual Json::Value GenerateAttributes();
	//如果出现问题返回空
	static ObjectBase* CreateFromAttributes(const Json::Value& applyJson);




protected:

	bool working = false;
	IModel* _model = NULL;
	Pack resourcePack;


	//模型控件
	std::vector<ModelAnimationControl> modelAnimationVec;


private:




};



































#endif