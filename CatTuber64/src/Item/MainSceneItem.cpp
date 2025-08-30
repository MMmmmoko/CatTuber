

#include"Item/MainSceneItem.h"

#include"Item/TableObject.h"




void ClassicItem::Update(uint64_t deltaTicksNS)
{
	//TODO 处理“预览”
	//先刷新桌子
	if(_table)
		_table->Update(deltaTicksNS);
	//if(_character)
	//	_character->Update(deltatime);
	//if(_handHeldItem)
	//	_handHeldItem->Update(deltatime);
}

void ClassicItem::Draw(SDL_GPUTexture* renderTarget, SDL_GPUTexture* depth, int width, int height, SDL_GPUCommandBuffer* mainCmdBuffer, SDL_GPUCommandBuffer* copyCmdBuffer)
{


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
	//if (_character)
	//{
	//	json["Character"] = _character->GenerateAttributes();
	//}
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
		offsetX = applyJson["Transform"]["Offset"][0].isDouble()?static_cast<float>(applyJson["Transform"]["Offset"][0].asDouble()):0.f;
		offsetY = applyJson["Transform"]["Offset"][1].isDouble()?static_cast<float>(applyJson["Transform"]["Offset"][1].asDouble()) : 0.f;
		offsetZ = applyJson["Transform"]["Offset"][2].isDouble()?static_cast<float>(applyJson["Transform"]["Offset"][2].asDouble()) : 0.f;
	}
	if (applyJson.isMember("Transform") && applyJson["Transform"].isMember("Scale") && applyJson["Transform"]["Scale"].isDouble())
	{
		scale = static_cast<float>(applyJson["Transform"]["Scale"].asDouble());
	}




	if (applyJson.isMember("Table"))
	{
		_table = TableObject::CreateFromAttributes(applyJson["Table"]);
	}
	//if (applyJson.isMember("Character"))
	//{
	//	_character = CharacterObject::CreateFromAttributes(applyJson["Character"]);
	//}
	//if (applyJson.isMember("HandheldItem"))
	//{
	//	_handHeldItem = HandheldItemObject::CreateFromAttributes(applyJson["HandheldItem"]);
	//}








}


void ClassicItem::Reset()
{
	offsetX = 0.f;
	offsetY = 0.f;
	offsetZ = 0.f;
	scale = 1.f;

	if (_table)
	{
		delete _table;
		_table = nullptr;
	}
	//TODO/FIXME
	//if (_character)
	//{
	//	delete _character;
	//	_character = nullptr;
	//}
	//if (_handHeldItem)
	//{
	//	delete _handHeldItem;
	//	_handHeldItem = nullptr;
	//}

}