#ifndef _MainSceneItem_h
#define _MainSceneItem_h



#include"SceneItem.h"

//场景主物品   为角色桌子鼠标组 或者独立模型（一体化模型IntegralItem） 或者Bongo Cat



class MainSceneItem :public ISceneItem
{
	

};






//经典CatTuber组合 角色桌子鼠标
class TableObject;
class CharacterObject;
class HandheldItemObject;


class ClassicItem:public MainSceneItem
{
	//
public:
	virtual ~ClassicItem() { Reset(); };
	virtual const char* GetType() override { return _GetType(); };
	static const char* _GetType() { return "ClassicItem"; };


	virtual void Update(uint64_t deltaTicksNS)override;
	//virtual void Draw(SDL_GPUTexture* renderTarget, SDL_GPUTexture* depth, int width, int height, SDL_GPUCommandBuffer* mainCmdBuffer, SDL_GPUCommandBuffer* copyCmdBuffer)override;
	virtual void Draw(SDL_GPURenderPass*, int width, int height, SDL_GPUCommandBuffer* mainCmdBuffer, SDL_GPUCommandBuffer* copyCmdBuffer)override;
	virtual void DrawMix(MixDrawList* mix);


	virtual Json::Value GenerateAttributes()override;
	virtual void ApplyAttributes(const Json::Value& applyJson)override;


	//预览功能包括在Item内
	
private:
	//重置并释放资源
	void Reset();

	//变换参数
	float offsetX=0.f;
	float offsetY=0.f;
	float offsetZ=0.f;//3d预留？
	float scale=1.f;

	TableObject* _table = nullptr;
	CharacterObject* _character = nullptr;
	//QUESTION 手持物品到底有没有必要与桌子分开设置？
	HandheldItemObject* _handHeldItem = nullptr;

	MixDrawList mixDraw;//仅内部进行混合绘制?


};










#endif