#ifndef _DecorationItem_h
#define _DecorationItem_h
#include"SceneItem.h"
#include<glm/glm.hpp>

//经典CatTuber组合 角色桌子鼠标
class DecorationObject;
class DecorationItem :public ISceneItem
{
	//
public:
	virtual ~DecorationItem() { Reset(); };
	virtual const char* GetType() override { return _GetType(); };
	static const char* _GetType() { return "DecorationItem"; };


	virtual void Update(uint64_t deltaTicksNS)override;
	//virtual void Update_Late(uint64_t deltaTicksNS)override;
	//virtual void Draw(SDL_GPUTexture* renderTarget, SDL_GPUTexture* depth, int width, int height, SDL_GPUCommandBuffer* mainCmdBuffer, SDL_GPUCommandBuffer* copyCmdBuffer)override;
	virtual void Draw(SDL_GPURenderPass*, int width, int height, SDL_GPUCommandBuffer* mainCmdBuffer, SDL_GPUCommandBuffer* copyCmdBuffer)override;
	virtual void DrawMix(MixDrawList* mix);
	virtual void OnLoopEnd()override;

	virtual Json::Value GenerateAttributes()override;
	virtual void ApplyAttributes(const Json::Value& applyJson)override;


	void ResetObj(DecorationObject* obj);
	void SetTransform(float posX, float posY, float posZ);
	void SetScale(float scale);


	DecorationObject* GetObj() { return _itemObj; };
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


	DecorationObject* _itemObj;


};


#endif