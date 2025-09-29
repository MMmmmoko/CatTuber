

#include<SDL3_image/SDL_image.h>
#include <Utils/CubismString.hpp>
#include<Id/CubismIdManager.hpp>
#include<../../Live2DFramework/SDL3Renderer/CubismRenderer_SDL3.hpp>
#include <Motion/CubismMotion.hpp>



#include <CubismModelSettingJson.hpp>
#include <CubismDefaultParameterId.hpp>



#include"Model/Live2DModelBase.h"
#include"Util/Util.h"

#include<filesystem>


#include"AppContext.h"
#include"Renderer/MixDrawList.h"
#include"Item/Scene.h"


#include"Live2DCubismCore.h"

//屏蔽C4276
//DISABLE_WARNING_PUSH
//DISABLE_WARNING_MSVC(4267)
//DISABLE_WARNING_GCC("-Wconversion")
//DISABLE_WARNING_GCC("-Wsign-conversion")
using namespace Live2D::Cubism::Framework;

CubismLive2DModel::~CubismLive2DModel()
{
	ReleaseMotions();
	ReleaseExpressions();
	if(_modelSetting)
	for (csmInt32 i = 0; i < _modelSetting->GetMotionGroupCount(); i++)
	{
		const csmChar* group = _modelSetting->GetMotionGroupName(i);
		ReleaseMotionGroup(group);
	}
	//释放纹理
	for (csmUint32 i = 0; i < _bindTexture.GetSize(); i++)
	{
		SDL_ReleaseGPUTexture(AppContext::GetGraphicDevice(),_bindTexture[i]);
	}
	_bindTexture.Clear();
	delete _modelSetting;
	_modelSetting = NULL;

	//释放motionManagers
	for (auto it = _motionManagers.begin(); it != _motionManagers.end(); it++)
	{
		//0是默认的motionManager，不在这里释放
		if(it->first!=0)
			CSM_DELETE(it->second);
	}
	_motionManagers.clear();

}

bool CubismLive2DModel::LoadFromFile(const char* packPath, const char* pathInPack)
{
	if (!_pack.Open(packPath))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to LoadAssets:%s", packPath);
		return false;
	}


	//判断path类型？？
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"Loading model at %s:%s", packPath, pathInPack);

	std::filesystem::path _tempath= pathInPack;
	_modelHomeDir = _tempath.remove_filename().u8string().c_str();



	//_modelHomeDir = path;
	size_t size;
	uint8_t*  filemem=CreateBuffer(pathInPack,&size);
	if (filemem == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to LoadAssets %s:%s", packPath, pathInPack);
		return false;
	}

	ICubismModelSetting* setting = new CubismModelSettingJson(filemem, static_cast<Csm::csmSizeInt>(size));
	DeleteBuffer(filemem);



	SetupModel(setting);

	if (_model == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Failed to LoadAssets %s:%s", packPath, pathInPack);
		return false;
	}


	CreateRenderer();

	SetupTexture();

	_motionManagers[0] = _motionManager;
	return true;
}



Csm::CubismMotionQueueEntryHandle CubismLive2DModel::StartMotion(const Csm::csmChar* group, Csm::csmInt32 no, Csm::csmInt32 priority, Csm::ACubismMotion::FinishedMotionCallback onFinishedMotionHandler, Csm::ACubismMotion::BeganMotionCallback onBeganMotionHandler)
{
	//获取对应的track
	int track = 0;
	auto trackIter=_animationTrackMap.find(group);
	if (trackIter == _animationTrackMap.end())
	{
		track = 0;
	}
	else
	{
		track = trackIter->second;
	}


	CubismMotionManager* targetMotionManager=_motionManager;
	{
		auto it = _motionManagers.find(track);
		if (it != _motionManagers.end())
		{
			targetMotionManager = it->second;
		}
		else
		{
			targetMotionManager = CSM_NEW CubismMotionManager;
			_motionManagers[track] = targetMotionManager;
		}
	}



	if (priority == Live2DMitonPriority_Force)
	{
		targetMotionManager->SetReservePriority(priority);
	}
	else if (!targetMotionManager->ReserveMotion(priority))
	{
		if (_debugMode)
		{
			SDL_Log("[APP]can't start motion.");
		}
		return InvalidMotionQueueEntryHandleValue;
	}

	const csmString fileName = _modelSetting->GetMotionFileName(group, no);

	//ex) idle_0
	csmString name = Utils::CubismString::GetFormatedString("%s_%d", group, no);
	CubismMotion* motion = static_cast<CubismMotion*>(_motions[name.GetRawString()]);
	csmBool autoDelete = false;

	//motion为空则从文件加载
	if (motion == NULL)
	{
		csmString path = fileName;
		path = _modelHomeDir + path;

		csmByte* buffer;
		size_t size;

		buffer = CreateBuffer(path.GetRawString(), &size);
		motion = static_cast<CubismMotion*>(LoadMotion(buffer, static_cast<Csm::csmSizeInt>(size), NULL, onFinishedMotionHandler, onBeganMotionHandler, _modelSetting, group, no));

		if (motion)
		{
			motion->SetEffectIds(_eyeBlinkIds, _lipSyncIds);
			autoDelete = true; // 終了時にメモリから削除
		}

		DeleteBuffer(buffer, path.GetRawString());
	}
	else
	{
		motion->SetBeganMotionHandler(onBeganMotionHandler);
		motion->SetFinishedMotionHandler(onFinishedMotionHandler);
	}

	//voice
	csmString voice = _modelSetting->GetMotionSoundFileName(group, no);
	if (strcmp(voice.GetRawString(), "") != 0)
	{
		//暂时等待SDL3_mixer发布稳定版本
		//https://github.com/libsdl-org/SDL_mixer

		//csmString path = voice;
		//path = _modelHomeDir + path;
		//_wavFileHandler.Start(path);
	}

	if (_debugMode)
	{
		SDL_Log("[APP]start motion: [%s_%d]", group, no);
	}
	return  targetMotionManager->StartMotionPriority(motion, autoDelete, priority);

}

Csm::CubismMotionQueueEntryHandle CubismLive2DModel::StartRandomMotion(const Csm::csmChar* group, Csm::csmInt32 priority,Csm::ACubismMotion::FinishedMotionCallback onFinishedMotionHandler, Csm::ACubismMotion::BeganMotionCallback onBeganMotionHandler)
{

	if (_modelSetting->GetMotionCount(group) == 0)
	{
		return InvalidMotionQueueEntryHandleValue;
	}
	auto no=SDL_rand(_modelSetting->GetMotionCount(group));
	return StartMotion(group,no,priority, onFinishedMotionHandler,onBeganMotionHandler);
}

std::vector<std::string> CubismLive2DModel::GetParamList()
{
	Csm::csmInt32 paramCount=_model->GetParameterCount();
	std::vector<std::string> paramVec;
	for (int i = 0; i < paramCount; i++)
	{
		Csm::CubismIdHandle id=_model->GetParameterId(i);
		paramVec.push_back(id->GetString().GetRawString());
	}

	return paramVec;
}

std::vector<std::string> CubismLive2DModel::GetAnimationList()
{
	auto motionGroupCount=_modelSetting->GetMotionGroupCount();
	std::vector<std::string> resultVec;
	for (int i = 0; i < motionGroupCount; i++)
	{
		auto groupName=_modelSetting->GetMotionGroupName(i);
		//Idle或IdleBackground
		if (SDL_memcmp(groupName, CATTUBER_MODEL_IDLEGROUPNAME,sizeof(CATTUBER_MODEL_IDLEGROUPNAME)-1))
		{
			//不允许CatTuber软件直接控制待机动画
			continue;
		}
		

		//只获取动画组，触发动画时在动画组里随机分配
		//待机动画交给Live2d自动控制，
		resultVec.push_back(groupName);


		//auto motionCount=_modelSetting->GetMotionCount(groupName);
		//for (int motionNo = 0; motionNo < motionCount; motionNo++)
		//{
		//	_modelSetting->getmo
		//	resultVec.push_back();
		//}


	}
	return resultVec;
}

void CubismLive2DModel::ReleaseMotions()
{
	for (csmMap<csmString, ACubismMotion*>::const_iterator iter = _motions.Begin(); iter != _motions.End(); ++iter)
	{
		ACubismMotion::Delete(iter->Second);
	}
	_motions.Clear();

}

void CubismLive2DModel::ReleaseExpressions()
{
	for (csmMap<csmString, ACubismMotion*>::const_iterator iter = _expressions.Begin(); iter != _expressions.End(); ++iter)
	{
		ACubismMotion::Delete(iter->Second);
	}
	_expressions.Clear();
}

void CubismLive2DModel::ReleaseMotionGroup(const Csm::csmChar* group) const
{
	const csmInt32 count = _modelSetting->GetMotionCount(group);
	for (csmInt32 i = 0; i < count; i++)
	{
		csmString voice = _modelSetting->GetMotionSoundFileName(group,i);
		if(strcmp(voice.GetRawString(),"")!=0)
		{
			csmString path = voice;
			path = _modelHomeDir + path;
			//???怎么官方案例这里突然结束了
			//todo 未来释放音频文件
		}



	}


}

uint8_t* CubismLive2DModel::CreateBuffer(const char* pathInPack, size_t* size)
{
	return _pack.LoadFile(pathInPack, size);
}

void CubismLive2DModel::DeleteBuffer(uint8_t* mem, const char* LogPath)
{
	_pack.ReleaseMem(mem);
}

void CubismLive2DModel::SetupModel(Csm::ICubismModelSetting* setting)
{
	_updating = true;
	_initialized = false;

	_modelSetting = setting;
	csmByte* buffer;
	size_t size;
	//Cubism Model
	if (strcmp(_modelSetting->GetModelFileName(), "") != 0)
	{
		csmString path = _modelSetting->GetModelFileName();
		path = _modelHomeDir + path;

		if (_debugMode)
		{
			SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,"[L2D]create model: %s", setting->GetModelFileName());
		}

		buffer =CreateBuffer(path.GetRawString(), &size);
		LoadModel(buffer, static_cast<Csm::csmSizeInt>(size), _mocConsistency);
		DeleteBuffer(buffer);
	}


	//Expression
	if (_modelSetting->GetExpressionCount() > 0)
	{
		const csmInt32 count = _modelSetting->GetExpressionCount();
		for (csmInt32 i = 0; i < count; i++)
		{
			csmString name = _modelSetting->GetExpressionName(i);
			csmString path = _modelSetting->GetExpressionFileName(i);
			path = _modelHomeDir + path;

			buffer =CreateBuffer(path.GetRawString(), &size);
			ACubismMotion* motion = LoadExpression(buffer, static_cast<Csm::csmSizeInt>(size), name.GetRawString());

			if (motion)
			{
				if (_expressions[name] != NULL)
				{
					ACubismMotion::Delete(_expressions[name]);
					_expressions[name] = NULL;
				}
				_expressions[name] = motion;
			}
			DeleteBuffer(buffer);
		}
	}

	//Physics
	if (strcmp(_modelSetting->GetPhysicsFileName(), "") != 0)
	{
		csmString path = _modelSetting->GetPhysicsFileName();
		path = _modelHomeDir + path;

		buffer =CreateBuffer(path.GetRawString(), &size);
		LoadPhysics(buffer, static_cast<Csm::csmSizeInt>(size));
		DeleteBuffer(buffer);
	}

	//Pose
	if (strcmp(_modelSetting->GetPoseFileName(), "") != 0)
	{
		csmString path = _modelSetting->GetPoseFileName();
		path = _modelHomeDir + path;

		buffer =CreateBuffer(path.GetRawString(), &size);
		LoadPose(buffer, static_cast<Csm::csmSizeInt>(size));
		DeleteBuffer(buffer);
	}

	//EyeBlink
	if (_modelSetting->GetEyeBlinkParameterCount() > 0)
	{
		_eyeBlink = CubismEyeBlink::Create(_modelSetting);
	}

	//Breath
	{
		_breath = CubismBreath::Create();

		csmVector<CubismBreath::BreathParameterData> breathParameters;


		//自动处理其他参数，这里先验证渲染，后续再加入这些自动呼吸参数
		//breathParameters.PushBack(CubismBreath::BreathParameterData(_idParamAngleX, 0.0f, 15.0f, 6.5345f, 0.5f));
		//breathParameters.PushBack(CubismBreath::BreathParameterData(_idParamAngleY, 0.0f, 8.0f, 3.5345f, 0.5f));
		//breathParameters.PushBack(CubismBreath::BreathParameterData(_idParamAngleZ, 0.0f, 10.0f, 5.5345f, 0.5f));
		//breathParameters.PushBack(CubismBreath::BreathParameterData(_idParamBodyAngleX, 0.0f, 4.0f, 15.5345f, 0.5f));

		//Live2D样本给的权重是0.5，用户反馈呼吸参数未满，所以这里把权重调为1
		breathParameters.PushBack(CubismBreath::BreathParameterData(CubismFramework::GetIdManager()
			->GetId(DefaultParameterId::ParamBreath), 0.5f, 0.5f, 3.2345f, 1.f));

		_breath->SetParameters(breathParameters);
	}

	//UserData
	if (strcmp(_modelSetting->GetUserDataFile(), "") != 0)
	{
		csmString path = _modelSetting->GetUserDataFile();
		path = _modelHomeDir + path;
		buffer =CreateBuffer(path.GetRawString(), &size);
		LoadUserData(buffer, static_cast<Csm::csmSizeInt>(size));
		DeleteBuffer(buffer);
	}

	// EyeBlinkIds
	{
		csmInt32 eyeBlinkIdCount = _modelSetting->GetEyeBlinkParameterCount();
		for (csmInt32 i = 0; i < eyeBlinkIdCount; ++i)
		{
			
			_eyeBlinkIds.PushBack(_modelSetting->GetEyeBlinkParameterId(i));
		}
	}

	// LipSyncIds
	{
		csmInt32 lipSyncIdCount = _modelSetting->GetLipSyncParameterCount();
		for (csmInt32 i = 0; i < lipSyncIdCount; ++i)
		{
			_lipSyncIds.PushBack(_modelSetting->GetLipSyncParameterId(i));
		}
	}

	//怎么Live2d的example到这里了才检查..
	if (_modelSetting == NULL || _modelMatrix == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR,"Failed to SetupModel().");
		return;
	}

	//Layout
	csmMap<csmString, csmFloat32> layout;
	_modelSetting->GetLayoutMap(layout);
	_modelMatrix->SetupFromLayout(layout);

	//保存参数快照？
	_model->SaveParameters();

	for (csmInt32 i = 0; i < _modelSetting->GetMotionGroupCount(); i++)
	{
		const csmChar* group = _modelSetting->GetMotionGroupName(i);
		_PreloadMotionGroup(group);
	}

	_motionManager->StopAllMotions();

	_updating = false;
	_initialized = true;
}

void CubismLive2DModel::SetupTexture()
{
#ifdef PREMULTIPLIED_ALPHA_ENABLE
	const bool isPreMult = true;
	// 如果要强行在这里对未合成ALPHA通道的纹理进行合成实验，请将其设置为true。
	const bool isTextureMult = false;
#else
	const bool isPreMult = false;
	const bool isTextureMult = false;
#endif

	_bindTexture.Clear();

	for (csmInt32 modelTextureNumber = 0; modelTextureNumber < _modelSetting->GetTextureCount(); modelTextureNumber++)
	{
		//如果纹理名称为空字符串，则跳过加载和绑定处理
		if (strcmp(_modelSetting->GetTextureFileName(modelTextureNumber), "") == 0)
		{
			continue;
		}


		//从文件读纹理,后续修改读取方式的时候也要改这里
		//这个是相对路径，
		csmString texturePath = _modelSetting->GetTextureFileName(modelTextureNumber);
		
		texturePath = _modelHomeDir + texturePath;
		//从PACK加载文件
		size_t fileSize;
		uint8_t* texMem=_pack.LoadFile(texturePath.GetRawString(),&fileSize);
		if (!texMem)
		{
			SDL_LogError(SDL_LOG_CATEGORY_RENDER, "[L2D] WARNING: Can not find texture: %s,%s", _pack.GetPath(),texturePath.GetRawString());
			//continue;
		}

		SDL_IOStream* io = SDL_IOFromConstMem(texMem, fileSize);
		if (!io) {
			SDL_LogError(SDL_LOG_CATEGORY_RENDER, "SDL_IOFromConstMem failed: %s", SDL_GetError());
			//continue;
		}
		SDL_Surface* surface=IMG_Load_IO(io,true);
		if (texMem)_pack.ReleaseMem(texMem);
		if (!surface)
		{
			SDL_LogError(SDL_LOG_CATEGORY_RENDER,"[L2D] WARNING: Can not find texture: %s", texturePath.GetRawString());
		}
		if (isPreMult)
		{
			//对图像进行预乘    我觉得这个变量命名为 needPreMult比较合理，
			//先进行代码验证，暂时不进行预乘（a不变， r、g、b乘等a）
		}
		SDL_GPUTextureCreateInfo texInfo = {};
		texInfo.type = SDL_GPU_TEXTURETYPE_2D,
			texInfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
			texInfo.width = (surface!=NULL)?static_cast<uint32_t>(surface->w):10,
			texInfo.height = (surface != NULL) ? static_cast<uint32_t>(surface->h) : 10,
			texInfo.layer_count_or_depth = 1,
			//texInfo.num_levels = 1,
			texInfo.num_levels = (uint32_t)floor(log2((double)(texInfo.width > texInfo.height ? texInfo.width : texInfo.height))) + 1; 
			texInfo.usage = (texInfo.num_levels==1)? SDL_GPU_TEXTUREUSAGE_SAMPLER:(SDL_GPU_TEXTUREUSAGE_SAMPLER|SDL_GPU_TEXTUREUSAGE_COLOR_TARGET);
		//SDL_GPUTexture* s

		SDL_GPUTexture* gpuTexture = SDL_CreateGPUTexture(AppContext::GetGraphicDevice(), &texInfo);
		
		GetRenderer<Rendering::CubismRenderer_SDL3>()->BindTexture(modelTextureNumber, gpuTexture);

		if (gpuTexture)
		{


			//上传纹理
			if (surface)
			{
				SDL_GPUTransferBufferCreateInfo tbInfo = {};
				tbInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
				tbInfo.size = surface->pitch * surface->h;


				SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(AppContext::GetGraphicDevice(), &tbInfo);
				void* mapped = SDL_MapGPUTransferBuffer(AppContext::GetGraphicDevice(), transferBuffer, false);
				memcpy(mapped, surface->pixels, tbInfo.size);
				SDL_UnmapGPUTransferBuffer(AppContext::GetGraphicDevice(), transferBuffer);

				SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(AppContext::GetGraphicDevice());
				SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);

				SDL_GPUTextureTransferInfo src = { };
				src.offset = 0;
				src.pixels_per_row = surface->w;
				src.rows_per_layer = surface->h;
				src.transfer_buffer = transferBuffer;

				SDL_GPUTextureRegion dst = {};
				dst.texture = gpuTexture;
				//mip_level layer被初始化为0了，不管
				dst.w = surface->w;
				dst.h = surface->h;
				dst.d = 1;
				

				SDL_UploadToGPUTexture(copyPass, &src, &dst, false);
				SDL_EndGPUCopyPass(copyPass);

				//MIPMAP
				if(texInfo.num_levels>1)
					SDL_GenerateMipmapsForGPUTexture(cmd, gpuTexture);

				SDL_SubmitGPUCommandBuffer(cmd);

				SDL_ReleaseGPUTransferBuffer(AppContext::GetGraphicDevice(), transferBuffer);
			}
			_bindTexture.PushBack(gpuTexture);

		}

		if(surface)SDL_DestroySurface(surface);
	}


	//设置预乘
	GetRenderer<Rendering::CubismRenderer_SDL3>()->IsPremultipliedAlpha(isPreMult);
}



void CubismLive2DModel::Update(float deltaTimeSeconds)
{

	_dragManager->Update(deltaTimeSeconds);
	_dragX = _dragManager->GetX();
	_dragY = _dragManager->GetY();


	csmBool motionUpdated = false;

	//-----------------------------------------------------------------
	_model->LoadParameters(); // 加载之前save的参数



	//if (_motionManager->IsFinished())
	//{
	//	//没有动作播放的时候随机播放待机动作
	//	//StartRandomMotion(MotionGroupIdle, PriorityIdle);
	//}
	//else
	//{
	//	motionUpdated = _motionManager->UpdateMotion(_model, deltaTimeSeconds); // モーションを更新
	//}
	for (auto& track : _motionManagers)
	{
		if (track.first == _idleMotionTrack&& track.second->IsFinished())
		{
			//对于idle所在的动画轨道，没有动作播放的时候随机播放待机动作
			StartRandomMotion("Idle", Csm::Live2DMitonPriority_Idle);
		}


		//如果有动画则进行刷新
		 if (!track.second->IsFinished())
		{
			
			track.second->UpdateMotion(_model, deltaTimeSeconds);
		}
	}

	//对于长期应用的参数
	for (auto& x : _paramSetCache_longterm)
	{
		if (x.op== _ParamInfo::OP_SET)
			GetModel()->SetParameterValue(static_cast<Csm::csmInt32>(x.param),x.value);
		else if (x.op == _ParamInfo::OP_ADD)
			GetModel()->AddParameterValue(static_cast<Csm::csmInt32>(x.param), x.value);
		else if (x.op == _ParamInfo::OP_MULTIPLY)
			GetModel()->MultiplyParameterValue(static_cast<Csm::csmInt32>(x.param), x.value);
	}
	_paramSetCache_longterm.clear();


	_model->SaveParameters(); //保存应用动作后的参数
	//保存是为了之后计算各种参数的最终值后不会影响到下帧（下帧加载参数重新计算，不保存其他状态）
	//保存后设置只影响当前帧的参数改动
	for (auto& x : _paramSetCache_curFrame)
	{
		if (x.op == _ParamInfo::OP_SET)
			GetModel()->SetParameterValue(static_cast<Csm::csmInt32>(x.param), x.value);
		else if (x.op == _ParamInfo::OP_ADD)
			GetModel()->AddParameterValue(static_cast<Csm::csmInt32>(x.param), x.value);
		else if (x.op == _ParamInfo::OP_MULTIPLY)
			GetModel()->MultiplyParameterValue(static_cast<Csm::csmInt32>(x.param), x.value);
	}
	//GetModel()->SetParameterValue(Csm::CubismFramework::GetIdManager()->GetId("CAT_KEY_Q"),1.f);
	_paramSetCache_curFrame.clear();
	//-----------------------------------------------------------------

	// 不透明度
	_opacity = _model->GetModelOpacity();

	//原Live2D example代码在有motion更新的时候不应用眨眼效果，为什么要这么做？
	// まばたき
	//if (!motionUpdated)
	//{
		if (_eyeBlink != NULL)
		{
			// メインモーションの更新がないとき
			_eyeBlink->UpdateParameters(_model, deltaTimeSeconds); // 目パチ
		}
	//}

	if (_expressionManager != NULL)
	{
		_expressionManager->UpdateMotion(_model, deltaTimeSeconds); // 表情でパラメータ更新（相対変化）
	}

	//被拉动时的参数变化
	//_model->AddParameterValue(_idParamAngleX, _dragX * 30); // -30から30の値を加える
	//_model->AddParameterValue(_idParamAngleY, _dragY * 30);
	//_model->AddParameterValue(_idParamAngleZ, _dragX * _dragY * -30);
	// 
	//_model->AddParameterValue(_idParamBodyAngleX, _dragX * 10); // -10から10の値を加える
	// 
	//_model->AddParameterValue(_idParamEyeBallX, _dragX); // -1から1の値を加える
	//_model->AddParameterValue(_idParamEyeBallY, _dragY);

	// 呼吸など
	if (_breath != NULL)
	{
		_breath->UpdateParameters(_model, deltaTimeSeconds);
	}

	// 物理演算の設定
	if (_physics != NULL)
	{
		_physics->Evaluate(_model, deltaTimeSeconds);
	}

	// リップシンクの設定 //后续使用面捕或者声捕， 不使用这个音频功率逻辑
	if (_lipSync)
	{
		//// リアルタイムでリップシンクを行う場合、システムから音量を取得して0〜1の範囲で値を入力します。
		//csmFloat32 value = 0.0f;

		//// 状態更新/RMS値取得
		//_wavFileHandler.Update(deltaTimeSeconds);
		//value = _wavFileHandler.GetRms();

		//for (csmUint32 i = 0; i < _lipSyncIds.GetSize(); ++i)
		//{
		//	_model->AddParameterValue(_lipSyncIds[i], value, 0.8f);
		//}
	}

	// ポーズの設定
	if (_pose != NULL)
	{
		_pose->UpdateParameters(_model, deltaTimeSeconds);
	}

	_model->Update();


}

void CubismLive2DModel::Draw()
{
	//OPTIMIZE:Live2D的HIT AREA会调用DrawCall，即使其中没有任何东西需要绘制。
	//因为Live2D的hit area其实就是个图像为透明的正常图层（纹理为16*16方块）
	//可以想办法指定部分图层不进行绘制？

	Rendering::CubismRenderer_SDL3* renderer = GetRenderer<Rendering::CubismRenderer_SDL3>();
	renderer->UseHighPrecisionMask(true);
	if (_model == NULL /*|| _deleteModel*/ || renderer == NULL)
	{
		return;
	}

	//这里有个投影计算 先不管  (view mat)
	//matrix.MultiplyByMatrix(_modelMatrix);
	//renderer->SetMvpMatrix(&matrix);
	//CubismMatrix44 projection;

	////调渲染问题暂时先手动设置
	//projection.Scale(2.f,2.f);
	//projection.Translate(400,400);

	//projection.MultiplyByMatrix(_modelMatrix);

	//GetRenderer<Rendering::CubismRenderer_SDL3>()->SetMvpMatrix(&projection);




	//renderer->UseHighPrecisionMask(true);




	renderer->DrawModel();
}

void CubismLive2DModel::DrawMix(MixDrawList* pMix, glm::mat4x4& view_projMat)
{
	Rendering::CubismRenderer_SDL3* renderer = GetRenderer<Rendering::CubismRenderer_SDL3>();

	//auto projMat=Csm::Rendering::ConvertToCsmMat(view_projMat);

	//目前没有设置model矩阵， 直接传入view-proj矩阵
	auto csmMat = Csm::Rendering::ConvertToCsmMat(view_projMat);
	csmMat.MultiplyByMatrix(_modelMatrix);
	renderer->Rendering::CubismRenderer::SetMvpMatrix(&csmMat);
	

	renderer->SetMixCallback(MixDrawList::InsertDrawCommandCallback, pMix);
	renderer->SetMixDraw(true);
	Draw();
	renderer->SetMixDraw(false);

}















void CubismLive2DModel::_PreloadMotionGroup(const csmChar* group)
{
	const csmInt32 count = _modelSetting->GetMotionCount(group);

	for (csmInt32 i = 0; i < count; i++)
	{
		//ex) idle_0
		csmString name = Utils::CubismString::GetFormatedString("%s_%d", group, i);
		csmString path = _modelSetting->GetMotionFileName(group, i);
		path = _modelHomeDir + path;

		if (_debugMode)
		{
			SDL_LogInfo(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION,"[L2D]load motion: %s => [%s_%d] ", path.GetRawString(), group, i);
		}

		csmByte* buffer;
		size_t size;
		buffer =CreateBuffer(path.GetRawString(), &size);
		CubismMotion* tmpMotion = static_cast<CubismMotion*>(LoadMotion(buffer, static_cast<Csm::csmSizeInt>(size), name.GetRawString(), NULL, NULL, _modelSetting, group, i));

		if (tmpMotion)
		{
			tmpMotion->SetEffectIds(_eyeBlinkIds, _lipSyncIds);

			if (_motions[name] != NULL)
			{
				ACubismMotion::Delete(_motions[name]);
			}
			_motions[name] = tmpMotion;
		}

		DeleteBuffer(buffer);
	}
}








Live2DModelBase* Live2DModelBase::LoadFromFile(const char* packPath,const char* filepath)
{
	Live2DModelBase* pResult = new Live2DModelBase;
	if (pResult->l2dmodel.LoadFromFile(packPath, filepath))
		return pResult;
	pResult->Release();
	return NULL;
}

Live2DModelBase* Live2DModelBase::LoadFromFolder(const char* packPath, const char* foldPath)
{	
	//Live2D需要在路径中识别*.model3.json
	Pack pack;
	if (!pack.Open(packPath))
	{
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION,"Can not load Live2D Model: Can not open %s", packPath);
		return NULL;
	}
	std::vector<std::string> fileVec=pack.GetFileList(foldPath);

	for (auto& x : fileVec)
	{
		if (util::IsStringEndsWith(x, ".model3.json"))
		{
			//只加载1个模型，读取第一个即可退出
			Live2DModelBase* pResult=new Live2DModelBase;
			if (pResult->l2dmodel.LoadFromFile(packPath, x.c_str()))
				return pResult;
			pResult->Release();
			return NULL;
		}
	}
	SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Live2D Model File Not Found.");
	return NULL;
}



void Live2DModelBase::Update(uint64_t deltaTicksNS)
{
	//Live2D需要转换为秒
	float deltaTimeS = util::NanoTicksToSeconds(deltaTicksNS);
	l2dmodel.Update(deltaTimeS);
}

void Live2DModelBase::Draw()
{
	l2dmodel.Draw();
}

void Live2DModelBase::DrawMix(MixDrawList* pMix)
{

	l2dmodel.DrawMix(pMix,GetScene()->Get2DProj());
}

std::vector<std::string> Live2DModelBase::GetParamList()
{
	return l2dmodel.GetParamList();
}

std::vector<std::string> Live2DModelBase::GetAnimationList()
{
	return l2dmodel.GetAnimationList();
}

void Live2DModelBase::PlayAnimation(const std::string& name, bool loop)
{
	//TODO/FIXME LOOP
	l2dmodel.StartRandomMotion(name.c_str(), Live2DMitonPriority_Force);

}

ParamHandle Live2DModelBase::GetParamHandle(const std::string& param)
{
	//应该直接获取到ParameterIndex，跳过id*到ParameterIndex的转换过程
	return static_cast<ParamHandle>(l2dmodel.GetModel()->GetParameterIndex(Csm::CubismFramework::GetIdManager()->GetId(param.c_str())));
}

HandPosHandle Live2DModelBase::GetHandHandle(const std::string& param)
{
	//在Live2D中HandHandle是drawAbleIndex
	
	for (int i=0;i<l2dmodel.GetModel()->GetDrawableCount();i++)
	{
		auto id=l2dmodel.GetModel()->GetDrawableId(i);
		if (id->GetString().GetRawString() == param)
		{
			uint64_t handle=0;
			UTIL_SETLOW32VALUE(handle,i);
			return handle;
		}
	}
	return INVALID_HANDHANDLE;
}

void Live2DModelBase::GetHandPosFromHandle(HandPosHandle handPosHandle, float* x, float* y)
{
	//if (handPosHandle == INVALID_HANDHANDLE)
	//{
	//	return;
	//}
	//计算handle对应的mesh的中点
	//TODO 斟酌一下具体返回什么坐标系


	auto vertexCount=csmGetDrawableVertexCounts(l2dmodel.GetModel()->GetModel());
	auto vertexPos= csmGetDrawableVertexPositions(l2dmodel.GetModel()->GetModel());
	if (vertexCount&&vertexCount[handPosHandle])
	{
		float resultX=0.f;
		float resultY = 0.f;
		for (int i = 0; i < vertexCount[handPosHandle]; i++)
		{
			resultX += vertexPos[handPosHandle][i].X;
			resultY +=vertexPos[handPosHandle][i].Y;
		}
		resultX /= static_cast<float>(vertexCount[handPosHandle]);
		resultY /= static_cast<float>(vertexCount[handPosHandle]);


		//如在4比3的画布下获取右下角的点的数据的时候
		//经过下面的变换后
		//x在1.3333   y在-1


		*x = l2dmodel.GetModelMatrix()->TransformX(resultX);
		*y = l2dmodel.GetModelMatrix()->TransformY(resultY);
		

	}
	

}

void Live2DModelBase::SetParamValue(ParamHandle param, float value, bool normallizeValue, bool longTerm)
{
	l2dmodel.SetParamValue(param,value, normallizeValue,longTerm);
}

void Live2DModelBase::AddParamValue(ParamHandle param, float value, bool normallizeValue, bool longTerm)
{
	l2dmodel.AddParamValue(param,value, normallizeValue,longTerm);
}

void Live2DModelBase::MultiplyParamValue(ParamHandle param, float value, bool longTerm)
{
	l2dmodel.MultiplyParamValue(param,value,longTerm);
}

void CubismLive2DModel::SetParamValue(ParamHandle param, float value, bool normallizeValue , bool longTerm)
{
	if (normallizeValue)
	{
		float maxValue=_model->GetParameterMaximumValue(static_cast<Csm::csmUint32>(param));
		float minValue=_model->GetParameterMinimumValue(static_cast<Csm::csmUint32>(param));
		if (value > 0.f)
		{
			if(maxValue>0.f)
				value = value * maxValue;
		}
		else
		{
			if (minValue < 0.f)
				value = -1.f*value * minValue;
		}
	}
	if (longTerm)
	{
		_paramSetCache_longterm.push_back({ param,value,_ParamInfo::OP_SET});
	}
	else
	{
		_paramSetCache_curFrame.push_back({ param,value,_ParamInfo::OP_SET });
	}
}

void CubismLive2DModel::AddParamValue(ParamHandle param, float value, bool normallizeValue, bool longTerm)
{
	if (longTerm)
	{
		_paramSetCache_longterm.push_back({ param,value,_ParamInfo::OP_ADD });
	}
	else
	{
		_paramSetCache_curFrame.push_back({ param,value,_ParamInfo::OP_ADD });
	}
}
void CubismLive2DModel::MultiplyParamValue(ParamHandle param, float value, bool longTerm)
{
	if (longTerm)
	{
		_paramSetCache_longterm.push_back({ param,value,_ParamInfo::OP_MULTIPLY });
	}
	else
	{
		_paramSetCache_curFrame.push_back({ param,value,_ParamInfo::OP_MULTIPLY });
	}
}
