#include"AppContext.h"
#include"Pack/Pack.h" 
#include"Util/Util.h"
#include"Input/InputManager.h"
#include"Input/InputParser.h"

#include"Model/Live2DModelBase.h"

#include"Item/Scene.h"
#include"Item/DecorationObject.h"

bool DecorationObject::LoadFromPath(const char* u8PackPath, const Json::Value& bindingJson)
{
	//如果重新加载
	if (_model)
	{
		delete _model;
		_model = NULL;
	}

	//文件完整性检查：需要有Model文件夹和模型描述json
	std::string packPath = u8PackPath;

	Pack pack;
	if (!pack.Open(u8PackPath))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "File Load Faied: %s", u8PackPath);
		return false;
	}

	//检查Model文件夹中是否存在完整模型
	//后续添加spine模型的时候
	if (!_model)
	{
		_model = IModel::CreateFromFolder(u8PackPath, CATTUBER_MODEL_FOLDERDIR);
		if (!_model)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Can not create model from : %s/%s", u8PackPath, CATTUBER_MODEL_FOLDERDIR);
			return false;
		}
	}

	

	//使用json统一存储描述文件，不再采用直接写入包体的形式
	//Desc中包含关上方自定义参数

	Json::Value desc;
	size_t memSize;
	if (pack.IsFileExist(CATTUBER_MODELRESOURCE_DESC_FILENAME))
	{
		uint8_t* jsonMem = pack.LoadFile(CATTUBER_MODELRESOURCE_DESC_FILENAME, &memSize);

		desc = util::BuildJsonFromMem((const char*)jsonMem, memSize);
	}

	bool buttonHandled = false;
	bool axisHandled = false;

	ModelControl::SetUpDefaultControl(desc, _model,NULL, NULL, &modelAnimationVec);
	//如果提供的绑定信息，则直接应用（比如来自程序退出时的自动保存）
	if (!bindingJson.empty())
		ModelControl::SetUpBindingByJson(bindingJson, NULL, NULL, &modelAnimationVec);
	else
		LoadBinding();
	//此时当前应该使用的绑定已经写入各vec中了，向InputManager里注册各个绑定
	ModelControl::ApplyControlBindings(NULL, NULL, &modelAnimationVec);
	resourcePath = u8PackPath;
	working = true;
	return true;
}

void DecorationObject::Update(uint64_t deltaTicksNS)
{
	//处理面捕数据？

	if (working)
	{

		uint64_t curMsTicks = SDL_GetTicks();



		_model->Update(deltaTicksNS);
	}
}


void DecorationObject::Draw(MixDrawList* drawList)
{
	if (working)
	{
		_model->DrawMix(drawList);
	}
}

Json::Value DecorationObject::GenerateAttributes()
{
	if (resourcePath.empty())
	{
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Saving a object with NULL pack path!");
		throw(std::runtime_error("Saving a object with NULL pack path!"));
	}
	Json::Value json;
	json["PackPath"] = resourcePath;
	json["Bindings"] = ModelControl::GenerateJsonBinding(NULL, NULL, &modelAnimationVec);


	//todo/FIXME 补完其他需保存的内容 
	return json;
}

DecorationObject* DecorationObject::CreateFromAttributes(const Json::Value& applyJson)
{
	if (!(applyJson.isMember("PackPath") && applyJson["PackPath"].isString()))
	{
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Create decoration with invalid json! No path info exist.");
		return nullptr;
	}
	auto resultObj = new DecorationObject;

	std::string pathStr = AppContext::ResolvePathToAbsolute(applyJson["PackPath"].asString());
	if (!resultObj->LoadFromPath(pathStr.c_str(), applyJson["Bindings"]))
	{
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Can not create decoration at path: %s.", pathStr.c_str());
		delete resultObj;
		return nullptr;
	}




	return resultObj;
}


DecorationObject* DecorationObject::CreateFromPath(const char* packPath)
{
	auto resultObj = new DecorationObject;
	if (!resultObj->LoadFromPath(packPath))
	{
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Can not create decoration at path: %s.", packPath);
		delete resultObj;
		return nullptr;
	}
	return resultObj;

}


void DecorationObject::ReleaseObj(DecorationObject* obj)
{
	//上面怎么创建，这里就怎么释放
	if (!obj)return;
	obj->working = false;
	//移除绑定
	obj->ClearBinding();

	if (obj->_model)
	{
		obj->_model->Release();
		obj->_model = nullptr;
	}


	delete obj;
}



bool DecorationObject::LoadBindingByName(const char* bindingName)
{

	return ModelControl::LoadBindingByName(resourcePath, bindingName,NULL, NULL, &modelAnimationVec);

}


void DecorationObject::LoadBinding()
{
	if (LoadBindingByName("##Save"))
	{
		return;
	}

	for (auto& x : modelAnimationVec)
	{
		x.binding = x.defaultBinding;
	}
}


void DecorationObject::ClearBinding()
{
	for (auto& x : modelAnimationVec)
	{
		for (auto& y : x.binding)
			y.UnRegisterBinding();
	}


	UnregisterAllActionFunc();
}


void DecorationObject::RegisterAllActionFunc(bool falseToUnregister)
{
	//模型控件相关
	auto& im = InputManager::GetIns();


	{
		ActionCallback animationActionCallBack;
		animationActionCallBack.userData = this;
		animationActionCallBack.callback = [](const char* actionName, float value, void* userData, uint64_t userData2)
			{
				((DecorationObject*)userData)->OnAnimationPlay(UTIL_GETLOW32VALUE(userData2));
			};

		for (int i = 0; i < modelAnimationVec.size(); i++)
		{
			std::string animationActionName = "Decoration.Animation." + std::to_string(i) + ".Start";
			UTIL_SETLOW32VALUE(animationActionCallBack.userData2, i);
			if (falseToUnregister)
				im.RegisterActionCallback(animationActionName.c_str(), animationActionCallBack);
			else
				im.UnregisterActionCallback(animationActionName.c_str(), animationActionCallBack);

		}
	}
}

void DecorationObject::UnregisterAllActionFunc()
{
	RegisterAllActionFunc(false);
}

void DecorationObject::OnAnimationPlay(int animationIndex)
{
	if (animationIndex >= modelAnimationVec.size())return;

	if (!modelAnimationVec[animationIndex].controlName.empty())
	{
		_model->PlayAnimation(modelAnimationVec[animationIndex].controlName.c_str());
	}


}






