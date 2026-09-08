#include"AppContext.h"
#include"Pack/Pack.h" 
#include"Util/Util.h"
#include"Input/InputManager.h"
#include"Input/InputParser.h"

#include"Model/Live2DModelBase.h"

#include"Item/Scene.h"
#include"Item/CharacterObject.h"



#define NEED_SET_OLD_MAPPING_LEFT ((void*)1)
#define NEED_SET_OLD_MAPPING_RIGHT ((void*)2)

bool CharacterObject::LoadFromPath(const char* u8PackPath, const Json::Value& bindingJson)
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


	//读入其他hand参数
	if (desc["Config"]["Hand"].isArray())
	{
		int handIndex = 0;
		for (auto& curHandJson : desc["Config"]["Hand"])
		{
			handIndex++;
			std::string paramX;
			std::string paramY;
			std::string paramZ;
			std::string posTest;
			float defaultPos[3] = {0.f,0.f,0.f};
			float activeHeight = 1.f;

			if (curHandJson["PosTest"].isString())
			{
				posTest = curHandJson["PosTest"].asString();
			}
			else
			{
				if (handIndex == 1&&_model->GetHandHandle("LHandPos")!= INVALID_HANDHANDLE)
				{
					posTest = "LHandPos";
				}
				else if (handIndex == 2 && _model->GetHandHandle("RHandPos") != INVALID_HANDHANDLE)
				{
					posTest = "RHandPos";
				}
				else
				{
					posTest= "Hand"+std::to_string(handIndex)+"Pos";
				}
			}

			if (curHandJson["ParamX"].isString())
				paramX=curHandJson["ParamX"].asString();
			else
			{
				if (handIndex == 1)
				{
					paramX = "LeftHandX";
				}
				else if (handIndex == 2)
				{
					paramX = "RightHandX";
				}
				else
				{
					paramX= "Hand"+std::to_string(handIndex)+"X";
				}
			}
			if (curHandJson["ParamY"].isString())
				paramY=curHandJson["ParamY"].asString();
			else
			{
				if (handIndex == 1)
				{
					paramY = "LeftHandY";
				}
				else if (handIndex == 2)
				{
					paramY = "RightHandY";
				}
				else
				{
					paramY= "Hand"+std::to_string(handIndex)+"Y";
				}
			}
			if (curHandJson["ParamZ"].isString() || curHandJson["ParamUp"].isString())
			{
				if(curHandJson["ParamZ"].isString())
					paramZ = curHandJson["ParamZ"].asString();
				else
					paramZ = curHandJson["ParamUp"].asString();
			}
			else
			{
				if (handIndex == 1)
				{
					paramZ = "LeftHandUp";
				}
				else if (handIndex == 2)
				{
					paramZ = "RightHandUp";
				}
				else
				{
					paramZ= "Hand"+std::to_string(handIndex)+"Up";
				}
			}



			if (curHandJson["DefaultPos"].isArray())
			{
				for (uint32_t i = 0; i < curHandJson["DefaultPos"].size() && i < 3; i++)
				{
					if (curHandJson["DefaultPos"][i].isDouble())
						defaultPos[i] = static_cast<float>( curHandJson["DefaultPos"][i].asDouble());
				}
			}

			if (curHandJson["ActiveHeight"].isDouble())
			{
				activeHeight = static_cast<float>(curHandJson["ActiveHeight"].asFloat());
			}

	

			auto& curHand = hands.emplace_back();
			_SetUpHand(curHand, posTest.c_str(),paramX.c_str(),paramY.c_str(),paramZ.c_str(),
				defaultPos[0],defaultPos[1],defaultPos[2],activeHeight);
		}
	}
	else
	{
		//未设置手数据，使用默认值
		//作为兼容，寻找模型中是否有旧版手参数？
		auto paramVec=_model->GetParamList();
		const char* LeftHandX = nullptr;
		const char* LeftHandY = nullptr;
		const char* LeftHandUp = nullptr;
		const char* RightHandX = nullptr;
		const char* RightHandY = nullptr;
		const char* RightHandUp = nullptr;

		for (auto& x : paramVec)
		{
			if (x == "LeftHandX")
			{
				LeftHandX = x.c_str();
				continue;
			}
			if (x == "CAT_LeftHandX"&& LeftHandX==nullptr)
			{
				LeftHandX = x.c_str();
				continue;
			}
			if (x == "LeftHandY")
			{
				LeftHandY = x.c_str();
				continue;
			}
			if (x == "CAT_LeftHandY"&& LeftHandY==nullptr)
			{
				LeftHandY =  x.c_str();
				continue;
			}
			if (x == "LeftHandUp")
			{
				LeftHandUp = x.c_str();
				continue;
			}
			if (x == "CAT_LeftHandUp"&& LeftHandUp ==nullptr)
			{
				LeftHandUp =  x.c_str();
				continue;
			}

			if (x == "RightHandX")
			{
				RightHandX = x.c_str();
				continue;
			}
			if (x == "CAT_RightHandX"&& RightHandX==nullptr)
			{
				RightHandX = x.c_str();
				continue;
			}
			if (x == "RightHandY")
			{
				RightHandY = x.c_str();
				continue;
			}
			if (x == "CAT_RightHandY"&& RightHandY==nullptr)
			{
				RightHandY =  x.c_str();
				continue;
			}
			if (x == "RightHandUp")
			{
				RightHandUp = x.c_str();
				continue;
			}
			if (x == "CAT_RightHandUp"&& RightHandUp ==nullptr)
			{
				RightHandUp =  x.c_str();
				continue;
			}
		}


		//if (!LeftHandX)LeftHandX = "LeftHandX";
#define __SETUPHANDSTR(X) if(!X)X=#X
		__SETUPHANDSTR(LeftHandX);
		__SETUPHANDSTR(LeftHandY);
		__SETUPHANDSTR(LeftHandUp);
		__SETUPHANDSTR(RightHandX);
		__SETUPHANDSTR(RightHandY);
		__SETUPHANDSTR(RightHandUp);
#undef __SETUPHANDSTR




		auto& leftHand=hands.emplace_back();			
		_SetUpHand(leftHand, "LHandPos", LeftHandX, LeftHandY, LeftHandUp,
			-21.1f/30.f, -22.9f/30.f, 0.f, 0.8f);
		auto& rightHand = hands.emplace_back();
		_SetUpHand(rightHand, "RHandPos", RightHandX, RightHandY, RightHandUp,
			19.3f/30.f, -30.f/30.f, 0.f, 0.8f);
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

void CharacterObject::Update(uint64_t deltaTicksNS)
{
	//处理面捕数据？

	if (working)
	{

		uint64_t curMsTicks = SDL_GetTicks();

		//手
		{
			//设置手的参数
			if (isAnyHandActive)
			{
				//使用MSTICKS  当激活的时间超过了10秒以上将状态设置为恢复态
				if (curMsTicks - activeTimeMS > 10'000)
				{
					for (auto& hand : hands)
					{
						if (hand.handState == HandData::Active)
						{
							if (hand.isDown)
							{
								hand.handState = HandData::Default;
							}
							else
							{
								hand.handState = HandData::Recovering;
								hand.point_active_to_default.SetStartPointAndStartMove(
									hand.point_active_up.GetX(),
									hand.point_active_up.GetY(),
									hand.point_active_up.GetZ()
								);
							}
						}
					}
					isAnyHandActive = false;
				}




			}

			for (auto& hand : hands)
			{
				if (hand.isDown)
				{
					_model->SetParamValue(hand.handParamPosX, hand.x, true);
					_model->SetParamValue(hand.handParamPosY, hand.y, true);
					_model->SetParamValue(hand.handParamPosZ, 0.f, true);
				}
				else
				{
					float dt = static_cast<float>(deltaTicksNS) / 1000'000'000.f;
					switch (hand.handState)
					{
					case HandData::Default:
					{
						hand.point_default_up.Update(dt);
						_model->SetParamValue(hand.handParamPosX, hand.point_default_up.GetX(), true);
						_model->SetParamValue(hand.handParamPosY, hand.point_default_up.GetY(), true);
						_model->SetParamValue(hand.handParamPosZ, hand.point_default_up.GetZ(), true);
						break;
					}
					case HandData::Recovering:
					{
						hand.point_active_to_default.Update(dt);
						_model->SetParamValue(hand.handParamPosX, hand.point_active_to_default.GetX(), true);
						_model->SetParamValue(hand.handParamPosY, hand.point_active_to_default.GetY(), true);
						_model->SetParamValue(hand.handParamPosZ, hand.point_active_to_default.GetZ(), true);
						break;
					}
					case HandData::Active:
					{
						hand.point_active_up.Update(dt);
						_model->SetParamValue(hand.handParamPosX, hand.point_active_up.GetX(), true);
						_model->SetParamValue(hand.handParamPosY, hand.point_active_up.GetY(), true);
						_model->SetParamValue(hand.handParamPosZ, hand.point_active_up.GetZ(), true);
						break;
					}
					default:
						break;
					}




				}


			}
		}
		_model->Update(deltaTicksNS);
	}
}


void CharacterObject::Draw(MixDrawList* drawList)
{
	if (working)
	{
		_model->DrawMix(drawList);
	}
}

Json::Value CharacterObject::GenerateAttributes()
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

CharacterObject* CharacterObject::CreateFromAttributes(const Json::Value& applyJson)
{
	if (!(applyJson.isMember("PackPath") && applyJson["PackPath"].isString()))
	{
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Create Character with invalid json! No path info exist.");
		return nullptr;
	}
	auto resultObj = new CharacterObject;

	std::string pathStr = AppContext::ResolvePathToAbsolute(applyJson["PackPath"].asString());
	if (!resultObj->LoadFromPath(pathStr.c_str(), applyJson["Bindings"]))
	{
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Can not create Character at path: %s.", pathStr.c_str());
		delete resultObj;
		return nullptr;
	}




	return resultObj;
}

CharacterObject* CharacterObject::CreateFromPath(const char* packPath)
{

	auto resultObj = new CharacterObject;

	std::string pathStr = AppContext::ResolvePathToAbsolute(packPath);
	if (!resultObj->LoadFromPath(pathStr.c_str()))
	{
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Can not create Character at path: %s.", pathStr.c_str());
		delete resultObj;
		return nullptr;
	}




	return resultObj;
}



void CharacterObject::ReleaseObj(CharacterObject* obj)
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



bool CharacterObject::LoadBindingByName(const char* bindingName)
{

	return ModelControl::LoadBindingByName(resourcePath, bindingName,NULL, NULL, &modelAnimationVec);

}


void CharacterObject::LoadBinding()
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


void CharacterObject::ClearBinding()
{
	for (auto& x : modelAnimationVec)
	{
		for (auto& y : x.binding)
			y.UnRegisterBinding();
	}

	UnregisterAllActionFunc();
}

void CharacterObject::SetHandPosition(int handIndex, bool bPress, float x, float y)
{
	//设置一侧手的按下情况
	//如果是抬起转按下，则触发handdown ，按下转抬起触发up

	//这个函数是桌子调用的，桌子传输过来的xy是模型空间的坐标数据，需要转换到角色模型的模型空间，然后通过映射计算参数值
	if (handIndex >= hands.size())return;

	float realX = (x - offsetX)/scale;
	float realY= (y - offsetY) /scale;


	auto& currentHand = hands[handIndex];
	if (currentHand.isDown==true && bPress==false)
	{
	//抬起
		_OnHandUp(handIndex);

		return;
	}

	//计算参数值

	if (currentHand.handPosMapping.Valid())
	{

		if (currentHand.isDown == false && bPress == true)
		{
			float paramX = 0.f;
			float paramY = 0.f;
			currentHand.handPosMapping.GetParamValueAtPos(realX, realY, &paramX, &paramY);
			_OnHandDown(handIndex, paramX, paramY);
			return;
		}
		if (bPress)
		{
			float paramX = 0.f;
			float paramY = 0.f;
			currentHand.handPosMapping.GetParamValueAtPos(realX, realY, &paramX, &paramY);
			_SetHandPos(handIndex, paramX, paramY);
		}
	}
	else
	{
		//映射不可用，原位按下 //这是否会导致用户难以排查映射问题呢？
		if (currentHand.isDown == false && bPress == true)
		{
			_OnHandDown(handIndex, currentHand.x, currentHand.y);
			return;
		}
	}
}

void CharacterObject::RegisterAllActionFunc(bool falseToUnregister)
{
	//模型控件相关
	auto& im = InputManager::GetIns();


	{
		ActionCallback animationActionCallBack;
		animationActionCallBack.userData = this;
		animationActionCallBack.callback = [](const char* actionName, float value, void* userData, uint64_t userData2)
			{
				((CharacterObject*)userData)->OnAnimationPlay(UTIL_GETLOW32VALUE(userData2));
			};

		for (int i = 0; i < modelAnimationVec.size(); i++)
		{
			std::string animationActionName = "Character.Animation." + std::to_string(i) + ".Start";
			UTIL_SETLOW32VALUE(animationActionCallBack.userData2, i);
			if (falseToUnregister)
				im.RegisterActionCallback(animationActionName.c_str(), animationActionCallBack);
			else
				im.UnregisterActionCallback(animationActionName.c_str(), animationActionCallBack);

		}
	}
}

void CharacterObject::UnregisterAllActionFunc()
{
	RegisterAllActionFunc(false);
}

void CharacterObject::OnAnimationPlay(int animationIndex)
{
	if (animationIndex >= modelAnimationVec.size())return;

	if (!modelAnimationVec[animationIndex].controlName.empty())
	{
		_model->PlayAnimation(modelAnimationVec[animationIndex].controlName.c_str());
	}


}

void CharacterObject::_OnHandDown(int handIndex, float paramX, float paramY)
{
	if (handIndex >= hands.size())return;
	auto& currentHand=hands[handIndex];
	currentHand.x = paramX;
	currentHand.y = paramY;
	currentHand.isDown = true;

	if (currentHand.handState == HandData::Active)
	{
		activeTimeMS = SDL_GetTicks();
	}
}

void CharacterObject::_OnHandUp(int handIndex)
{
	if (handIndex >= hands.size())return;
	auto& currentHand=hands[handIndex];
	currentHand.isDown = false;

	if (currentHand.handState == HandData::Active)
	{
		activeTimeMS = SDL_GetTicks();//UP也重置active时间
		currentHand.point_active_up.SetTarget(currentHand.x,currentHand.y,currentHand.activeHeight);
		currentHand.point_active_up.SetStartPointAndStartMove(currentHand.x,currentHand.y,0);
	}
	else
	{
			auto currentTick = SDL_GetTicks();
			currentHand.triggerTicks[currentHand.triggerTicksIndex] = currentTick;
			currentHand.triggerTicksIndex++;
			if (currentHand.triggerTicksIndex > 2)currentHand.triggerTicksIndex = 0;

			uint64_t duration = currentTick - currentHand.triggerTicks[currentHand.triggerTicksIndex];
			if (duration < 1000)
			{
				//最老的一次按下也是在1000ms里完成，说明达成了1秒3按的激活条件
				currentHand.handState = HandData::Active;
				activeTimeMS = currentTick;
				isAnyHandActive=true;
				currentHand.point_active_up.SetTarget(currentHand.x,currentHand.y,currentHand.activeHeight);
				currentHand.point_active_up.SetStartPointAndStartMove(currentHand.x,currentHand.y,0.f);

			}
			else
			{
				currentHand.handState = HandData::Default;
				currentHand.point_default_up.SetStartPointAndStartMove(currentHand.x, currentHand.y, 0.f);

			}

	}



}

void CharacterObject::_SetHandPos(int handIndex, float paramX, float paramY)
{
	if (handIndex >= hands.size())return;
	auto& currenthand = hands[handIndex];
	currenthand.x = paramX;
	currenthand.y = paramY;
}

void CharacterObject::_SetUpHand(HandData& curHand, const char* posTestMesh, const char* paramX, const char* paramY, const char* paramZ, float defaultX, float defaultY, float defaultHeight, float activeHeight)
{
	curHand.handHandle = _model->GetHandHandle(posTestMesh);



	curHand.handParamPosX = _model->GetParamHandle(paramX);
	curHand.handParamPosY = _model->GetParamHandle(paramY);
	curHand.handParamPosZ = _model->GetParamHandle(paramZ);

	if (INVALID_HANDHANDLE == curHand.handHandle)
	{
		//没有目标mesh时使用烘焙的旧版映射
		if (SDL_strcmp(posTestMesh, "LHandPos") == 0)
		{
			curHand.handPosMapping.SetUpAsOldHandLeft();
		}
		else if (SDL_strcmp(posTestMesh, "RHandPos") == 0)
		{
			curHand.handPosMapping.SetUpAsOldHandRight();
		}
		else
			curHand.handPosMapping.BuildMeshMapping(_model, curHand.handHandle, curHand.handParamPosX, curHand.handParamPosY, curHand.handParamPosZ);

	}
	else
		curHand.handPosMapping.BuildMeshMapping(_model, curHand.handHandle, curHand.handParamPosX, curHand.handParamPosY, curHand.handParamPosZ);


	curHand.activeHeight = activeHeight;



	curHand.point_default_up.SetTarget(defaultX , defaultY , defaultHeight);
	curHand.point_active_to_default.SetTarget(defaultX , defaultY , defaultHeight);
	curHand.point_active_up.SetTarget(defaultX , defaultY , activeHeight);
	curHand.point_default_up.SetFixSpeed(6.5f);
	curHand.point_active_to_default.SetFixDuration(0.5, StraightMovingPoint::MoveTypeWithFixTime::AccelerateThenDecelerates);
	curHand.point_active_up.SetFixDuration(0.09f, StraightMovingPoint::MoveTypeWithFixTime::Decelerates);
	curHand.point_default_up.SetStartPointAndStartMove(defaultX, defaultY, defaultHeight);
	curHand.point_active_to_default.SetStartPointAndStartMove(defaultX, defaultY, defaultHeight);
	curHand.point_active_up.SetStartPointAndStartMove(defaultX, defaultY, activeHeight);
}






