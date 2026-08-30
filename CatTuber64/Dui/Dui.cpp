#include"Dui.h"
#include"UiThread.h"
#include"Form/MainUiForm.h"



#include"ImageDecoder/ImageDecoder_EX.h"
#include"../../ThirdPart/skia/include/utils/SkParse.h"
#include"Pack/Pack.h"

static bool duiInited = false;
static std::unique_ptr<UIMainThread> uiMainThread;
static MainUiForm* uiMainForm;



//extern  unsigned int nsvgPatch_getColorName(const char* str);
void Dui::Init()
{
	if (duiInited)return;
	uiMainThread.reset(new UIMainThread);
	uiMainThread->StartWithoutLoop();
	duiInited = true;



	//自定义颜色
	auto& colorManager=ui::GlobalManager::Instance().Color();
	colorManager.AddColor(L"panelBkColor",ui::UiColor( 0xFFF7F7FC));// 高到低 ARGB    低到高BGRA
	colorManager.AddColor(L"panelBorderColor", ui::UiColor(0xFFDBDDE1));

	colorManager.AddColor(L"textNormalColor", ui::UiColor(0xFF414A5E));
	colorManager.AddColor(L"textLightColor", ui::UiColor(0xFF7C8291));
	colorManager.AddColor(L"textActiveColor", ui::UiColor(0xFF283248));
	colorManager.AddColor(L"itemHoverColor", ui::UiColor(0xFFE5E6E7));
	colorManager.AddColor(L"itemPushColor", ui::UiColor(0xFFCACDCF));
	colorManager.AddColor(L"itemSeparatorColor", ui::UiColor(0xFFEBEDF1));

	colorManager.AddColor(L"iconNormalColor", ui::UiColor(0xFFabb1b6));
	colorManager.AddColor(L"iconNormalColor_high", ui::UiColor(0xFF8E99A6));
	colorManager.AddColor(L"iconHotColor", ui::UiColor(0xFF283248));
	colorManager.AddColor(L"iconDisableColor", ui::UiColor(0xFFe2e4e6));



	colorManager.AddColor(L"shadowColor", ui::UiColor(0x38000000));//阴影色

	colorManager.AddColor(L"warningColor", ui::UiColor(0xffdf3664));//警告文本或图标的颜色

	colorManager.AddColor(L"subjectColor", ui::UiColor(0xFFff7bb8));//主题色
	colorManager.AddColor(L"subjectColor_hot", ui::UiColor(0xFFFF95D0));//主题色
	colorManager.AddColor(L"subjectColor_push", ui::UiColor(0xFFF159AE));//主题色
	colorManager.AddColor(L"subjectColor_content", ui::UiColor(0xFFFFFFFF));//主题色下的内容颜色


	//设置默认文字颜色减少xml编辑量
	colorManager.SetDefaultTextColor(L"textNormalColor");


	auto colorFindFullback = [](const char* colorName,size_t len,SkColor* out)->const char* {
		ui::UiColor color=ui::GlobalManager::Instance().Color().GetColor(ui::StringConvert::UTF8ToWString(colorName));
		if (color.GetA() == 0)return nullptr;
		*out=SkColorSetARGB(color.GetA(), color.GetR(), color.GetG(),color.GetB());
		return colorName+len;
		};

	SkParse::SetColorFindFullback(colorFindFullback);



	//设置Skia的颜色查找fullback
	








	//引用一下函数保证编译器不忽略
	//nsvgPatch_getColorName("red");


	//图片解析器
	ui::GlobalManager::Instance().ImageDecoders().AddImageDecoder(std::make_shared<ui::ImageDecoder_PNG_EX>());
	ui::GlobalManager::Instance().ImageDecoders().AddImageDecoder(std::make_shared<ui::ImageDecoder_JPG_EX>());
	ui::GlobalManager::Instance().ImageDecoders().AddImageDecoder(std::make_shared<ui::ImageDecoder_GIF_EX>());




	//PACK路径识别函数 将以下路径合法化
	//[C:pack.pack][fileInPack.txt]
	auto packFileToUIFullPath = [](const ui::FilePath& packFilePath)-> ui::FilePath
		{
			//如果这里速度变慢，可能应该在确认是Pack路径之后直接返回，而不是确定文件是否存在
			std::wstring path = packFilePath.ToString();
			std::wstring packPath = Pack::PackPath_GetPackPath(path);
			if (packPath.empty())return ui::FilePath();
			Pack pack;
			if (pack.Open(ui::StringConvert::WStringToUTF8(packPath).c_str()))
			{
				if (pack.IsFileExist(ui::StringConvert::WStringToUTF8(Pack::PackPath_GetFilePathInPack(path)).c_str()))
				{
					return packFilePath;
				}
			}

			return ui::FilePath();
		};
	ui::GlobalManager::Instance().SetFilePathToFullPathFuncFunc(packFileToUIFullPath);


}

void Dui::ShutDown()
{
	if (uiMainForm)uiMainForm->Close();
	uiMainForm = NULL;
	if (uiMainThread)
	{
		uiMainThread->OnCleanup();
	}
	uiMainThread.reset();
}

void Dui::OpenMainUiWindow()
{
	if (uiMainForm)
	{
		uiMainForm->ShowWindow(ui::ShowWindowCommands::kSW_SHOW);
	}
	else
	{
		uiMainForm = new MainUiForm;
		uiMainForm->CreateWnd(nullptr, ui::WindowCreateParam(L"CatTuber Settings",true));
		uiMainForm->AttachWindowClose([](const ui::EventArgs&) ->bool{
			uiMainForm = NULL;
			return true;
			});
		uiMainForm->ShowWindow(ui::kSW_SHOW_NORMAL);
	}




}


