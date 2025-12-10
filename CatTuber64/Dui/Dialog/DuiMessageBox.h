#ifndef _DuiMessageBox_h
#define _DuiMessageBox_h

#include <string>
#include<functional>

class DuiSimpleMessageBox :public ui::WindowImplBase
{

public:
    //如果名称有变化返回true，否则返回false

    enum Button
    {
        BUTTON_OK = 0x1,
        BUTTON_CANCEL = 0x2,
        BUTTON_CLOSE = 0x4
    };

    static Button ShowModalDlg(ui::Window* parent, const std::wstring& title, const std::wstring& content, int buttonFlag);






private:
    DuiSimpleMessageBox(const std::wstring& title, const std::wstring& content, int buttonFlag, Button* resultOut);
    virtual ~DuiSimpleMessageBox() override {};

    /** 资源相关接口
     * GetSkinFolder 接口设置你要绘制的窗口皮肤资源路径
     * GetSkinFile 接口设置你要绘制的窗口的 xml 描述文件
     */
    virtual std::wstring GetSkinFolder() override { return L"CatTuber_default"; };
    virtual std::wstring GetSkinFile() override { return L"DuiSimpleMessageBox.xml"; };

    /** 当窗口创建完成以后调用此函数，供子类中做一些初始化的工作
    */
    virtual void OnInitWindow() override;



    bool OnButtonClick(const ui::EventArgs& args);
    //监听回车键 ESC键
    virtual LRESULT OnKeyDownMsg(ui::VirtualKeyCode vkCode, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled) override;


    std::wstring title;
    std::wstring content;
    int buttonFlag;
    Button* resultOut = nullptr;

};
#endif