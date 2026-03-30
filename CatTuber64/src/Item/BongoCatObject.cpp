#include"AppContext.h"
#include"Util/Util.h"
#include"Input/InputManager.h"
#include"Input/InputParser.h"

#include"Model/Live2DModelBase.h"

#include"Item/Scene.h"
#include"Item/BongoCatObject.h"

BongoCatObject::~BongoCatObject()
{

}

bool BongoCatObject::LoadFromPath(const char* u8PackPath, const Json::Value& bindingJson)
{
	//如果重新加载
	if (_model)
	{
		delete _model;
		_model = NULL;
	}

	//文件完整性检查：需要有Model文件夹和模型描述json
	std::string packPath = u8PackPath;

	//Pack pack;
	if (!pack.Open(u8PackPath))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "File Load Faied: %s", u8PackPath);
		return false;
	}








	//使用json统一存储描述文件，不再采用直接写入包体的形式
	//Desc中包含关上方自定义参数

	Json::Value desc;
	if (pack.IsFileExist(CATTUBER_MODELRESOURCE_DESC_FILENAME))
	{
		size_t memSize;
		uint8_t* jsonMem = pack.LoadFile(CATTUBER_MODELRESOURCE_DESC_FILENAME, &memSize);
		desc = util::BuildJsonFromMem((const char*)jsonMem, memSize);
		pack.ReleaseMem(jsonMem);
	}

	Json::Value config;
	if (pack.IsFileExist("config.json"))
	{
		size_t memSize;
		uint8_t* jsonMem = pack.LoadFile("config.json", &memSize);
		config = util::BuildJsonFromMem((const char*)jsonMem, memSize);
		pack.ReleaseMem(jsonMem);
	}

	//从JSON中读取重要数据


		//模式
	if (desc["Config"]["Mode"].isUInt64())
	{
		auto modeNum = desc["Config"]["Mode"].asUInt64();
		if (modeNum > BongoCatMverMode_MaxCount || modeNum == 0)modeNum = 1;
		mode = static_cast<BongoCatMverMode>(modeNum - 1);
	}
	else if (config["mode"].isUInt64())
	{
		auto modeNum = config["mode"].asUInt64();
		if (modeNum > BongoCatMverMode_MaxCount || modeNum == 0)modeNum = 1;
		mode = static_cast<BongoCatMverMode>(modeNum - 1);
	}

	//是否使用Live2D模型
	if (desc["Config"]["Live2D"].isBool())
	{
		isUsingLive2D = desc["Config"]["Live2D"].asBool();
	}
	else
	{
		switch (mode)
		{
		case BongoCatObject::BongoCatMverMode_Standard:
			isUsingLive2D = config["standard"]["l2d"].isBool() ? config["standard"]["l2d"].asBool() : isUsingLive2D;
			break;
		case BongoCatObject::BongoCatMverMode_Keyboard:
			isUsingLive2D = config["keyboard"]["l2d"].isBool() ? config["keyboard"]["l2d"].asBool() : isUsingLive2D;
			break;
		case BongoCatObject::BongoCatMverMode_Gamepad:
			isUsingLive2D = config["gamepad"]["l2d"].isBool() ? config["gamepad"]["l2d"].asBool() : isUsingLive2D;
			break;
		default:
			break;
		}
	}


	//是否使用Live2D 手进行按键
	if (desc["Config"]["Live2DHand"].isBool())
	{
		isUsingLive2DHand = desc["Config"]["Live2DHand"].asBool();
	}

	//是否使用Live2D桌子
	if (desc["Config"]["Live2DDesk"].isBool())
	{
		isUsingLive2DDesk = desc["Config"]["Live2DDesk"].asBool();
	}

	//是否使用笔
	if (desc["Config"]["Pen"].isBool())
	{
		isUsingPen = desc["Config"]["Pen"].asBool();
	}
	else if (config["standard"]["mouse"].isBool())
	{
		isUsingPen = config["standard"]["mouse"].asBool();
	}



	//基于上述参数加载文件
	switch (mode)
	{
	case BongoCatObject::BongoCatMverMode_Standard:
		return _LoadResource_Standard(config);
		break;
	case BongoCatObject::BongoCatMverMode_Keyboard:
		return _LoadResource_Keyboard(config);
		break;
	case BongoCatObject::BongoCatMverMode_Gamepad:
		return _LoadResource_Gamepad(config);
		break;
	default:
		break;
	}


	const char* modeFolderPath[BongoCatMverMode_MaxCount] = {
"img/standard/",
"img/keyboard/",
"img/gamepad/"
	};
	std::string modeFolder = modeFolderPath[mode];
	//角色模型
	if (isUsingLive2D)
	{
		if (!_model)
		{
			std::string l2dModelFolder = modeFolder + "cat_model";
			_model = IModel::CreateFromFolder(u8PackPath, l2dModelFolder.c_str());
			if (!_model)
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Can not create model from : %s/%s", u8PackPath, l2dModelFolder.c_str());
				return false;
			}
		}
	}
	else
	{
		pngResource.cat;
	}




































	bool buttonHandled = false;
	bool axisHandled = false;

	ModelControl::SetUpDefaultControl(desc, _model, NULL, NULL, &modelAnimationVec);
	//如果提供的绑定信息，则直接应用（比如来自程序退出时的自动保存）
	if (!bindingJson.empty())
		ModelControl::SetUpBindingByJson(bindingJson, NULL, NULL, &modelAnimationVec);
	else
		LoadBinding();
	//此时当前应该使用的绑定已经写入各vec中了，向InputManager里注册各个绑定
	ModelControl::ApplyControlBindings(NULL, NULL, &modelAnimationVec);
	//resourcePath = u8PackPath;
	working = true;
	return true;
}

void BongoCatObject::Update(uint64_t deltaTicksNS)
{
	//处理面捕数据？

	if (working)
	{

		uint64_t curMsTicks = SDL_GetTicks();



		_model->Update(deltaTicksNS);
	}
}


void BongoCatObject::Draw(MixDrawList* drawList)
{
	if (working)
	{
		_model->DrawMix(drawList);
	}
}

Json::Value BongoCatObject::GenerateAttributes()
{
	if (!pack.IsAvaliable())
	{
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Saving a object with NULL pack path!");
		throw(std::runtime_error("Saving a object with NULL pack path!"));
	}
	Json::Value json;
	json["PackPath"] = pack.GetPath();
	json["Bindings"] = ModelControl::GenerateJsonBinding(NULL, NULL, &modelAnimationVec);


	//todo/FIXME 补完其他需保存的内容 
	return json;
}

BongoCatObject* BongoCatObject::CreateFromAttributes(const Json::Value& applyJson)
{
	if (!(applyJson.isMember("PackPath") && applyJson["PackPath"].isString()))
	{
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Create Desk with invalid json! No path info exist.");
		return nullptr;
	}
	auto resultObj = new BongoCatObject;

	std::string pathStr = AppContext::ResolvePathToAbsolute(applyJson["PackPath"].asString());
	if (!resultObj->LoadFromPath(pathStr.c_str(), applyJson["Bindings"]))
	{
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Can not create Desk at path: %s.", pathStr.c_str());
		delete resultObj;
		return nullptr;
	}




	return resultObj;
}

void BongoCatObject::ReleaseObj(BongoCatObject* obj)
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



bool BongoCatObject::LoadBindingByName(const char* bindingName)
{

	return ModelControl::LoadBindingByName(pack.GetPath(), bindingName, NULL, NULL, &modelAnimationVec);

}


void BongoCatObject::LoadBinding()
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


void BongoCatObject::ClearBinding()
{
	for (auto& x : modelAnimationVec)
	{
		for (auto& y : x.binding)
			y.UnRegisterBinding();
	}
}


void BongoCatObject::RegisterAllActionFunc(bool falseToUnregister)
{
	//模型控件相关
	auto& im = InputManager::GetIns();


	{
		ActionCallback animationActionCallBack;
		animationActionCallBack.userData = this;
		animationActionCallBack.callback = [](const char* actionName, float value, void* userData, uint64_t userData2)
			{
				((BongoCatObject*)userData)->OnAnimationPlay(UTIL_GETLOW32VALUE(userData2));
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

void BongoCatObject::UnregisterAllActionFunc()
{
	RegisterAllActionFunc(false);
}

void BongoCatObject::OnAnimationPlay(int animationIndex)
{
	if (animationIndex >= modelAnimationVec.size())return;

	if (!modelAnimationVec[animationIndex].controlName.empty())
	{
		_model->PlayAnimation(modelAnimationVec[animationIndex].controlName.c_str());
	}


}

void BongoCatObject::_ReadKeysFromJsonArray(const Json::Value& jsonvalue, std::vector<std::vector<unsigned char>>& vec)
{
	if (jsonvalue.isArray())
	{
		for (auto& mkey : jsonvalue)
		{
			if (mkey.isArray())
			{
				auto& current = vec.emplace_back();
				for (auto& key : mkey)
				{
					if (key.isUInt())
					{
						current.push_back(key.asUInt());
					}
					else
					{
						break;
					}
				}
			}
			else
			{
				break;
			}
		}
	}
}


bool BongoCatObject::_LoadSprite(const char* pathInPack, BongoCatSprite& sprite)
{
	int w, h;
	SDL_GPUTexture* pTex= util::LoadTextureFromPack(&pack, pathInPack,&w,&h);
	if (!pTex)return false;
	sprite.SetUp(pTex,w,h);
	return true;
}

void BongoCatObject::StartL2DExpression(int index)
{
	//重复激活同一个表情表示取消
	if (index!=0&&currentStates.l2dExpressionIndex == index)
	{
		StopL2DExpression();
		return;
	}
	currentStates.l2dExpressionIndex = index;
	_model->SetExpression(index);
}

void BongoCatObject::StartL2DMotion(int index)
{
	_model->PlayAnimationEX("CAT_motion", index);
}




static void LockHandMotionFinished(void* pLockHand)
{
	*(bool*)pLockHand = false;
}

static void LockHandMotionBegan(void* pLockHand)
{
	*(bool*)pLockHand = true;
}


void BongoCatObject::StartL2DMotion_HideHand(int index)
{
	_model->PlayAnimationEX("CAT_motion_lock", index, 
		LockHandMotionFinished,&currentStates.isLockingHand, 
		LockHandMotionBegan, &currentStates.isLockingHand);
}

void BongoCatObject::StopL2DExpression()
{
	StartL2DExpression(0);
}

void BongoCatObject::StopL2DMotion()
{
	//貌似没有地方会调用这个

}
