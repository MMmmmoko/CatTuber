#include"AppContext.h"
#include"Pack/Pack.h" 
#include"Util/Util.h"
#include"Input/InputManager.h"
#include"Input/InputParser.h"

#include"Model/Live2DModelBase.h"

#include"Item/Scene.h"
#include"Item/BongoCatObject.h"
//#include"SDL3_mixer/SDL_mixer.h"






bool BongoCatObject::_LoadResource_Gamepad(Json::Value& config)
{
	//读取键表
	auto& stdjson = config["gamepad"];

	_ReadKeysFromJsonArray(stdjson["face"], faceKeyVec);
	_ReadKeysFromJsonArray(stdjson["lefthand"], leftHandKeyVec);
	_ReadKeysFromJsonArray(stdjson["righthand"], rightHandKeyVec);
	_ReadKeysFromJsonArray(stdjson["keyboard"], keyboardKeyVec);
	_ReadKeysFromJsonArray(stdjson["l2d_expression"], l2dExpressionKeyVec);
	_ReadKeysFromJsonArray(stdjson["l2d_motion"], l2dMotionKeyVec);
	_ReadKeysFromJsonArray(stdjson["l2d_motion_lockhand"], l2dMotionKeyVec_LockHand);
	_ReadKeysFromJsonArray(stdjson["sounds"], soundsKeyVec);

	if (stdjson["L3"].isArray())
	{
		for (auto& key : stdjson["L3"])
		{
			if (key.isUInt())
			{
				leftStickKeyVec.push_back(key.asUInt());
			}
		}
	}

	if (stdjson["R3"].isArray())
	{
		for (auto& key : stdjson["R3"])
		{
			if (key.isUInt())
			{
				rightStickKeyVec.push_back(key.asUInt());
			}
		}
	}




	//角色模型
	if (isUsingLive2D)
	{
		if (!_model)
		{
			const char* l2dModelFolder = "img/gamepad/cat_model";
			_model = IModel::CreateFromFolder(pack.GetPath(), l2dModelFolder);
			if (!_model)
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Can not create model from : %s/%s", pack.GetPath(), l2dModelFolder);
				return false;
			}
		}


		//使用Live2D模型时，获取模型参数句柄
		leftHandPosParamX = _model->GetParamHandle("CatParamStickLX");
		leftHandPosParamY = _model->GetParamHandle("CatParamStickLY");
		rightHandPosParamX = _model->GetParamHandle("CatParamStickRX");
		rightHandPosParamY = _model->GetParamHandle("CatParamStickRY");
		leftHandDown = _model->GetParamHandle("CatParamLeftHandDown");
		rightHandDown = _model->GetParamHandle("CatParamRightHandDown");
		leftStickDown = _model->GetParamHandle("CatParamStickLeftDown");
		rightStickDown = _model->GetParamHandle("CatParamStickRightDown");
		showStickHandLeft = _model->GetParamHandle("CatParamStickShowLeftHand");
		showStickHandRight = _model->GetParamHandle("CatParamStickShowRightHand");




	}
	else
	{
		_LoadSprite("img/gamepad/cat.png", pngResource.cat);
	}


	//按键表情
	pngResource.facevec.reserve(faceKeyVec.size());
	for (int i = 0; i < faceKeyVec.size(); i++)
	{
		auto& psprite = pngResource.facevec.emplace_back();
		_LoadSprite(("img/gamepad/face/" + std::to_string(i) + ".png").c_str(), psprite);
	}






	//右手(持鼠标的手)
	if (isUsingLive2D)
	{

	}
	else
	{
		//右手纹理arm.png???


		righthand.SetColor(decoration.armColor[0], decoration.armColor[1], decoration.armColor[2]);

		
		righthand.SetTextture(util::LoadTextureFromPack(&pack,"img/gamepad/arm_R.png"));
		righthand.SetStartPoint(175.f, 108.f, -0.7237f, 0.69f);
		righthand.SetEndPoint(220.f, 178.f, -0.6f, 0.8f);
		righthand.SetHandSize(60.f, 20.f);
		righthand.SetPressPoint(0.f, 0.f);
		righthand.SetOffset(decoration.handOffset[0], decoration.handOffset[1]);




		lefthand.SetColor(decoration.armColor[0], decoration.armColor[1], decoration.armColor[2]);
		lefthand.SetTextture(util::LoadTextureFromPack(&pack,"img/gamepad/arm_L.png"));
		lefthand.SetStartPoint(392.f, 209.f, 0.f, 1.f);
		lefthand.SetEndPoint(472.f, 212.f, -0.1465f, 0.9892f);
		lefthand.SetHandSize(60.f, 40.f);
		lefthand.SetPressPoint(0.f, 0.f);
		lefthand.SetOffset(decoration.handOffset[0], decoration.handOffset[1]);




		//righthand.SetViewport(decoration.initialSize[0], decoration.initialSize[1]);


		//处理手持物品scale
		//一开始制作素材的时候就应该弄好素材大小啊草
	}



	//手
	if (isUsingLive2D && isUsingLive2DDesk)
	{


	}
	else
	{

		_LoadSprite("img/gamepad/lefthand/leftup.png", pngResource.leftUp);

		pngResource.LHandvec.reserve(leftHandKeyVec.size());
		for (int i = 0; i < leftHandKeyVec.size(); i++)
		{
			auto& psprite = pngResource.LHandvec.emplace_back();
			_LoadSprite(("img/gamepad/lefthand/" + std::to_string(i) + ".png").c_str(), psprite);
		}



		_LoadSprite("img/gamepad/righthand/rightup.png", pngResource.rightUp);

		pngResource.RHandvec.reserve(rightHandKeyVec.size());
		for (int i = 0; i < rightHandKeyVec.size(); i++)
		{
			auto& psprite = pngResource.RHandvec.emplace_back();
			_LoadSprite(("img/gamepad/righthand/" + std::to_string(i) + ".png").c_str(), psprite);
		}

	}






	//桌子
	if (isUsingLive2D&&isUsingLive2DDesk)
	{


	}
	else
	{

			_LoadSprite("img/gamepad/bg.png", pngResource.mousebg);


		//键盘光效
		pngResource.keyboardvec.reserve(keyboardKeyVec.size());
		for (int i = 0; i < keyboardKeyVec.size(); i++)
		{
			auto& psprite = pngResource.keyboardvec.emplace_back();
			_LoadSprite( ("img/gamepad/keyboard/" + std::to_string(i) + ".png").c_str(), psprite);
		}



		//手柄遥感&光效
		_LoadSprite("img/gamepad/left_stick.png", pngResource.leftStickNormal);
		_LoadSprite("img/gamepad/left_stick_down.png", pngResource.leftStickDown);
		_LoadSprite("img/gamepad/right_stick.png", pngResource.rightStickNormal);
		_LoadSprite("img/gamepad/right_stick_down.png", pngResource.rightStickDown);

		


	}





	currentStates.keyboardStates.resize(keyboardKeyVec.size());







	//加载音频
	{

	std::string soundPath;
	for (int i = 0; i < soundsKeyVec.size(); i++)
	{
		//载入音频
		auto& ptrack = audioTrackResource.emplace_back();
		auto& psound = audioSoundResource.emplace_back();
		//加载
		soundPath = "img/gamepad/sounds/" + std::to_string(i) + ".wav";
		bool fileExist = pack.IsFileExist(soundPath.c_str());
		if (!fileExist)
		{
			soundPath = "img/gamepad/sounds/" + std::to_string(i) + ".ogg";
			fileExist = pack.IsFileExist(soundPath.c_str());
		}
		if (!fileExist)
		{
			soundPath = "img/gamepad/sounds/" + std::to_string(i) + ".mp3";
			fileExist = pack.IsFileExist(soundPath.c_str());
		}
		if (!fileExist)
		{
			soundPath = "img/gamepad/sounds/" + std::to_string(i) + ".flac";
			fileExist = pack.IsFileExist(soundPath.c_str());
		}
		if (fileExist)
		{
			psound=util::LoadSoundFromPack(&pack, soundPath.c_str());
		}
		else
		{
			psound = nullptr;
		}

		if (psound)
		{
			ptrack = MIX_CreateTrack(AppContext::GetMixerDevice());

			if (ptrack)
				MIX_SetTrackAudio(ptrack, psound);
			else
			{

				SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Can not create track : %s/%s, %s", pack.GetPath(), soundPath.c_str(), SDL_GetError());
			}
		}
	}
	
	
	}






	return true;


}


void BongoCatObject::_Update_Gamepad(uint64_t dtNS)
{
	if (isUsingLive2D)
	{







			//右侧摇杆按下的意思
			_model->SetParamValue(rightStickDown, currentStates.useRightHandPos);
			//左侧摇杆按下的意思
			_model->SetParamValue(leftStickDown, currentStates.useRightHandPos);


			_model->SetParamValue(leftHandPosParamX, currentStates.leftHandPos[0]);
			_model->SetParamValue(leftHandPosParamY, currentStates.leftHandPos[1]);

			_model->SetParamValue(rightHandPosParamX, currentStates.rightHandPos[0]);
			_model->SetParamValue(rightHandPosParamY, currentStates.rightHandPos[1]);



			if (!currentStates.leftHandStateStack.empty())
			{
				_model->SetParamValue(leftHandDown, 1.f);
			}
			else if (currentStates.leftHandPos[0] != 0 || currentStates.leftHandPos[1] != 0 || currentStates.useLeftHandPos)
			{
				//摇杆不为0，绘制摇杆手
				_model->SetParamValue(leftHandDown, 1.f);
				_model->SetParamValue(showStickHandLeft, 1.f);
			}
			else
			{
			}


			if (!currentStates.rightHandStateStack.empty())
			{
				_model->SetParamValue(rightHandDown, 1.f);
			}
			else if (currentStates.rightHandPos[0] != 0 || currentStates.rightHandPos[1] != 0 || currentStates.useRightHandPos)
			{
				//摇杆不为0，绘制摇杆手
				_model->SetParamValue(rightHandDown, 1.f);
				_model->SetParamValue(showStickHandRight, 1.f);
			}
			else
			{
			}

			if (currentStates.isLockingHand)
			{
				_model->SetParamValue(leftHandDown, 0.f);
				_model->SetParamValue(showStickHandLeft, 0.f);
				_model->SetParamValue(rightHandDown, 0.f);
				_model->SetParamValue(showStickHandRight, 0.f);
			}





		_model->Update(dtNS);

	}



}


void BongoCatObject::_Draw_Gamepad()
{

	//CubismMatrix44 _projection = decoration.projection;
	//pmodel->DrawDirect(_projection);

	//return;
	if (isUsingLive2D)
	{







		if (!isUsingLive2DDesk)
			pngResource.mousebg.Draw();//绘制背景




		if (!isUsingLive2DDesk)
		{
			for (int i = 0; i < currentStates.keyboardStates.size(); i++)
			{
				if (currentStates.keyboardStates[i])
				{
					_DRAW(pngResource.keyboardvec[i]);
				}
			}
		}


		//Live2D模型在左手之下
		//CubismMatrix44 _projection = decoration.projection;
		//pmodel->DrawDirect(_projection);


		_Draw2DModel();


		if (!isUsingLive2DHandForKeyPress)
		{
			if (!currentStates.isLockingHand && !currentStates.leftHandStateStack.empty())
			{
				_DRAW(pngResource.LHandvec[currentStates.leftHandStateStack.back()]);
			}
			if (!currentStates.isLockingHand && !currentStates.rightHandStateStack.empty())
			{
				_DRAW(pngResource.RHandvec[currentStates.rightHandStateStack.back()]);
			}
		}





		if (currentStates.emoticonIndex >= 0)
		{
			_DRAW(pngResource.facevec[currentStates.emoticonIndex]);
		}





	}
	else
	{
		_DRAW(pngResource.cat);

		//绘制背景

		_DRAW(pngResource.mousebg);
		//TODO 绘制鼠标手
		//pngResource.mouse->SetPosition();



		for (int i = 0; i < currentStates.keyboardStates.size(); i++)
		{
			if (currentStates.keyboardStates[i])
			{
				_DRAW(pngResource.keyboardvec[i]);
			}
		}


		float lposX, lposY, rposX, rposY;


		lposX = float(decoration.stick_offset_L[0] + decoration.stick_offset_L[2] * (-currentStates.leftHandPos[0] * 266 / 270.66 - currentStates.leftHandPos[1] * 5 / 13) + decoration.handOffset[0]);
		lposY = float(decoration.stick_offset_L[1] + decoration.stick_offset_L[2] * (-currentStates.leftHandPos[0] * 50 / 270.66 + currentStates.leftHandPos[1] * 5 / 13) + decoration.handOffset[1]);

		rposX = float(decoration.stick_offset_R[0] + decoration.stick_offset_R[2] * (-currentStates.rightHandPos[0] * 266 / 270.66 - 1.2 * currentStates.rightHandPos[1] / 1.414) + decoration.handOffset[0]);
		rposY = float(decoration.stick_offset_R[1] + decoration.stick_offset_R[2] * (-currentStates.rightHandPos[0] * 50 / 270.66 + 1.2 * currentStates.rightHandPos[1] / 1.414) + decoration.handOffset[1]);





		if (currentStates.useRightHandPos)
		{
			pngResource.rightStickDown.SetPosition(rposX, rposY);
			pngResource.rightStickDown.Draw();
		}
		else
		{
			pngResource.rightStickNormal.SetPosition(rposX, rposY);
			pngResource.rightStickNormal.Draw();
		}
		if (currentStates.useLeftHandPos)
		{
			pngResource.leftStickDown.SetPosition(lposX, lposY);
			pngResource.leftStickDown.Draw();
		}
		else
		{
			pngResource.leftStickNormal.SetPosition(lposX, lposY);
			pngResource.leftStickNormal.Draw();
		}




		if (!currentStates.leftHandStateStack.empty())
		{
			_DRAW(pngResource.LHandvec[currentStates.leftHandStateStack.back()]);
		}
		else if (currentStates.leftHandPos[0] != 0 || currentStates.leftHandPos[1] != 0 || currentStates.useLeftHandPos)
		{
			//摇杆不为0，绘制摇杆手
			lefthand.SetPressPoint(-14.5f * currentStates.leftHandPos[0] + 1.5f * currentStates.leftHandPos[1] + 414.5f, -5.f * currentStates.leftHandPos[0] + 9.f * currentStates.leftHandPos[1] + 283.f);
			lefthand.Update();
			lefthand.Draw();
		}
		else
		{
			_DRAW(pngResource.leftUp);
		}


		if (!currentStates.rightHandStateStack.empty())
		{
			_DRAW(pngResource.RHandvec[currentStates.rightHandStateStack.back()]);
		}
		else if (currentStates.rightHandPos[0] != 0 || currentStates.rightHandPos[1] != 0 || currentStates.useRightHandPos)
		{
			//摇杆不为0，绘制摇杆手
			righthand.SetPressPoint(-3.f * currentStates.rightHandPos[0] - 27.f * currentStates.rightHandPos[1] + 89.f, -10.5f * currentStates.rightHandPos[0] - 3.5f * currentStates.rightHandPos[1] + 208.5f);
			righthand.Update();
			righthand.Draw();
		}
		else
		{
			_DRAW(pngResource.rightUp);
		}






		if (currentStates.emoticonIndex >= 0)
		{
			_DRAW(pngResource.facevec[currentStates.emoticonIndex]);
		}
	}



}
