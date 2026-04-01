#include"Item/SceneItem.h"
#include"Util/Util.h"
#include"Item/MainSceneItem.h"
#include"Item/ClassicItem.h"
#include"Item/BongoCatItem.h"
#include"Item/DecorationItem.h"
ISceneItem* ISceneItem::CreateItem(const char* type, Scene* _scene, const Json::Value& json)
{
	if (UTIL_IS_CSTR_EMPTY(type))
		return nullptr;
	//根据type字符串确认类型
	if (0 == SDL_strcmp(type, ClassicItem::_GetType()))
	{
		ClassicItem* item = new ClassicItem;
		item->ApplyAttributes(json);
		item->scene = _scene;
		return item;
	}
	else if(0 == SDL_strcmp(type, BongoCatItem::_GetType()))
	{
		BongoCatItem* item = new BongoCatItem;
		item->ApplyAttributes(json);
		item->scene = _scene;
		return item;
	}
	else if(0 == SDL_strcmp(type, DecorationItem::_GetType()))
	{
		DecorationItem* item = new DecorationItem;
		item->ApplyAttributes(json);
		item->scene = _scene;
		return item;
	}


	//

	

	return nullptr;
}


void ISceneItem::FreeItem(ISceneItem* item)
{
	if (item)delete item;
}