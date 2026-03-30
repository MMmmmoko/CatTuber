#include"AppContext.h"
#include"Pack/Pack.h" 
#include"Util/Util.h"
#include"Input/InputManager.h"
#include"Input/InputParser.h"

#include"Model/Live2DModelBase.h"

#include"Item/Scene.h"
#include"Item/BongoCatObject.h"




bool BongoCatObject::_LoadResource_Standard(Json::Value& config)
{
	//读取键表
	auto& stdjson = config["standard"];

	_ReadKeysFromJsonArray(stdjson["face"], faceKeyVec);
	_ReadKeysFromJsonArray(stdjson["hand"], leftHandKeyVec);
	_ReadKeysFromJsonArray(stdjson["keyboard"], keyboardKeyVec);
	_ReadKeysFromJsonArray(stdjson["l2d_expression"], l2dExpressionKeyVec);
	_ReadKeysFromJsonArray(stdjson["l2d_motion"], l2dMotionKeyVec);
	_ReadKeysFromJsonArray(stdjson["l2d_motion_lockhand"], l2dMotionKeyVec_LockHand);

	{
		if (stdjson["mouse_left"].isArray())
		{
			for (auto& key : stdjson["mouse_left"])
			{
				if (key.isUInt())
				{
					mouseLeft_KeyVec.push_back(key.asUInt());
				}
			}
		}
		else
		{
			mouseLeft_KeyVec.push_back(1);
		}
		if (stdjson["mouse_right"].isArray())
		{
			for (auto& key : stdjson["mouse_right"])
			{
				if (key.isUInt())
				{
					mouseRight_KeyVec.push_back(key.asUInt());
				}
			}
		}
		else
		{
			mouseRight_KeyVec.push_back(2);
		}
		if (stdjson["mouse_side"].isArray())
		{
			for (auto& key : stdjson["mouse_side"])
			{
				if (key.isUInt())
				{
					mouseSide_KeyVec.push_back(key.asUInt());
				}
			}
		}
		else
		{
			mouseSide_KeyVec.push_back(5);
			mouseSide_KeyVec.push_back(6);
		}
	}
	_ReadKeysFromJsonArray(stdjson["sounds"], sounds_KeyVec);





	//角色模型
	if (isUsingLive2D)
	{
		if (!_model)
		{
			const char* l2dModelFolder = "img/standard/cat_model";
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
		_LoadSprite("img/standard/cat.png", pngResource.cat);
	}


	//按键表情
	for (int i = 0; i < faceKeyVec.size(); i++)
	{
		auto& psprite = pngResource.facevec.emplace_back();
		_LoadSprite(("img/standard/face" + std::to_string(i) + ".png").c_str(), psprite);
	}






	//右手(持鼠标的手)
	if (isUsingLive2D)
	{

	}
	else
	{
		//右手纹理arm.png???

		if (isUsingPen)
		{
			_LoadSprite("img/standard/tablet.png", pngResource.mouse);
			_LoadSprite("img/standard/tablet_left.png", pngResource.mouse_left);
			_LoadSprite("img/standard/tablet_right.png", pngResource.mouse_right);
			_LoadSprite("img/standard/tablet_side.png", pngResource.mouse_side);
		}
		else
		{
			_LoadSprite("img/standard/mouse.png", pngResource.mouse);
			_LoadSprite("img/standard/mouse_left.png", pngResource.mouse_left);
			_LoadSprite("img/standard/mouse_right.png", pngResource.mouse_right);
			_LoadSprite("img/standard/mouse_side.png", pngResource.mouse_side);
		}



		righthand.SetColor(decoration.armColor[0], decoration.armColor[1], decoration.armColor[2]);

		
		righthand.SetTextture(util::LoadTextureFromPack(&pack,"img/standard/arm.png"));
		righthand.SetStartPoint(174.f, 108.f, -0.7237f, 0.69f);
		righthand.SetEndPoint(220.f, 178.f, -0.6f, 0.8f);
		righthand.SetHandSize(60.f, 20.f);
		righthand.SetPressPoint(0.f, 0.f);
		righthand.SetOffset(decoration.handOffset[0], decoration.handOffset[1]);
		//righthand.SetViewport(decoration.initialSize[0], decoration.initialSize[1]);


		//处理手持物品scale
		//一开始制作素材的时候就应该弄好素材大小啊草
	}



	//左手

	if (isUsingLive2D && isUsingLive2DDesk)
	{


	}
	else
	{

		_LoadSprite("img/standard/up.png", pngResource.leftUp);


		for (int i = 0; i < leftHandKeyVec.size(); i++)
		{
			auto& psprite = pngResource.LHandvec.emplace_back();
			_LoadSprite(("img/standard/hand" + std::to_string(i) + ".png").c_str(), psprite);
		}

	}






	//桌子
	if (isUsingLive2D&&isUsingLive2DDesk)
	{


	}
	else
	{
		if (isUsingPen)
		{
			_LoadSprite("img/standard/tabletbg.png", pngResource.mousebg);
		}
		else
		{
			_LoadSprite("img/standard/mousebg.png", pngResource.mousebg);
		}


		//键盘光效
		for (int i = 0; i < keyboardKeyVec.size(); i++)
		{
			auto& psprite = pngResource.keyboardvec.emplace_back();
			_LoadSprite( ("img/standard/keyboard" + std::to_string(i) + ".png").c_str(), psprite);
		}
	}

	//for (int i = 0; i < sounds_KeyVec.size(); i++)
	//{
	//	//载入音频
	//	auto& psound = soundList.emplace_back();
	//	psound = soundManager.CreateSound(stdpath + "\\sounds\\" + std::to_string(i) + ".wav");
	//	if (!psound)
	//		psound = soundManager.CreateSound(stdpath + "\\sounds\\" + std::to_string(i) + ".ogg");
	//	if (!psound)
	//		psound = soundManager.CreateSound(stdpath + "\\sounds\\" + std::to_string(i) + ".flac");
	//	if (psound)psound->SetMultiInstance(decoration.soundKeep);
	//}




	currentStates.keyboardStates.resize(keyboardKeyVec.size());










	return true;


}


void BongoCatObject::_Update_Standard(uint64_t dtNS)
{
	if (isUsingLive2D)
	{
		_model->Update(dtNS);
	}
}


void BongoCatObject::_Draw_Standard()
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



		righthand.Update();

		float posx, posy;
		righthand.GetMousePos(posx, posy);


		if (!isUsingPen)
		{
			posx += decoration.mousePNGOffset[0];
			posy += decoration.mousePNGOffset[1];


				pngResource.mouse.SetPosition(posx, posy);
				_DRAW(pngResource.mouse);
			if (currentStates.mouseButtonStates[0])
			{
				{
					pngResource.mouse_left.SetPosition(posx, posy);
					_DRAW(pngResource.mouse_left);
				}
			}
			if (currentStates.mouseButtonStates[1])
			{
				{
					pngResource.mouse_right.SetPosition(posx, posy);
					pngResource.mouse_right.Draw();
				}
			}
			if (currentStates.mouseButtonStates[2])
			{
				{
					pngResource.mouse_side.SetPosition(posx, posy);
					pngResource.mouse_side.Draw();
				}
			}
		}

		righthand.Draw();


		if (isUsingPen)
		{
			posx += decoration.penPNGOffset[0];
			posy += decoration.penPNGOffset[1];

			bool hasDrawPen = false;
			//if (pngResource.pen)
			//{
			//	pngResource.pen->SetPosition(posx, posy);
			//	_DRAW(pngResource.pen);
			//}
			if (currentStates.mouseButtonStates[2])
			{
				if (pngResource.mouse_side.Avaliable())
				{
					pngResource.mouse_side.SetPosition(posx, posy);
					pngResource.mouse_side.Draw();
					hasDrawPen = true;
				}
			}
			else
				if (currentStates.mouseButtonStates[1])
				{
					if (pngResource.mouse_right.Avaliable())
					{
						pngResource.mouse_right.SetPosition(posx, posy);
						pngResource.mouse_right.Draw();
						hasDrawPen = true;
					}
				}
				else if (currentStates.mouseButtonStates[0])
				{
					if (pngResource.mouse_left.Avaliable())
					{
						pngResource.mouse_left.SetPosition(posx, posy);
						pngResource.mouse_left.Draw();
						hasDrawPen = true;
					}
				}

			if (!hasDrawPen)
			{
				if (pngResource.mouse.Avaliable())
				{
					pngResource.mouse.SetPosition(posx, posy);
					pngResource.mouse.Draw();
				}
			}

		}

		if (!currentStates.leftHandStateStack.empty())
			_DRAW(pngResource.LHandvec[currentStates.leftHandStateStack.back()]);
		else
		{
			_DRAW(pngResource.leftUp);
		}

		if (currentStates.emoticonIndex >= 0)
		{
			_DRAW(pngResource.facevec[currentStates.emoticonIndex]);
		}
	}



}