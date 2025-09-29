

#include <glm/gtc/matrix_transform.hpp>
#include"Item/Scene.h"
#include"Item/MainSceneItem.h"
#include"Item/ClassicItem.h"
#include"Item/TableObject.h"
#include"Item/CharacterObject.h"


void ClassicItem::Update(uint64_t deltaTicksNS)
{
	//TODO 处理“预览”
	//先刷新桌子
	if (_table)
	{
		_table->_pParentItem = this;
		_table->Update(deltaTicksNS);
	}
	if (_character)
	{
		_character->_pParentItem = this;
		_character->Update(deltaTicksNS);
	}
	//if(_handHeldItem)
	//	_handHeldItem->Update(deltatime);
}



//void ClassicItem::Draw(SDL_GPUTexture* renderTarget, SDL_GPUTexture* depth, int width, int height, SDL_GPUCommandBuffer* mainCmdBuffer, SDL_GPUCommandBuffer* copyCmdBuffer)
void ClassicItem::Draw(SDL_GPURenderPass* mainRenderPass, int width, int height, SDL_GPUCommandBuffer* mainCmdBuffer, SDL_GPUCommandBuffer* copyCmdBuffer)
{
	auto& _2dMat = GetScene()->Get2DProj();


	if (_table)
	{
		_table->GetModel()->SetScene(scene);
		_table->Draw(&mixDraw);
	}

	//已确认添加角色模型后桌子绘制出现问题是因为SDL每个slot、每个管线阶段只申请一块uniform buffer，
	//这个uniform大小为32468字节，并且数据以256字节对其
	//每次push uniform data即向uniform buffer的末尾添加新的数据
	//这导致在128个图层push了unifordata后，uniform buffer数据已满
	//第129个图层会创建一个新的uniform buffer，这导致了数据丢失，第1个图层会使用第129图层的数据
	//在这次出现的Shift框体消失的问题即是这种情况
	// 
	//因此 Live2D的每图层一个用于着色器的常数缓存不适合使用SDL uniform buffer
	// （图层会轻易上数百层， uniform buffer甚至会覆写两三次）
	//SDL UNIFORM buffer可能适合管理更少绑定次数的数据
	//
	// 将Live2D的缓存改用正常的常数缓存并自己管理.
	//
	if (_character)
	{
		_character->GetModel()->SetScene(scene);
		_character->Draw(&mixDraw);
	}


	mixDraw.DoDraw(mainCmdBuffer, mainRenderPass);

}
void ClassicItem::DrawMix(MixDrawList* mix)
{
	if (_table)
	{
		_table->Draw(mix);
	}
	if (_character)
	{
		_table->Draw(mix);
	}
}
void ClassicItem::OnLoopEnd()
{
	if (_table)
	{
		_table->OnLoopEnd();
	}
	//character目前没有loopend的方法
	//if (_character)
	//{
	//	_character->OnLoopEnd();
	//}
}



Json::Value ClassicItem::GenerateAttributes()
{
	Json::Value json;

	json["Transform"]["Offset"][0] = offsetX;
	json["Transform"]["Offset"][1] = offsetY;
	json["Transform"]["Scale"] = scale;

	if (_table)
	{
		json["Table"] = _table->GenerateAttributes();
	}
	if (_character)
	{
		json["Character"] = _character->GenerateAttributes();
	}
	//if (_handHeldItem)
	//{
	//	json["HandheldItem"] = _handHeldItem->GenerateAttributes();
	//}
	return json;
}

void ClassicItem::ApplyAttributes(const Json::Value& applyJson)
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




	if (applyJson.isMember("Table"))
	{
		_table = TableObject::CreateFromAttributes(applyJson["Table"]);
	}
	if (applyJson.isMember("Character"))
	{
		_character = CharacterObject::CreateFromAttributes(applyJson["Character"]);
	}
	//if (applyJson.isMember("HandheldItem"))
	//{
	//	_handHeldItem = HandheldItemObject::CreateFromAttributes(applyJson["HandheldItem"]);
	//}








}



void ClassicItem::_GenerateTransformMat()
{
	// 单位矩阵
	glm::mat4x4 transform = glm::mat4x4(1.0f);
	transform=glm::translate(transform,glm::vec3(offsetX, offsetY, offsetZ));
	transform=glm::scale(transform,glm::vec3(scale, scale, scale));
}

void ClassicItem::SetTransform(float posX, float posY, float posZ)
{
	if (offsetX != posX || offsetY != posY || offsetZ != posZ)
	{
		offsetX = posX;
		offsetY = posY;
		offsetZ = posZ;
		_GenerateTransformMat();
	}





}

void ClassicItem::SetScale(float _scale)
{
	if (scale != _scale)
	{
		scale = _scale;
		_GenerateTransformMat();
	}
}


void ClassicItem::Reset()
{
	offsetX = 0.f;
	offsetY = 0.f;
	offsetZ = 0.f;
	scale = 1.f;

	if (_table)
	{
		TableObject::ReleaseObj(_table);
		_table = nullptr;
	}
	if (_character)
	{
		CharacterObject::ReleaseObj(_character);
		_character = nullptr;
	}

	//TODO/FIXME
	//if (_handHeldItem)
	//{
	//	delete _handHeldItem;
	//	_handHeldItem = nullptr;
	//}

}