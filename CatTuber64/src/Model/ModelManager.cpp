#include"Model/ModelManager.h"



void ModelManager::Update(uint64_t deltaTicksNS)const
{

	for (auto& x : _models)
	{
		x->Update(deltaTicksNS);
	}

}



void ModelManager::Draw()
{










}