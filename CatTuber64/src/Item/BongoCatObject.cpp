#include"AppContext.h"
#include"Util/Util.h"
#include"Input/InputManager.h"
#include"Input/InputParser.h"

#include"Model/Live2DModelBase.h"

#include"Item/Scene.h"
#include"Item/BongoCatObject.h"



#if 0

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




	if (!_model)
	{
		_model = BongoCatModel::LoadFromPack(u8PackPath);
		if (!_model)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Can not create model from : %s", u8PackPath);
			return false;
		}
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


	bool buttonHandled = false;
	bool axisHandled = false;


	//ModelControl::SetUpDefaultControl(desc, _model, &modelButtonVec, &modelAxisVec, &modelAnimationVec);
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

void BongoCatObject::Update(uint64_t deltaTicksNS)
{
	//处理面捕数据？

	if (working&&_model)
	{

		switch (_model->GetBongoCatMode())
		{
		case BongoCatModel::BongoCatMverMode_Standard:
		{
			bool anyButtonDown = false;
			for (auto& button : modelButtonVec)
			{
				if (button.isDown)
				{
					anyButtonDown = true;
					_model->SetParamValue(button.paramHandle,1.f);
				}

			}
		
		
		
		}
			//_Update_Standard(deltaTicksNS);
			break;
		case BongoCatModel::BongoCatMverMode_Keyboard:
			_Update_Keyboard(deltaTicksNS);
			break;
		case BongoCatModel::BongoCatMverMode_Gamepad:
			_Update_Gamepad(deltaTicksNS);
			break;
		default:
			break;
		}
	}
}


//void BongoCatObject::Draw(MixDrawList* drawList)
//{
//	if (working)
//	{
//		_model->DrawMix(drawList);
//	}
//}

void BongoCatObject::Draw()
{
	if (working)
	{
		_model->Draw();
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

void BongoCatObject::SetUpDefaultControl(Json::Value& desc, Json::Value& config)
{
	//填充
	//std::vector<ModelButtonControl> modelButtonVec;
	//std::vector<ModelAxisControl> modelAxisVec;
	//std::vector<ModelAnimationControl> modelAnimationVec;

	for ( int i=0;i< _model->leftHandKeyVec.size();i++)
	{
		auto& key = _model->leftHandKeyVec[i];
		auto& curButton = modelButtonVec.emplace_back();
		for (int j = 0; j < key.size(); j++)
		{

			const char* curKey=InputParser::BongoCatKeyToButtonBaseName(key[i], _model->mode == BongoCatModel::BongoCatMverMode_Gamepad);
			if (j == 0)
				curButton.uiName = curKey;
			else
				curButton.uiName = curButton.uiName + "+" + curKey;
			curButton.defaultBinding.controllList.push_back(curKey);
		}


		curButton.paramID = "Key_L" + std::to_string(i);
		curButton.paramHandle = _model->GetParamHandle(curButton.paramID);
		curButton.defaultBinding.type = BindingInfo::Button_ActualButton;

		curButton.handControl.handIndex = HandControl::LEFT;
	}

	for (int i = 0; i < _model->rightHandKeyVec.size(); i++)
	{
		auto& key = _model->rightHandKeyVec[i];
		auto& curButton = modelButtonVec.emplace_back();
		for (int j = 0; j < key.size(); j++)
		{
			const char* curKey=InputParser::BongoCatKeyToButtonBaseName(key[i], _model->mode == BongoCatModel::BongoCatMverMode_Gamepad);
			if (j == 0)
				curButton.uiName = curKey;
			else
				curButton.uiName = curButton.uiName + "+" + curKey;
			curButton.defaultBinding.controllList.push_back(curKey);
		}
		curButton.paramID = "Key_R" + std::to_string(i);
		curButton.paramHandle = _model->GetParamHandle(curButton.paramID);
		curButton.defaultBinding.type = BindingInfo::Button_ActualButton;

		curButton.handControl.handIndex = HandControl::RIGHT;
	}
	




	//for (int i = 0; i < _model->keyboardKeyVec.size(); i++)
	//{
	//	auto& key = _model->keyboardKeyVec[i];
	//	auto& curButton = modelButtonVec.emplace_back();
	//	for (int j = 0; j < key.size(); j++)
	//	{

	//		const char* curKey = InputParser::BongoCatKeyToButtonBaseName(key[i], _model->mode == BongoCatModel::BongoCatMverMode_Gamepad);
	//		if (j == 0)
	//			curButton.uiName = curKey;
	//		else
	//			curButton.uiName = curButton.uiName + "+" + curKey;
	//		curButton.defaultBinding.controllList.push_back(curKey);
	//	}

	//	//RegisterAllActionFunc
	//	//curButton.downAction;
	//}
}



void BongoCatObject::RegisterAllActionFunc(bool falseToUnregister)
{
	//模型控件相关
	auto& im = InputManager::GetIns();



	//{
	//	ActionCallback bongoCatKeyDown;
	//}

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






#else 

//索引为BongoCat键码
//static ButtonProxy bcmbutton[256] = {};
//static bool bcmbuttonInited = false;

BongoCatObject::~BongoCatObject()
{
	for (auto x : audioTrackResource)
	{
		MIX_DestroyTrack(x);
	}
	for (auto x : audioSoundResource)
	{
		MIX_DestroyAudio(x);
	}
	audioTrackResource.clear();
	audioSoundResource.clear();

	MIX_DestroyTrack(mainTrack);
	mainTrack = nullptr;

}

bool BongoCatObject::LoadFromPath(const char* u8PackPath, const Json::Value& bindingJson)
{
#if 0
	//填充BongoCat按钮
	if (!bcmbuttonInited)
	{

		auto& im=InputManager::GetIns();
		bcmbutton[0x01] = im.GetButton("Mouse.Left");
		bcmbutton[0x02] = im.GetButton("Mouse.Right");
		bcmbutton[0x04] = im.GetButton("Mouse.Middle");
		bcmbutton[0x05] = im.GetButton("Mouse.X1");
		bcmbutton[0x06] = im.GetButton("Mouse.X2");

		bcmbutton[0x08] = im.GetButton("Keyboard.BACKSPACE");
		bcmbutton[0x09] = im.GetButton("Keyboard.TAB");

		//bcmbutton[0x0C] = VK_CLEAR;
		bcmbutton[0x0D] = im.GetButton("Keyboard.RETURN");

		//默认左shift
		bcmbutton[0x10] = im.GetButton("Keyboard.LSHIFT");
		bcmbutton[0x11] = im.GetButton("Keyboard.LCTRL");
		bcmbutton[0x12] = im.GetButton("Keyboard.LALT");
		bcmbutton[0x13] = im.GetButton("Keyboard.MEDIA_PLAYPAUSE");
		bcmbutton[0x14] = im.GetButton("Keyboard.CAPSLOCK");
		bcmbutton[0x15] = im.GetButton("Keyboard.KANA");
		//bcmbutton[0x16] = VK_IME_ON
		//bcmbutton[0x15] = VK_JUNJA;
		//bcmbutton[0x18] = VK_FINAL;
		//bcmbutton[0x19] = VK_KANJI;
		//bcmbutton[0x1A] = VK_IME_OFF;

		bcmbutton[0x1B] = im.GetButton("Keyboard.ESC");

		bcmbutton[0x20] = im.GetButton("Keyboard.SPACE");
		bcmbutton[0x21] = im.GetButton("Keyboard.PAGEUP");
		bcmbutton[0x22] = im.GetButton("Keyboard.PAGEDOWN");
		bcmbutton[0x23] = im.GetButton("Keyboard.END");
		bcmbutton[0x24] = im.GetButton("Keyboard.HOME");
		bcmbutton[0x25] = im.GetButton("Keyboard.LEFT");
		bcmbutton[0x26] = im.GetButton("Keyboard.UP");
		bcmbutton[0x27] = im.GetButton("Keyboard.RIGHT");
		bcmbutton[0x28] = im.GetButton("Keyboard.DOWN");

		bcmbutton[0x2C] = im.GetButton("Keyboard.SYSREQ");
		bcmbutton[0x2D] = im.GetButton("Keyboard.INSERT");
		bcmbutton[0x2E] = im.GetButton("Keyboard.DELETE");
		bcmbutton[0x2F] = im.GetButton("Keyboard.INSERT");

		bcmbutton[0x30] = im.GetButton("Keyboard.0");
		bcmbutton[0x31] = im.GetButton("Keyboard.1");
		bcmbutton[0x32] = im.GetButton("Keyboard.2");
		bcmbutton[0x33] = im.GetButton("Keyboard.3");
		bcmbutton[0x34] = im.GetButton("Keyboard.4");
		bcmbutton[0x35] = im.GetButton("Keyboard.5");
		bcmbutton[0x36] = im.GetButton("Keyboard.6");
		bcmbutton[0x37] = im.GetButton("Keyboard.7");
		bcmbutton[0x38] = im.GetButton("Keyboard.8");
		bcmbutton[0x39] = im.GetButton("Keyboard.9");

		bcmbutton[0x41] = im.GetButton("Keyboard.A");
		bcmbutton[0x42] = im.GetButton("Keyboard.B");
		bcmbutton[0x43] = im.GetButton("Keyboard.C");
		bcmbutton[0x44] = im.GetButton("Keyboard.D");
		bcmbutton[0x45] = im.GetButton("Keyboard.E");
		bcmbutton[0x46] = im.GetButton("Keyboard.F");
		bcmbutton[0x47] = im.GetButton("Keyboard.G");
		bcmbutton[0x48] = im.GetButton("Keyboard.H");
		bcmbutton[0x49] = im.GetButton("Keyboard.I");
		bcmbutton[0x4A] = im.GetButton("Keyboard.J");
		bcmbutton[0x4B] = im.GetButton("Keyboard.K");
		bcmbutton[0x4C] = im.GetButton("Keyboard.L");
		bcmbutton[0x4D] = im.GetButton("Keyboard.M");
		bcmbutton[0x4E] = im.GetButton("Keyboard.N");
		bcmbutton[0x4F] = im.GetButton("Keyboard.O");
		bcmbutton[0x50] = im.GetButton("Keyboard.P");
		bcmbutton[0x51] = im.GetButton("Keyboard.Q");
		bcmbutton[0x52] = im.GetButton("Keyboard.R");
		bcmbutton[0x53] = im.GetButton("Keyboard.S");
		bcmbutton[0x54] = im.GetButton("Keyboard.T");
		bcmbutton[0x55] = im.GetButton("Keyboard.U");
		bcmbutton[0x56] = im.GetButton("Keyboard.V");
		bcmbutton[0x57] = im.GetButton("Keyboard.W");
		bcmbutton[0x58] = im.GetButton("Keyboard.X");
		bcmbutton[0x59] = im.GetButton("Keyboard.Y");
		bcmbutton[0x5A] = im.GetButton("Keyboard.Z");

		bcmbutton[0x5B] = im.GetButton("Keyboard.LWIN");
		bcmbutton[0x5C] = im.GetButton("Keyboard.RWIN");
		bcmbutton[0x5D] = im.GetButton("Keyboard.RWIN");
		bcmbutton[0x5F] = im.GetButton("Keyboard.SLEEP");
		bcmbutton[0x60] = im.GetButton("Keyboard.KP_0");
		bcmbutton[0x61] = im.GetButton("Keyboard.KP_1");
		bcmbutton[0x62] = im.GetButton("Keyboard.KP_2");
		bcmbutton[0x63] = im.GetButton("Keyboard.KP_3");
		bcmbutton[0x64] = im.GetButton("Keyboard.KP_4");
		bcmbutton[0x65] = im.GetButton("Keyboard.KP_5");
		bcmbutton[0x66] = im.GetButton("Keyboard.KP_6");
		bcmbutton[0x67] = im.GetButton("Keyboard.KP_7");
		bcmbutton[0x68] = im.GetButton("Keyboard.KP_8");
		bcmbutton[0x69] = im.GetButton("Keyboard.KP_9");
		bcmbutton[0x6A] = im.GetButton("Keyboard.KP_MULTIPLY");
		bcmbutton[0x6B] = im.GetButton("Keyboard.KP_PLUS");
		//bcmbutton[0x6C] = SEPARATOR;
		bcmbutton[0x6D] = im.GetButton("Keyboard.KP_MINUS");
		bcmbutton[0x6E] = im.GetButton("Keyboard.KP_PERIOD");
		bcmbutton[0x6F] = im.GetButton("Keyboard.KP_DIVIDE");
		bcmbutton[0X70] = im.GetButton("Keyboard.F1");
		bcmbutton[0X71] = im.GetButton("Keyboard.F2");
		bcmbutton[0X72] = im.GetButton("Keyboard.F3");
		bcmbutton[0X73] = im.GetButton("Keyboard.F4");
		bcmbutton[0X74] = im.GetButton("Keyboard.F5");
		bcmbutton[0X75] = im.GetButton("Keyboard.F6");
		bcmbutton[0X76] = im.GetButton("Keyboard.F7");
		bcmbutton[0X77] = im.GetButton("Keyboard.F8");
		bcmbutton[0X78] = im.GetButton("Keyboard.F9");
		bcmbutton[0X79] = im.GetButton("Keyboard.F10");
		bcmbutton[0X7A] = im.GetButton("Keyboard.F11");
		bcmbutton[0X7B] = im.GetButton("Keyboard.F12");
		bcmbutton[0X7C] = im.GetButton("Keyboard.F13");
		bcmbutton[0X7D] = im.GetButton("Keyboard.F14");
		bcmbutton[0X7E] = im.GetButton("Keyboard.F15");
		//bcmbutton[0X7F] = im.GetButton("Keyboard.F16");
		//bcmbutton[0X80] = im.GetButton("Keyboard.F17");
		//bcmbutton[0X81] = im.GetButton("Keyboard.F18");
		//bcmbutton[0X82] = im.GetButton("Keyboard.F19");
		//...

		bcmbutton[0X90] = im.GetButton("Keyboard.NUMLOCK");
		bcmbutton[0X91] = im.GetButton("Keyboard.SCROLLLOCK");

		bcmbutton[0XA0] = im.GetButton("Keyboard.LSHIFT");
		bcmbutton[0XA1] = im.GetButton("Keyboard.RSHIFT");
		bcmbutton[0XA2] = im.GetButton("Keyboard.LCTRL");
		bcmbutton[0XA3] = im.GetButton("Keyboard.RCTRL");
		bcmbutton[0XA4] = im.GetButton("Keyboard.LALT");
		bcmbutton[0XA5] = im.GetButton("Keyboard.RALT");
		bcmbutton[0XA6] = im.GetButton("Keyboard.WEBBACK");
		bcmbutton[0XA7] = im.GetButton("Keyboard.WEBFORWARD");
		bcmbutton[0XA8] = im.GetButton("Keyboard.WEBREFRESH");
		bcmbutton[0XA9] = im.GetButton("Keyboard.WEBSTOP");
		bcmbutton[0XAA] = im.GetButton("Keyboard.WEBSEARCH");
		bcmbutton[0XAB] = im.GetButton("Keyboard.BOOKMARKS");
		bcmbutton[0XAC] = im.GetButton("Keyboard.WEBHOME");
		bcmbutton[0XAD] = im.GetButton("Keyboard.MUTE");
		bcmbutton[0XAE] = im.GetButton("Keyboard.VOLUMEDOWN");
		bcmbutton[0XAF] = im.GetButton("Keyboard.VOLUMEUP");
		bcmbutton[0XB0] = im.GetButton("Keyboard.MEDIA_NEXTTRACK");
		bcmbutton[0XB1] = im.GetButton("Keyboard.MEDIA_PREVTRACK");
		bcmbutton[0XB2] = im.GetButton("Keyboard.MEDIA_STOP");
		bcmbutton[0XB3] = im.GetButton("Keyboard.MEDIA_PLAYPAUSE");
		
		bcmbutton[0XB5] = im.GetButton("Keyboard.MEDIA_SELECT");

		bcmbutton[0XBA] = im.GetButton("Keyboard.SEMICOLON");
		bcmbutton[0XBB] = im.GetButton("Keyboard.EQUALS");
		bcmbutton[0XBC] = im.GetButton("Keyboard.COMMA");
		bcmbutton[0XBD] = im.GetButton("Keyboard.MINUS");
		bcmbutton[0XBE] = im.GetButton("Keyboard.PERIOD");
		bcmbutton[0XBF] = im.GetButton("Keyboard.SLASH");
		bcmbutton[0XC0] = im.GetButton("Keyboard.GRAVE");



		bcmbutton[0XC3] = im.GetButton("Gamepad.A");
		bcmbutton[0XC4] = im.GetButton("Gamepad.B");
		bcmbutton[0XC5] = im.GetButton("Gamepad.X");
		bcmbutton[0XC6] = im.GetButton("Gamepad.Y");
		bcmbutton[0XC7] = im.GetButton("Gamepad.RB");
		bcmbutton[0XC8] = im.GetButton("Gamepad.LB");
		bcmbutton[0XC9] = im.GetButton("Gamepad.LT");
		bcmbutton[0XCA] = im.GetButton("Gamepad.RT");
		bcmbutton[0XCB] = im.GetButton("Gamepad.DPAD_UP");
		bcmbutton[0XCC] = im.GetButton("Gamepad.DPAD_DOWN");
		bcmbutton[0XCD] = im.GetButton("Gamepad.DPAD_LEFT");
		bcmbutton[0XCE] = im.GetButton("Gamepad.DPAD_RIGHT");
		bcmbutton[0XCF] = im.GetButton("Gamepad.START");
		bcmbutton[0XD0] = im.GetButton("Gamepad.BACK");
		bcmbutton[0XD1] = im.GetButton("Gamepad.LS");
		bcmbutton[0XD2] = im.GetButton("Gamepad.RS");
		//左遥感向上，  不太方便支持...
		//bcmbutton[0XD3] = VK_GAMEPAD_LEFT_THUMBSTICK_UP;

		//bcmbutton[0XCA] = im.GetButton("Gamepad.RT");
		//todo :开发手柄功能时完善游戏板相关


		

		bcmbutton[0XDB] = im.GetButton("Keyboard.LBRACKET");
		bcmbutton[0XDC] = im.GetButton("Keyboard.BACKSLASH");
		bcmbutton[0XDD] = im.GetButton("Keyboard.RBRACKET");
		bcmbutton[0XDE] = im.GetButton("Keyboard.APOSTROPHE");
		bcmbutton[0XDF] = im.GetButton("Keyboard.RCTRL");

		bcmbutton[0XE2] = im.GetButton("Keyboard.BACKSLASH");


		bcmbutton[0xFA] = im.GetButton("Keyboard.MEDIA_PLAYPAUSE");










		bcmbuttonInited = true;
	}
#endif
























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


	//MAINTRACK
	mainTrack = MIX_CreateTrack(AppContext::GetMixerDevice());




	//基于上述参数加载文件
	bool resourceLoadResult = false;
	switch (mode)
	{
	case BongoCatObject::BongoCatMverMode_Standard:
		resourceLoadResult=_LoadResource_Standard(config);
		break;
	case BongoCatObject::BongoCatMverMode_Keyboard:
		resourceLoadResult = _LoadResource_Keyboard(config);
		break;
	case BongoCatObject::BongoCatMverMode_Gamepad:
		resourceLoadResult = _LoadResource_Gamepad(config);
		break;
	default:
		break;
	}

	if (!resourceLoadResult)return false;












	//填充按钮控件与默认绑定
	std::vector<unsigned char> handledButton;
	for (int buttonIndex=0; buttonIndex<leftHandKeyVec.size(); buttonIndex++)
	{
		auto& curButton = leftHandButtonVec.emplace_back();

		curButton.uiName = "Left_" + std::to_string(buttonIndex);
		for (auto& y : leftHandKeyVec[buttonIndex])
		{
			//检查当前按钮是否已经进行了映射
			const char* curKey = InputParser::BongoCatKeyToButtonBaseName(y, !isUsingGamepadKeycode&&mode ==BongoCatMverMode_Gamepad);
			//特殊按键重映射
			assert(curKey);
			if (SDL_strcmp(curKey, "GamepadAxis.LT") == 0 || SDL_strcmp(curKey, "GamepadAxis.RT") == 0)
			{
				curButton.defaultBinding.type = BindingInfo::Button_ActualAxisToButton;
				curButton.defaultBinding.controllList.push_back(curKey);
				curButton.binding = curButton.defaultBinding;
			}
			else if (isUsingGamepadKeycode && (y == 0xD3 || y == 0XD7 || y == 0xD5 || y == 0XD9))
			{
				curButton.defaultBinding.type = BindingInfo::Button_ActualAxisToButton;
				curButton.defaultBinding.controllList.push_back(curKey);
				curButton.binding = curButton.defaultBinding;
			}
			else if (isUsingGamepadKeycode && (y == 0xD4 || y == 0XD8 || y == 0xD6 || y == 0XDA))
			{
				curButton.defaultBinding.type = BindingInfo::Button_ActualAxisToButton;
				curButton.defaultBinding.controllList.push_back(curKey);
				curButton.defaultBinding.controlValue = -curButton.defaultBinding.controlValue;
				curButton.binding = curButton.defaultBinding;
			}
			else
			{
				curButton.defaultBinding.type = BindingInfo::Button_ActualButton;
				curButton.defaultBinding.controllList.push_back(curKey);
				curButton.binding = curButton.defaultBinding;
			}
		}

	}
	for (int buttonIndex=0; buttonIndex<rightHandKeyVec.size(); buttonIndex++)
	{
		auto& curButton = rightHandButtonVec.emplace_back();

		curButton.uiName = "Right_" + std::to_string(buttonIndex);
		for (auto& y : rightHandKeyVec[buttonIndex])
		{
			//检查当前按钮是否已经进行了映射
			const char* curKey = InputParser::BongoCatKeyToButtonBaseName(y, !isUsingGamepadKeycode && mode ==BongoCatMverMode_Gamepad);
			//特殊按键重映射
			assert(curKey);
			if (SDL_strcmp (curKey,"GamepadAxis.LT")==0|| SDL_strcmp(curKey, "GamepadAxis.RT")==0)
			{
				curButton.defaultBinding.type = BindingInfo::Button_ActualAxisToButton;
				curButton.defaultBinding.controllList.push_back(curKey);
				curButton.binding = curButton.defaultBinding;
			}
			else if (isUsingGamepadKeycode&&(y==0xD3||y== 0XD7||y==0xD5||y==0XD9))
			{
				curButton.defaultBinding.type = BindingInfo::Button_ActualAxisToButton;
				curButton.defaultBinding.controllList.push_back(curKey);
				curButton.binding = curButton.defaultBinding;
			}
			else if (isUsingGamepadKeycode&&(y==0xD4||y== 0XD8||y==0xD6||y==0XDA))
			{
				curButton.defaultBinding.type = BindingInfo::Button_ActualAxisToButton;
				curButton.defaultBinding.controllList.push_back(curKey);
				curButton.defaultBinding.controlValue = -curButton.defaultBinding.controlValue;
				curButton.binding = curButton.defaultBinding;
			}
			else
			{
				curButton.defaultBinding.type = BindingInfo::Button_ActualButton;
				curButton.defaultBinding.controllList.push_back(curKey);
				curButton.binding = curButton.defaultBinding;
			}



		}
	}

	for (int buttonIndex=0; buttonIndex<keyboardKeyVec.size(); buttonIndex++)
	{
		auto& curButton = keyboardButtonVec.emplace_back();

		curButton.uiName = "Keyboard_" + std::to_string(buttonIndex);
		for (auto& y : keyboardKeyVec[buttonIndex])
		{
			//检查当前按钮是否已经进行了映射
			const char* curKey = InputParser::BongoCatKeyToButtonBaseName(y, !isUsingGamepadKeycode && mode ==BongoCatMverMode_Gamepad);
			//特殊按键重映射
			assert(curKey);
			if (SDL_strcmp(curKey, "GamepadAxis.LT") == 0 || SDL_strcmp(curKey, "GamepadAxis.RT") == 0)
			{
				curButton.defaultBinding.type = BindingInfo::Button_ActualAxisToButton;
				curButton.defaultBinding.controllList.push_back(curKey);
				curButton.binding = curButton.defaultBinding;
			}
			else if (isUsingGamepadKeycode && (y == 0xD3 || y == 0XD7 || y == 0xD5 || y == 0XD9))
			{
				curButton.defaultBinding.type = BindingInfo::Button_ActualAxisToButton;
				curButton.defaultBinding.controllList.push_back(curKey);
				curButton.binding = curButton.defaultBinding;
			}
			else if (isUsingGamepadKeycode && (y == 0xD4 || y == 0XD8 || y == 0xD6 || y == 0XDA))
			{
				curButton.defaultBinding.type = BindingInfo::Button_ActualAxisToButton;
				curButton.defaultBinding.controllList.push_back(curKey);
				curButton.defaultBinding.controlValue = -curButton.defaultBinding.controlValue;
				curButton.binding = curButton.defaultBinding;
			}
			else
			{
				curButton.defaultBinding.type = BindingInfo::Button_ActualButton;
				curButton.defaultBinding.controllList.push_back(curKey);
				curButton.binding = curButton.defaultBinding;
			}
		}
	}

	for (int buttonIndex=0; buttonIndex< soundsKeyVec.size(); buttonIndex++)
	{
		auto& curButton = soundsButtonVec.emplace_back();
		curButton.uiName = "Sound_" + std::to_string(buttonIndex);
		for (auto& y : soundsKeyVec[buttonIndex])
		{
			//检查当前按钮是否已经进行了映射
			const char* curKey = InputParser::BongoCatKeyToButtonBaseName(y);
			curButton.defaultBinding.type = BindingInfo::Button_ActualButton;
			curButton.defaultBinding.controllList.push_back(curKey);
			curButton.binding = curButton.defaultBinding;
		}
	}

	//属性控制按钮
	{
		controlClearSound.uiName="ClearSound";
		for (auto& y : stopSound_KeyVec)
		{
			//检查当前按钮是否已经进行了映射
			const char* curKey = InputParser::BongoCatKeyToButtonBaseName(y);
			controlClearSound.defaultBinding.type = BindingInfo::Button_ActualButton;
			controlClearSound.defaultBinding.controllList.push_back(curKey);
			controlClearSound.binding = controlClearSound.defaultBinding;
		}
	}
	{
		controlClearEmotion.uiName="ClearEmotion";
		for (auto& y : stopEmotion_KeyVec)
		{
			//检查当前按钮是否已经进行了映射
			const char* curKey = InputParser::BongoCatKeyToButtonBaseName(y);
			controlClearEmotion.defaultBinding.type = BindingInfo::Button_ActualButton;
			controlClearEmotion.defaultBinding.controllList.push_back(curKey);
			controlClearEmotion.binding = controlClearEmotion.defaultBinding;
		}
	}


	//手轴
	{
		if (mode == BongoCatMverMode_Standard)
		{
			rightHandAxis.uiName = "RightHand";
			{
				ModelAxisControl::AxisInfo mouseX;
				mouseX.defaultBinding.type = BindingInfo::Axis_ActualAxis;
				mouseX.defaultBinding.controllList.push_back("Mouse.Pos.X");
				mouseX.binding = mouseX.defaultBinding;
				rightHandAxis.axisVec.push_back(mouseX);
			}
			{
				ModelAxisControl::AxisInfo mouseY;
				mouseY.defaultBinding.type = BindingInfo::Axis_ActualAxis;
				mouseY.defaultBinding.controllList.push_back("Mouse.Pos.Y");
				mouseY.binding = mouseY.defaultBinding;
				rightHandAxis.axisVec.push_back(mouseY);
			}
		}
		else if (mode == BongoCatMverMode_Gamepad)
		{
			leftHandAxis.uiName = "LeftHand";
			{
				ModelAxisControl::AxisInfo mouseX;
				mouseX.defaultBinding.type = BindingInfo::Axis_ActualAxis;
				mouseX.defaultBinding.controllList.push_back("GamepadAxis.LS.X");
				mouseX.binding = mouseX.defaultBinding;
				leftHandAxis.axisVec.push_back(mouseX);
			}
			{
				ModelAxisControl::AxisInfo mouseY;
				mouseY.defaultBinding.type = BindingInfo::Axis_ActualAxis;
				mouseY.defaultBinding.controllList.push_back("GamepadAxis.LS.Y");
				mouseY.binding = mouseY.defaultBinding;
				leftHandAxis.axisVec.push_back(mouseY);
			}
			
			rightHandAxis.uiName = "RightHand";
			{
				ModelAxisControl::AxisInfo mouseX;
				mouseX.defaultBinding.type = BindingInfo::Axis_ActualAxis;
				mouseX.defaultBinding.controllList.push_back("GamepadAxis.RS.X");
				mouseX.binding = mouseX.defaultBinding;
				rightHandAxis.axisVec.push_back(mouseX);
			}
			{
				ModelAxisControl::AxisInfo mouseY;
				mouseY.defaultBinding.type = BindingInfo::Axis_ActualAxis;
				mouseY.defaultBinding.controllList.push_back("GamepadAxis.RS.Y");
				mouseY.binding = mouseY.defaultBinding;
				rightHandAxis.axisVec.push_back(mouseY);
			}
		}





	



	
	}


	//特定渲染状态
	if (mode == BongoCatMverMode_Standard)
	{
		currentStates.useRightHandPos = true;
		currentStates.rightHandPosChanged = true;
	}







	bool buttonHandled = false;
	bool axisHandled = false;

	//ModelControl::SetUpDefaultControl(desc, _model, NULL, NULL, &modelAnimationVec);
	//如果提供的绑定信息，则直接应用（比如来自程序退出时的自动保存）
	//if (!bindingJson.empty())
	//	ModelControl::SetUpBindingByJson(bindingJson, NULL, NULL, &modelAnimationVec);
	if (!bindingJson.empty())
		SetUpBindingByJson();
	else
		LoadBinding();



	ApplyControlBindings();

	//为绑定的Action设置具体的执行函数
	RegisterAllActionFunc();



	working = true;
	return true;
}

void BongoCatObject::Update(uint64_t deltaTicksNS)
{
	//处理键盘输入数据

	if (working)
	{

		switch (mode)
		{
		case BongoCatObject::BongoCatMverMode_Standard:
			_Update_Standard(deltaTicksNS);
			break;
		case BongoCatObject::BongoCatMverMode_Keyboard:
			_Update_Standard(deltaTicksNS);
			break;
		case BongoCatObject::BongoCatMverMode_Gamepad:
			_Update_Standard(deltaTicksNS);
			break;
		default:
			break;
		}

		currentStates.leftHandPosChanged = false;
		currentStates.rightHandPosChanged = false;
	}
}


//void BongoCatObject::Draw(MixDrawList* drawList)
//{
//	if (working)
//	{
//		_model->DrawMix(drawList);
//	}
//}

void BongoCatObject::Draw()
{
	if (working)
	{
		//BongoCat部分const资源需要在外部设置
		//因为Sprite的顶点单位是像素，且Y轴向下，且需要模拟特定视口
		//SDL_Rect vp;//特定于BongoCat的vp？
		//float canvasW;
		//float canvasH;
		//float rtW;//渲染目标
		//float rtH;



		//float curXinCanvas;
		//float curYinCanvas;

		//float x = curXinCanvas / canvasW;
		//float y=curXinCanvas / canvasH;
		//float realX = vp.x + vp.w * x;
		//float realY = vp.y + vp.h * y;
		//float vertexPosX = 2.f* realX / rtW  - 1.f;
		//float vertexPosY = 1.f - 2.f * realY / rtH;

	

		
		struct
		{
			SDL_FRect bongoCatVp;
			float _1_canvasW;
			float _1_canvasH;
			float _1_rtW;
			float _1_rtH;
		}uniformData;



		auto pContext=AppContext::GetSDL3RenderContext();
		auto& vp = pContext->GetViewport();

		float rtW = vp.w;
		float rtH = vp.h;

		uniformData._1_canvasW = 1.f / decoration.initialSize[0];
		uniformData._1_canvasH = 1.f / decoration.initialSize[1];
		uniformData._1_rtW = 1.f/ rtW;
		uniformData._1_rtH = 1.f / rtH;
		



		if (rtW * decoration.initialSize[1] > decoration.initialSize[0] * rtH)
		{
			//渲染目标比BongoCat比例更宽的时候
			//BongoCat高度占满，两侧留空
			uniformData.bongoCatVp.h = rtH;
			uniformData.bongoCatVp.w = uniformData.bongoCatVp.h * ((float)decoration.initialSize[0] / decoration.initialSize[1]);
			uniformData.bongoCatVp.x = (rtW - uniformData.bongoCatVp.w)*0.5f;
			uniformData.bongoCatVp.y = 0;
		}
		else
		{	
			//BongoCat宽度占满，上下留空
			uniformData.bongoCatVp.w =rtW;
			uniformData.bongoCatVp.h = uniformData.bongoCatVp.w * ((float)decoration.initialSize[1] / decoration.initialSize[0]);
			uniformData.bongoCatVp.x = 0;
			uniformData.bongoCatVp.y = (rtH - uniformData.bongoCatVp.h) * 0.5f;
		}

		//计算uniformData offset
		uniformData.bongoCatVp.x = uniformData.bongoCatVp.x-(scale * 0.5f * uniformData.bongoCatVp.w- 0.5f * uniformData.bongoCatVp.w);
		uniformData.bongoCatVp.y = uniformData.bongoCatVp.y-(scale * 0.5f * uniformData.bongoCatVp.h- 0.5f * uniformData.bongoCatVp.h);
		uniformData.bongoCatVp.x += offsetX;//应该不能单位为像素吧...
		uniformData.bongoCatVp.y += offsetY;
		uniformData.bongoCatVp.w *= scale;
		uniformData.bongoCatVp.h *= scale;







		pContext->SetVertexUniformData(0,&uniformData,sizeof(uniformData));



		//根据当前渲染状态进行渲染














		switch (mode)
		{
		case BongoCatObject::BongoCatMverMode_Standard:
			_Draw_Standard();
			break;
		case BongoCatObject::BongoCatMverMode_Keyboard:
			_Draw_Keyboard();
			break;
		case BongoCatObject::BongoCatMverMode_Gamepad:
			_Draw_Gamepad();
			break;
		default:
			break;
		}
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
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Create BongoCatObject with invalid json! No path info exist.");
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

BongoCatObject* BongoCatObject::CreateFromPath(const char* packPath)
{
	auto resultObj = new BongoCatObject;
	if (!resultObj->LoadFromPath(packPath))
	{
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Can not create Desk at path: %s.", packPath);
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
	for (auto& x : leftHandButtonVec)
	{
			x.binding.UnRegisterBinding();
	}
	for (auto& x : rightHandButtonVec)
	{
			x.binding.UnRegisterBinding();
	}
	for (auto& x : keyboardButtonVec)
	{
			x.binding.UnRegisterBinding();
	}
	for (auto& x : soundsButtonVec)
	{
			x.binding.UnRegisterBinding();
	}
	for (auto& x : emotionButtonVec)
	{
			x.binding.UnRegisterBinding();
	}
	for (auto& x : leftHandAxis.axisVec)
	{
			x.binding.UnRegisterBinding();
	}
	
	for (auto& x : rightHandAxis.axisVec)
	{
			x.binding.UnRegisterBinding();
	}
	controlClearSound.binding.UnRegisterBinding();
	controlClearEmotion.binding.UnRegisterBinding();

	for (auto& x : modelAnimationVec)
	{
		for (auto& y : x.binding)
			y.UnRegisterBinding();
	}



	UnregisterAllActionFunc();
}


void BongoCatObject::RegisterAllActionFunc(bool falseToUnregister)
{
	//模型控件相关
	auto& im = InputManager::GetIns();
	//左手
	{
		ActionCallback downActionCallBack;
		downActionCallBack.userData = this;
		downActionCallBack.callback = [](const char* actionName, float value, void* userData, uint64_t userData2)
			{
				((BongoCatObject*)userData)->SetLeftHandState(UTIL_GETLOW32VALUE(userData2),true);
			};

		ActionCallback upActionCallBack;
		upActionCallBack.userData = this;
		upActionCallBack.callback = [](const char* actionName, float value, void* userData, uint64_t userData2)
			{
				((BongoCatObject*)userData)->SetLeftHandState(UTIL_GETLOW32VALUE(userData2),false);
			};



		for (int i = 0; i < leftHandButtonVec.size(); i++)
		{
			std::string downActionName = "BCM.LeftHand." + std::to_string(i) + ".Down";
			std::string upActionName = "BCM.LeftHand." + std::to_string(i) + ".Up";

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
		ActionCallback axisActionCallBack;
		axisActionCallBack.userData = this;
		axisActionCallBack.callback = [](const char* actionName, float value, void* userData, uint64_t userData2)
			{
				//由于多轴的存在，这里userData2需要拆分成两个参数
				//低位是轴组的索引，高位是轴在组中的索引
				((BongoCatObject*)userData)->currentStates.leftHandPosChanged = true;
				((BongoCatObject*)userData)->currentStates.leftHandPos[UTIL_GETHIGH32VALUE(userData2)]=value;
				//((BongoCatObject*)userData)->OnAxisValueChange(UTIL_GETLOW32VALUE(userData2), UTIL_GETHIGH32VALUE(userData2), value);
			};

		for (int i = 0; i < leftHandAxis.axisVec.size(); i++)
		{
			std::string axisActionName = std::string("BCM.LeftHand.0.") + std::to_string(i) + ".Change";
			if (falseToUnregister)
				im.RegisterActionCallback(axisActionName.c_str(), axisActionCallBack);
			else
				im.UnregisterActionCallback(axisActionName.c_str(), axisActionCallBack);
		}

	}
	//右手
	{
		ActionCallback downActionCallBack;
		downActionCallBack.userData = this;
		downActionCallBack.callback = [](const char* actionName, float value, void* userData, uint64_t userData2)
			{
				((BongoCatObject*)userData)->SetRightHandState(UTIL_GETLOW32VALUE(userData2),true);
			};

		ActionCallback upActionCallBack;
		upActionCallBack.userData = this;
		upActionCallBack.callback = [](const char* actionName, float value, void* userData, uint64_t userData2)
			{
				((BongoCatObject*)userData)->SetRightHandState(UTIL_GETLOW32VALUE(userData2),false);
			};



		for (int i = 0; i < rightHandButtonVec.size(); i++)
		{
			std::string downActionName = "BCM.RightHand." + std::to_string(i) + ".Down";
			std::string upActionName = "BCM.RightHand." + std::to_string(i) + ".Up";

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


		ActionCallback axisActionCallBack;
		axisActionCallBack.userData = this;
		axisActionCallBack.callback = [](const char* actionName, float value, void* userData, uint64_t userData2)
			{
				//由于多轴的存在，这里userData2需要拆分成两个参数
				//低位是轴组的索引，高位是轴在组中的索引
				((BongoCatObject*)userData)->currentStates.rightHandPosChanged = true;
				((BongoCatObject*)userData)->currentStates.rightHandPos[UTIL_GETHIGH32VALUE(userData2)] = value;
				//((BongoCatObject*)userData)->OnAxisValueChange(UTIL_GETLOW32VALUE(userData2), UTIL_GETHIGH32VALUE(userData2), value);
			};

		for (int i = 0; i < rightHandAxis.axisVec.size(); i++)
		{
			UTIL_SETHIGH32VALUE(axisActionCallBack.userData2, i);
			std::string axisActionName = std::string("BCM.RightHand.0.") + std::to_string(i) + ".Change";
			if (falseToUnregister)
				im.RegisterActionCallback(axisActionName.c_str(), axisActionCallBack);
			else
				im.UnregisterActionCallback(axisActionName.c_str(), axisActionCallBack);
		}
	}
	//键盘
	{
		ActionCallback downActionCallBack;
		downActionCallBack.userData = this;
		downActionCallBack.callback = [](const char* actionName, float value, void* userData, uint64_t userData2)
			{
				((BongoCatObject*)userData)->KeyboardActive(UTIL_GETLOW32VALUE(userData2), true);
			};

		ActionCallback upActionCallBack;
		upActionCallBack.userData = this;
		upActionCallBack.callback = [](const char* actionName, float value, void* userData, uint64_t userData2)
			{
				((BongoCatObject*)userData)->KeyboardActive(UTIL_GETLOW32VALUE(userData2), false);
			};



		for (int i = 0; i < keyboardKeyVec.size(); i++)
		{
			std::string downActionName = "Desk.Button." + std::to_string(i) + ".Down";
			std::string upActionName = "Desk.Button." + std::to_string(i) + ".Up";

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
	//音频
	{
		ActionCallback downActionCallBack;
		downActionCallBack.userData = this;
		downActionCallBack.callback = [](const char* actionName, float value, void* userData, uint64_t userData2)
			{
				((BongoCatObject*)userData)->Play_Sound(UTIL_GETLOW32VALUE(userData2));
			};

		ActionCallback upActionCallBack;
		upActionCallBack.userData = this;
		upActionCallBack.callback = [](const char* actionName, float value, void* userData, uint64_t userData2)
			{
				((BongoCatObject*)userData)->Play_Sound(UTIL_GETLOW32VALUE(userData2));
			};



		for (int i = 0; i < keyboardKeyVec.size(); i++)
		{
			std::string downActionName = "BCM.Sound." + std::to_string(i) + ".Down";
			//std::string upActionName = "BCM.Sound." + std::to_string(i) + ".Up";

			UTIL_SETLOW32VALUE(downActionCallBack.userData2, i);
			UTIL_SETLOW32VALUE(upActionCallBack.userData2, i);
			//downActionCallBack.userData2 = (void*)i;
			//upActionCallBack.userData2 = (void*)i;
			if (falseToUnregister)
			{
				im.RegisterActionCallback(downActionName.c_str(), downActionCallBack);
				//im.RegisterActionCallback(upActionName.c_str(), upActionCallBack);
			}
			else
			{
				im.UnregisterActionCallback(downActionName.c_str(), downActionCallBack);
				//im.UnregisterActionCallback(upActionName.c_str(), upActionCallBack);

			}
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













void BongoCatObject::SetUpBindingByJson()
{
}

void BongoCatObject::ApplyControlBindings()
{
	auto& im = InputManager::GetIns();
	for (uint32_t i=0;i<leftHandButtonVec.size();i++)
	{
		leftHandButtonVec[i].binding.RegisterBindingEx("BCM.LeftHand.", i);
	}
	for (uint32_t i=0;i<rightHandButtonVec.size();i++)
	{
		rightHandButtonVec[i].binding.RegisterBindingEx("BCM.RightHand.", i);
	}
	for (uint32_t i=0;i< keyboardButtonVec.size();i++)
	{
		keyboardButtonVec[i].binding.RegisterBinding(i);//使用桌子的按钮系统以触发其他事件
	}

	//可能不应该使用按钮的绑定系统
	for (uint32_t i = 0; i < soundsButtonVec.size(); i++)
	{
		soundsButtonVec[i].binding.RegisterBindingEx("BCM.Sound.", i);
	}
	for (uint32_t i = 0; i < emotionButtonVec.size(); i++)
	{
		emotionButtonVec[i].binding.RegisterBindingEx("BCM.Emotion.", i);
	}

	for (int j = 0; j < leftHandAxis.axisVec.size(); j++)
	{
		leftHandAxis.axisVec[j].binding.RegisterBindingEx("BCM.LeftHand.", 0,j);
	}
	for (int j = 0; j < rightHandAxis.axisVec.size(); j++)
	{
		rightHandAxis.axisVec[j].binding.RegisterBindingEx("BCM.RightHand.", 0,j);
	}

	//controlClearSound.binding.



}






void BongoCatObject::Play_Sound(int index)
{
	//if (false)
	if (decoration.soundKeep)
	{
		if (index < audioTrackResource.size())
		{
			auto curTrack = audioTrackResource[index];
			if (curTrack)
			{
				MIX_PlayTrack(curTrack, 0);
			}
		}
	}
	else
	{
		if (mainTrack)
		{
			MIX_SetTrackAudio(mainTrack, audioSoundResource[index]);
			MIX_PlayTrack(mainTrack,0);
		}
	}


}

void BongoCatObject::SetLeftHandState(int index, bool bdown)
{
	if(bdown)
		currentStates.leftHandStateStack.push_back(index);
	else
	{
		for (int i = 0; i < currentStates.leftHandStateStack.size();i++)
		{
			if (currentStates.leftHandStateStack[i] == index)
			{
				currentStates.leftHandStateStack.erase(currentStates.leftHandStateStack.begin()+i);
				return;
			}
		}
	}
}

void BongoCatObject::SetRightHandState(int index, bool bdown)
{
	if(bdown)
		currentStates.rightHandStateStack.push_back(index);
	else
	{
		for (int i = 0; i < currentStates.rightHandStateStack.size();i++)
		{
			if (currentStates.rightHandStateStack[i] == index)
			{
				currentStates.rightHandStateStack.erase(currentStates.rightHandStateStack.begin()+i);
				return;
			}
		}
	}
}

void BongoCatObject::KeyboardActive(int index, bool bactive)
{
	currentStates.keyboardStates[index] = bactive;
}





//怎么写着写着就一千多行了..














































#endif
