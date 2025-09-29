#ifndef _ClassicItem_h
#define _ClassicItem_h
#include"MainSceneItem.h"
#include<glm/glm.hpp>

//经典CatTuber组合 角色桌子鼠标
class TableObject;
class CharacterObject;
class HandheldItemObject;
class ClassicItem :public MainSceneItem
{
	//
public:
	virtual ~ClassicItem() { Reset(); };
	virtual const char* GetType() override { return _GetType(); };
	static const char* _GetType() { return "ClassicItem"; };


	virtual void Update(uint64_t deltaTicksNS)override;
	//virtual void Update_Late(uint64_t deltaTicksNS)override;
	//virtual void Draw(SDL_GPUTexture* renderTarget, SDL_GPUTexture* depth, int width, int height, SDL_GPUCommandBuffer* mainCmdBuffer, SDL_GPUCommandBuffer* copyCmdBuffer)override;
	virtual void Draw(SDL_GPURenderPass*, int width, int height, SDL_GPUCommandBuffer* mainCmdBuffer, SDL_GPUCommandBuffer* copyCmdBuffer)override;
	virtual void DrawMix(MixDrawList* mix);
	virtual void OnLoopEnd()override;

	virtual Json::Value GenerateAttributes()override;
	virtual void ApplyAttributes(const Json::Value& applyJson)override;


	void SetTransform(float posX, float posY, float posZ);
	void SetScale(float scale);

	TableObject* GetTable() { return _table; };
	CharacterObject* GetCharacter() { return _character; };
	HandheldItemObject* GetHandheldItem() { return _handHeldItem; };


private:
	//重置并释放资源
	void Reset();

	//变换参数
	void _GenerateTransformMat();
	float offsetX = 0.f;
	float offsetY = 0.f;
	float offsetZ = 0.f;//3d预留？
	float scale = 1.f;
	//对应的矩阵
	glm::mat4x4 _translatitonMat;



	TableObject* _table = nullptr;
	CharacterObject* _character = nullptr;
	//QUESTION 手持物品到底有没有必要与桌子分开设置？
	HandheldItemObject* _handHeldItem = nullptr;

	MixDrawList mixDraw;//仅内部进行混合绘制?


};


#endif