#ifndef _SceneItem_h
#define _SceneItem_h

#include <SDL3/SDL.h>
#include"Renderer/MixDrawList.h"
#include"json/json.h"
#include<iostream>
//场景中的物件



class ISceneItem
{
	friend class Scene;
public:
	virtual ~ISceneItem() = default;

	static ISceneItem* CreateItem(const char* type,Scene* scene,const Json::Value& json= Json::Value());
	static void FreeItem(ISceneItem* item);

	//核心功能
	virtual void Update(uint64_t deltaTicksNS)=0;
	virtual void Update_Late(uint64_t deltaTicksNS) {};
	//virtual void Draw(SDL_GPUTexture* renderTarget, SDL_GPUTexture* depth, int width, int height, SDL_GPUCommandBuffer* mainCmdBuffer, SDL_GPUCommandBuffer* copyCmdBuffer)=0;
	virtual void Draw(SDL_GPURenderPass* mainPass, int width, int height, SDL_GPUCommandBuffer* mainCmdBuffer, SDL_GPUCommandBuffer* copyCmdBuffer)=0;
	virtual void DrawMix(MixDrawList* mix) {};
	virtual void Draw_Late(SDL_GPURenderPass* mainPass, int width, int height, SDL_GPUCommandBuffer* mainCmdBuffer, SDL_GPUCommandBuffer* copyCmdBuffer) {}//循环的末尾调用(绘制后)
	virtual void OnLoopEnd() {};//循环的末尾调用

	//其他信息
	//GetType将用于工厂创建ISceneItem
	virtual const char* GetType() = 0;
	virtual Json::Value GenerateAttributes() = 0;
	virtual void ApplyAttributes(const Json::Value& applyJson) = 0;

	virtual void OnCanvasSizeChange(int newWitdh, int newHeight) {};
	//virtual Json::Value GetSettingPage() = 0;......




	
	Scene* GetScene() { return scene; }
protected:
	Scene* scene;
};




















#endif