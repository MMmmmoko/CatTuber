

#include <glm/gtc/matrix_transform.hpp>
#include"Item/Scene.h"
#include"Item/MainSceneItem.h"
#include"Item/DecorationItem.h"
#include"Item/DecorationObject.h"


void DecorationItem::Update(uint64_t deltaTicksNS)
{
	//TODO 处理“预览”
	//先刷新桌子
	if (_itemObj)
	{
		//_desk->_pParentItem = this;
		_itemObj->Update(deltaTicksNS);
	}
}



//void DecorationItem::Draw(SDL_GPUTexture* renderTarget, SDL_GPUTexture* depth, int width, int height, SDL_GPUCommandBuffer* mainCmdBuffer, SDL_GPUCommandBuffer* copyCmdBuffer)
void DecorationItem::Draw(SDL_GPURenderPass* mainRenderPass, int width, int height, SDL_GPUCommandBuffer* mainCmdBuffer, SDL_GPUCommandBuffer* copyCmdBuffer)
{
	auto& _2dMat = GetScene()->Get2DProj();


	if (_itemObj)
	{
		_itemObj->GetModel()->SetScene(scene);
		_itemObj->GetModel()->Draw();
		//_desk->Draw(&mixDraw);
	}



}
void DecorationItem::DrawMix(MixDrawList* mix)
{
	if (_itemObj)
	{
		_itemObj->Draw(mix);
	}
}
void DecorationItem::OnLoopEnd()
{
	if (_itemObj)
	{
		//_itemObj->OnLoopEnd();
	}


}



Json::Value DecorationItem::GenerateAttributes()
{
	Json::Value json;

	json["Transform"]["Offset"][0] = offsetX;
	json["Transform"]["Offset"][1] = offsetY;
	json["Transform"]["Scale"] = scale;

	if (_itemObj)
	{
		json["Object"] = _itemObj->GenerateAttributes();
	}
	return json;
}

void DecorationItem::ApplyAttributes(const Json::Value& applyJson)
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
		_itemObj = DecorationObject::CreateFromAttributes(applyJson["Object"]);
	}





}



void DecorationItem::_GenerateTransformMat()
{
	// 单位矩阵
	glm::mat4x4 transform = glm::mat4x4(1.0f);
	transform=glm::translate(transform,glm::vec3(offsetX, offsetY, offsetZ));
	transform=glm::scale(transform,glm::vec3(scale, scale, scale));
}

void DecorationItem::SetTransform(float posX, float posY, float posZ)
{
	if (offsetX != posX || offsetY != posY || offsetZ != posZ)
	{
		offsetX = posX;
		offsetY = posY;
		offsetZ = posZ;
		_GenerateTransformMat();
	}
}

void DecorationItem::SetScale(float _scale)
{
	if (scale != _scale)
	{
		scale = _scale;
		_GenerateTransformMat();
	}
}


void DecorationItem::Reset()
{
	offsetX = 0.f;
	offsetY = 0.f;
	offsetZ = 0.f;
	scale = 1.f;

	if (_itemObj)
	{
		DecorationObject::ReleaseObj(_itemObj);
		_itemObj = nullptr;
	}
}