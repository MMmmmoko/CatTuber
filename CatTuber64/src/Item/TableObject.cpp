
#include"AppContext.h"
#include"Pack/Pack.h"
#include"Util/Util.h"
#include"Input/InputManager.h"
#include"Input/InputParser.h"

#include"Model/Live2DModelBase.h"

#include"Item/Scene.h"
#include"Item/TableObject.h"
//入参应该是个文件夹或者资源包

//PACK中有一个Model文件夹和一些关于此资源的信息（封面、作者信息等）
bool TableObject::LoadFromPath(const char* u8PackPath, const Json::Value& bindingJson)
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
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"File Load Faied: %s", u8PackPath);
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
	Json::Value desc;
	size_t memSize;
	if (pack.IsFileExist(CATTUBER_MODELRESOURCE_PROPERTIES_FILENAME))
	{
		uint8_t* jsonMem = pack.LoadFile(CATTUBER_MODELRESOURCE_PROPERTIES_FILENAME, &memSize);

		desc = util::BuildJsonFromMem((const char*)jsonMem, memSize);
	}

	bool buttonHandled=false;
	bool axisHandled=false;

	_SetUpControlAndAnimation(desc);
	//如果提供的绑定信息，则直接应用（比如来自程序退出时的自动保存）
	if (!bindingJson.empty())
		_SetUpJsonBinding(bindingJson);
	else
		LoadBinding();
	//此时当前应该使用的绑定已经写入各vec中了，向InputManager里注册各个绑定
	_ApplyControlBindings();
	resourcePath = u8PackPath;
	working = true;
	return true;



	/*
	if (!desc.empty())
	{
		//1读取模型按钮与参数与动画信息？
		// 
		//2读取保存的绑定，无的话读取desc内置的默认绑定，无的话尝试从模型参数构建默认绑定

		//这个函数设置好模型的按钮、轴、动画信息以及默认的绑定信息



		//desc["ItemInfo"]

		//desc["Button"]存放每个按钮建模的UI名、模型的控制参数、参数控制表现、甚至图标路径等信息
		//desc["Animation"]["Name"]
		//desc["Animation"]["Track"]=1;
		// 
		// 
		//desc[DefaultBinding]存放物理按键与建模按键的绑定TableObject.h

		//在这个Loadmodel里即完成绑定的加载。


		// 
		//按钮的UI与控制（告诉软件这个模型提供了多少按键（UI展示），以及这些按键应该如何表现（软件控制））
		//desc["Button"][0]["UiName"]["ch"]="按钮1"
		//desc["Button"][0]["ParamID"]="Keyboard_A"
		//desc["Button"][0]["Icon"]="*.png"/可选
		//desc["Button"][0]["DownAnimation"]="Down"/可选
		//desc["Button"][0]["UpAnimation"]="Up"/可选
		// 
		//
		//desc[DefaultBinding]["ButtonAction"][i]["Mapping"][x]?
		//desc["Axis"][i]["Mapping"][x]


	{
		auto paramVec=_model->GetParamList();
		int buttonIndex = 0;
		int axisIndex = 0;
		for (auto& paramStr : paramVec)
		{
			//统一大写
			ui::StringUtil::UpperString(paramStr);


			//按钮前缀BUTTON_ eg BUTTON_F2
			if (buttonHandled==false&&paramStr.substr(0, sizeof(CATTUBER_MODEL_BUTTON_PARAM_HEAD) - 1) == CATTUBER_MODEL_BUTTON_PARAM_HEAD)
			{
				std::string keyName=paramStr.substr(sizeof(CATTUBER_MODEL_BUTTON_PARAM_HEAD)-1);
				
				std::string actionName = "Table.Button."+ std::to_string(buttonIndex) + ".Down";
				

				
				std::string baseName=input::ButtonNameToBaseName(keyName);

				if (!baseName.empty())
				{
					//能从参数中读取映射的按键的话就直接进行注册
					InputManager::GetIns().RegisterButtonActionBinding(actionName.c_str(),&baseName);
				}
				//InputManager::RegisterButtonActionBinding();
				buttonIndex++;
			}
			
			//轴前缀
			else if (axisHandled == false && paramStr.substr(0, sizeof(CATTUBER_MODEL_AXIS_PARAM_HEAD) - 1) == CATTUBER_MODEL_AXIS_PARAM_HEAD)
			{
				std::string axisName = paramStr.substr(sizeof(CATTUBER_MODEL_AXIS_PARAM_HEAD) - 1);
			
				std::string actionName = "Table.Axis." + std::to_string(axisIndex) + ".Change";
			
				const char* baseName = input::AxisNameToBaseName(axisName);

				if (baseName)
				{
					//能从参数中读取映射的按键的话就直接进行注册
					InputManager::GetIns().RegisterAxisActionBinding(actionName.c_str(), &baseName);
				}
				axisIndex++;
			}

			//无法以默认的形式绑定表情

				
		}
		


	}

	resourcePath = u8PackPath;
	return true;
	*/
}


void TableObject::Update(uint64_t deltaTicksNS)
{
	if (working)
	{
		_model->Update(deltaTicksNS);
		//发送数据给角色？
	}
}


void TableObject::Draw(MixDrawList* drawList)
{
	if (working)
	{
		_model->DrawMix(drawList);
	}
}

Json::Value TableObject::GenerateAttributes()
{
	if (resourcePath.empty())
	{
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION,"Saving a object with NULL pack path!");
		throw(std::runtime_error("Saving a object with NULL pack path!"));
	}
	Json::Value json;
	json["PackPath"] = resourcePath;
	json["Bindings"] = _GenerateJsonBinding();

	//todo/FIXME 补完其他需保存的内容 
	return json;
}

TableObject* TableObject::CreateFromAttributes(const Json::Value& applyJson)
{
	if (!(applyJson.isMember("PackPath")&& applyJson["PackPath"].isString()))
	{
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Create Table with invalid json! No path info exist.");
		return nullptr;
	}
	auto resultObj = new TableObject;
	
	std::string pathStr = AppContext::ResolvePathToAbsolute(applyJson["PackPath"].asString());
	if (!resultObj->LoadFromPath(pathStr.c_str(), applyJson["Bindings"]))
	{
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Can not create Table at path: %s.", pathStr.c_str());
		delete resultObj;
		return nullptr;
	}
	



	return resultObj;
}

void TableObject::ReleaseObj(TableObject* obj)
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

bool TableObject::LoadBindingByName(const char* bindingName)
{
	//preferpath/Bindings/88KTable_0.binding
	//Pack文件名_id 如 88KKeyboard_0 
	
	//构建要寻找的目标路径
	std::string packFileName = util::RemoveExtension((util::GetFileNameFromPath(resourcePath)));
	
	std::string bindingFileFolder = AppContext::GetPrefPath();
	bindingFileFolder = bindingFileFolder + "/Bindings";

	if (!SDL_CreateDirectory(bindingFileFolder.c_str()))
	{
		return false;
	}

	std::string pattern = packFileName + "_*.binding";
	int count;
	char** fileList;
	fileList=SDL_GlobDirectory(bindingFileFolder.c_str(), pattern.c_str(),0,&count);
	if (!fileList)
	{
		return false;
	}
	
	for (int i = 0; i < count; i++)
	{
		Json::Value json= util::BuildJsonFromFile(fileList[i]);
		//先匹配绑定名
		if (json.isMember("BindingName") && json["BindingName"].isString())
		{
			if (json["BindingName"].asString() == bindingName)
			{
				//绑定名匹配，查看路径是否匹配
				if (json.isMember("PackPath") && json["PackPath"].isString())
				{
					std::string packPathInJson=json["PackPath"].asString();
					packPathInJson=AppContext::ResolvePathToAbsolute(packPathInJson);
					if (util::PathEqual(packPathInJson, resourcePath))
					{

						SDL_LogInfo(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION,"Binding match success, Load at %s", packPathInJson.c_str());
						_SetUpJsonBinding(json);
						SDL_free(fileList);
						return true;
					}
				}
			}
		}
	}

	SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Can not find binding %s, pack path: %s", bindingName, resourcePath.c_str());
	SDL_free(fileList);
	return false;

}

void TableObject::LoadBinding()
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
		x.binding=x.defaultBinding;
	}
	for (auto& x : modelAxisVec)
	{
		x.binding = x.defaultBinding;
	}
	for (auto& x : modelAnimationVec)
	{
		x.binding = x.defaultBinding;
	}
}

void TableObject::ClearBinding()
{
	for (auto& x : modelButtonVec)
	{
		x.binding.UnRegisterBinding();
	}
	for (auto& x : modelAxisVec)
	{
		x.binding.UnRegisterBinding();
	}	
	for (auto& x : modelAnimationVec)
	{
		for(auto&y:x.binding)
			y.UnRegisterBinding();
	}



}

//在从文件加载模型时调用，如果json无效则尝试直接在模型参数中读取
//按钮的UI与控制（告诉软件这个模型提供了多少按键（UI展示），以及这些按键应该如何表现（软件控制））
//desc["Button"][0]["UiName"]["ch"]="按钮1"
//desc["Button"][0]["ParamID"]="Keyboard_A"
//desc["Button"][0]["Icon"]="*.png"/可选
//desc["Button"][0]["DownAnimation"]="Down"/可选
//desc["Button"][0]["UpAnimation"]="Up"/可选
//desc["Button"][0]["DefaultBinding"][Type]="KeyBoardButton"
//desc["Button"][0]["DefaultBinding"][Button][0]="Keyborad.A"
void TableObject::_SetUpControlAndAnimation(const Json::Value& descItemInfo)
{
	//传入的是模型desc.json的desc["Controls"]
	auto& im=InputManager::GetIns();
	//按钮
	//按钮
	//按钮
	//按钮
	//按钮
	if (descItemInfo.isMember("Buttons") && descItemInfo["Buttons"].isArray())
	{
		for (unsigned int i = 0; i < descItemInfo["Buttons"].size(); i++)
		{
			const Json::Value& curButtonJson = descItemInfo["Buttons"][i];
			auto& button = modelButtonVec.emplace_back();
			//按钮的显示名
			if(curButtonJson.isMember("UiName"))
				button.uiName =  util::GetStringFromMultiLangJsonNode(curButtonJson["UiName"]);
			//按钮的参数ID
			if (curButtonJson.isMember("ParamID") && curButtonJson["ParamID"].isString())
			{
				button.paramID = curButtonJson["ParamID"].asString();
			}

			if (button.uiName.empty())
			{
				button.uiName = button.paramID;
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
				button.downAction = im.GetAction(  curButtonJson["DownAction"].asString().c_str());
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
								SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION,"Warn: No value or incorrect value in [Button][%d][DefaultBinding][Button][%d]",i, buttonNameIndex);
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
							button.defaultBinding.controlValue= static_cast<float>(curButtonJson["DefaultBinding"]["ActiveValue"].asDouble());
						}
					}
					else
					{
						SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Warn: No value or incorrect value in [Button][%d][DefaultBinding][Axis]",i);
					}
				}

				//轴绑定不写在Button里
			}
			
			



		}
	}
	else
	{

		std::vector<std::string> paramVec=_model->GetParamList();
		//判断按钮前缀KEYBOARD BUTTON CTB_KEYBOARD CAT_KEY  ex: CAT_KEY_A

		for (auto&  x: paramVec)
		{
			const char* keyBaseName = InputParser::ParamNameToButtonBaseName(x);
			if (keyBaseName)
			{
				auto& button = modelButtonVec.emplace_back();
				button.uiName = InputParser::ButtonBaseNameToUIName(keyBaseName);
				button.defaultBinding.type = BindingInfo::Button_ActualButton;
				button.defaultBinding.controllList.push_back(keyBaseName);
			}
		}
	}


	//轴
	//轴
	//轴
	//轴
	//轴
	if (descItemInfo.isMember("Axes") && descItemInfo["Axes"].isArray())
	{
		for (unsigned int i = 0; i < descItemInfo["Axes"].size(); i++)
		{
			const Json::Value& curAxisJson = descItemInfo["Axes"][i];
			auto& axis = modelAxisVec.emplace_back();
			//按钮的显示名
			if (curAxisJson.isMember("UiName"))
				axis.uiName = util::GetStringFromMultiLangJsonNode(curAxisJson["UiName"]);

			//轴的参数ID
			if (curAxisJson.isMember("ParamID") && curAxisJson["ParamID"].isString())
			{
				axis.paramID = curAxisJson["ParamID"].asString();
			}
			if (axis.uiName.empty())
			{
				axis.uiName = axis.paramID;
			}


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
				axis.activeAction =im.GetAction(curAxisJson["ActiveAction"].asString().c_str());
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

			//默认绑定
			if (curAxisJson.isMember("DefaultBinding") && curAxisJson["DefaultBinding"].isMember("Type") && curAxisJson["DefaultBinding"]["Type"].isString())
			{
				std::string bindingStr = curAxisJson["DefaultBinding"]["Type"].asString();
				if (bindingStr == "ActualAxis")
				{
					axis.defaultBinding.type = BindingInfo::Axis_ActualAxis;
					if (curAxisJson["DefaultBinding"].isMember("Axis") && curAxisJson["DefaultBinding"]["Axis"].isString())
					{

						axis.defaultBinding.controllList.push_back(curAxisJson["DefaultBinding"]["Axis"].asString());

					}
					else
					{
						SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Warn: No value or incorrect value in [Axis][%d][DefaultBinding][Axis]", i);
					}
				}
				else if (bindingStr == "ActualButtonToAxis")
				{
					axis.defaultBinding.type = BindingInfo::Axis_ActualButtonToAxis;
					if (curAxisJson["DefaultBinding"].isMember("Button") && curAxisJson["DefaultBinding"]["Button"].isArray())
					{
						for (unsigned int buttonNameIndex = 0; buttonNameIndex < curAxisJson["DefaultBinding"]["Button"].size(); buttonNameIndex++)
						{
							if (curAxisJson["DefaultBinding"]["Button"][buttonNameIndex].isString())
							{
								axis.defaultBinding.controllList.push_back(curAxisJson["DefaultBinding"]["Button"][buttonNameIndex].asString());
							}
							else
							{
								SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Warn: No value or incorrect value in [Axis][%d][DefaultBinding][Button][%d]", i, buttonNameIndex);
							}
						}
					}
					if (curAxisJson["DefaultBinding"].isMember("ActiveValue") && curAxisJson["DefaultBinding"]["ActiveValue"].isDouble())
					{
						axis.defaultBinding.controlValue = static_cast<float>(curAxisJson["DefaultBinding"]["ActiveValue"].asDouble());
					}
				}
			}


		};
	}
	else
	{
		//无轴信息，尝试读取模型参数
		std::vector<std::string> paramVec = _model->GetParamList();
		//判断按钮前缀KEYBOARD BUTTON CTB_KEYBOARD CAT_KEY  ex: CAT_KEY_A

		for (auto& x : paramVec)
		{
			const char* axisBaseName = InputParser::ParamNameToAxisBaseName(x);
			if (axisBaseName)
			{
				auto& axis = modelAxisVec.emplace_back();
				axis.uiName = axisBaseName;
				axis.defaultBinding.type = BindingInfo::Axis_ActualAxis;
				axis.defaultBinding.controllList.push_back(axisBaseName);
			}
		}
	}



	//动画
	//动画
	//动画
	//动画
	//动画
	//动画
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
						else if (bindingStr=="AxisInactive")
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
						else if (bindingStr=="Action")
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

void TableObject::_ApplyControlBindings()
{
	//std::vector<ModelButtonControl> modelButtonVec;
	//std::vector<ModelAxisControl> modelAxisVec;
	//std::vector<ModelAnimationControl> modelAnimationVec;
	//将上面vec中存储的binding注册到inputmanager里
	
	for (int i=0;i< modelButtonVec.size();i++)
	{
		modelButtonVec[i].binding.RegisterBinding(i);
	}
	
	for (int i=0;i< modelAxisVec.size();i++)
	{
		modelAxisVec[i].binding.RegisterBinding(i);
	}
	
	for (int i=0;i< modelAnimationVec.size();i++)
	{
		for (auto& animationBinding : modelAnimationVec[i].binding)
			animationBinding.RegisterBinding(i);
	}

}

void TableObject::_SetUpJsonBinding(const Json::Value& bindingJson)
{

	if (bindingJson.isMember("Buttons") && bindingJson["Buttons"].isArray())
	{
		//i不可大于按钮数
		for (unsigned int i = 0; i < bindingJson["Buttons"].size()&&i< modelButtonVec.size(); i++)
		{
			auto& curButtonBindingJson=bindingJson["Buttons"][i];

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

	if (bindingJson.isMember("Axes") && bindingJson["Axes"].isArray())
	{
		//i不可大于轴数
		for (unsigned int i = 0; i < bindingJson["Axes"].size() && i < modelAxisVec.size(); i++)
		{
			auto& curAxisBindingJson = bindingJson["Axes"][i];
			if (curAxisBindingJson.isMember("Type") && curAxisBindingJson["Type"].isString())
			{
				std::string bindingStr = curAxisBindingJson["Type"].asString();
				if (bindingStr == "ActualAxis")
				{
					modelAxisVec[i].binding.type = BindingInfo::Axis_ActualAxis;
					if (curAxisBindingJson.isMember("Axis") && curAxisBindingJson["Axis"].isString())
					{

						modelAxisVec[i].binding.controllList.push_back(curAxisBindingJson["Axis"].asString());

					}
					else
					{
						SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Warn: No value or incorrect value in [Axes][%d][ActualAxis][Axis]", i);
					}
				}
				else if (bindingStr == "ActualButtonToAxis")
				{
					modelAxisVec[i].binding.type = BindingInfo::Axis_ActualButtonToAxis;
					if (curAxisBindingJson.isMember("Button") && curAxisBindingJson["Button"].isArray())
					{
						for (unsigned int buttonNameIndex = 0; buttonNameIndex < curAxisBindingJson["Button"].size(); buttonNameIndex++)
						{
							if (curAxisBindingJson["Button"][buttonNameIndex].isString())
							{
								modelAxisVec[i].binding.controllList.push_back(curAxisBindingJson["Button"][buttonNameIndex].asString());
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


	//Animations
	//Animations
	//Animations
	if (bindingJson.isMember("Animations") && bindingJson["Animations"].isArray())
	{
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

Json::Value TableObject::_GenerateJsonBinding()
{
	Json::Value bindingRoot;


	for (int i = 0; i < modelButtonVec.size(); i++)
	{
		auto& curButton=bindingRoot["Buttons"][i];
		auto& curBinding = modelButtonVec[i].binding;
		if (curBinding.type == BindingInfo::Button_ActualButton)
		{
			curButton["Type"] = "ActualButton";
			for (int buttonIndex = 0; buttonIndex < curBinding.controllList.size(); buttonIndex++)
			{
				curButton["Button"][i] = curBinding.controllList[i];
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
	for (int i = 0; i < modelAxisVec.size(); i++)
	{

		auto& curAxis = bindingRoot["Axes"][i];
		auto& curBinding = modelAxisVec[i].binding;
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






	return bindingRoot;
}

//具体action到函数成员方法
void TableObject::RegisterAllActionFunc(bool falseToUnregister)
{
	//模型控件相关
	auto& im = InputManager::GetIns();
	{
		ActionCallback downActionCallBack;
		downActionCallBack.userData = this;
		downActionCallBack.callback = [](const char* actionName, float value, void* userData, void* userData2)
			{
				((TableObject*)userData)->OnButtonDown(UTIL_GETLOW32VALUE(userData2));
			};

		ActionCallback upActionCallBack;
		upActionCallBack.userData = this;
		upActionCallBack.callback = [](const char* actionName, float value, void* userData, void* userData2)
			{
				((TableObject*)userData)->OnButtonUp(UTIL_GETLOW32VALUE(userData2));
			};

		for (int i = 0; i < modelButtonVec.size(); i++)
		{
			std::string downActionName = "Table.Button." + std::to_string(i) + ".Down";
			std::string upActionName = "Table.Button." + std::to_string(i) + ".Up";

			UTIL_SETLOW32VALUE(downActionCallBack.userData2,i);
			UTIL_SETLOW32VALUE(upActionCallBack.userData2,i);
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
		axisActionCallBack.callback = [](const char* actionName, float value, void* userData, void* userData2)
			{
				((TableObject*)userData)->OnAxisValueChange(UTIL_GETLOW32VALUE(userData2), value);
			};

		for (int i = 0; i < modelAxisVec.size(); i++)
		{
			std::string axisActionName = "Table.Axis." + std::to_string(i) + ".Change";
			UTIL_SETLOW32VALUE( axisActionCallBack.userData2,i);
			if(falseToUnregister)
			im.RegisterActionCallback(axisActionName.c_str(), axisActionCallBack);
			else
				im.UnregisterActionCallback(axisActionName.c_str(), axisActionCallBack);
		}

	}

	{
		ActionCallback animationActionCallBack;
		animationActionCallBack.userData = this;
		animationActionCallBack.callback = [](const char* actionName, float value, void* userData, void* userData2)
			{
				((TableObject*)userData)->OnAnimationPlay(UTIL_GETLOW32VALUE(userData2));
			};

		for (int i = 0; i < modelAnimationVec.size(); i++)
		{
			std::string animationActionName = "Table.Animation." + std::to_string(i) + ".Start";
			UTIL_SETLOW32VALUE(animationActionCallBack.userData2, i);
			if(falseToUnregister)
			im.RegisterActionCallback(animationActionName.c_str(), animationActionCallBack);
			else
				im.UnregisterActionCallback(animationActionName.c_str(), animationActionCallBack);

		}
	}
}

void TableObject::UnregisterAllActionFunc()
{
	RegisterAllActionFunc(false);
}

void TableObject::OnButtonDown(int btnIndex)
{
	if (btnIndex >= modelButtonVec.size())return;

	auto& curButton = modelButtonVec[btnIndex];
	curButton.isDown = true;
	if (!curButton.downAnimation.empty())
		_model->PlayAnimation(curButton.downAnimation);
	if (curButton.downAction)
		InputManager::GetIns().RiseAction(curButton.downAction);
}

void TableObject::OnButtonUp(int btnIndex)
{
	if (btnIndex >= modelButtonVec.size())return;

	auto& curButton = modelButtonVec[btnIndex];
	curButton.isDown = false;
	curButton.upTickMs=SDL_GetTicks();

	if (!curButton.upAnimation.empty())
		_model->PlayAnimation(curButton.upAnimation);
	if (curButton.upAction)
		InputManager::GetIns().RiseAction(curButton.upAction);


}

void TableObject::OnAxisValueChange(int axisIndex, float value)
{
	if (axisIndex >= modelAxisVec.size())return;
	
	auto& curAxis=modelAxisVec[axisIndex];
	
	if (curAxis.actionActiveValue > 0)
	{
		if (value > curAxis.actionActiveValue && curAxis.value <= curAxis.actionActiveValue)
		{
			//active
			if(!curAxis.activeAnimation.empty())
				_model->PlayAnimation(curAxis.activeAnimation);
			if(curAxis.activeAction)
				InputManager::GetIns().RiseAction(curAxis.activeAction);
		}
		else if (value < curAxis.actionActiveValue && curAxis.value >= curAxis.actionActiveValue)
		{
			//inactive
			if (!curAxis.inactiveAnimation.empty())
				_model->PlayAnimation(curAxis.inactiveAnimation);
			if (curAxis.inactiveAction)
				InputManager::GetIns().RiseAction(curAxis.inactiveAction);
		}
	}
	else
	{
		if (value < curAxis.actionActiveValue && curAxis.value >= curAxis.actionActiveValue)
		{
			//active
			if (!curAxis.activeAnimation.empty())
				_model->PlayAnimation(curAxis.activeAnimation);
			if (curAxis.activeAction)
				InputManager::GetIns().RiseAction(curAxis.activeAction);
		}
		else if (value > curAxis.actionActiveValue && curAxis.value <= curAxis.actionActiveValue)
		{
			//inactive
			if (!curAxis.inactiveAnimation.empty())
				_model->PlayAnimation(curAxis.inactiveAnimation);
			if (curAxis.inactiveAction)
				InputManager::GetIns().RiseAction(curAxis.inactiveAction);
		}
	}

	curAxis.value = value;
}

void TableObject::OnAnimationPlay(int animationIndex)
{
	if (animationIndex >= modelAnimationVec.size())return;

	if (!modelAnimationVec[animationIndex].controlName.empty())
	{
		_model->PlayAnimation(modelAnimationVec[animationIndex].controlName);
	}

}




//物理按键到具体action
void BindingInfo::RegisterBinding(int index)
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
		//构造actionname  Table.Axis.0.Change
		std::string axisActionName= "Table.Axis."+ std::to_string(index) + ".Change";
		_bindingHandleList.push_back(
		im.RegisterAxisChangeActionBinding(axisActionName.c_str(), controllList[0].c_str()));
		break;
	}
	case BindingInfo::Axis_ActualButtonToAxis:
	{
		//构造actionname  Table.Axis.0.Change
		std::string axisActionName = "Table.Axis." + std::to_string(index) + ".Change";

		_bindingHandleList.push_back(
			im.RegisterButtonToAxisActionBinding(axisActionName.c_str(), controllList[0].c_str(),controlValue));
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
	if(type< TypeMax&& type>= Undefined)
		return strList[type];
	return strList[Undefined];
}
