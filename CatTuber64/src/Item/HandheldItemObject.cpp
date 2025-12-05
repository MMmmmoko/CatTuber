
#include"AppContext.h"
#include"Pack/Pack.h"
#include"Util/Util.h"
#include"Input/InputManager.h"

#include"Model/Live2DModelBase.h"

#include"Item/Scene.h"
#include"Item/HandheldItemObject.h"
#include"Item/ModelControl.h"
#include"Item/ClassicItem.h"

//从桌子那边拷贝过来的 如果有什么需要修改的地方应该一起修改
bool HandheldItemObject::LoadFromPath(const char* u8PackPath, const Json::Value& bindingJson)
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
	//Name
	//SubDescription
	//Description
	//
	//Author
	//
	Json::Value desc;
	size_t memSize;
	if (pack.IsFileExist(CATTUBER_MODELRESOURCE_DESC_FILENAME))
	{
		uint8_t* jsonMem = pack.LoadFile(CATTUBER_MODELRESOURCE_DESC_FILENAME, &memSize);

		desc = util::BuildJsonFromMem((const char*)jsonMem, memSize);
	}

	bool buttonHandled = false;
	bool axisHandled = false;

	ModelControl::SetUpDefaultControl(desc, _model, &modelButtonVec, &modelAxisVec, &modelAnimationVec);
	//如果提供的绑定信息，则直接应用（比如来自程序退出时的自动保存）
	if (!bindingJson.empty())
		ModelControl::SetUpBindingByJson(bindingJson, &modelButtonVec, &modelAxisVec, &modelAnimationVec);
	else
		LoadBinding();
	//此时当前应该使用的绑定已经写入各vec中了，向InputManager里注册各个绑定
	ModelControl::ApplyControlBindings(&modelButtonVec, &modelAxisVec, &modelAnimationVec);
	//为绑定的Action设置具体的执行函数
	RegisterAllActionFunc();
	resourcePath = u8PackPath;
	working = true;
	return true;
}

void HandheldItemObject::Update(uint64_t deltaTicksNS)
{
	if (working)
	{
		auto curTickMs = SDL_GetTicks();


		//pushedButtnVec是table那边的代码用的，用于给当前帧处于按下状态的按钮打表来减少遍历长度
		// 按下的键是用于计算手部位置的，所以handheldItem不需要
		//std::vector<ModelButtonControl*> pushedButtnVec;
		for (auto& button : modelButtonVec)
		{
			float value;
			if (button.isDown)
			{
				value = 1.f;

				//pushedButtnVec.push_back(&button);
			}
			else
			{

				value = 0.F;
				//手持物不使用渐出效果
				//已经抬起的时间
				//0.25时长  0.38峰值
				//float upDuration = static_cast<float> (curTickMs - button.upTickMs) / 1000.f;
				//if (upDuration > button.recoveryDuration)
				//{
				//	value = 0.f;
				//}
				//else
				//{
				//	value = (1.f - upDuration / button.recoveryDuration) * 0.38F;
				//}
			}
			_model->AddParamValue(button.paramHandle, value);
		}
		//optimize好像其实可以扔在AxisValueChange里设置长期， 但一般模型不会有太多轴数
		for (auto& axis : modelAxisVec)
		{
			//CatTuber多轴混合
			for (auto& curAxis : axis.axisVec)
			{
				_model->AddParamValue(curAxis.paramHandle, curAxis.value);
			}
		}

		_model->Update(deltaTicksNS);






	}
}

void HandheldItemObject::Draw(MixDrawList* drawList)
{
	if (working)
	{
		_model->DrawMix(drawList);
	}
}


void HandheldItemObject::OnLoopEnd()
{
	//绘制之后的调用，可以用于状态清理重置
	for (auto& axisGroup : modelAxisVec)
	{
		axisGroup.valueChangedCurFrame = false;
	}
}

Json::Value HandheldItemObject::GenerateAttributes()
{
	if (resourcePath.empty())
	{
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Saving a object with NULL pack path!");
		throw(std::runtime_error("Saving a object with NULL pack path!"));
	}
	Json::Value json;
	json["PackPath"] = resourcePath;
	json["Bindings"] = ModelControl::GenerateJsonBinding(&modelButtonVec, &modelAxisVec, &modelAnimationVec);

	//todo/FIXME 补完其他需保存的内容 
	return json;
}


HandheldItemObject* HandheldItemObject::CreateFromAttributes(const Json::Value& applyJson)
{
	if (!(applyJson.isMember("PackPath") && applyJson["PackPath"].isString()))
	{
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Create HandheldItem with invalid json! No path info exist.");
		return nullptr;
	}
	auto resultObj = new HandheldItemObject;

	std::string pathStr = AppContext::ResolvePathToAbsolute(applyJson["PackPath"].asString());
	if (!resultObj->LoadFromPath(pathStr.c_str(), applyJson["Bindings"]))
	{
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Can not create HandheldItem at path: %s.", pathStr.c_str());
		delete resultObj;
		return nullptr;
	}

	return resultObj;
}

void HandheldItemObject::ReleaseObj(HandheldItemObject* obj)
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
bool HandheldItemObject::LoadBindingByName(const char* bindingName)
{
	return ModelControl::LoadBindingByName(resourcePath, bindingName, &modelButtonVec, &modelAxisVec, &modelAnimationVec);
}
void HandheldItemObject::LoadBinding()
{
	if (LoadBindingByName("##Save"))
	{
		return;
	}
	//如果没有成功加载于App中保存的绑定，则查询Pack文件夹内中

	//defaultbinding已经在其他地方(_SetUpControlAndAnimation)读取进入modelButtonVec
	//将binding从vec元素的defaultbingding复制到binding即可
	for (auto& x : modelButtonVec)
	{
		x.binding = x.defaultBinding;
	}
	for (auto& x : modelAxisVec)
	{
		for (auto& curAxis : x.axisVec)
		{
			curAxis.binding = curAxis.defaultBinding;
		}
	}
	for (auto& x : modelAnimationVec)
	{
		x.binding = x.defaultBinding;
	}
}

void HandheldItemObject::ClearBinding()
{
	for (auto& x : modelButtonVec)
	{
		x.binding.UnRegisterBinding();
	}
	for (auto& x : modelAxisVec)
	{
		for (auto& curAxis : x.axisVec)
		{
			curAxis.binding.UnRegisterBinding();
		}
	}
	for (auto& x : modelAnimationVec)
	{
		for (auto& y : x.binding)
			y.UnRegisterBinding();
	}
}

void HandheldItemObject::SetPosition(float x, float y)
{
	//todo


}

bool HandheldItemObject::IsAntButtonPushed()
{

	for (auto& x : modelButtonVec)
	{
		if (x.isDown = true)
		{
			return true;
		}
	}
	for (auto& x : modelAxisVec)
	{
		if (x.valueChangedCurFrame = true)
		{
			return true;
		}
	}
	return false;
}


//具体action到函数成员方法
void HandheldItemObject::RegisterAllActionFunc(bool falseToUnregister)
{
	//模型控件相关
	auto& im = InputManager::GetIns();
	{
		ActionCallback downActionCallBack;
		downActionCallBack.userData = this;
		downActionCallBack.callback = [](const char* actionName, float value, void* userData, uint64_t userData2)
			{
				((HandheldItemObject*)userData)->OnButtonDown(UTIL_GETLOW32VALUE(userData2));
			};

		ActionCallback upActionCallBack;
		upActionCallBack.userData = this;
		upActionCallBack.callback = [](const char* actionName, float value, void* userData, uint64_t userData2)
			{
				((HandheldItemObject*)userData)->OnButtonUp(UTIL_GETLOW32VALUE(userData2));
			};

		for (int i = 0; i < modelButtonVec.size(); i++)
		{
			std::string downActionName = "HandheldItem.Button." + std::to_string(i) + ".Down";
			std::string upActionName = "HandheldItem.Button." + std::to_string(i) + ".Up";

			UTIL_SETLOW32VALUE(downActionCallBack.userData2, i);
			UTIL_SETLOW32VALUE(upActionCallBack.userData2, i);
			//downActionCallBack.userData2 = (void*)i;
			//upActionCallBack.userData2 = (void*)i;
			if (falseToUnregister)
			{
				im.RegisterActionCallback(downActionName.c_str(), downActionCallBack);
				im.RegisterActionCallback(upActionName.c_str(), upActionCallBack);
			}
			else
			{
				im.UnregisterActionCallback(downActionName.c_str(), downActionCallBack);
				im.UnregisterActionCallback(upActionName.c_str(), upActionCallBack);

			}
		}
	}


	{
		ActionCallback axisActionCallBack;
		axisActionCallBack.userData = this;
		axisActionCallBack.callback = [](const char* actionName, float value, void* userData, uint64_t userData2)
			{
				//由于多轴的存在，这里userData2需要拆分成两个参数
				//低位是轴组的索引，高位是轴在组中的索引
				((HandheldItemObject*)userData)->OnAxisValueChange(UTIL_GETLOW32VALUE(userData2), UTIL_GETHIGH32VALUE(userData2), value);
			};

		for (int i = 0; i < modelAxisVec.size(); i++)
		{

			UTIL_SETLOW32VALUE(axisActionCallBack.userData2, i);
			for (int j = 0; j < modelAxisVec[i].axisVec.size(); j++)
			{
				UTIL_SETHIGH32VALUE(axisActionCallBack.userData2, j);

				std::string axisActionName = "HandheldItem.Axis." + std::to_string(i) + "." + std::to_string(j) + "Change";
				if (falseToUnregister)
					im.RegisterActionCallback(axisActionName.c_str(), axisActionCallBack);
				else
					im.UnregisterActionCallback(axisActionName.c_str(), axisActionCallBack);
			}
		}

	}

	{
		ActionCallback animationActionCallBack;
		animationActionCallBack.userData = this;
		animationActionCallBack.callback = [](const char* actionName, float value, void* userData, uint64_t userData2)
			{
				((HandheldItemObject*)userData)->OnAnimationPlay(UTIL_GETLOW32VALUE(userData2));
			};

		for (int i = 0; i < modelAnimationVec.size(); i++)
		{
			std::string animationActionName = "HandheldItem.Animation." + std::to_string(i) + ".Start";
			UTIL_SETLOW32VALUE(animationActionCallBack.userData2, i);
			if (falseToUnregister)
				im.RegisterActionCallback(animationActionName.c_str(), animationActionCallBack);
			else
				im.UnregisterActionCallback(animationActionName.c_str(), animationActionCallBack);

		}
	}
}

void HandheldItemObject::UnregisterAllActionFunc()
{
	RegisterAllActionFunc(false);
}

void HandheldItemObject::OnButtonDown(int btnIndex)
{
	if (btnIndex >= modelButtonVec.size())return;

	auto& curButton = modelButtonVec[btnIndex];
	curButton.isDown = true;
	if (!curButton.downAnimation.empty())
		_model->PlayAnimation(curButton.downAnimation);
	if (curButton.downAction)
		InputManager::GetIns().RiseAction(curButton.downAction);
}

void HandheldItemObject::OnButtonUp(int btnIndex)
{
	if (btnIndex >= modelButtonVec.size())return;

	auto& curButton = modelButtonVec[btnIndex];
	curButton.isDown = false;
	curButton.upTickMs = SDL_GetTicks();

	if (!curButton.upAnimation.empty())
		_model->PlayAnimation(curButton.upAnimation);
	if (curButton.upAction)
		InputManager::GetIns().RiseAction(curButton.upAction);


}


void HandheldItemObject::OnAxisValueChange(int axisGroupIndex, int axisIndex, float value)
{
	if (axisGroupIndex >= modelAxisVec.size()
		|| axisIndex >= modelAxisVec[axisGroupIndex].axisVec.size()
		|| modelAxisVec[axisGroupIndex].axisVec[axisIndex].value == value
		)return;

	modelAxisVec[axisGroupIndex].axisVec[axisIndex].value = value;
	modelAxisVec[axisGroupIndex].valueChangedCurFrame = true;


}

void HandheldItemObject::OnAnimationPlay(int animationIndex)
{
	if (animationIndex >= modelAnimationVec.size())return;

	if (!modelAnimationVec[animationIndex].controlName.empty())
	{
		_model->PlayAnimation(modelAnimationVec[animationIndex].controlName);
	}

}

void HandheldItemObject::_UpdateAxisVec()
{
	//为了防止axis组的多次触发，先收集好信息后再调用此函数进行动作触发


	for (auto& axisGroup : modelAxisVec)
	{
		//根据组里的数值计算新的value(模长)
		float newValue = 0.f;
		{
			if (axisGroup.axisVec.size() == 1)
			{
				newValue = axisGroup.axisVec[0].value;
			}
			else
			{
				for (auto& axis : axisGroup.axisVec)
					newValue += axis.value * axis.value;
				newValue = SDL_sqrtf(newValue);
			}
		}




		if (axisGroup.actionActiveValue > 0)
		{
			if (newValue > axisGroup.actionActiveValue && axisGroup.groupValue <= axisGroup.actionActiveValue)
			{
				//active
				if (!axisGroup.activeAnimation.empty())
					_model->PlayAnimation(axisGroup.activeAnimation);
				if (axisGroup.activeAction)
					InputManager::GetIns().RiseAction(axisGroup.activeAction);
			}
			else if (newValue < axisGroup.actionActiveValue && axisGroup.groupValue >= axisGroup.actionActiveValue)
			{
				//inactive
				if (!axisGroup.inactiveAnimation.empty())
					_model->PlayAnimation(axisGroup.inactiveAnimation);
				if (axisGroup.inactiveAction)
					InputManager::GetIns().RiseAction(axisGroup.inactiveAction);
			}
		}
		else
		{
			if (newValue < axisGroup.actionActiveValue && axisGroup.groupValue >= axisGroup.actionActiveValue)
			{
				//active
				if (!axisGroup.activeAnimation.empty())
					_model->PlayAnimation(axisGroup.activeAnimation);
				if (axisGroup.activeAction)
					InputManager::GetIns().RiseAction(axisGroup.activeAction);
			}
			else if (newValue > axisGroup.actionActiveValue && axisGroup.groupValue <= axisGroup.actionActiveValue)
			{
				//inactive
				if (!axisGroup.inactiveAnimation.empty())
					_model->PlayAnimation(axisGroup.inactiveAnimation);
				if (axisGroup.inactiveAction)
					InputManager::GetIns().RiseAction(axisGroup.inactiveAction);
			}
		}
		axisGroup.groupValue = newValue;
	}


}

