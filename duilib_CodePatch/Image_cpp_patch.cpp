//由于一些原因，CatTuber必须改动duilib的部分代码
//为了方便更新维护，修改的代码不存放在源文件中。


//这个修改是增加关于SVG颜色的私有属性

//#include "duilib/Image/Image.h"
//
////只对cpp的话直接用cpp文件，这样在更新库代码的时候会有重复定义的提醒
//namespace ui
//{
//    ImageLoadAttribute Image::GetImageLoadAttribute() const
//    {
//        return ImageLoadAttribute(m_imageAttribute.m_srcWidth.c_str(),
//            m_imageAttribute.m_srcHeight.c_str(),
//            m_imageAttribute.m_srcDpiScale,
//            m_imageAttribute.m_bHasSrcDpiScale,
//            m_imageAttribute.m_iconSize,
//            m_imageAttribute.svgFillColor.c_str());
//    }
//
//
//
//    //ImageDecoder修改SKIASVG为nanosvg
//
//
//
//
//
//}
