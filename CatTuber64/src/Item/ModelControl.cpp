#include"Item/ModelControl.h"
#include "Util.h"
#include"AppContext.h"
#include"Input/InputManager.h"
#include "Input/InputParser.h"


//物理按键到具体action
void BindingInfo::RegisterBinding(int index, int index2)
{
	UnRegisterBinding();
	auto& im = InputManager::GetIns();

	if (controllList.empty())return;

	switch (type)
	{
	case BindingInfo::Undefined:
		break;
	case BindingInfo::Button_ActualButton:
	{
		//向InputManager注册关于物理按键的绑定

		//构造actionname  Table.Button.0.Down
		std::string downActionName = "Table.Button." + std::to_string(index) + ".Down";
		std::string upActionName = "Table.Button." + std::to_string(index) + ".Up";
		_bindingHandleList.push_back(
			im.RegisterButtonActionBinding(downActionName.c_str(), upActionName.c_str(), controllList.data(), static_cast<int>(controllList.size()))
		);

		break;
	}
	case BindingInfo::Button_ActualAxisToButton:
	{
		//构造actionname  Table.Button.0.Down
		std::string downActionName = "Table.Button." + std::to_string(index) + ".Down";
		std::string upActionName = "Table.Button." + std::to_string(index) + ".Up";

		if (controlValue > 0)
		{
			_bindingHandleList.push_back(
				im.RegisterAxisExceedActionBinding(downActionName.c_str(), controllList[0].c_str(), controlValue));
			_bindingHandleList.push_back(
				im.RegisterAxisDroppedActionBinding(upActionName.c_str(), controllList[0].c_str(), controlValue));
		}
		else
		{

			_bindingHandleList.push_back(
				im.RegisterAxisDroppedActionBinding(downActionName.c_str(), controllList[0].c_str(), controlValue));
			_bindingHandleList.push_back(
				im.RegisterAxisExceedActionBinding(upActionName.c_str(), controllList[0].c_str(), controlValue));
		}
		break;
	}
	case BindingInfo::Axis_ActualAxis:
	{
		//构造actionname  Table.Axis.0.0.Change
		std::string axisActionName = "Table.Axis." + std::to_string(index) + "." + std::to_string(index2) +".Change";
		_bindingHandleList.push_back(
			im.RegisterAxisChangeActionBinding(axisActionName.c_str(), controllList[0].c_str()));
		break;
	}
	case BindingInfo::Axis_ActualButtonToAxis:
	{
		//构造actionname  Table.Axis.0.1.Change
		std::string axisActionName = "Table.Axis." + std::to_string(index) + "." + std::to_string(index2) + ".Change";

		_bindingHandleList.push_back(
			im.RegisterButtonToAxisActionBinding(axisActionName.c_str(), controllList[0].c_str(), controlValue));
		break;
	}
	case BindingInfo::Animation_ActualButton:
	{
		//用物理按键触发动画 Table.Animation.animationName.Start
		std::string animationActionName = "Table.Animation." + std::to_string(index) + ".Start";
		_bindingHandleList.push_back(
			im.RegisterButtonActionBinding(animationActionName.c_str(), NULL, controllList.data(), static_cast<int>(controllList.size()))
		);
		break;
	}
	case BindingInfo::Animation_ActualAxisActive:
	{
		std::string animationActionName = "Table.Animation." + std::to_string(index) + ".Start";

		if (controlValue > 0)
		{
			_bindingHandleList.push_back(
				im.RegisterAxisExceedActionBinding(animationActionName.c_str(), controllList[0].c_str(), controlValue));
		}
		else
		{

			_bindingHandleList.push_back(
				im.RegisterAxisDroppedActionBinding(animationActionName.c_str(), controllList[0].c_str(), controlValue));
		}

		break;
	}
	case BindingInfo::Animation_ActualAxisInactive:
	{
		std::string animationActionName = "Table.Animation." + std::to_string(index) + ".Start";

		if (controlValue < 0)
		{
			_bindingHandleList.push_back(
				im.RegisterAxisExceedActionBinding(animationActionName.c_str(), controllList[0].c_str(), controlValue));
		}
		else
		{
			_bindingHandleList.push_back(
				im.RegisterAxisDroppedActionBinding(animationActionName.c_str(), controllList[0].c_str(), controlValue));
		}
		break;
	}
	case BindingInfo::Animation_Action:
	{
		std::string animationActionName = "Table.Animation." + std::to_string(index) + ".Start";
		_bindingHandleList.push_back(
			im.RegisterActionByActionBinding(animationActionName.c_str(), controllList[0].c_str(), controlValue));
		break;
	}
	default:
		break;
	}





}

void BindingInfo::UnRegisterBinding()
{
	//取消注册
	auto& im = InputManager::GetIns();
	for (auto& bindingHandle : _bindingHandleList)
	{
		im.UnregisterActionBinding(bindingHandle);
	}

}

const char* BindingInfo::GetTypeJsonStr(Type type)
{
	const char* strList[] = { "Undefined",
		"ActualButton","ActualAxisToButton",
		"ActualAxis","ActualButtonToAxis",
		"ActualButton","AxisActive","AxisInactive","Action"
	};
	if (type < TypeMax && type >= Undefined)
		return strList[type];
	return strList[Undefined];
}

bool ModelControl::LoadBindingByName(const std::string& packPath, const char* bindingName, std::vector<ModelButtonControl>* modelButtonVec, std::vector<ModelAxisControl>* modelAxisVec, std::vector<ModelAnimationControl>* modelAnimationVec)
{
	//preferpath/Bindings/88KTable_0.binding
//Pack文件名_id 如 88KKeyboard_0 

//构建要寻找的目标路径
	std::string packFileName = util::RemoveExtension((util::GetFileNameFromPath(packPath)));

	std::string bindingFileFolder = AppContext::GetPrefPath();
	bindingFileFolder = bindingFileFolder + "/Bindings";

	if (!SDL_CreateDirectory(bindingFileFolder.c_str()))
	{
		return false;
	}

	std::string pattern = packFileName + "_*.binding";
	int count;
	char** fileList;
	fileList = SDL_GlobDirectory(bindingFileFolder.c_str(), pattern.c_str(), 0, &count);
	if (!fileList)
	{
		return false;
	}

	for (int i = 0; i < count; i++)
	{
		Json::Value json = util::BuildJsonFromFile(fileList[i]);
		//先匹配绑定名
		if (json.isMember("BindingName") && json["BindingName"].isString())
		{
			if (json["BindingName"].asString() == bindingName)
			{
				//绑定名匹配，查看路径是否匹配
				if (json.isMember("PackPath") && json["PackPath"].isString())
				{
					std::string packPathInJson = json["PackPath"].asString();
					packPathInJson = AppContext::ResolvePathToAbsolute(packPathInJson);
					if (util::PathEqual(packPathInJson, packPath))
					{

						SDL_LogInfo(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Binding match success, Load at %s", packPathInJson.c_str());
						SetUpBindingByJson(json, modelButtonVec,modelAxisVec,modelAnimationVec);
						SDL_free(fileList);
						return true;
					}
				}
			}
		}
	}

	SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Can not find binding %s, pack path: %s", bindingName, packPath.c_str());
	SDL_free(fileList);
	return false;
}

void ModelControl::SetUpBindingByJson(const Json::Value& bindingJson, std::vector<ModelButtonControl>* pmodelButtonVec, std::vector<ModelAxisControl>* pmodelAxisVec, std::vector<ModelAnimationControl>* pmodelAnimationVec)
{

	if (pmodelButtonVec&&bindingJson.isMember("Buttons") && bindingJson["Buttons"].isArray())
	{
		auto& modelButtonVec = *pmodelButtonVec;
		//i不可大于按钮数
		for (unsigned int i = 0; i < bindingJson["Buttons"].size() && i < modelButtonVec.size(); i++)
		{
			auto& curButtonBindingJson = bindingJson["Buttons"][i];

			if (curButtonBindingJson.isMember("Type") && curButtonBindingJson["Type"].isString())
			{
				std::string bindingTypeStr = curButtonBindingJson["Type"].asString();
				if (bindingTypeStr == "ActualButton")
				{
					modelButtonVec[i].binding.type = BindingInfo::Button_ActualButton;
					if (curButtonBindingJson.isMember("Button") && curButtonBindingJson["Button"].isArray())
					{
						for (unsigned int buttonNameIndex = 0; buttonNameIndex < curButtonBindingJson["Button"].size(); buttonNameIndex++)
						{
							if (curButtonBindingJson["Button"][buttonNameIndex].isString())
							{
								modelButtonVec[i].binding.controllList.push_back(curButtonBindingJson["Button"][buttonNameIndex].asString());
							}
							else
							{
								SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Warn: No value or incorrect value in [Buttons][%d][ActualButton][%d]", i, buttonNameIndex);
							}
						}
					}
				}
				else if (bindingTypeStr == "ActualAxisToButton")
				{
					modelButtonVec[i].binding.type = BindingInfo::Button_ActualAxisToButton;
					if (curButtonBindingJson.isMember("Axis") && curButtonBindingJson["Axis"].isString())
					{
						modelButtonVec[i].binding.controllList.push_back(curButtonBindingJson["Axis"].asString());

						if (curButtonBindingJson.isMember("ActiveValue") && curButtonBindingJson["ActiveValue"].isDouble())
						{
							modelButtonVec[i].binding.controlValue = static_cast<float>(curButtonBindingJson["ActiveValue"].asDouble());
						}
					}
					else
					{
						SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Warn: No value or incorrect value in [Buttons][%d][ActualAxisToButton][Axis]", i);
					}

				}
			}
		}
	}

	if (pmodelAxisVec&&bindingJson.isMember("Axes") && bindingJson["Axes"].isArray())
	{
		auto& modelAxisVec = *pmodelAxisVec;
		//i不可大于轴数
		for (unsigned int i = 0; i < bindingJson["Axes"].size() && i < modelAxisVec.size(); i++)
		{
			if(bindingJson["Axes"][i].isArray())
			{ 
				for (unsigned int j = 0;j< bindingJson["Axes"][i].size()&&j< modelAxisVec[i].axisVec.size();j++)
				{
					auto& curAxisBindingJson = bindingJson["Axes"][i][j];
					if (curAxisBindingJson.isMember("Type") && curAxisBindingJson["Type"].isString())
					{
						std::string bindingStr = curAxisBindingJson["Type"].asString();
						if (bindingStr == "ActualAxis")
						{
							modelAxisVec[i].axisVec[j].binding.type = BindingInfo::Axis_ActualAxis;
							if (curAxisBindingJson.isMember("Axis") && curAxisBindingJson["Axis"].isString())
							{

								modelAxisVec[i].axisVec[j].binding.controllList.push_back(curAxisBindingJson["Axis"].asString());

							}
							else
							{
								SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Warn: No value or incorrect value in [Axes][%d][ActualAxis][Axis]", i);
							}
						}
						else if (bindingStr == "ActualButtonToAxis")
						{
							modelAxisVec[i].axisVec[j].binding.type = BindingInfo::Axis_ActualButtonToAxis;
							if (curAxisBindingJson.isMember("Button") && curAxisBindingJson["Button"].isArray())
							{
								for (unsigned int buttonNameIndex = 0; buttonNameIndex < curAxisBindingJson["Button"].size(); buttonNameIndex++)
								{
									if (curAxisBindingJson["Button"][buttonNameIndex].isString())
									{
										modelAxisVec[i].axisVec[j].binding.controllList.push_back(curAxisBindingJson["Button"][buttonNameIndex].asString());
									}
									else
									{
										SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Warn: No value or incorrect value in [Axes][%d][ActualButtonToAxis][Button][%d]", i, buttonNameIndex);
									}
								}
							}
						}
					}
				}
			}
		}
	}


	//Animations
	//Animations
	//Animations
	if (pmodelAnimationVec&& bindingJson.isMember("Animations") && bindingJson["Animations"].isArray())
	{
		auto& modelAnimationVec = *pmodelAnimationVec;

		//i不可大于动画数
		for (unsigned int i = 0; i < bindingJson["Animations"].size() && i < modelAnimationVec.size(); i++)
		{

			if (bindingJson["Animations"][i].isArray())
			{
				for (unsigned int bindingIndex = 0; bindingIndex < bindingJson["Animations"][i].size(); bindingIndex++)
				{
					auto& curAnimationBindingJson = bindingJson["Animations"][i][bindingIndex];
					if (curAnimationBindingJson["Type"].isString())
					{
						std::string bindingStr = curAnimationBindingJson["Type"].asString();
						if (bindingStr == "ActualButton")
						{
							auto& curBinding = modelAnimationVec[i].binding[bindingIndex];
							curBinding.type = BindingInfo::Animation_ActualButton;
							if (curAnimationBindingJson.isMember("Button") && curAnimationBindingJson["Button"].isArray())
							{
								for (unsigned int buttonNameIndex = 0; buttonNameIndex < curAnimationBindingJson["Button"].size(); buttonNameIndex++)
								{
									if (curAnimationBindingJson["Button"][buttonNameIndex].isString())
									{
										curBinding.controllList.push_back(curAnimationBindingJson["Button"][buttonNameIndex].asString());
									}
									else
									{
										SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Warn: No value or incorrect value in [Animations][%d][%d][ActualButton][%d]", i, bindingIndex, buttonNameIndex);
									}
								}
							}
						}
						else if (bindingStr == "AxisActive")
						{
							auto& curBinding = modelAnimationVec[i].binding[bindingIndex];
							curBinding.type = BindingInfo::Animation_ActualAxisActive;
							if (curAnimationBindingJson.isMember("Axis") && curAnimationBindingJson["Axis"].isString())
							{
								curBinding.controllList.push_back(curAnimationBindingJson["Axis"].asString());

								if (curAnimationBindingJson.isMember("ActiveValue") && curAnimationBindingJson["ActiveValue"].isDouble())
								{
									curBinding.controlValue = static_cast<float>(curAnimationBindingJson["ActiveValue"].asDouble());
								}
							}
							else
							{
								SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Warn: No axis name in [Animations][%d][%d][AxisActive]", i, bindingIndex);
							}
						}
						else if (bindingStr == "AxisInactive")
						{
							auto& curBinding = modelAnimationVec[i].binding[bindingIndex];
							curBinding.type = BindingInfo::Animation_ActualAxisInactive;
							if (curAnimationBindingJson.isMember("Axis") && curAnimationBindingJson["Axis"].isString())
							{
								curBinding.controllList.push_back(curAnimationBindingJson["Axis"].asString());
								if (curAnimationBindingJson.isMember("ActiveValue") && curAnimationBindingJson["ActiveValue"].isDouble())
								{
									curBinding.controlValue = static_cast<float>(curAnimationBindingJson["ActiveValue"].asDouble());
								}
							}
							else
							{
								SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Warn: No axis name in [Animations][%d][%d][AxisInactive]", i, bindingIndex);
							}
						}
						else if (bindingStr == "Action")
						{
							auto& curBinding = modelAnimationVec[i].binding[bindingIndex];
							curBinding.type = BindingInfo::Animation_Action;
							if (curAnimationBindingJson.isMember("Action") && curAnimationBindingJson["Action"].isString())
							{
								curBinding.controllList.push_back(curAnimationBindingJson["Action"].asString());
								//if (curAnimationBindingJson.isMember("ActiveValue") && curAnimationBindingJson["ActiveValue"].isDouble())
								//{
								//	curBinding.controlValue = static_cast<float>(curAnimationBindingJson["ActiveValue"].asDouble());
								//}
							}
							else
							{
								SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Warn: No action name in [Animations][%d][%d][Action]", i, bindingIndex);
							}
						}
						else
						{
							SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Warn: Can not match binding type in [Animations][%d][%d]: %s", i, bindingIndex, bindingStr.c_str());
						}




					}
				}
			}

		}
	}











}

Json::Value ModelControl::GenerateJsonBinding(std::vector<ModelButtonControl>* pmodelButtonVec, std::vector<ModelAxisControl>* pmodelAxisVec, std::vector<ModelAnimationControl>* pmodelAnimationVec)
{
	Json::Value bindingRoot;

	if (pmodelButtonVec)
	{
		auto& modelButtonVec = *pmodelButtonVec;
		for (int i = 0; i < modelButtonVec.size(); i++)
		{
			auto& curButton = bindingRoot["Buttons"][i];
			auto& curBinding = modelButtonVec[i].binding;
			if (curBinding.type == BindingInfo::Button_ActualButton)
			{
				curButton["Type"] = "ActualButton";
				for (int buttonIndex = 0; buttonIndex < curBinding.controllList.size(); buttonIndex++)
				{
					curButton["Button"][buttonIndex] = curBinding.controllList[buttonIndex];
				}
			}
			else if (curBinding.type == BindingInfo::Button_ActualAxisToButton)
			{
				curButton["Type"] = "ActualAxisToButton";
				if (!curBinding.controllList.empty())
					curButton["Axis"] = curBinding.controllList[0];
				curButton["ActiveValue"] = curBinding.controlValue;
			}
			else
			{
				curButton["Type"] = "Undefined";
				SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Warn: GenerateJsonBinding with Undefined Button[%d]", i);
			}
		}
	}

	if (pmodelAxisVec)
	{
		auto& modelAxisVec = *pmodelAxisVec;
		for (int i = 0; i < modelAxisVec.size(); i++)
		{

			for (int j = 0; j < modelAxisVec[i].axisVec.size();j++)
			{
				auto& curAxis = bindingRoot["Axes"][i][j];
				auto& curBinding = modelAxisVec[i].axisVec[j].binding;
				if (curBinding.type == BindingInfo::Axis_ActualAxis)
				{
					curAxis["Type"] = "ActualAxis";
					if (!curBinding.controllList.empty())
						curAxis["Axis"] = curBinding.controllList[0];
				}
				else if (curBinding.type == BindingInfo::Axis_ActualButtonToAxis)
				{
					curAxis["Type"] = "ActualButtonToAxis";
					for (int buttonIndex = 0; buttonIndex < curBinding.controllList.size(); buttonIndex++)
					{
						curAxis["Button"][i] = curBinding.controllList[i];
					}
					curAxis["ActiveValue"] = curBinding.controlValue;
				}
				else
				{
					curAxis["Type"] = "Undefined";
					SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Warn: GenerateJsonBinding with Undefined Axis[%d]", i);
				}
			}
		}
	}
	if (pmodelAnimationVec)
	{
		auto& modelAnimationVec = *pmodelAnimationVec;
		for (int i = 0; i < modelAnimationVec.size(); i++)
		{

			auto& curAnimation = bindingRoot["Animations"][i];
			for (int bindingIndex = 0; bindingIndex < modelAnimationVec[i].binding.size(); bindingIndex++)
			{
				auto& curBinding = modelAnimationVec[i].binding[bindingIndex];

				if (curBinding.type == BindingInfo::Animation_ActualButton)
				{
					curAnimation[bindingIndex]["Type"] = "ActualButton";
					for (int buttonIndex = 0; buttonIndex < curBinding.controllList.size(); buttonIndex++)
					{
						curAnimation[bindingIndex]["Button"][i] = curBinding.controllList[i];
					}
				}
				else if (curBinding.type == BindingInfo::Animation_ActualAxisActive)
				{
					curAnimation[bindingIndex]["Type"] = "AxisActive";
					if (!curBinding.controllList.empty())
						curAnimation[bindingIndex]["Axis"] = curBinding.controllList[0];
					curAnimation[bindingIndex]["ActiveValue"] = curBinding.controlValue;
				}
				else if (curBinding.type == BindingInfo::Animation_ActualAxisInactive)
				{
					curAnimation[bindingIndex]["Type"] = "AxisInactive";
					if (!curBinding.controllList.empty())
						curAnimation[bindingIndex]["Axis"] = curBinding.controllList[0];
					curAnimation[bindingIndex]["ActiveValue"] = curBinding.controlValue;
				}
				else if (curBinding.type == BindingInfo::Animation_Action)
				{
					curAnimation[bindingIndex]["Type"] = "Action";
					if (!curBinding.controllList.empty())
						curAnimation[bindingIndex]["Action"] = curBinding.controllList[0];
					else
						SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Warn: No action name in Animation[%d]", i);
				}
			}
		}
	}
	return bindingRoot;
}

void ModelControl::SetUpDefaultControl(const Json::Value& descItemInfo, IModel* _model, std::vector<ModelButtonControl>* pmodelButtonVec, std::vector<ModelAxisControl>* pmodelAxisVec, std::vector<ModelAnimationControl>* pmodelAnimationVec)
{

	//在从文件加载模型时调用，如果json无效则尝试直接在模型参数中读取
	//按钮的UI与控制（告诉软件这个模型提供了多少按键（UI展示），以及这些按键应该如何表现（软件控制））
	//desc["Button"][0]["UiName"]["ch"]="按钮1"
	//desc["Button"][0]["ParamID"]="Keyboard_A"
	//desc["Button"][0]["Icon"]="*.png"/可选
	//desc["Button"][0]["DownAnimation"]="Down"/可选
	//desc["Button"][0]["UpAnimation"]="Up"/可选
	//desc["Button"][0]["DefaultBinding"][Type]="KeyBoardButton"
	//desc["Button"][0]["DefaultBinding"][Button][0]="Keyborad.A"


	//传入的是模型desc.json的desc["Controls"]
	auto& im = InputManager::GetIns();
	//按钮
	//按钮
	//按钮
	//按钮
	//按钮
	if (pmodelButtonVec)
	{
		auto& modelButtonVec = *pmodelButtonVec;
		if (descItemInfo.isMember("Buttons") && descItemInfo["Buttons"].isArray())
		{
			for (unsigned int i = 0; i < descItemInfo["Buttons"].size(); i++)
			{
				const Json::Value& curButtonJson = descItemInfo["Buttons"][i];
				auto& button = modelButtonVec.emplace_back();
				//按钮的显示名
				if (curButtonJson.isMember("UiName"))
					button.uiName = util::GetStringFromMultiLangJsonNode(curButtonJson["UiName"]);
				//按钮的参数ID
				if (curButtonJson.isMember("ParamID") && curButtonJson["ParamID"].isString())
				{
					button.paramID = curButtonJson["ParamID"].asString();
					button.paramHandle = _model->GetParamHandle(button.paramID);
				}


				//HandControl
				{
					if (curButtonJson.isMember("Hand") && curButtonJson["Hand"].isString()
						&& (curButtonJson["Hand"].asString() == "right" || curButtonJson["Hand"].asString() == "Right"))
					{
						button.handControl.handIndex = HandControl::RIGHT;
					}
					else
					{
						button.handControl.handIndex = HandControl::LEFT;
					}
					if (curButtonJson.isMember("HandPos") && curButtonJson["HandPos"].isString())
					{
						button.handControl.handPosHandle = _model->GetHandHandle(curButtonJson["HandPos"].asString());
					}
					if (curButtonJson.isMember("HandMoveType") && curButtonJson["HandMoveType"].isString())
					{
						const auto handMoveTypeStr = curButtonJson["HandMoveType"].asString();
						if (handMoveTypeStr == "Mouse")
						{
							button.handControl.moveType = HandControl::MOUSE;

							if (curButtonJson.isMember("RequireHandheldItem") && curButtonJson["RequireHandheldItem"].isBool())
							{
								button.handControl.moveCalcData.mouseData.requiredHandheldItem =curButtonJson["RequireHandheldItem"].asBool();
							}
							else
							{
								button.handControl.moveCalcData.mouseData.requiredHandheldItem = false;
							}
						}
						else if (handMoveTypeStr == "Stick")
						{
							button.handControl.moveType = HandControl::STICK;
						}
						else if (handMoveTypeStr == "Moving")
						{
							button.handControl.moveType = HandControl::MOVING;
						}
					}
					if (curButtonJson.isMember("HandWeight") && curButtonJson["HandWeight"].isDouble())
					{
						const auto handMoveTypeStr = curButtonJson["HandWeight"].asDouble();
						button.handControl.handWeight = static_cast<float>(handMoveTypeStr);
					}
				}
				


				//按钮的动画参数,注意这个动画参数是按钮用的用来表达按键的， 不应用于其他如状态转换之类的用途
				if (curButtonJson.isMember("DownAnimation") && curButtonJson["DownAnimation"].isString())
				{
					button.downAnimation = curButtonJson["DownAnimation"].asString();
				}
				if (curButtonJson.isMember("UpAnimation") && curButtonJson["UpAnimation"].isString())
				{
					button.upAnimation = curButtonJson["UpAnimation"].asString();
				}
				if (curButtonJson.isMember("DownAction") && curButtonJson["DownAction"].isString())
				{
					button.downAction = im.GetAction(curButtonJson["DownAction"].asString().c_str());
				}
				if (curButtonJson.isMember("UpAction") && curButtonJson["UpAction"].isString())
				{
					button.upAction = im.GetAction(curButtonJson["UpAction"].asString().c_str());
				}


				//默认绑定
				if (curButtonJson.isMember("DefaultBinding") && curButtonJson["DefaultBinding"].isMember("Type") && curButtonJson["DefaultBinding"]["Type"].isString())
				{
					std::string bindingStr = curButtonJson["DefaultBinding"]["Type"].asString();
					if (bindingStr == "ActualButton")
					{
						button.defaultBinding.type = BindingInfo::Button_ActualButton;
						if (curButtonJson["DefaultBinding"].isMember("Button") && curButtonJson["DefaultBinding"]["Button"].isArray())
						{
							for (unsigned int buttonNameIndex = 0; buttonNameIndex < curButtonJson["DefaultBinding"]["Button"].size(); buttonNameIndex++)
							{
								if (curButtonJson["DefaultBinding"]["Button"][buttonNameIndex].isString())
								{
									button.defaultBinding.controllList.push_back(curButtonJson["DefaultBinding"]["Button"][buttonNameIndex].asString());
								}
								else
								{
									SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Warn: No value or incorrect value in [Button][%d][DefaultBinding][Button][%d]", i, buttonNameIndex);
								}
							}
						}
					}
					else if (bindingStr == "ActualAxisToButton")
					{
						button.defaultBinding.type = BindingInfo::Button_ActualAxisToButton;
						if (curButtonJson["DefaultBinding"].isMember("Axis") && curButtonJson["DefaultBinding"]["Axis"].isString())
						{
							button.defaultBinding.controllList.push_back(curButtonJson["DefaultBinding"]["Axis"].asString());

							if (curButtonJson["DefaultBinding"].isMember("ActiveValue") && curButtonJson["DefaultBinding"]["ActiveValue"].isDouble())
							{
								button.defaultBinding.controlValue = static_cast<float>(curButtonJson["DefaultBinding"]["ActiveValue"].asDouble());
							}
						}
						else
						{
							SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Warn: No value or incorrect value in [Button][%d][DefaultBinding][Axis]", i);
						}
					}

					//轴绑定不写在Button里
				}



				if (button.uiName.empty())
				{
					button.uiName = button.paramID;
				}

			}
		}
		else
		{

			std::vector<std::string> paramVec = _model->GetParamList();
			//判断按钮前缀KEYBOARD BUTTON CTB_KEYBOARD CAT_KEY  ex: CAT_KEY_A

			for (auto& x : paramVec)
			{
				const char* keyBaseName = InputParser::ParamNameToButtonBaseName(x);
				if (keyBaseName)
				{
					auto& button = modelButtonVec.emplace_back();
					button.uiName = InputParser::ButtonBaseNameToUIName(keyBaseName);
					button.paramID = x;
					button.paramHandle = _model->GetParamHandle(button.paramID);


					button.defaultBinding.type = BindingInfo::Button_ActualButton;
					button.defaultBinding.controllList.push_back(keyBaseName);
				}
			}
		}
	}

	//轴
	//轴
	//轴
	//轴
	//轴
	if (pmodelAxisVec)
	{
		auto& modelAxisVec = *pmodelAxisVec;
		if (descItemInfo.isMember("AxisGroup") && descItemInfo["AxisGroup"].isArray())
		{
			for (unsigned int i = 0; i < descItemInfo["Axes"].size(); i++)
			{
				const Json::Value& curAxisJson = descItemInfo["Axes"][i];
				auto& axis = modelAxisVec.emplace_back();
				//按钮的显示名
				if (curAxisJson.isMember("UiName"))
					axis.uiName = util::GetStringFromMultiLangJsonNode(curAxisJson["UiName"]);





				//轴的动画参数,注意这个动画参数是按钮用的用来表达按键的， 不应用于其他如状态转换之类的用途
				if (curAxisJson.isMember("ActiveAnimation") && curAxisJson["ActiveAnimation"].isString())
				{
					axis.activeAnimation = curAxisJson["ActiveAnimation"].asString();
				}
				if (curAxisJson.isMember("InactiveAnimation") && curAxisJson["InactiveAnimation"].isString())
				{
					axis.inactiveAnimation = curAxisJson["InactiveAnimation"].asString();
				}
				if (curAxisJson.isMember("ActiveAction") && curAxisJson["ActiveAction"].isString())
				{
					axis.activeAction = im.GetAction(curAxisJson["ActiveAction"].asString().c_str());
				}
				if (curAxisJson.isMember("InactiveAction") && curAxisJson["InactiveAction"].isString())
				{
					axis.inactiveAction = im.GetAction(curAxisJson["InactiveAction"].asString().c_str());
				}
				if (curAxisJson.isMember("ChangeAction") && curAxisJson["ChangeAction"].isString())
				{
					axis.changeAction = curAxisJson["ChangeAction"].asString();
				}
				if (curAxisJson.isMember("ActionActiveValue") && curAxisJson["ActionActiveValue"].isDouble())
				{
					axis.actionActiveValue = static_cast<float>(curAxisJson["ActionActiveValue"].asDouble());
				}

				//轴组的子轴
				if (curAxisJson.isMember("SubAxis") && curAxisJson["SubAxis"].isArray())
				{
					for (unsigned int j = 0; j < curAxisJson["SubAxis"].size(); j++)
					{
						//轴的参数ID
						auto& curSubAxisJson = curAxisJson["SubAxis"][j];
						auto& curSubAxis = axis.axisVec.emplace_back();
						if (curSubAxisJson.isMember("ParamID") && curSubAxisJson["ParamID"].isString())
						{
							curSubAxis.paramID = curSubAxisJson["ParamID"].asString();

							if (axis.uiName.empty())
							{
								axis.uiName = curSubAxis.paramID;
							}
						}
						//默认绑定
						if (curSubAxisJson.isMember("DefaultBinding") && curSubAxisJson["DefaultBinding"].isMember("Type") && curSubAxisJson["DefaultBinding"]["Type"].isString())
						{
							std::string bindingStr = curSubAxisJson["DefaultBinding"]["Type"].asString();
							if (bindingStr == "ActualAxis")
							{
								curSubAxis.defaultBinding.type = BindingInfo::Axis_ActualAxis;
								if (curSubAxisJson["DefaultBinding"].isMember("Axis") && curSubAxisJson["DefaultBinding"]["Axis"].isString())
								{

									curSubAxis.defaultBinding.controllList.push_back(curSubAxisJson["DefaultBinding"]["Axis"].asString());

								}
								else
								{
									SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Warn: No value or incorrect value in [Axis][%d][DefaultBinding][Axis]", i);
								}
							}
							else if (bindingStr == "ActualButtonToAxis")
							{
								curSubAxis.defaultBinding.type = BindingInfo::Axis_ActualButtonToAxis;
								if (curSubAxisJson["DefaultBinding"].isMember("Button") && curSubAxisJson["DefaultBinding"]["Button"].isArray())
								{
									for (unsigned int buttonNameIndex = 0; buttonNameIndex < curSubAxisJson["DefaultBinding"]["Button"].size(); buttonNameIndex++)
									{
										if (curSubAxisJson["DefaultBinding"]["Button"][buttonNameIndex].isString())
										{
											curSubAxis.defaultBinding.controllList.push_back(curSubAxisJson["DefaultBinding"]["Button"][buttonNameIndex].asString());
										}
										else
										{
											SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Warn: No value or incorrect value in [Axis][%d][DefaultBinding][Button][%d]", i, buttonNameIndex);
										}
									}
								}
								if (curSubAxisJson["DefaultBinding"].isMember("ActiveValue") && curSubAxisJson["DefaultBinding"]["ActiveValue"].isDouble())
								{
									curSubAxis.defaultBinding.controlValue = static_cast<float>(curSubAxisJson["DefaultBinding"]["ActiveValue"].asDouble());
								}
							}
						}

					}
				}

				if (axis.uiName.empty())
				{
					axis.uiName = "Unknown Axis";
				}
	

				//HandControl
				{
					if (curAxisJson.isMember("Hand") && curAxisJson["Hand"].isString()
						&& (curAxisJson["Hand"].asString() == "right" || curAxisJson["Hand"].asString() == "Right"))
					{
						axis.handControl.handIndex = HandControl::RIGHT;
					}
					else
					{
						axis.handControl.handIndex = HandControl::LEFT;
					}
					if (curAxisJson.isMember("HandPos") && curAxisJson["HandPos"].isString())
					{
						axis.handControl.handPosHandle = _model->GetHandHandle(curAxisJson["HandPos"].asString());
					}
					if (curAxisJson.isMember("HandMoveType") && curAxisJson["HandMoveType"].isString())
					{
						const auto handMoveTypeStr = curAxisJson["HandMoveType"].asString();
						if (handMoveTypeStr == "Mouse")
						{
							axis.handControl.moveType = HandControl::MOUSE;
						}
						else if (handMoveTypeStr == "Stick")
						{
							axis.handControl.moveType = HandControl::STICK;
						}
						else if (handMoveTypeStr == "Moving")
						{
							axis.handControl.moveType = HandControl::MOVING;
						}
					}
					if (curAxisJson.isMember("HandWeight") && curAxisJson["HandWeight"].isDouble())
					{
						const auto handMoveTypeStr = curAxisJson["HandWeight"].asDouble();
						axis.handControl.handWeight = static_cast<float>(handMoveTypeStr);
					}
				}

			};
		}
		else
		{
			//无轴信息，尝试读取模型参数
			std::vector<std::string> paramVec = _model->GetParamList();
			//判断按钮前缀KEYBOARD BUTTON CTB_KEYBOARD CAT_KEY  ex: CAT_KEY_A

			//TODO:对部分常见组进行直接识别？
			//如识别mouse参数
			for (auto& x : paramVec)
			{
				const char* axisBaseName = InputParser::ParamNameToAxisBaseName(x);
				if (axisBaseName)
				{
					auto& axis = modelAxisVec.emplace_back();
					axis.uiName = axisBaseName;
					axis.axisVec.emplace_back();
					axis.axisVec[0].defaultBinding.type = BindingInfo::Axis_ActualAxis;
					axis.axisVec[0].defaultBinding.controllList.push_back(axisBaseName);
				}
			}
		}
	}


	//动画
	//动画
	//动画
	//动画
	//动画
	//动画
	if (pmodelAnimationVec)
	{
		auto& modelAnimationVec = *pmodelAnimationVec;

		if (descItemInfo.isMember("Animations") && descItemInfo["Animations"].isArray())
		{
			for (unsigned int i = 0; i < descItemInfo["Animations"].size(); i++)
			{
				const Json::Value& curAnimationJson = descItemInfo["Animations"][i];
				auto& animation = modelAnimationVec.emplace_back();
				//动画的UI显示名
				if (curAnimationJson.isMember("UiName"))
					animation.uiName = util::GetStringFromMultiLangJsonNode(curAnimationJson["UiName"]);

				//动画的实际控制名
				if (curAnimationJson.isMember("Name") && curAnimationJson["Name"].isString())
				{
					animation.controlName = curAnimationJson["Name"].asString();
				}
				if (animation.uiName.empty())
				{
					animation.uiName = animation.controlName;
				}

				//动画的预览图路径
				if (curAnimationJson.isMember("ImageFile") && curAnimationJson["ImageFile"].isString())
				{
					animation.imageFile = curAnimationJson["ImageFile"].asString();
				}
				////动画的播放轨道 轨道由模型直接管理
				//if (curAnimationJson.isMember("Track") && curAnimationJson["Track"].isInt())
				//{
				//	animation.track = curAnimationJson["Track"].asInt();
				//}

				//默认绑定
				if (curAnimationJson.isMember("DefaultBinding") && curAnimationJson["DefaultBinding"].isArray())
				{
					for (unsigned int bindingIndex = 0; bindingIndex < curAnimationJson["DefaultBinding"].size(); bindingIndex++)
					{
						auto& curAnimationBindingJson = curAnimationJson["DefaultBinding"][bindingIndex];
						if (curAnimationBindingJson["Type"].isString())
						{
							std::string bindingStr = curAnimationBindingJson["Type"].asString();
							if (bindingStr == "ActualButton")
							{
								auto& curBinding = animation.defaultBinding.emplace_back();
								curBinding.type = BindingInfo::Animation_ActualButton;
								if (curAnimationBindingJson.isMember("Button") && curAnimationBindingJson["Button"].isArray())
								{
									for (unsigned int buttonNameIndex = 0; buttonNameIndex < curAnimationBindingJson["Button"].size(); buttonNameIndex++)
									{
										if (curAnimationBindingJson["Button"][buttonNameIndex].isString())
										{
											curBinding.controllList.push_back(curAnimationBindingJson["Button"][buttonNameIndex].asString());
										}
										else
										{
											SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Warn: No value or incorrect value in [Animations][%d][DefaultBinding][%d][Button][%d]", i, bindingIndex, buttonNameIndex);
										}
									}
								}
							}
							else if (bindingStr == "AxisActive")
							{
								auto& curBinding = animation.defaultBinding.emplace_back();
								curBinding.type = BindingInfo::Animation_ActualAxisActive;
								if (curAnimationBindingJson.isMember("Axis") && curAnimationBindingJson["Axis"].isString())
								{
									curBinding.controllList.push_back(curAnimationBindingJson["Axis"].asString());

									if (curAnimationBindingJson.isMember("ActiveValue") && curAnimationBindingJson["ActiveValue"].isDouble())
									{
										curBinding.controlValue = static_cast<float>(curAnimationBindingJson["ActiveValue"].asDouble());
									}
								}
								else
								{
									SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Warn: No axis name in [Animations][%d][DefaultBinding][%d][Axis]", i, bindingIndex);
								}
							}
							else if (bindingStr == "AxisInactive")
							{
								auto& curBinding = animation.defaultBinding.emplace_back();
								curBinding.type = BindingInfo::Animation_ActualAxisInactive;
								if (curAnimationBindingJson.isMember("Axis") && curAnimationBindingJson["Axis"].isString())
								{
									curBinding.controllList.push_back(curAnimationBindingJson["Axis"].asString());
									if (curAnimationBindingJson.isMember("ActiveValue") && curAnimationBindingJson["ActiveValue"].isDouble())
									{
										curBinding.controlValue = static_cast<float>(curAnimationBindingJson["ActiveValue"].asDouble());
									}
								}
								else
								{
									SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Warn: No axis name in [Animations][%d][DefaultBinding][%d][Axis]", i, bindingIndex);
								}
							}
							else if (bindingStr == "Action")
							{
								auto& curBinding = animation.defaultBinding.emplace_back();
								curBinding.type = BindingInfo::Animation_Action;
								if (curAnimationBindingJson.isMember("Action") && curAnimationBindingJson["Action"].isString())
								{
									curBinding.controllList.push_back(curAnimationBindingJson["Action"].asString());
									//if (curAnimationBindingJson.isMember("ActiveValue") && curAnimationBindingJson["ActiveValue"].isDouble())
									//{
									//	curBinding.controlValue = static_cast<float>(curAnimationBindingJson["ActiveValue"].asDouble());
									//}
								}
								else
								{
									SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Warn: No action name in [Animations][%d][DefaultBinding][%d][Action]", i, bindingIndex);
								}
							}
							else
							{
								SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Warn: Can not match binding type in [Animations][%d][DefaultBinding][%d]: %s", i, bindingIndex, bindingStr.c_str());
							}




						}
					}
				}


			};
		}
		else
		{
			//无动画信息，尝试读取模型动画列表
			//读取除Idle以外的所有待机动画
			std::vector<std::string> animationVec = _model->GetAnimationList();
			//判断按钮前缀KEYBOARD BUTTON CTB_KEYBOARD CAT_KEY  ex: CAT_KEY_A

			for (auto& x : animationVec)
			{
				if (x != "Idle")
				{
					auto& animation = modelAnimationVec.emplace_back();
					animation.uiName = x;
					//动画不提供初始绑定
				}
			}
		}
	}



}

void ModelControl::ApplyControlBindings(std::vector<ModelButtonControl>* modelButtonVec, std::vector<ModelAxisControl>* modelAxisVec, std::vector<ModelAnimationControl>* modelAnimationVec)
{

	if(modelButtonVec)
	for (int i = 0; i < modelButtonVec->size(); i++)
	{
		(*modelButtonVec)[i].binding.RegisterBinding(i);
	}
	if (modelAxisVec)
	for (int i = 0; i < modelAxisVec->size(); i++)
	{
		for (int j = 0; j < (*modelAxisVec)[i].axisVec.size(); j++)
		{
			(*modelAxisVec)[i].axisVec[j].binding.RegisterBinding(i,j);
		}
	}
	if (modelAnimationVec)
	for (int i = 0; i < modelAnimationVec->size(); i++)
	{
		for (auto& animationBinding : (*modelAnimationVec)[i].binding)
			animationBinding.RegisterBinding(i);
	}
}




