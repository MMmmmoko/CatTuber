#ifndef  ModelManager_H
#define ModelManager_H


//#include<memory>
#include<vector>
#include<map>

#include"IModel.h"

//模型管理器
//一个场景中模型生成、销毁、点击事件、更新、渲染


class ModelManager
{

public:
	//被打上释放标记的模型
	struct ReleaseModel
	{
		ReleaseModel()
		{
			//_model
		}
	};
	

	void ShutDown();

	void Update(uint64_t deltaTicksNS) const;

	void Draw();

	void EndFrame();


private:
	std::vector<IModel*> _models;
};














#endif // ! ModelManager_H
