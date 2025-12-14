//由于一些原因，CatTuber必须改动duilib的部分代码
//为了方便更新维护，修改的代码不存放在源文件中。


//这个修改是增加关于SVG颜色的私有属性



#ifdef ImageAttribute_patch_h

#include "duilib/Core/UiTypes.h"

namespace ui
{
    class DpiManager;

    /** 图片属性
    */
    class UILIB_API ImageAttribute
    {
    public:
        ImageAttribute();
        ~ImageAttribute();
        ImageAttribute(const ImageAttribute&);
        ImageAttribute& operator=(const ImageAttribute&);

        /** 对数据成员进行初始化
        */
        void Init();

        /** 根据图片参数进行初始化(先调用Init初始化成员变量，再按照传入参数进行更新部分属性)
        * @param [in] strImageString 图片参数字符串
        * @param [in] dpi DPI缩放接口
        */
        void InitByImageString(const DString& strImageString, const DpiManager& dpi);

        /** 根据图片参数修改属性值（仅更新新设置的图片属性, 未包含的属性不进行更新）
        * @param [in] strImageString 图片参数字符串
        * @param [in] dpi DPI缩放接口
        */
        void ModifyAttribute(const DString& strImageString, const DpiManager& dpi);

    public:
        /** 判断rcDest区域是否含有有效值
        * @param [in] rcDest 需要判断的区域
        */
        static bool HasValidImageRect(const UiRect& rcDest);

        /** 对图片的源区域、目标区域、圆角大小进行校验修正和DPI自适应
        * @param [in] imageWidth 图片的宽度
        * @param [in] imageHeight 图片的高度
        * @param [in] dpi DPI缩放接口
        * @param [in] bImageDpiScaled 图片是否做过DPI自适应操作
        * @param [out] rcDestCorners 绘制目标区域的圆角信息，传出参数，内部根据rcImageCorners来设置，然后传出
        * @param [in/out] rcSource 图片区域
        * @param [in/out] rcSourceCorners 图片区域的圆角信息
        */
        static void ScaleImageRect(uint32_t imageWidth, uint32_t imageHeight,
            const DpiManager& dpi, bool bImageDpiScaled,
            UiRect& rcDestCorners,
            UiRect& rcSource, UiRect& rcSourceCorners);

        /** 计算保持比例的自适应绘制区域
         * @param nImageWidth 原始图片宽度
         * @param nImageHeight 原始图片高度
         * @param targetRect 目标矩形区域(left, top, right, bottom)
         * @param hAlign 横向对齐方式 (left, center, right)
         * @param vAlign 纵向对齐方式 (top, center, bottom)
         * @return 自适应后的绘制区域(left, top, right, bottom)
         */
        static UiRect CalculateAdaptiveRect(int32_t nImageWidth, int32_t nImageHeight,
            const UiRect& targetRect,
            const DString& hAlign,
            const DString& vAlign);

    public:
        /** 获取rcSource(未进行DPI缩放)
        */
        UiRect GetImageSourceRect() const;

        /** 获取rcCorner(未进行DPI缩放)
        */
        UiRect GetImageCorner() const;

        /** 获取rcDest(按配置决定是否进行DPI缩放)
        * @param [in] imageWidth 图像的宽度
        * @param [in] imageHeight 图像的高度
        * @param [in] dpi DPI缩放接口
        */
        UiRect GetImageDestRect(int32_t imageWidth, int32_t imageHeight, const DpiManager& dpi) const;

        /** 获取图片属性的内边距
        * @param [in] dpi DPI缩放管理器
        * @return 返回按照传入DPI缩放管理器适应的内边距数据
        */
        UiPadding GetImagePadding(const DpiManager& dpi) const;

        /** 设置图片属性的内边距(内部不做DPI自适应)
        * @param [in] newPadding 需要设置的内边距
        * @param [in] bNeedDpiScale 是否需要对newPadding进行DPI缩放
        * @param [in] dpi 与newPadding数据关联的DPI管理器
        */
        void SetImagePadding(const UiPadding& newPadding, bool bNeedDpiScale, const DpiManager& dpi);

    public:
        //图片文件属性字符串
        UiString m_sImageString;

        //图片文件文件名，含相对路径，不包含属性
        UiString m_sImagePath;

        //设置图片宽度，可以放大或缩小图像：pixels或者百分比%，比如300，或者30%
        UiString m_srcWidth;

        //设置图片高度，可以放大或缩小图像：pixels或者百分比%，比如200，或者30%
        UiString m_srcHeight;

        //rcSource的DPI自适应属性（仅当bHasSrcDpiScale为true时有效）
        bool m_srcDpiScale;

        //加载图片时，是否设置了DPI自适应属性（"dpi_scale"）
        bool m_bHasSrcDpiScale;

        //rcDest属性的DPI自适应属性（仅当bHasDestDpiScale时有效）
        bool m_destDpiScale;

        //rcDest是否设置了DPI自适应属性（"dest_scale"）
        bool m_bHasDestDpiScale;

        //在绘制目标区域中横向对齐方式(如果指定了rcDest值，则此选项无效)
        UiString m_hAlign;

        //在绘制目标区域中纵向对齐方式(如果指定了rcDest值，则此选项无效)
        UiString m_vAlign;

        //透明度（0 - 255）
        uint8_t m_bFade;

        //横向平铺
        bool m_bTiledX;

        //横向完全平铺，仅当bTiledX为true时有效
        bool m_bFullTiledX;

        //纵向平铺
        bool m_bTiledY;

        //纵向完全平铺，仅当bTiledY为true时有效
        bool m_bFullTiledY;

        //九宫格绘制时，不绘制中间部分（比如窗口阴影，只需要绘制边框，不需要绘制中间部分，以避免不必要的绘制动作）
        bool m_bWindowShadowMode;

        //平铺时的边距（仅当bTiledX为true或者bTiledY为true时有效）
        int32_t m_nTiledMargin;

        //如果是GIF等动画图片，可以指定播放次数 -1 ：一直播放，缺省值。
        int32_t m_nPlayCount;

        //如果是ICO文件，用于指定需要加载的ICO图片的大小
        //(ICO文件中包含很多个不同大小的图片，常见的有256，48，32，16，并且每个大小都有32位真彩、256色、16色之分）
        //目前ICO文件在加载时，只会选择一个大小的ICO图片进行加载，加载后为单张图片
        uint32_t m_iconSize;

        //可绘制标志：true表示允许绘制，false表示禁止绘制
        bool m_bPaintEnabled;

        //是否自动适应目标区域（等比例缩放图片）
        bool m_bAdaptiveDestRect;

        //rcPadding对应的DPI缩放百分比
        uint16_t m_rcPaddingScale;


        //增加SVG填充颜色
        UiString svgFillColor;
    private:
        //绘制目标区域位置和大小(相对于控件区域的位置, 未进行DPI缩放)
        UiRect* m_rcDest;

        //在绘制目标区域中的内边距(如果指定了rcDest值，则此选项无效)
        UiPadding16* m_rcPadding;

        //图片源区域位置和大小(未进行DPI缩放)
        UiRect* m_rcSource;

        //圆角属性(未进行DPI缩放)
        UiRect* m_rcCorner;




    };

} // namespace ui

#endif // ImageAttribute_patch_h



#ifdef ImageAttribute_patch_cpp
void ImageAttribute::ModifyAttribute(const DString& strImageString, const DpiManager& dpi)
{
    if (strImageString.find(_T('=')) == DString::npos) {
        //不含有等号，说明没有属性，直接返回
        return;
    }
    std::vector<std::pair<DString, DString>> attributeList;
    AttributeUtil::ParseAttributeList(strImageString, _T('\''), attributeList);

    ImageAttribute& imageAttribute = *this;
    imageAttribute.m_bHasSrcDpiScale = false;
    imageAttribute.m_bHasDestDpiScale = false;
    for (const auto& attribute : attributeList) {
        const DString& name = attribute.first;
        const DString& value = attribute.second;
        if (name.empty() || value.empty()) {
            continue;
        }
        if (name == _T("file") || name == _T("res")) {
            //图片资源文件名，根据此设置去加载图片资源
            imageAttribute.m_sImagePath = value;
        }
        else if (name == _T("width")) {
            //设置图片宽度，可以放大或缩小图像：pixels或者百分比%，比如300，或者30%
            imageAttribute.m_srcWidth = value;
        }
        else if (name == _T("height")) {
            //设置图片高度，可以放大或缩小图像：pixels或者百分比%，比如200，或者30%
            imageAttribute.m_srcHeight = value;
        }
        else if ((name == _T("src")) || (name == _T("source"))) {
            //图片源区域设置：可以用于仅包含源图片的部分图片内容（比如通过此机制，将按钮的各个状态图片整合到一张大图片上，方便管理图片资源）
            if (imageAttribute.m_rcSource == nullptr) {
                imageAttribute.m_rcSource = new UiRect;
            }
            AttributeUtil::ParseRectValue(value.c_str(), *imageAttribute.m_rcSource);
        }
        else if (name == _T("corner")) {
            //图片的圆角属性，如果设置此属性，绘制图片的时候，采用九宫格绘制方式绘制图片：
            //    四个角不拉伸图片，四个边部分拉伸，中间部分可以拉伸或者根据xtiled、ytiled属性来平铺绘制
            if (imageAttribute.m_rcCorner == nullptr) {
                imageAttribute.m_rcCorner = new UiRect;
            }
            AttributeUtil::ParseRectValue(value.c_str(), *imageAttribute.m_rcCorner);
        }
        else if ((name == _T("dpi_scale")) || (name == _T("dpiscale"))) {
            //加载图片时，按照DPI缩放图片大小（会影响width属性、height属性、sources属性、corner属性）
            imageAttribute.m_srcDpiScale = (value == _T("true"));
            imageAttribute.m_bHasSrcDpiScale = true;
        }
        else if (name == _T("dest")) {
            //设置目标区域，该区域是指相对于所属控件的Rect区域
            if (!value.empty()) {
                if (imageAttribute.m_rcDest == nullptr) {
                    imageAttribute.m_rcDest = new UiRect;
                }
                UiRect& rect = *imageAttribute.m_rcDest;
                DString::value_type* pstr = nullptr;
                rect.left = StringUtil::StringToInt32(value.c_str(), &pstr, 10); ASSERT(pstr);
                AttributeUtil::SkipSepChar(pstr);
                if (*pstr != _T('\0')) {
                    rect.top = StringUtil::StringToInt32(pstr, &pstr, 10); ASSERT(pstr);
                    AttributeUtil::SkipSepChar(pstr);
                }
                if (*pstr != _T('\0')) {
                    rect.right = StringUtil::StringToInt32(pstr, &pstr, 10); ASSERT(pstr);
                    AttributeUtil::SkipSepChar(pstr);
                }
                if (*pstr != _T('\0')) {
                    rect.bottom = StringUtil::StringToInt32(pstr, &pstr, 10); ASSERT(pstr);
                }
            }
        }
        else if ((name == _T("dest_scale")) || (name == _T("destscale"))) {
            //加载时，对dest属性按照DPI缩放图片，仅当设置了dest属性时有效（会影响dest属性）
            //绘制时（内部使用），控制是否对dest属性进行DPI缩放
            imageAttribute.m_destDpiScale = (value == _T("true"));
            imageAttribute.m_bHasDestDpiScale = true;
        }
        else if (name == _T("padding")) {
            //在目标区域中设置内边距
            UiPadding padding;
            AttributeUtil::ParsePaddingValue(value.c_str(), padding);
            imageAttribute.SetImagePadding(padding, true, dpi);
        }
        else if (name == _T("halign")) {
            //在目标区域中设置横向对齐方式            
            ASSERT((value == _T("left")) || (value == _T("center")) || (value == _T("right")));
            if ((value == _T("left")) || (value == _T("center")) || (value == _T("right"))) {
                imageAttribute.m_hAlign = value;
            }
        }
        else if (name == _T("valign")) {
            //在目标区域中设置纵向对齐方式
            ASSERT((value == _T("top")) || (value == _T("center")) || (value == _T("bottom")));
            if ((value == _T("top")) || (value == _T("center")) || (value == _T("bottom"))) {
                imageAttribute.m_vAlign = value;
            }
        }
        else if (name == _T("fade")) {
            //图片的透明度
            imageAttribute.m_bFade = (uint8_t)StringUtil::StringToInt32(value);
        }
        else if (name == _T("xtiled")) {
            //横向平铺
            imageAttribute.m_bTiledX = (value == _T("true"));
        }
        else if ((name == _T("full_xtiled")) || (name == _T("fullxtiled"))) {
            //横向平铺时，保证整张图片绘制
            imageAttribute.m_bFullTiledX = (value == _T("true"));
        }
        else if (name == _T("ytiled")) {
            //纵向平铺
            imageAttribute.m_bTiledY = (value == _T("true"));
        }
        else if ((name == _T("full_ytiled")) || (name == _T("fullytiled"))) {
            //纵向平铺时，保证整张图片绘制
            imageAttribute.m_bFullTiledY = (value == _T("true"));
        }
        else if (name == _T("window_shadow_mode")) {
            //九宫格绘制时，不绘制中间部分（比如窗口阴影，只需要绘制边框，不需要绘制中间部分）
            imageAttribute.m_bWindowShadowMode = (value == _T("true"));
        }
        else if ((name == _T("tiled_margin")) || (name == _T("tiledmargin"))) {
            //平铺绘制时，各平铺图片之间的间隔，包括横向平铺和纵向平铺
            imageAttribute.m_nTiledMargin = StringUtil::StringToInt32(value);
        }
        else if ((name == _T("icon_size")) || (name == _T("iconsize"))) {
            //指定加载ICO文件的图片大小(仅当图片文件是ICO文件时有效)
            imageAttribute.m_iconSize = (uint32_t)StringUtil::StringToInt32(value);
        }
        else if ((name == _T("play_count")) || (name == _T("playcount"))) {
            //如果是GIF、APNG、WEBP等动画图片，可以指定播放次数 -1 ：一直播放，缺省值。
            imageAttribute.m_nPlayCount = StringUtil::StringToInt32(value);
            if (imageAttribute.m_nPlayCount < 0) {
                imageAttribute.m_nPlayCount = -1;
            }
        }
        else if (name == _T("adaptive_dest_rect")) {
            //自动适应目标区域（等比例缩放图片）
            imageAttribute.m_bAdaptiveDestRect = (value == _T("true"));
        }
        else if (name == _T("svgFillColor")) {
            //自动适应目标区域（等比例缩放图片）
            imageAttribute.svgFillColor = value;
        }
        else {
            ASSERT(!"ImageAttribute::ModifyAttribute: fount unknown attribute!");
        }
    }
}
#endif