#include"DuiCommon.h"

static ui::UiColor currentSVGColor;
extern unsigned int nsvgPatch_getColorName(const char* str)
{
	ui::UiColor color;
	if (0 == SDL_strcmp(str, "currentColor"))
	{
		color = currentSVGColor;
	}
	else
		color=ui::GlobalManager::Instance().Color().GetColor(ui::StringConvert::UTF8ToWString(str));

	//COLOR字节序为 B G R A
	//nanoSVG颜色字节序为R G B
	//修改字节序号
	uint32_t newColor=0;

	uint8_t* pChanel = (uint8_t*)&newColor;
	pChanel[0] = color.GetRed();
	pChanel[1] = color.GetGreen();
	pChanel[2] = color.GetBlue();

	return newColor;
}

extern void nsvgPatch_setCurrentColor(const ui::UiColor& color)
{
	currentSVGColor = color;
}

//static void* dummy = (void*)&nsvgPatch_getColorName;