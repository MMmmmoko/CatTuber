#ifndef _UISceneCoverCropper_Dlg_h
#define _UISceneCoverCropper_Dlg_h



#include"UserControl/ImageCropperControl.h"

class UISceneCoverCropper_Dlg :public ui::WindowImplBase
{


public:
	//如果名称有变化返回true，否则返回false
    static bool ShowModalDlg(class UISceneItem* item,const char* imgPath);






private:
    UISceneCoverCropper_Dlg(class UISceneItem* item, const char* imgPath);
    virtual ~UISceneCoverCropper_Dlg() override;

    /** 资源相关接口
     * GetSkinFolder 接口设置你要绘制的窗口皮肤资源路径
     * GetSkinFile 接口设置你要绘制的窗口的 xml 描述文件
     */
	virtual std::wstring GetSkinFolder() override { return L"CatTuber_default"; };
	virtual std::wstring GetSkinFile() override { return L"UISceneCoverCropper_Dlg.xml"; };

    /** 当窗口创建完成以后调用此函数，供子类中做一些初始化的工作
    */
    virtual void OnInitWindow() override;



    bool OnButtonClick(const ui::EventArgs& args);
    bool OnPreviewItemClick(const ui::EventArgs& args);
    bool OnSliderValueChange(const ui::EventArgs& args);
    
    bool DoSaveCover();

    //监听回车键
    virtual LRESULT OnKeyDownMsg(ui::VirtualKeyCode vkCode, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled) override;


    ImageCropperControl imageCropperControl;

    std::string imgPath;

	UISceneItem* pItem = nullptr;
};





#endif