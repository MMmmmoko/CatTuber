
#if 0
#include<SDL3_image/SDL_image.h>


#include <CubismModelSettingJson.hpp>
#include <CubismDefaultParameterId.hpp>



#include"Model/BongoCatModel.h"
#include"Util/Util.h"

//#include<filesystem>


#include"AppContext.h"
#include"Renderer/MixDrawList.h"
#include"Item/Scene.h"









BongoCatModel* BongoCatModel::LoadFromPack(const char* packPath)
{	

	//Live2D需要在路径中识别*.model3.json

	//BongoCatModel* pResult= new BongoCatModel;
	std::unique_ptr<BongoCatModel>pResult(new BongoCatModel);

	Pack pack;
	if (!pack.Open(packPath))
	{
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION,"Can not load BongoCat Model: Can not open %s", packPath);
		return NULL;
	}

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


	//从desc.json中读取重要数据
	//模式
	if (desc["Config"]["Mode"].isUInt64())
	{
		auto modeNum = desc["Config"]["Mode"].asUInt64();
		if (modeNum > BongoCatMverMode_MaxCount || modeNum == 0)modeNum = 1;
		pResult->mode = static_cast<BongoCatMverMode>(modeNum - 1);
	}
	else if (config["mode"].isUInt64())
	{
		auto modeNum = config["mode"].asUInt64();
		if (modeNum > BongoCatMverMode_MaxCount || modeNum == 0)modeNum = 1;
		pResult->mode = static_cast<BongoCatMverMode>(modeNum - 1);
	}


	//是否使用Live2D模型
	if (desc["Config"]["Live2D"].isBool())
	{
		pResult->isUsingLive2D = desc["Config"]["Live2D"].asBool();
	}
	else
	{
		switch (pResult->mode)
		{
		case BongoCatMverMode_Standard:
			pResult->isUsingLive2D = config["standard"]["l2d"].isBool() ? config["standard"]["l2d"].asBool() : pResult->isUsingLive2D;
			break;
		case BongoCatMverMode_Keyboard:
			pResult->isUsingLive2D = config["keyboard"]["l2d"].isBool() ? config["keyboard"]["l2d"].asBool() : pResult->isUsingLive2D;
			break;
		case BongoCatMverMode_Gamepad:
			pResult->isUsingLive2D = config["gamepad"]["l2d"].isBool() ? config["gamepad"]["l2d"].asBool() : pResult->isUsingLive2D;
			break;
		default:
			break;
		}
	}


	//是否使用Live2D 手进行按键
	if (desc["Config"]["Live2DHand"].isBool())
	{
		pResult->isUsingLive2DHand = desc["Config"]["Live2DHand"].asBool();
	}

	//是否使用Live2D桌子
	if (desc["Config"]["Live2DDesk"].isBool())
	{
		pResult->isUsingLive2DDesk = desc["Config"]["Live2DDesk"].asBool();
	}

	//是否使用笔
	if (desc["Config"]["Pen"].isBool())
	{
		pResult->isUsingPen = desc["Config"]["Pen"].asBool();
	}
	else if (config["standard"]["mouse"].isBool())
	{
		pResult->isUsingPen = config["standard"]["mouse"].asBool();
	}




	//基于上述参数加载文件
	bool resourceLoadResult = false;
	switch (pResult->mode)
	{
	case BongoCatMverMode_Standard:
		resourceLoadResult = _LoadResource_Standard(config);
		break;
	case BongoCatMverMode_Keyboard:
		resourceLoadResult = _LoadResource_Keyboard(config);
		break;
	case BongoCatMverMode_Gamepad:
		resourceLoadResult = _LoadResource_Gamepad(config);
		break;
	default:
		break;
	}


	if (!resourceLoadResult)return nullptr;


	return pResult.release();
}



void BongoCatModel::Update(uint64_t deltaTicksNS)
{
	switch (mode)
	{
	case BongoCatMverMode_Standard:
		_Update_Standard(deltaTicksNS);
		break;
	case BongoCatMverMode_Keyboard:
		_Update_Keyboard(deltaTicksNS);
		break;
	case BongoCatMverMode_Gamepad:
		_Update_Gamepad(deltaTicksNS);
		break;
	default:
		break;
	}
}

void BongoCatModel::Draw()
{


	struct
	{
		SDL_FRect bongoCatVp;
		float _1_canvasW;
		float _1_canvasH;
		float _1_rtW;
		float _1_rtH;
	}uniformData;



	auto pContext = AppContext::GetSDL3RenderContext();
	auto& vp = pContext->GetViewport();

	float rtW = vp.w;
	float rtH = vp.h;

	uniformData._1_canvasW = 1.f / decoration.initialSize[0];
	uniformData._1_canvasH = 1.f / decoration.initialSize[1];
	uniformData._1_rtW = 1.f / rtW;
	uniformData._1_rtH = 1.f / rtH;




	if (rtW * decoration.initialSize[1] > decoration.initialSize[0] * rtH)
	{
		//渲染目标比BongoCat比例更宽的时候
		//BongoCat高度占满，两侧留空
		uniformData.bongoCatVp.h = rtH;
		uniformData.bongoCatVp.w = uniformData.bongoCatVp.h * ((float)decoration.initialSize[0] / decoration.initialSize[1]);
		uniformData.bongoCatVp.x = (rtW - uniformData.bongoCatVp.w) * 0.5f;
		uniformData.bongoCatVp.y = 0;
	}
	else
	{
		//BongoCat宽度占满，上下留空
		uniformData.bongoCatVp.w = rtW;
		uniformData.bongoCatVp.h = uniformData.bongoCatVp.w * ((float)decoration.initialSize[1] / decoration.initialSize[0]);
		uniformData.bongoCatVp.x = 0;
		uniformData.bongoCatVp.y = (rtH - uniformData.bongoCatVp.h) * 0.5f;
	}

	//计算uniformData offset
	uniformData.bongoCatVp.x = uniformData.bongoCatVp.x - (scale * 0.5f * uniformData.bongoCatVp.w - 0.5f * uniformData.bongoCatVp.w);
	uniformData.bongoCatVp.y = uniformData.bongoCatVp.y - (scale * 0.5f * uniformData.bongoCatVp.h - 0.5f * uniformData.bongoCatVp.h);
	uniformData.bongoCatVp.x += offsetX;//应该不能单位为像素吧...
	uniformData.bongoCatVp.y += offsetY;
	uniformData.bongoCatVp.w *= scale;
	uniformData.bongoCatVp.h *= scale;







	pContext->SetVertexUniformData(0, &uniformData, sizeof(uniformData));








	switch (mode)
	{
	case BongoCatMverMode_Standard:
		_Draw_Standard();
		break;
	case BongoCatMverMode_Keyboard:
		_Draw_Keyboard();
		break;
	case BongoCatMverMode_Gamepad:
		_Draw_Gamepad();
		break;
	default:
		break;
	}
}

void BongoCatModel::DrawMix(MixDrawList* pMix)
{
	//
	assert(false);//暂时不进行混合绘制
	//如果进行混合绘制，bongo cat应该绘制在索引500处;
}

std::vector<std::string> BongoCatModel::GetParamList()
{
	//应该不会调用到这里
	assert(false);
	return std::vector<std::string>();
}

std::vector<std::string> BongoCatModel::GetAnimationList()
{

	if(l2dmodel)
		return l2dmodel->GetAnimationList();
	return std::vector<std::string>();
}

void BongoCatModel::PlayAnimation(const char* name, bool loop)
{
	if (l2dmodel)
	l2dmodel->PlayAnimation(name, loop);
}







void BongoCatModel::PlayAnimationEX(const  char* name, int index, FinishedAnimationCallback finishedCall, void* finishedCallUserData, BeganAnimationCallback beganCall, void* beganCallUserData)
{
	if (l2dmodel)
	l2dmodel->PlayAnimationEX(name,index,finishedCall,finishedCallUserData,beganCall,beganCallUserData);

}

void BongoCatModel::SetExpression(const char* expressionID)
{
	//区分模型表情和普通表情？
	l2dmodel->SetExpression(expressionID);
}

void BongoCatModel::SetExpression(int expressionIndex)
{
	//区分模型表情和普通表情？
	l2dmodel->SetExpression(expressionIndex);
}

ParamHandle BongoCatModel::GetParamHandle(const std::string& param)
{
	//应该直接获取到ParameterIndex，跳过id*到ParameterIndex的转换过程
	return l2dmodel->GetParamHandle(param);
}

HandPosHandle BongoCatModel::GetHandHandle(const std::string& param)
{
	//在Live2D中HandHandle是drawAbleIndex
	

	//if(isUsingLive2D&&isus)
	HandPosHandle result = INVALID_HANDHANDLE;
	if (l2dmodel)
	{
		result = l2dmodel->GetHandHandle(param);
	}

	if (result == INVALID_HANDHANDLE)
	{
		assert(false);

	}
	return result;


}

void BongoCatModel::GetHandPosFromHandle(HandPosHandle handPosHandle, float* x, float* y)
{
	if (l2dmodel)
	{
		l2dmodel->GetHandPosFromHandle(handPosHandle,x,y);
		return;
	}

}

void BongoCatModel::SetParamValue(ParamHandle param, float value, bool normallizeValue, bool longTerm)
{
	//设置参数值
	//BongoCatMver模型有固定参数，和底层live2D模型无关
	uint32_t* reader = (uint32_t*) & param;
	if (reader[0] == ParamHandleType::ParamHandleType_Key)
	{
		keyStates[reader[1]]= value;
	}
	else if (reader[0] == ParamHandleType::ParamHandleType_HandL)
	{
		lHandDownIndex=SDL_min(reader[1], lHandDownIndex);
	}
	else if (reader[0] == ParamHandleType::ParamHandleType_HandR)
	{
		lHandDownIndex=SDL_min(reader[1], lHandDownIndex);
	}

	//if (l2dmodel)
	//{
	//	l2dmodel->SetParamValue(param, value, normallizeValue, longTerm);
	//}
}

void BongoCatModel::AddParamValue(ParamHandle param, float value, bool normallizeValue, bool longTerm)
{
	uint32_t* reader = (uint32_t*)&param;
	if (reader[0] == ParamHandleType::ParamHandleType_Key)
	{
		keyStates[reader[1]]=keyStates[reader[1]] + value;

	}
	else if (reader[0] == ParamHandleType::ParamHandleType_HandL)
	{
		lHandDownIndex = SDL_min(reader[1], lHandDownIndex);
	}
	else if (reader[0] == ParamHandleType::ParamHandleType_HandR)
	{
		lHandDownIndex = SDL_min(reader[1], lHandDownIndex);
	}
	//if (l2dmodel)
	//{
	//	l2dmodel->AddParamValue(param, value, normallizeValue, longTerm);
	//}
}

void BongoCatModel::MultiplyParamValue(ParamHandle param, float value, bool longTerm)
{
	assert(false);
	//if (l2dmodel)
	//{
	//	l2dmodel->MultiplyParamValue(param, value, longTerm);
	//}
}



#endif