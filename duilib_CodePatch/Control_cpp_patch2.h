//由于一些原因，CatTuber必须改动duilib的部分代码
//为了方便更新维护，修改的代码不存放在源文件中。


//这个修改为了添加图片自适应填充的支持
//删除如下片段并插入

//bool bAdaptiveDestRect = newImageAttribute.m_bAdaptiveDestRect; //自动适应目标区域（等比例缩放后，按指定对齐方式绘制）
//if (!bAdaptiveDestRect && (!newImageAttribute.m_hAlign.empty() || !newImageAttribute.m_vAlign.empty())) {
//    if (!newImageAttribute.m_hAlign.empty() && (nImageWidth > rcDest.Width())) {
//        bAdaptiveDestRect = true;
//    }
//    else if (!newImageAttribute.m_vAlign.empty() && (nImageHeight > rcDest.Height())) {
//        bAdaptiveDestRect = true;
//    }
//}
//if (bAdaptiveDestRect) {
//    //自动适应目标区域（等比例缩放图片）：根据图片大小，调整绘制区域
//    rcDest = ImageAttribute::CalculateAdaptiveRect(nImageWidth, nImageHeight,
//        rcDest,
//        newImageAttribute.m_hAlign.c_str(),
//        newImageAttribute.m_vAlign.c_str());
//}


//#define GlobalManager_patch_cpp
//#include"../../CatTuber64/duilib_CodePatch/GlobalManager_patch.h"
//#undef GlobalManager_patch_cpp


bool bAdaptiveDestRect = newImageAttribute.m_bAdaptiveDestRect;
bool bAdaptiveDestRectFill = newImageAttribute.m_bAdaptiveDestRectFill;

if (bAdaptiveDestRectFill)
{
    if (nImageWidth * rcDest.Height() > rcDest.Width() * nImageHeight)
    {
        //图片宽于目标，高度方向始终顶满
        //计算SRC宽度
        int32_t srcW = nImageHeight * rcDest.Width() / rcDest.Height();
        int32_t srcX;
        if (newImageAttribute.m_hAlign == _T("left"))
        {
            srcX = 0;
        }
        else if (newImageAttribute.m_hAlign == _T("right"))
        {
            srcX = nImageWidth - srcW;
        }
        else
        {
            //在中心
            srcX = (nImageWidth - srcW) / 2;
        }
        rcSource.left = srcX;
        rcSource.right = srcX + srcW;
        rcSource.top = 0;
        rcSource.bottom = nImageHeight;
    }
    else
    {
        //水平方向顶满
        int32_t srcH = nImageWidth * rcDest.Height() / rcDest.Width();
        int32_t srcY;
        if (newImageAttribute.m_vAlign == _T("top"))
        {
            srcY = 0;
        }
        else if (newImageAttribute.m_hAlign == _T("bottom"))
        {
            srcY = nImageHeight - srcH;
        }
        else
        {
            //在中心
            srcY = (nImageHeight - srcH) / 2;
        }
        rcSource.left = 0;
        rcSource.right = nImageWidth;
        rcSource.top = srcY;
        rcSource.bottom = srcY + srcH;
    }

}
else
{
    if (!bAdaptiveDestRect && (!newImageAttribute.m_hAlign.empty() || !newImageAttribute.m_vAlign.empty())) {
        if (!newImageAttribute.m_hAlign.empty() && (nImageWidth > rcDest.Width())) {
            bAdaptiveDestRect = true;
        }
        else if (!newImageAttribute.m_vAlign.empty() && (nImageHeight > rcDest.Height())) {
            bAdaptiveDestRect = true;
        }
    }
    if (bAdaptiveDestRect) {
        //自动适应目标区域（等比例缩放图片）：根据图片大小，调整绘制区域
        rcDest = ImageAttribute::CalculateAdaptiveRect(nImageWidth, nImageHeight,
            rcDest,
            newImageAttribute.m_hAlign.c_str(),
            newImageAttribute.m_vAlign.c_str());
    }
    else {
        //应用对齐方式后，图片将不再拉伸，而是按原大小展示
        if (!newImageAttribute.m_hAlign.empty()) {
            if (newImageAttribute.m_hAlign == _T("left")) {
                rcDest.right = rcDest.left + nImageWidth;
            }
            else if (newImageAttribute.m_hAlign == _T("center")) {
                rcDest.left = rcDest.CenterX() - nImageWidth / 2;
                rcDest.right = rcDest.left + nImageWidth;
            }
            else if (newImageAttribute.m_hAlign == _T("right")) {
                rcDest.left = rcDest.right - nImageWidth;
            }
            else {
                rcDest.right = rcDest.left + nImageWidth;
            }

            if (newImageAttribute.m_vAlign.empty()) {
                rcDest.bottom = rcDest.top + nImageHeight;
            }
        }
        if (!newImageAttribute.m_vAlign.empty()) {
            if (newImageAttribute.m_vAlign == _T("top")) {
                rcDest.bottom = rcDest.top + nImageHeight;
            }
            else if (newImageAttribute.m_vAlign == _T("center")) {
                rcDest.top = rcDest.CenterY() - nImageHeight / 2;
                rcDest.bottom = rcDest.top + nImageHeight;
            }
            else if (newImageAttribute.m_vAlign == _T("bottom")) {
                rcDest.top = rcDest.bottom - nImageHeight;
            }
            else {
                rcDest.bottom = rcDest.top + nImageHeight;
            }

            if (newImageAttribute.m_hAlign.empty()) {
                rcDest.right = rcDest.left + nImageWidth;
            }
        }
    }
}

//cpp中插入LoadImageInfo函数的ImageLoadPathType::kVirtualPath判别条件前

//if ((rcSource.left < 0) || (rcSource.left >= (int32_t)pBitmap->GetWidth())) {
//    rcSource.left = 0;
//}
//if ((rcSource.top < 0) || (rcSource.top >= (int32_t)pBitmap->GetHeight())) {
//    rcSource.top = 0;
//}
//if ((rcSource.right < 0) || (rcSource.right > (int32_t)pBitmap->GetWidth())) {
//    rcSource.right = (int32_t)pBitmap->GetWidth();
//}
//if ((rcSource.bottom < 0) || (rcSource.bottom > (int32_t)pBitmap->GetHeight())) {
//    rcSource.bottom = (int32_t)pBitmap->GetHeight();
//}