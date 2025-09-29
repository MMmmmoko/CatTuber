
#include"AppContext.h"
#include"Pack/Pack.h"
#include"Util/Util.h"
#include"Input/InputManager.h"

#include"Model/Live2DModelBase.h"

#include"Item/Scene.h"
#include"Item/TableObject.h"
#include"Item/CharacterObject.h"
#include"Item/ModelControl.h"
#include"Item/ClassicItem.h"
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

	bool buttonHandled=false;
	bool axisHandled=false;

	ModelControl::SetUpDefaultControl(desc,_model,&modelButtonVec,&modelAxisVec,&modelAnimationVec);
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
		auto curTickMs=SDL_GetTicks();

		float sumLHandX=0.f;
		float sumLHandY=0.f;
		float sumLWeights = 0.f;

		float sumRHandX=0.f;
		float sumRHandY=0.f;
		float sumRWeights = 0.f;


	
#define _PUSH_HAND_DATA(hand,X,Y,weight) \
if(hand==HandControl::LEFT){sumLHandX+=X*weight; sumLHandY+=Y*weight;sumLWeights+=weight;}\
else {sumRHandX+=X*weight; sumRHandY+=Y*weight;sumRWeights+=weight;}

		//optimize 应该是可以实现设置长期参数的 比如仅维护一个近期抬起的按键表在此处更新
		std::vector<ModelButtonControl*> pushedButtnVec;
		for (auto& button : modelButtonVec)
		{
			float value;
			if (button.isDown)
			{
				value = 1.f;

				pushedButtnVec.push_back(&button);
			}
			else
			{
				//已经抬起的时间
				//0.25时长  0.38峰值
				float upDuration = static_cast<float> (curTickMs - button.upTickMs) / 1000.f;
				if (upDuration > button.recoveryDuration)
				{
					value = 0.f;
				}
				else
				{
					value = (1.f-upDuration / button.recoveryDuration)*0.38F;
				}
			}
			_model->AddParamValue(button.paramHandle, value);
		}
		//optimize好像其实可以扔在AxisValueChange里设置长期， 但一般模型不会有太多轴数
		for (auto& axis:modelAxisVec)
		{
			//CatTuber多轴混合
			for (auto& curAxis : axis.axisVec)
			{
				_model->AddParamValue(curAxis.paramHandle, curAxis.value);
			}
		}

		_model->Update(deltaTicksNS);

		
		//轴相关的特殊处理 不想使用std::vector
		//鼠标没移动时需要看是否存在其他有效对象，所以需要一个临时区域
		float sumLHandX_mouse = 0.f;
		float sumLHandY_mouse = 0.f;
		float sumLWeights_mouse = 0.f;

		float sumRHandX_mouse = 0.f;
		float sumRHandY_mouse = 0.f;
		float sumRWeights_mouse = 0.f;

		std::vector<ModelAxisControl*> _noMoveMouseAxisControls_L;//未移动但是被标注了active的鼠标轴
		std::vector<ModelAxisControl*> _noMoveMouseAxisControls_R;//未移动但是被标注了active的鼠标轴

		for (auto button : pushedButtnVec)
		{
				float x, y;
				_model->GetHandPosFromHandle(button->handControl.handPosHandle, &x, &y);
				//上面获取的是点在模型中的坐标（模型空间），没有经过变换
				//我们可以选择直接进行计算，仅对结果进行变换

				_PUSH_HAND_DATA(button->handControl.handIndex, x, y, button->handControl.handWeight)
		}


		for (auto& axis : modelAxisVec)
		{
			//轴的模式是用来控制轴当前的手位置数据是否参与加权计算的

			switch (axis.handControl.moveType)
			{
			case HandControl::FIXPOS:
			{
				assert(false && "Undeveloped Function: FIXPOS");
				break;
			}
			case HandControl::MOUSE:
			{
				//鼠标：如果有附带的按钮按下，则不论是否移动，均参与计算。
				//鼠标在此帧未移动时，如果没有其他动作，则鼠标持续有效
				//此外这里直接计算需要发送给鼠标模型的数据？

				

				if (axis.handControl.moveCalcData.mouseData.requiredHandheldItem==false
					||_pParentItem->GetHandheldItem())
				{
					if (axis.valueChangedCurFrame)
					{
						//对于鼠标模式，如果位置有任何偏移均视为有效数据
						float x, y;
						_model->GetHandPosFromHandle(axis.handControl.handPosHandle, &x, &y);
						_PUSH_HAND_DATA(axis.handControl.handIndex, x, y, axis.handControl.handWeight)

							axis.handControl.moveCalcData.mouseData.mouseDataActive = true;
					}
					else
					{
						//如果位置没有偏移则观察是否有其他数据正在生效
						if (axis.handControl.moveCalcData.mouseData.mouseDataActive)
						{
							float x, y;
							_model->GetHandPosFromHandle(axis.handControl.handPosHandle, &x, &y);
							if (axis.handControl.handIndex == HandControl::LEFT)
							{
								sumLHandX_mouse += x;
								sumLHandY_mouse += y;
								sumLWeights_mouse += axis.handControl.handWeight;

								_noMoveMouseAxisControls_L.push_back(&axis);
							}
							else
							{
								sumRHandX_mouse += x;
								sumRHandY_mouse += y;
								sumRWeights_mouse += axis.handControl.handWeight;

								_noMoveMouseAxisControls_R.push_back(&axis);
							}
						}
					}
				}
				break;
			}
			case HandControl::STICK:
			{
				//当为0的时候计算为0的时长，超过0.15秒再设置为无效状态

				//判断是否在零点
				bool bZero = true;
				for (auto& subAxis : axis.axisVec)
				{
					if (subAxis.value != 0.f)
					{
						bZero = false; break;
					}
				}
				
				//如果在零点（中心点），则计算时长，超长0.15秒则无效
				if (bZero&& curTickMs - axis.handControl.moveCalcData.stickDataZeroTimeMs>150)
				{
					//无效
				}
				else
				{
					//有效

					float x, y;
					_model->GetHandPosFromHandle(axis.handControl.handPosHandle, &x, &y);
					_PUSH_HAND_DATA(axis.handControl.handIndex, x, y, axis.handControl.handWeight);
					if (!bZero)
					{
					//如果非0则更新0点信息
						axis.handControl.moveCalcData.stickDataZeroTimeMs = curTickMs;
					}
				}
				break;
			}
			case HandControl::MOVING:
			{
				//仅在数据有变化时生效，为了减少抖动，停止运动后的0.1秒时间内也继续视为有效
				if (axis.valueChangedCurFrame)
				{
					float x, y;
					_model->GetHandPosFromHandle(axis.handControl.handPosHandle, &x, &y);
					_PUSH_HAND_DATA(axis.handControl.handIndex, x, y, axis.handControl.handWeight)
						axis.handControl.moveCalcData.movingDataStopTimeMs = curTickMs;
				}
				else
				{
					if (curTickMs - axis.handControl.moveCalcData.movingDataStopTimeMs < 100)
					{
						float x, y;
						_model->GetHandPosFromHandle(axis.handControl.handPosHandle, &x, &y);
						_PUSH_HAND_DATA(axis.handControl.handIndex, x, y, axis.handControl.handWeight);
					}
				}
				break;
			}
			case HandControl::ABSOLUTEBALL:
			case HandControl::INERTIA_ABSOLUTEBALL:
			{
				//绝对球 CatTuber32版中用于应对有范围突变的数据（如一个点的数据从-0.9降到-1时想要继续下降的话，它会突变到1，再从1开始下降到0.9）
				//CatTuber可能不需要这个类型。CatTuber可能做一层数据预处理会更好
				assert(false&&"Undeveloped Function: ABSOLUTEBALL");
				break;
			}
			default:
			{
				assert(false && "Unknown MoveType!");
				break;
			}



			}
		}
		
		//todo计算坐标，并将数据变换后发送给其他模型
		//x\y->桌子的模型坐标变换->  （在角色模型中进行后续处理）角色的模型坐标变换
		if (sumLWeights_mouse > 0.f)
		{
			float resultLHandX_mouse = sumLHandX_mouse / sumLWeights_mouse;
			float resultLHandY_mouse = sumLHandY_mouse / sumLWeights_mouse;

			//进行变换
			//CatTuber的变换逻辑：  对物体位移之调整scale，那么scale时物体不应该出现位移
			resultLHandX_mouse = resultLHandX_mouse * scale+offsetX;
			resultLHandY_mouse = resultLHandY_mouse * scale+offsetY;


		}


#define TRANSFORM_X(X) (X)*scale+offsetX
#define TRANSFORM_Y(X) (X)*scale+offsetY
		//发送数据给角色
		if(_pParentItem->GetCharacter())
		{
			if (sumLWeights != 0.f)
			{
				//左手有除鼠标外的其他动作
				//将特定鼠标轴设置为无效
				for (auto pmouse : _noMoveMouseAxisControls_L)
				{
					pmouse->handControl.moveCalcData.mouseData.mouseDataActive = false;
				}

				auto pCharacter = _pParentItem->GetCharacter();
				//0左1右
				//pCharacter->SetHandPosition(0,true, (2.f*242.f/400.f-1)*400.f/300.f
				//	, -2.f*(225.f/300.f-0.5f));
				pCharacter->SetHandPosition(0,true, TRANSFORM_X(sumLHandX / sumLWeights)
					, TRANSFORM_Y(sumLHandY / sumLWeights));
			}
			else
			{
				auto pCharacter = _pParentItem->GetCharacter();
				if (sumLWeights_mouse != 0.f)
				{

					pCharacter->SetHandPosition(0,true, TRANSFORM_X( sumLWeights_mouse / sumLWeights_mouse)
						, TRANSFORM_Y( sumLHandY_mouse / sumLWeights_mouse));
				}
				else
					pCharacter->SetHandPosition(0,false, 0.f, 0.f);
			}
			if (sumRWeights != 0.f)
			{
				for (auto pmouse : _noMoveMouseAxisControls_R)
				{
					pmouse->handControl.moveCalcData.mouseData.mouseDataActive = false;
				}

				auto pCharacter = _pParentItem->GetCharacter();
				pCharacter->SetHandPosition(1,true, TRANSFORM_X(sumRHandX / sumRWeights)
					, TRANSFORM_Y( sumRHandY / sumRWeights));
			}
			else
			{
				auto pCharacter = _pParentItem->GetCharacter();
				if (sumRWeights_mouse != 0.f)
				{

					pCharacter->SetHandPosition(1,true, TRANSFORM_X(sumRWeights_mouse / sumRWeights_mouse),
						TRANSFORM_Y(sumRHandY_mouse / sumRWeights_mouse));
				}
				else
					pCharacter->SetHandPosition(1,false, 0.f, 0.f);
			}
		}
		//发送数据给鼠标
		if (_pParentItem->GetHandheldItem())
		{
			//
			auto pHandheldItem=_pParentItem->GetHandheldItem();
			//pHandheldItem->SetPosition

		}







	}
}


void TableObject::Draw(MixDrawList* drawList)
{
	if (working)
	{
		_model->DrawMix(drawList);
	}
}

void TableObject::OnLoopEnd()
{
	//绘制之后的调用，可以用于状态清理重置
	for (auto& axisGroup:modelAxisVec)
	{
		axisGroup.valueChangedCurFrame=false;
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
	json["Bindings"] = ModelControl::GenerateJsonBinding(&modelButtonVec, &modelAxisVec, &modelAnimationVec);

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
	return ModelControl::LoadBindingByName(resourcePath, bindingName,&modelButtonVec,&modelAxisVec,&modelAnimationVec);
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

void TableObject::ClearBinding()
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
		for(auto&y:x.binding)
			y.UnRegisterBinding();
	}



}



//具体action到函数成员方法
void TableObject::RegisterAllActionFunc(bool falseToUnregister)
{
	//模型控件相关
	auto& im = InputManager::GetIns();
	{
		ActionCallback downActionCallBack;
		downActionCallBack.userData = this;
		downActionCallBack.callback = [](const char* actionName, float value, void* userData, uint64_t userData2)
			{
				((TableObject*)userData)->OnButtonDown(UTIL_GETLOW32VALUE(userData2));
			};

		ActionCallback upActionCallBack;
		upActionCallBack.userData = this;
		upActionCallBack.callback = [](const char* actionName, float value, void* userData, uint64_t userData2)
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
		axisActionCallBack.callback = [](const char* actionName, float value, void* userData, uint64_t userData2)
			{
				//由于多轴的存在，这里userData2需要拆分成两个参数
				//低位是轴组的索引，高位是轴在组中的索引
				((TableObject*)userData)->OnAxisValueChange(UTIL_GETLOW32VALUE(userData2),UTIL_GETHIGH32VALUE(userData2), value);
			};

		for (int i = 0; i < modelAxisVec.size(); i++)
		{
			
			UTIL_SETLOW32VALUE( axisActionCallBack.userData2,i);
			for (int j = 0; j < modelAxisVec[i].axisVec.size(); j++)
			{
				UTIL_SETHIGH32VALUE(axisActionCallBack.userData2, j);

				std::string axisActionName = "Table.Axis." + std::to_string(i) + "."+ std::to_string(j) +"Change";
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

void TableObject::OnAxisValueChange(int axisGroupIndex, int axisIndex, float value)
{
	if (axisGroupIndex >= modelAxisVec.size()
		|| axisIndex>= modelAxisVec[axisGroupIndex].axisVec.size()
		|| modelAxisVec[axisGroupIndex].axisVec[axisIndex].value== value
		)return;
	
	modelAxisVec[axisGroupIndex].axisVec[axisIndex].value = value;
	modelAxisVec[axisGroupIndex].valueChangedCurFrame = true;


	//为了防止多轴触发，AxisValueChange的具体触发将移动到其他地方
	//auto& curAxis=modelAxisVec[axisIndex];
	//
	//if (curAxis.actionActiveValue > 0)
	//{
	//	if (value > curAxis.actionActiveValue && curAxis.value <= curAxis.actionActiveValue)
	//	{
	//		//active
	//		if(!curAxis.activeAnimation.empty())
	//			_model->PlayAnimation(curAxis.activeAnimation);
	//		if(curAxis.activeAction)
	//			InputManager::GetIns().RiseAction(curAxis.activeAction);
	//	}
	//	else if (value < curAxis.actionActiveValue && curAxis.value >= curAxis.actionActiveValue)
	//	{
	//		//inactive
	//		if (!curAxis.inactiveAnimation.empty())
	//			_model->PlayAnimation(curAxis.inactiveAnimation);
	//		if (curAxis.inactiveAction)
	//			InputManager::GetIns().RiseAction(curAxis.inactiveAction);
	//	}
	//}
	//else
	//{
	//	if (value < curAxis.actionActiveValue && curAxis.value >= curAxis.actionActiveValue)
	//	{
	//		//active
	//		if (!curAxis.activeAnimation.empty())
	//			_model->PlayAnimation(curAxis.activeAnimation);
	//		if (curAxis.activeAction)
	//			InputManager::GetIns().RiseAction(curAxis.activeAction);
	//	}
	//	else if (value > curAxis.actionActiveValue && curAxis.value <= curAxis.actionActiveValue)
	//	{
	//		//inactive
	//		if (!curAxis.inactiveAnimation.empty())
	//			_model->PlayAnimation(curAxis.inactiveAnimation);
	//		if (curAxis.inactiveAction)
	//			InputManager::GetIns().RiseAction(curAxis.inactiveAction);
	//	}
	//}
	//curAxis.value = value;
}

void TableObject::OnAnimationPlay(int animationIndex)
{
	if (animationIndex >= modelAnimationVec.size())return;

	if (!modelAnimationVec[animationIndex].controlName.empty())
	{
		_model->PlayAnimation(modelAnimationVec[animationIndex].controlName);
	}

}

void TableObject::_UpdateAxisVec()
{
	//为了防止axis组的多次触发，先收集好信息后再调用此函数进行动作触发


	for (auto& axisGroup : modelAxisVec)
	{
		//根据组里的数值计算新的value(模长)
		float newValue=0.f;
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




