

#include <glm/gtc/matrix_transform.hpp>
#include"Item/Scene.h"
#include"Item/MainSceneItem.h"
#include"Item/BongoCatItem.h"
#include"Item/BongoCatObject.h"


void BongoCatItem::Update(uint64_t deltaTicksNS)
{


}



//void BongoCatItem::Draw(SDL_GPUTexture* renderTarget, SDL_GPUTexture* depth, int width, int height, SDL_GPUCommandBuffer* mainCmdBuffer, SDL_GPUCommandBuffer* copyCmdBuffer)
void BongoCatItem::Draw(SDL_GPURenderPass* mainRenderPass, int width, int height, SDL_GPUCommandBuffer* mainCmdBuffer, SDL_GPUCommandBuffer* copyCmdBuffer)
{
	auto& _2dMat = GetScene()->Get2DProj();

	if (_itemObj)
	{
		_itemObj->GetModel()->SetScene(scene);
		_itemObj->Draw();
	}

}
void BongoCatItem::DrawMix(MixDrawList* mix)
{
	
}
void BongoCatItem::OnLoopEnd()
{

}



Json::Value BongoCatItem::GenerateAttributes()
{
	Json::Value json;

	json["Transform"]["Offset"][0] = offsetX;
	json["Transform"]["Offset"][1] = offsetY;
	json["Transform"]["Scale"] = scale;


	return json;
}

void BongoCatItem::ApplyAttributes(const Json::Value& applyJson)
{

	Reset();

	if (applyJson.isMember("Transform") && applyJson["Transform"].isMember("Offset")
		&& applyJson["Transform"]["Offset"].isArray() && applyJson["Transform"]["Offset"].size() >= 2)
	{
		offsetX = applyJson["Transform"]["Offset"][0].isDouble() ? static_cast<float>(applyJson["Transform"]["Offset"][0].asDouble()) : 0.f;
		offsetY = applyJson["Transform"]["Offset"][1].isDouble() ? static_cast<float>(applyJson["Transform"]["Offset"][1].asDouble()) : 0.f;
		offsetZ = applyJson["Transform"]["Offset"][2].isDouble() ? static_cast<float>(applyJson["Transform"]["Offset"][2].asDouble()) : 0.f;
	}
	if (applyJson.isMember("Transform") && applyJson["Transform"].isMember("Scale") && applyJson["Transform"]["Scale"].isDouble())
	{
		scale = static_cast<float>(applyJson["Transform"]["Scale"].asDouble());
	}


	if (applyJson.isMember("Object"))
	{
		_itemObj= BongoCatObject::CreateFromAttributes(applyJson["Object"]);
	}






}



void BongoCatItem::_GenerateTransformMat()
{
	// 单位矩阵
	glm::mat4x4 transform = glm::mat4x4(1.0f);
	transform=glm::translate(transform,glm::vec3(offsetX, offsetY, offsetZ));
	transform=glm::scale(transform,glm::vec3(scale, scale, scale));
}

void BongoCatItem::SetTransform(float posX, float posY, float posZ)
{
	if (offsetX != posX || offsetY != posY || offsetZ != posZ)
	{
		offsetX = posX;
		offsetY = posY;
		offsetZ = posZ;
		_GenerateTransformMat();
	}





}

void BongoCatItem::SetScale(float _scale)
{
	if (scale != _scale)
	{
		scale = _scale;
		_GenerateTransformMat();
	}
}


void BongoCatItem::Reset()
{
	offsetX = 0.f;
	offsetY = 0.f;
	offsetZ = 0.f;
	scale = 1.f;


}