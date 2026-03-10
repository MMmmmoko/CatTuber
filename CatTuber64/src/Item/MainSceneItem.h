#ifndef _MainSceneItem_h
#define _MainSceneItem_h



#include"SceneItem.h"

//场景主物品   为角色桌子鼠标组 或者独立模型（一体化模型IntegralItem） 或者Bongo Cat



class MainSceneItem :public ISceneItem
{
public:
	enum MainSceneItemType
	{
		MainSceneItemType_Classic,
		MainSceneItemType_IntegralModel,
		MainSceneItemType_BongoCat
	};

	virtual MainSceneItemType GetMainItemType()=0;



};

















#endif