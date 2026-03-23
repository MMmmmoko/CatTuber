#include"duilib/Core/WindowBuilder.h"

#include "duilib/Core/GlobalManager.h"
#include "duilib/Core/Window.h"
#include "duilib/Core/Box.h"
#include "duilib/Core/Control.h"
#include "duilib/Core/ControlDragable.h"
#include "duilib/Core/ScrollBar.h"
#include "duilib/Core/WindowCreateAttributes.h"

#include "duilib/Control/TreeView.h"
#include "duilib/Control/DirectoryTree.h"
#include "duilib/Control/Combo.h"
#include "duilib/Control/ComboButton.h"
#include "duilib/Control/FilterCombo.h"
#include "duilib/Control/CheckCombo.h"
#include "duilib/Control/Slider.h"
#include "duilib/Control/Progress.h"
#include "duilib/Control/CircleProgress.h"
#include "duilib/Control/RichEdit.h"
#include "duilib/Control/RichText.h"
#include "duilib/Control/DateTime.h"
#include "duilib/Control/Split.h"
#include "duilib/Control/GroupBox.h"

#include "duilib/Control/ColorControl.h"
#include "duilib/Control/ColorSlider.h"
#include "duilib/Control/ColorPickerRegular.h"
#include "duilib/Control/ColorPickerStatard.h"
#include "duilib/Control/ColorPickerStatardGray.h"
#include "duilib/Control/ColorPickerCustom.h"
#include "duilib/Control/Line.h"
#include "duilib/Control/IPAddress.h"
#include "duilib/Control/HotKey.h"
#include "duilib/Control/HyperLink.h"
#include "duilib/Control/ListCtrl.h"
#include "duilib/Control/PropertyGrid.h"
#include "duilib/Control/TabCtrl.h"
#include "duilib/Control/IconControl.h"
#include "duilib/Control/AddressBar.h"

#include "duilib/Box/HBox.h"
#include "duilib/Box/VBox.h"
#include "duilib/Box/TabBox.h"
#include "duilib/Box/GridBox.h"
#include "duilib/Box/TileBox.h"
#include "duilib/Box/ScrollBox.h"
#include "duilib/Box/ListBox.h"
#include "duilib/Box/VirtualListBox.h"

#include "duilib/Utils/StringUtil.h"
#include "duilib/Utils/AttributeUtil.h"
#include "duilib/Utils/FilePathUtil.h"

#include "duilib/third_party/xml/pugixml.hpp"
#include <set>

//排除对xmns的解析
namespace ui
{
    void WindowBuilder::ParseWindowAttributes(Window* pWindow, const pugi::xml_node& root) const
    {
        ASSERT((pWindow != nullptr) && pWindow->IsWindow());
        if ((pWindow == nullptr) || !pWindow->IsWindow()) {
            return;
        }

        std::set<DString> knownNames;//支持的属性名称
        DString strName;
        DString strValue;

        bool bInitRenderBackendType = false;
        //首先设置"render_backend_type"属性
        for (pugi::xml_attribute attr : root.attributes()) {
            strName = attr.name();
            strValue = attr.value();
            if (strName == _T("render_backend_type")) {
                knownNames.insert(strName);
                RenderBackendType backendType = RenderBackendType::kRaster_BackendType;
                if (StringUtil::IsEqualNoCase(strValue, _T("GL")) || StringUtil::IsEqualNoCase(strValue, _T("GPU"))) {
                    backendType = RenderBackendType::kNativeGL_BackendType;
                }
                else if (StringUtil::IsEqualNoCase(strValue, _T("CPU"))) {
                    backendType = RenderBackendType::kRaster_BackendType;
                }
                else {
                    ASSERT(0);
                }
                pWindow->SetRenderBackendType(backendType);
                bInitRenderBackendType = true;
                break;
            }
        }
        if (!bInitRenderBackendType) {
            //首先初始化Render后台绘制方式, 此调用会创建Render
            pWindow->SetRenderBackendType(RenderBackendType::kRaster_BackendType);
        }

        //首先处理min_size/max_size/use_system_caption，因为其他属性有用到这些个属性的
        for (pugi::xml_attribute attr : root.attributes()) {
            strName = attr.name();
            strValue = attr.value();
            if ((strName == _T("min_size")) || (strName == _T("mininfo"))) {
                knownNames.insert(strName);
                UiSize size;
                AttributeUtil::ParseSizeValue(strValue.c_str(), size);
                pWindow->SetWindowMinimumSize(size, true);
            }
            else if ((strName == _T("max_size")) || (strName == _T("maxinfo"))) {
                knownNames.insert(strName);
                UiSize size;
                AttributeUtil::ParseSizeValue(strValue.c_str(), size);
                pWindow->SetWindowMaximumSize(size, true);
            }
            else if (strName == _T("use_system_caption")) {
                knownNames.insert(strName);
                pWindow->SetUseSystemCaption(strValue == _T("true"));
            }
        }
        //窗口配置的size是否包含阴影
        bool bSizeContainShadow = false;

        //窗口阴影是否开启
        bool bShadowAttached = false;
        bool bHasShadowAttached = false;
        Shadow::ShadowType nShadowType = Shadow::ShadowType::kShadowCount;

        //注：如果use_system_caption为true，则层窗口关闭（因为这两个属性互斥的）
        for (pugi::xml_attribute attr : root.attributes()) {
            strName = attr.name();
            strValue = attr.value();
            if ((strName == _T("size_box")) || (strName == _T("sizebox"))) {
                knownNames.insert(strName);
                UiRect rcSizeBox;
                AttributeUtil::ParseRectValue(strValue.c_str(), rcSizeBox);
                pWindow->SetSizeBox(rcSizeBox, true);
            }
            else if (strName == _T("caption")) {
                knownNames.insert(strName);
                UiRect rcCaption;
                AttributeUtil::ParseRectValue(strValue.c_str(), rcCaption);
                pWindow->SetCaptionRect(rcCaption, true);
            }
            else if (strName == _T("snap_layout_menu")) {
                knownNames.insert(strName);
                pWindow->SetEnableSnapLayoutMenu(strValue == _T("true"));
            }
            else if (strName == _T("sys_menu")) {
                knownNames.insert(strName);
                pWindow->SetEnableSysMenu(strValue == _T("true"));
            }
            else if (strName == _T("sys_menu_rect")) {
                knownNames.insert(strName);
                UiRect rcSysMenuRect;
                AttributeUtil::ParseRectValue(strValue.c_str(), rcSysMenuRect);
                pWindow->SetSysMenuRect(rcSysMenuRect, true);
            }
            else if (strName == _T("icon")) {
                knownNames.insert(strName);
                if (!strValue.empty()) {
                    //设置窗口图标
                    pWindow->SetWindowIcon(strValue);
                }
            }
            else if (strName == _T("text")) {
                knownNames.insert(strName);
                pWindow->SetText(strValue);
            }
            else if ((strName == _T("text_id")) || (strName == _T("textid"))) {
                knownNames.insert(strName);
                pWindow->SetTextId(strValue);
            }
            else if (strName == _T("round_corner") || strName == _T("roundcorner")) {
                knownNames.insert(strName);
                UiSize size;
                AttributeUtil::ParseSizeValue(strValue.c_str(), size);
                pWindow->SetRoundCorner(size.cx, size.cy, true);
            }
            else if (strName == _T("alpha_fix_corner") || strName == _T("alphafixcorner")) {
                knownNames.insert(strName);
                UiRect rc;
                AttributeUtil::ParseRectValue(strValue.c_str(), rc);
                pWindow->SetAlphaFixCorner(rc, true);
            }
            else if (strName == _T("size_contain_shadow")) {
                knownNames.insert(strName);
                //窗口配置的size是否包含阴影
                bSizeContainShadow = (strValue == _T("true"));
            }
            else if ((strName == _T("shadow_attached")) || (strName == _T("shadowattached"))) {
                knownNames.insert(strName);
                //设置是否支持窗口阴影（阴影实现有两种：分层窗口和普通窗口）
                bShadowAttached = (strValue == _T("true"));
                bHasShadowAttached = true;
            }
            else if (strName == _T("shadow_type")) {
                knownNames.insert(strName);
                //设置阴影类型
                Shadow::GetShadowType(strValue, nShadowType);
                if ((nShadowType >= Shadow::ShadowType::kShadowFirst) &&
                    (nShadowType < Shadow::ShadowType::kShadowCount)) {
                    pWindow->SetShadowType((Shadow::ShadowType)nShadowType);
                }
            }
            else if ((strName == _T("shadow_image")) || (strName == _T("shadowimage"))) {
                knownNames.insert(strName);
                //设置阴影图片
                pWindow->SetShadowImage(strValue);
            }
            else if ((strName == _T("shadow_corner")) || (strName == _T("shadowcorner"))) {
                knownNames.insert(strName);
                //设置窗口阴影的九宫格属性
                UiPadding padding;
                AttributeUtil::ParsePaddingValue(strValue.c_str(), padding);
                pWindow->SetShadowCorner(padding);
            }
            else if (strName == _T("shadow_border_round")) {
                knownNames.insert(strName);
                //设置窗口阴影的圆角大小
                UiSize szBorderRound;
                AttributeUtil::ParseSizeValue(strValue.c_str(), szBorderRound);
                pWindow->SetShadowBorderRound(szBorderRound);
            }
            else if (strName == _T("shadow_border_size")) {
                knownNames.insert(strName);
                //设置窗口阴影的边框大小
                pWindow->SetShadowBorderSize(StringUtil::StringToInt32(strValue));
            }
            else if (strName == _T("shadow_border_color")) {
                knownNames.insert(strName);
                //设置窗口阴影的边框颜色
                pWindow->SetShadowBorderColor(strValue);
            }
            else if (strName == _T("shadow_snap")) {
                knownNames.insert(strName);
                //设置阴影是否支持窗口贴边操作
                pWindow->SetEnableShadowSnap(strValue == _T("true"));
            }
            else if ((strName == _T("layered_window")) || (strName == _T("layeredwindow"))) {
                knownNames.insert(strName);
                //设置是否设置分层窗口属性（分层窗口还是普通窗口）
                if (!pWindow->IsUseSystemCaption()) {
                    pWindow->SetLayeredWindow(strValue == _T("true"), false);
                }
            }
            else if (strName == _T("alpha")) {
                knownNames.insert(strName);
                //设置窗口的透明度（0 - 255），仅当使用层窗口时有效，在在UpdateLayeredWindow函数中作为参数使用
                int32_t nAlpha = StringUtil::StringToInt32(strValue);
                ASSERT(nAlpha >= 0 && nAlpha <= 255);
                if ((nAlpha >= 0) && (nAlpha <= 255)) {
                    pWindow->SetLayeredWindowAlpha(nAlpha);
                }
            }
        }

        if (bHasShadowAttached) {
            //后设置，避免受到"shadow_type"的影响
            pWindow->SetShadowAttached(bShadowAttached);
        }

        bool bScaledCX = false;
        bool bScaledCY = false;
        bool bPercentCX = false;
        bool bPercentCY = false;

        //最后设置窗口的初始化大小，因为初始化大小与是否阴影等相关
        bool bLayeredWindowOpacityDefined = false;
        for (pugi::xml_attribute attr : root.attributes()) {
            strName = attr.name();
            strValue = attr.value();
            if (strName == _T("size")) {
                knownNames.insert(strName);
                UiSize windowSize;
                AttributeUtil::ParseWindowSize(pWindow, strValue.c_str(), windowSize, &bScaledCX, &bScaledCY, &bPercentCX, &bPercentCY);
                int32_t cx = windowSize.cx;
                int32_t cy = windowSize.cy;
                UiSize minSize = pWindow->GetWindowMinimumSize();
                UiSize maxSize = pWindow->GetWindowMaximumSize();
                if ((minSize.cx > 0) && (cx < minSize.cx)) {
                    cx = minSize.cx;
                }
                if ((maxSize.cx > 0) && (cx > maxSize.cx)) {
                    cx = maxSize.cx;
                }
                if ((minSize.cy > 0) && (cy < minSize.cy)) {
                    cy = minSize.cy;
                }
                if ((maxSize.cy > 0) && (cy > maxSize.cy)) {
                    cy = maxSize.cy;
                }

                if (!bSizeContainShadow) {
                    //XML配置中指定的窗口大小，如果设置的是固定值，则不包含阴影部分
                    UiPadding rcShadowCorner = pWindow->GetShadowCorner();
                    if (!bPercentCX && pWindow->IsShadowAttached() && !pWindow->IsWindowMaximized()) {
                        pWindow->Dpi().ScaleWindowSize(rcShadowCorner.left);
                        pWindow->Dpi().ScaleWindowSize(rcShadowCorner.right);
                        cx += rcShadowCorner.left + rcShadowCorner.right;
                    }
                    if (!bPercentCY && pWindow->IsShadowAttached() && !pWindow->IsWindowMaximized()) {
                        pWindow->Dpi().ScaleWindowSize(rcShadowCorner.top);
                        pWindow->Dpi().ScaleWindowSize(rcShadowCorner.bottom);
                        cy += rcShadowCorner.top + rcShadowCorner.bottom;
                    }
                }
                AttributeUtil::ValidateWindowSize(pWindow, cx, cy);
                pWindow->SetInitSize(cx, cy);
            }
            else if (strName == _T("opacity")) {
                knownNames.insert(strName);
                //设置窗口的不透明度（0 - 255），该值在SetLayeredWindowAttributes函数中作为参数使用(bAlpha)
                const int32_t nAlpha = StringUtil::StringToInt32(strValue);
                ASSERT(nAlpha >= 0 && nAlpha <= 255);
                if ((nAlpha >= 0) && (nAlpha <= 255)) {
                    pWindow->SetLayeredWindowOpacity(nAlpha);
                    bLayeredWindowOpacityDefined = true;
                }
            }
        }

#if defined (DUILIB_BUILD_FOR_WIN) && !defined (DUILIB_BUILD_FOR_SDL)
        if (pWindow->GetRenderBackendType() == RenderBackendType::kNativeGL_BackendType) {
            //使用OpenGL时，不能使用层窗口
            if (!bLayeredWindowOpacityDefined || (pWindow->GetLayeredWindowOpacity() == 255)) {
                pWindow->SetLayeredWindow(false, false);
            }
            if (pWindow->IsShadowAttached() && !pWindow->IsUseSystemCaption()) {
                //如果使用了阴影，则自动切换为使用系统标题栏，避免出现显示异常
                pWindow->SetUseSystemCaption(true);
            }
        }
#else
        UNUSED_VARIABLE(bLayeredWindowOpacityDefined);
#endif

#ifdef _DEBUG
        //检查是否有不支持的属性，然后预警，减少配置错误问题
        std::vector<DString> unknownNames;
        for (pugi::xml_attribute attr : root.attributes()) {
            strName = attr.name();
            if (knownNames.find(strName) == knownNames.end()) {

                ////
                ////
                ////
                ////
                ////
                ////
                ////
                ////
                ////
                ////排除xmlns
                if(strName!=L"xmlns:xs"&& strName!=L"xmlns")
                    unknownNames.push_back(strName);
                ////
                ////
                ////
                ////
                ////
                ////
                ////
                ////
                ////
                ////
            }
        }
        ASSERT_UNUSED_VARIABLE(unknownNames.empty() && "Found unknown window attributes in xml!");
#endif
    }
}