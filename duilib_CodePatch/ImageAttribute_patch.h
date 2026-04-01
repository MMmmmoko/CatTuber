//由于一些原因，CatTuber必须改动duilib的部分代码
//为了方便更新维护，修改的代码不存放在源文件中。


//这个修改是增加关于图片填充相关的参数



#ifdef ImageAttribute_patch_h
bool m_bAdaptiveDestRectFill=false;
#endif // ImageAttribute_patch_h


//神他妈有个拷贝构造函数
//ImageAttribute& ImageAttribute::operator=(const ImageAttribute& r)
#ifdef ImageAttribute_patch_cpp_copyc
m_bAdaptiveDestRectFill = r.m_bAdaptiveDestRectFill;
#endif

//void ImageAttribute::ModifyAttribute(const DString& strImageString, const DpiManager& dpi)
#ifdef ImageAttribute_patch_cpp
        else if (name == _T("adaptive_dest_rect_fill")) {
            //自动适应目标区域（等比例缩放图片）
            imageAttribute.m_bAdaptiveDestRectFill = (value == _T("true"));
        }
#endif