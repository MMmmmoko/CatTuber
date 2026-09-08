//这个修改在
//void Menu::ShowMenu(const DString& xml, const UiPoint& point, MenuPopupPosType popupPosType, bool noFocus, MenuItem* pOwner)
//对于没有父窗口的菜单设置样式以防止在任务栏出现图标



createWndParam.m_dwExStyle = kWS_EX_TOPMOST | kWS_EX_LAYERED;
if (!m_pParentWindow)
createWndParam.m_dwExStyle |= kWS_EX_TOOLWINDOW;
