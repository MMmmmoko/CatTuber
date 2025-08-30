#ifndef _Scene_h
#define _Scene_h




//CatTuber每个窗口一个场景
//场景中有一些固定槽位（角色 桌子 手持物品）和其他槽位（装饰品）
#include"Model/IModel.h"
#include"json/json.h"
#include<vector>
class TableObject;
class CharacterObject;
class HandheldItemObject;
class Decorationbject;


class ISceneItem;
class MainSceneItem;
//class MainItem;// 角色桌子鼠标的组合模型  、独立模型，Bongo Cat
//QUESTION采用全局Mix吗
class MixDrawList;//用混合绘制技术将多个Live2D模型或其他模型一起绘制。
//控制场景内容并直接与UI层接触
//直接与被绘制的对象接触
class Scene
{
public:
	//不要执行任何与GPU渲染相关的工作？
	void Update(uint64_t deltaTicksNS);
	//QUESTION 未来的3D相机需要作为Draw函数的参数进行传递吗
	//Clear在其他地方进行（Clear是窗口的工作），这里只需要专注往目标上绘制内容
	void Draw(SDL_GPUTexture* renderTarget, SDL_GPUTexture* depth,int width,int height,SDL_GPUCommandBuffer* mainCmdBuffer, SDL_GPUCommandBuffer* copyCmdBuffer=NULL);




	//保存或载入当前场景内容
	Json::Value GenerateAttributes();
	void ApplyAttributes(const Json::Value& jsonvalue);


	 


	//完全清理资源
	void Reset();
	//这个函数只会在帧绘制完时重建缓冲区的时候才调用(渲染线程中)
	void SetCanvasSize(int witdh, int height);




private:
	//3D Camera
	int canvasW=0;
	int canvasH=0;

	MainSceneItem* _mainItem=NULL;
	//UI界面中物品名为“decoration（装饰）”
	std::vector<ISceneItem*> _itemList;
	//MixDrawList* _mixDraw;//先把目前的软件跑起来再搞定Mix

};



















#endif
