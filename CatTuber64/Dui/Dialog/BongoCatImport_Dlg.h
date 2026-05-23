#ifndef _BongoCatImport_Dlg_h
#define _BongoCatImport_Dlg_h





class BongoCatImport_Dlg :public ui::WindowImplBase
{


public:
	//如果导入成功返回true
    static bool ShowModalDlg( ui::Window* parentWindow);






private:
    BongoCatImport_Dlg(const char* u8FolderPath);
    bool isInited = false;
    const wchar_t* errStrID=nullptr;
    bool dlgResult = false;
    std::string u8FolderPath;
    virtual ~BongoCatImport_Dlg() override;

    /** 资源相关接口
     * GetSkinFolder 接口设置你要绘制的窗口皮肤资源路径
     * GetSkinFile 接口设置你要绘制的窗口的 xml 描述文件
     */
	virtual std::wstring GetSkinFolder() override { return L"CatTuber_default"; };
	virtual std::wstring GetSkinFile() override { return L"BongoCatImport_Dlg.xml"; };

    /** 当窗口创建完成以后调用此函数，供子类中做一些初始化的工作
    */
    virtual void OnInitWindow() override;


    bool OnButtonClick(const ui::EventArgs& args);



    bool DoImport();

    ui::Button* btn_modeselect = nullptr;
    
};





#endif