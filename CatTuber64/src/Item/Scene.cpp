
#include<SDL3/SDL.h>
#include <glm/gtc/matrix_transform.hpp>

#include"Item/SceneItem.h"
#include"Item/MainSceneItem.h"
#include"Item/Scene.h"


void Scene::Update(uint64_t deltaTicksNS)
{
	//update camera

	//QUESTION:线程池分发各物件的Update？


	for (auto& sceneItem: _itemList)
	{
		sceneItem->Update(deltaTicksNS);
	}

}


//void Scene::Draw(SDL_GPUTexture* renderTarget, SDL_GPUTexture* depth, int width, int height, SDL_GPUCommandBuffer* mainCmdBuffer, SDL_GPUCommandBuffer* copyCmdBuffer)
void Scene::Draw(SDL_GPURenderPass* mainRenderPass, int width, int height, SDL_GPUCommandBuffer* mainCmdBuffer, SDL_GPUCommandBuffer* copyCmdBuffer)
{

	//3D PROJ




	for (auto& sceneItem : _itemList)
	{
		//sceneItem->Draw(renderTarget,depth, width, height, mainCmdBuffer, copyCmdBuffer);
		sceneItem->Draw(mainRenderPass, width, height, mainCmdBuffer, copyCmdBuffer);

	}

}

Json::Value Scene::GenerateAttributes()
{
	//有线程风险吗？删除物品的时候保存？
	Json::Value json;

	//TODO 3D Camera加入后这里需要写入3D Camera的信息


	for (int i = 0; i < _itemList.size(); i++)
	{
		auto& itemJson = json["Items"][i];
		itemJson["Type"] = _itemList[i]->GetType();
		//itemJson["Name"]如果考虑简化的话，应该不需要让用户管理名字
		//_itemList[i]->
		itemJson["Detail"] = _itemList[i]->GenerateAttributes();
	}


	return json;
}

void Scene::ApplyAttributes(const Json::Value& jsonvalue)
{
	//OPTIMIZE:检查是否有同名同资源的对象，让它自己决定是否有必要清理重建资源
	// （可能这种对象重置参数会更快，但载入场景并不是时间敏感的地方，所以未来优化的优先级很低）
	Reset();
	
	if (jsonvalue.isMember("Items") && jsonvalue["Items"].isArray())
	{
		for (unsigned int i = 0; i < jsonvalue["Items"].size(); i++)
		{
			auto& curJson = jsonvalue["Items"][i];
			if (curJson.isMember("Type") && curJson["Type"].isString())
			{
				ISceneItem* pitem = NULL;
				bool isMainItem = (curJson["Type"].asString() == "ClassicItem"
					|| curJson["Type"].asString() == "IntegralItem"
					|| curJson["Type"].asString() == "BongoCatItem"
					);
				if (curJson.isMember("Detail"))
				{
					pitem = ISceneItem::CreateItem(curJson["Type"].asString().c_str(),this, curJson["Detail"]);
				}
				else
				{
					pitem= ISceneItem::CreateItem(curJson["Type"].asString().c_str(),this );
				}

				if (pitem && _mainItem && isMainItem)
				{
					SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION,"Can not create Multiple MainItem!");
					ISceneItem::FreeItem(pitem);
					pitem = NULL;
				}



				if (pitem) 
				{
					if (isMainItem)_mainItem = dynamic_cast<MainSceneItem*>(pitem);
					_itemList.push_back(pitem);
				}
			}
		}
	}




}


void Scene::Reset()
{
	//仅在渲染线程调用
	for (auto& x: _itemList)
	{
		ISceneItem::FreeItem(x);
		x = NULL;
	}
	_itemList.clear();
	_mainItem = NULL;




}

void Scene::SetCanvasSize(int width, int height)
{
	if (canvasW == width && canvasH == height)
		return;
	canvasW = width;
	canvasH = height;
	//TODO/FIXME 新增3d相机后这里应该这是3D相机

	//更新PROJ
	float viewHeight = 2.0f;     // 你想看到的世界高度
	float aspect = static_cast<float>(width) / static_cast<float>(height);
	float viewWidth = viewHeight * aspect;
	proj2D = glm::orthoLH_ZO(
		-viewWidth / 2.0f, viewWidth / 2.0f,  // left, right
		-viewHeight / 2.0f, viewHeight / 2.0f, // bottom, top
		0.1f, 100.0f                           // near, far
	);

	for (auto& x : _itemList)
	{
		x->OnCanvasSizeChange(canvasW, canvasH);
	}
}
