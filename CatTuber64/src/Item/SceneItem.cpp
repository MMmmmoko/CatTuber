#include"Item/SceneItem.h"
#include"Util/Util.h"
#include"Item/MainSceneItem.h"

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
	//

	

	return nullptr;
}


void ISceneItem::FreeItem(ISceneItem* item)
{
	if (item)delete item;
}