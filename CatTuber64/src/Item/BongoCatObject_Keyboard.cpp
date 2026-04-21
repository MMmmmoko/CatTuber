#include"AppContext.h"
#include"Pack/Pack.h" 
#include"Util/Util.h"
#include"Input/InputManager.h"
#include"Input/InputParser.h"

#include"Model/Live2DModelBase.h"

#include"Item/Scene.h"
#include"Item/BongoCatObject.h"
//#include"SDL3_mixer/SDL_mixer.h"

bool BongoCatObject::_LoadResource_Keyboard(Json::Value& config)
{
	//读取键表
	auto& stdjson = config["keyboard"];

	_ReadKeysFromJsonArray(stdjson["face"], faceKeyVec);
	_ReadKeysFromJsonArray(stdjson["lefthand"], leftHandKeyVec);
	_ReadKeysFromJsonArray(stdjson["righthand"], rightHandKeyVec);
	_ReadKeysFromJsonArray(stdjson["keyboard"], keyboardKeyVec);
	_ReadKeysFromJsonArray(stdjson["l2d_expression"], l2dExpressionKeyVec);
	_ReadKeysFromJsonArray(stdjson["l2d_motion"], l2dMotionKeyVec);
	_ReadKeysFromJsonArray(stdjson["l2d_motion_lockhand"], l2dMotionKeyVec_LockHand);


	_ReadKeysFromJsonArray(stdjson["sounds"], soundsKeyVec);





	//角色模型
	if (isUsingLive2D)
	{
		if (!_model)
		{
			const char* l2dModelFolder = "img/keyboard/cat_model";
			_model = IModel::CreateFromFolder(pack.GetPath(), l2dModelFolder);
			if (!_model)
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Can not create model from : %s/%s", pack.GetPath(), l2dModelFolder);
				return false;
			}
		}
	}
	else
	{
		_LoadSprite("img/keyboard/cat.png", pngResource.cat);
	}


	//按键表情
	pngResource.facevec.reserve(faceKeyVec.size());
	for (int i = 0; i < faceKeyVec.size(); i++)
	{
		auto& psprite = pngResource.facevec.emplace_back();
		_LoadSprite(("img/keyboard/face/" + std::to_string(i) + ".png").c_str(), psprite);
	}








	//左手

	if (isUsingLive2D && isUsingLive2DHand)
	{


	}
	else
	{
		_LoadSprite("img/keyboard/lefthand/leftup.png", pngResource.leftUp);

		pngResource.LHandvec.reserve(leftHandKeyVec.size());
		for (int i = 0; i < leftHandKeyVec.size(); i++)
		{
			auto& psprite = pngResource.LHandvec.emplace_back();
			_LoadSprite(("img/keyboard/lefthand/" + std::to_string(i) + ".png").c_str(), psprite);
		}
		


		_LoadSprite("img/keyboard/righthand/rightup.png", pngResource.rightUp);

		pngResource.RHandvec.reserve(rightHandKeyVec.size());
		for (int i = 0; i < rightHandKeyVec.size(); i++)
		{
			auto& psprite = pngResource.RHandvec.emplace_back();
			_LoadSprite(("img/keyboard/righthand/" + std::to_string(i) + ".png").c_str(), psprite);
		}


	}






	//桌子
	if (isUsingLive2D&&isUsingLive2DDesk)
	{


	}
	else
	{

		_LoadSprite("img/standard/bg.png", pngResource.mousebg);



		//键盘光效
		pngResource.keyboardvec.reserve(keyboardKeyVec.size());
		for (int i = 0; i < keyboardKeyVec.size(); i++)
		{
			auto& psprite = pngResource.keyboardvec.emplace_back();
			_LoadSprite( ("img/standard/keyboard/" + std::to_string(i) + ".png").c_str(), psprite);
		}
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
		soundPath = "img/standard/sounds/" + std::to_string(i) + ".wav";
		bool fileExist = pack.IsFileExist(soundPath.c_str());
		if (!fileExist)
		{
			soundPath = "img/standard/sounds/" + std::to_string(i) + ".ogg";
			fileExist = pack.IsFileExist(soundPath.c_str());
		}
		if (!fileExist)
		{
			soundPath = "img/standard/sounds/" + std::to_string(i) + ".mp3";
			fileExist = pack.IsFileExist(soundPath.c_str());
		}
		if (!fileExist)
		{
			soundPath = "img/standard/sounds/" + std::to_string(i) + ".flac";
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


void BongoCatObject::_Update_Keyboard(uint64_t dtNS)
{
	if (isUsingLive2D)
	{
		_model->Update(dtNS);
	}
}


void BongoCatObject::_Draw_Keyboard()
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


		_model->Draw();


		if (!isUsingLive2DHand)
		{
			if (!currentStates.isLockingHand && !currentStates.leftHandStateStack.empty())
			{
				_DRAW(pngResource.LHandvec[currentStates.leftHandStateStack.back()]);
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




		if (!currentStates.leftHandStateStack.empty())
			_DRAW(pngResource.LHandvec[currentStates.leftHandStateStack.back()]);
		else
		{
			_DRAW(pngResource.leftUp);
		}


		if (!currentStates.rightHandStateStack.empty())
			_DRAW(pngResource.RHandvec[currentStates.rightHandStateStack.back()]);
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
