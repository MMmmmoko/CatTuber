#include"DuiCommon.h"

#include"ImageCropperControl.h"
#include"SDL3_image/SDL_image.h"


//ImageCropperControl::ImageCropperControl(ui::Window* pWindow)
//	/*:Box(pWindow)*/
//{
//
//	imgControl = new ui::Control(pWindow);
//	//clipFrameControl = new ui::Control(pWindow);
//	this->AddItem(imgControl);
//	//this->AddItem(clipFrameControl);
//
//	//image.reset(new ui::Image);
//
//
//
//	
//	AttachButtonDown(ui::UiBind(&ImageCropperControl::OnMouseButtonDown, this, std::placeholders::_1));
//	AttachButtonDown(ui::UiBind(&ImageCropperControl::OnMouseMove, this, std::placeholders::_1));
//	AttachButtonDown(ui::UiBind(&ImageCropperControl::OnMouseButtonUp, this, std::placeholders::_1));
//}



void ImageCropperControl::InitControls(ui::Window* pWindow)
	/*:Box(pWindow)*/
{
	cropperContainer = (ui::Box*)pWindow->FindControl(L"box_cropperContainer");
	imgControl = (ui::Control*)cropperContainer->FindSubControl(L"img_target");
	imgclipMask = (ui::Control*)cropperContainer->FindSubControl(L"img_clipMask");
	imgPreviewControl = (ui::Control*)pWindow->FindControl(L"img_preview");
	zoomSlider = (ui::Slider*)pWindow->FindControl(L"slider_imgScale");

	ASSERT(cropperContainer&& imgControl&& imgclipMask);

	//imgControl->setbk
	
	auto containerRect = cropperContainer->EstimateSize({9999,9999});
	containerW = containerRect.cx.GetInt32();
	containerH = containerRect.cy.GetInt32();



	cropperContainer->AttachButtonDown(ui::UiBind(&ImageCropperControl::OnMouseButtonDown, this, std::placeholders::_1));
	cropperContainer->AttachMouseMove(ui::UiBind(&ImageCropperControl::OnMouseMove, this, std::placeholders::_1));
	cropperContainer->AttachButtonUp(ui::UiBind(&ImageCropperControl::OnMouseButtonUp, this, std::placeholders::_1));
	cropperContainer->AttachAllEvents(ui::UiBind(&ImageCropperControl::HandleEvent, this, std::placeholders::_1));
}

bool ImageCropperControl::SetImageFromFile(const char* imageFilePath)
{
	imgControl->SetUTF8BkImage(imageFilePath);

	auto image = imgControl->GetEstimateImage();
	if (!image)return false;
	imgControl->LoadImageData(*image);
	//auto image=imgControl->GetEstimateImage();
	auto imageInfo=image->GetImageCache();
	imgWidth=imageInfo->GetWidth();
	imgHeight=imageInfo->GetHeight();

	imgFilePath = imageFilePath;



	if (imgPreviewControl)
	{
		imgPreviewControl->SetUTF8BkImage(imageFilePath);
	}


	return true;
}

void ImageCropperControl::SetAspectRatio(uint32_t wfactor, uint32_t hfactor)
{
	if (wfactor == 0 || hfactor == 0)return;
	aspectRatioW =wfactor;
	aspectRatioH = hfactor;
	
	//auto containerRect = cropperContainer->EstimateSize({9999,9999});
	//int32_t containerW = containerRect.cx.GetInt32();
	//int32_t containerH = containerRect.cy.GetInt32();

	bool isHor;
	if (aspectRatioW * containerH > containerW * aspectRatioH)
	{
		//我们的裁剪比例要更扁一些，那么X轴应该拉满

		//imgclipMask->SetFixedWidth(ui::UiFixedInt(containerW), true, false);
		int32_t clipViewH = (containerW * aspectRatioH + aspectRatioW - 1) / aspectRatioW;
		//imgclipMask->SetFixedHeight(ui::UiFixedInt(clipViewH), true, false);
		int32_t borderSize = (containerH - clipViewH + 1) / 2;
		imgclipMask->SetBorderSize({0.f,static_cast<float>(borderSize) ,0.F,static_cast<float>(borderSize) }, false);

		leftlimit = -0.5f* containerW;
		rightlimit = 0.5f * containerW;
		toplimit= -0.5f * containerW* aspectRatioH / aspectRatioW;
		bottomlimit= 0.5f * containerW* aspectRatioH / aspectRatioW;
		isHor = true;


		//预览比例
		previewW = containerW;
		previewH = containerW * aspectRatioH / aspectRatioW;
	}
	else
	{
		//Y轴顶满

		int32_t clipViewW = (containerH * aspectRatioW + aspectRatioH - 1) / aspectRatioH;
		//imgclipMask->SetFixedWidth(ui::UiFixedInt(clipViewW), true, false);
		//imgclipMask->SetFixedHeight(ui::UiFixedInt(containerH), true, false);

		int32_t borderSize = (containerW - clipViewW + 1) / 2;
		imgclipMask->SetBorderSize({ static_cast<float>(borderSize) ,0.F,static_cast<float>(borderSize),0.f }, false);


		leftlimit = -0.5f * containerH* aspectRatioW / aspectRatioH;
		rightlimit = 0.5f * containerH * aspectRatioW / aspectRatioH;
		toplimit = -0.5f * containerH;
		bottomlimit = 0.5f * containerH;
		isHor = false;


		previewH = containerH;
		previewW = containerH * aspectRatioW / aspectRatioH;
	}

	if (aspectRatioW * imgHeight > imgWidth * aspectRatioH)
	{
		//我们的裁剪比原图更扁
		//那么一开始应该让X轴方向画面填满

		float w = isHor ? containerW : (static_cast<float>(containerH)* aspectRatioW/ aspectRatioH);
		scale = w / imgWidth;
		scaleMin = scale;
	}
	else
	{
		float h = isHor ? (static_cast<float>(containerW) * aspectRatioH / aspectRatioW): containerH;
		scale =h / imgHeight;
		scaleMin = scale;
	}
	UpdateImage();
}

bool ImageCropperControl::OnMouseButtonDown(const ui::EventArgs& e)
{
	buttonDown = true;
	downPos=e.ptMouse;
	downImageOff = imageOff;
	return true;
}

bool ImageCropperControl::OnMouseMove(const ui::EventArgs& e)
{
	curPos =e.ptMouse;
	if (buttonDown && !draging
		&& ((curPos.x - downPos.x) * (curPos.x - downPos.x) + (curPos.y - downPos.y) * (curPos.y - downPos.y) > 9))
	{
		draging = true;
		SDL_Log("Start Draging. ");
	}

	if (draging)
	{
		imageOff.x = downImageOff.x + curPos.x - downPos.x;
		imageOff.y = downImageOff.y + curPos.y - downPos.y;
		UpdateImage();
	}
	return true;
}

bool ImageCropperControl::OnMouseButtonUp(const ui::EventArgs& e)
{
	draging = false;
	buttonDown = false;
	imageOff = realimageOff;
	return true;
}


void ImageCropperControl::SetZoomScale(float newScale)
{
	newScale = newScale * scaleMin;
	if (newScale < scaleMin)newScale = scaleMin;

	float mouseX = 0.5f * containerW;
	float mouseY = 0.5f * containerH;

	//计算当前图片中心相对于鼠标位置的坐标,(鼠标位置指向图像中心的向量)
	float offX = (imageOff.x + 0.5f * containerW) - mouseX;
	float offY = (imageOff.y + 0.5f * containerH) - mouseY;
	//对向量进行缩放
	float deltaX = offX * (newScale - scale) / scale;
	float deltaY = offY * (newScale - scale) / scale;

	downImageOff.x += deltaX;
	downImageOff.y += deltaY;

	imageOff.x += deltaX;
	imageOff.y += deltaY;
	scale = newScale;
	UpdateImage();
}

std::string ImageCropperControl::WriteImage(const char* targetFolder, const char* fileNameWithoutExtension)
{

	SDL_Rect srcRect;
	srcRect.x = static_cast<int>(srcLeft_Preview);
	srcRect.y = static_cast<int>(srcTop_Preview);
	srcRect.w = static_cast<int>(srcRight_Preview- srcLeft_Preview);
	srcRect.h = static_cast<int>(srcBottom_Preview - srcTop_Preview);

	if (srcRect.x >= imgWidth)srcRect.x = imgWidth - 1;
	if (srcRect.y >= imgHeight)srcRect.y = imgHeight - 1;
	if (srcRect.x < 0)srcRect.x = 0;
	if (srcRect.y < 0)srcRect.y = 0;
	if (srcRect.w <= 0)srcRect.w =1;
	if (srcRect.h <= 0)srcRect.h = 1;

	if (srcRect.x + srcRect.w> imgWidth)srcRect.w-=(srcRect.x + srcRect.w- imgWidth);
	if (srcRect.y +srcRect.h> imgHeight)srcRect.h -= (srcRect.y + srcRect.h - imgHeight);
	if (srcRect.w <= 0)srcRect.w =1;
	if (srcRect.h <= 0)srcRect.h = 1;




	//不想再进一步深入nimdui内部了，这里从文件读取

	//先判断是否为gif
	if (imgFilePath.size() - 4 == imgFilePath.find(".gif", imgFilePath.size() - 4))
	{
		ASSERT(false&&"save gif not supportted, wait for sdl_image 3.4.0");


		//GIF
		IMG_Animation* pAnimation = IMG_LoadAnimation(imgFilePath.c_str());
		if (!pAnimation)return "";


		for (int i = 0; i < pAnimation->count; i++)
		{
			SDL_Surface* pNewSurface = SDL_CreateSurface(180, 135, SDL_PIXELFORMAT_BGRA8888);

			SDL_BlitSurfaceScaled(pAnimation->frames[i], &srcRect, pNewSurface, nullptr, SDL_SCALEMODE_LINEAR);
			SDL_DestroySurface(pAnimation->frames[i]);
			pAnimation->frames[i] = pNewSurface;
		}
		pAnimation->w = 180;
		pAnimation->h = 135;
		
		//写入封面文件
		std::string writeFilePath = targetFolder;
		writeFilePath = writeFilePath + fileNameWithoutExtension + ".gif";
		//等待SDL_IMAGE 3.4.0正式发布
		//if (IMG_SaveGIFAnimation_IO(pNewSurface, writeFilePath.c_str()))
		//{
		//	std::swap(outFile, writeFilePath);
		//}
		//IMG_ANI
		
	}
	else
	{
		//常规图片
		SDL_Surface* pSurface= IMG_Load(imgFilePath.c_str());
		if (!pSurface)return "";
		SDL_Surface* pNewSurface = SDL_CreateSurface(180,135,SDL_PIXELFORMAT_BGRA8888);
		
		

		//bool result = false;
		std::string outFile;
		if (SDL_BlitSurfaceScaled(pSurface, &srcRect, pNewSurface, nullptr, SDL_SCALEMODE_LINEAR))
		{
			//写入封面文件
			std::string writeFilePath = targetFolder;
			writeFilePath=writeFilePath +fileNameWithoutExtension + ".png";
			if (IMG_SavePNG(pNewSurface, writeFilePath.c_str()))
			{
				std::swap(outFile, writeFilePath);
			}
		}
		SDL_DestroySurface(pSurface);
		SDL_DestroySurface(pNewSurface);
		return outFile;

	}




	return std::string();




}

void ImageCropperControl::ViewScale(bool up, float mouseX, float mouseY)
{

	//SDL_Log("relative pos: %f,%f", mouseX, mouseY);

	//mouseX mouseY为相对于容器左上角的坐标， 单位像素

	float newScale;
	if (up)
	{
		newScale=scale *1.1f;
	}
	else
	{
		newScale=scale * (1.f / 1.1f);
		if (newScale < scaleMin)newScale = scaleMin;
	}
	
	//计算当前图片中心相对于鼠标位置的坐标,(鼠标位置指向图像中心的向量)
	float offX=(imageOff.x + 0.5f * containerW)- mouseX;
	float offY = (imageOff.y + 0.5f * containerH)- mouseY;
	//对向量进行缩放
	float deltaX = offX * (newScale - scale) / scale;
	float deltaY = offY * (newScale - scale) / scale;

	downImageOff.x += deltaX;
	downImageOff.y += deltaY;

	imageOff.x += deltaX;
	imageOff.y += deltaY;
	scale = newScale;

	if (zoomSlider)
	{
		double val = scale / scaleMin;
		static const double areaMax = std::log(20);
		zoomSlider->SetValue(std::log(val) * 65535 / areaMax);
	}

	UpdateImage();

	//if (up)
	//{
	//	scale *= 1.1f;
	//}
	//else
	//{
	//	scale *= (1.f / 1.1f);
	//	if (scale < scaleMin)scale = scaleMin;
	//}
	//UpdateImage();

}






bool ImageCropperControl::HandleEvent(const ui::EventArgs& e)
{
	if (e.eventType == ui::kEventMouseWheel)
	{
		//需要计算鼠标位置
		auto rect=cropperContainer->GetPos();
		ViewScale(e.eventData>0, static_cast<float>(e.ptMouse.x-rect.left), static_cast<float>(e.ptMouse.y-rect.top));
		return true;
	}
	return false;
}



void ImageCropperControl::UpdateImage()
{
	//更新图片信息
	//0----2561
	//中心点imageOff
	//计算角落像素坐标
	float w_2= imgWidth* scale*0.5f;//w/2
	float h_2= imgHeight * scale*0.5f;//w/2

	float left = imageOff.x - w_2;
	float right = imageOff.x + w_2;
	float top = imageOff.y - h_2;
	float bottom = imageOff.y + h_2;


	//如果超出了绝对限制
	if (left > leftlimit)
	{
		right -= (left - leftlimit);
		left = leftlimit;
	}
	if (right < rightlimit)
	{
		left += (rightlimit - right);
		right = rightlimit;
	}
	if (top > toplimit)
	{
		bottom -= (top - toplimit);
		top = toplimit;
	}
	if (bottom < bottomlimit)
	{
		top += (bottomlimit - bottom);
		bottom = bottomlimit;
	}
	realimageOff.x = ( 0.5f * (left + right));
	realimageOff.y = (0.5f * (top + bottom));

	
	float srcLeft, srcRight,srcTop,srcBottom;
	float destLeft, destRight, destTop, destBottom;
	//如果左侧出界
	if (left < -containerW * 0.5f)
	{
		srcLeft = imgWidth*(-containerW * 0.5f- left)/(right - left);
		destLeft = 0;
	}
	else
	{
		//左侧未出界
		srcLeft = 0;
		destLeft = containerW * 0.5f+left;
	}
	//如果右侧出界
	if (right > containerW * 0.5f)
	{
		srcRight = imgWidth*(containerW * 0.5f- left)/(right - left);
		destRight = static_cast<float>(containerW);
	}
	else
	{
		//右侧侧未出界
		srcRight = static_cast<float>(imgWidth);
		destRight = containerW * 0.5f+ right;
	}


	if (top < -containerH * 0.5f)
	{
		srcTop = imgHeight *(-containerH * 0.5f- top)/(bottom - top);
		destTop = 0;
	}
	else
	{
		srcTop = 0;
		destTop = containerH * 0.5f+top;
	}
	if (bottom > containerH * 0.5f)
	{
		srcBottom = imgHeight *(containerH * 0.5f- top)/(bottom - top);
		destBottom = static_cast<float>(containerH);
	}
	else
	{
		srcBottom = static_cast<float>(imgHeight);
		destBottom = containerH * 0.5f+ bottom;
	}


	//构建img参数
	//"file='aaa.jpg' dest='0,0,0,0' source='0,0,0,0' corner='0,0,0,0' fade='255' xtiled='false' ytiled='false'"
	std::wstring newImgParam = L"dest='" +
		std::to_wstring(static_cast<int>(destLeft)) + L"," +
		std::to_wstring(static_cast<int>(destTop)) + L"," +
		std::to_wstring(static_cast<int>(destRight)) + L"," +
		std::to_wstring(static_cast<int>(destBottom)) + L"' source='" +

		std::to_wstring(static_cast<int>(srcLeft)) + L"," +
		std::to_wstring(static_cast<int>(srcTop)) + L"," +
		std::to_wstring(static_cast<int>(srcRight)) + L"," +
		std::to_wstring(static_cast<int>(srcBottom)) + L"'";


	
	auto image = imgControl->GetEstimateImage();
	image->UpdateImageAttribute(newImgParam, imgControl->Dpi());
	cropperContainer->Invalidate();





	////计算preview
	srcLeft_Preview = imgWidth * (-previewW * 0.5f - left) / (right - left);
	srcRight_Preview = imgWidth * (previewW * 0.5f - left) / (right - left);
	srcTop_Preview = imgHeight * (-previewH * 0.5f - top) / (bottom - top);
	srcBottom_Preview = imgHeight * (previewH * 0.5f - top) / (bottom - top);
	if(imgPreviewControl)
	{
		//float srcLeft_Preview, srcRight_Preview, srcTop_Preview, srcBottom_Preview;

		newImgParam= L"source='" +
			std::to_wstring(static_cast<int>(srcLeft_Preview)) + L"," +
			std::to_wstring(static_cast<int>(srcTop_Preview)) + L"," +
			std::to_wstring(static_cast<int>(srcRight_Preview)) + L"," +
			std::to_wstring(static_cast<int>(srcBottom_Preview)) + L"'";


		auto image = imgPreviewControl->GetEstimateImage();
		image->UpdateImageAttribute(newImgParam, imgPreviewControl->Dpi());
		imgPreviewControl->Invalidate();
	}







}







//void ImageCropperControl::Paint(ui::IRender* pRender, const ui::UiRect& rcPaint)
//{
//    auto paintRect = ui::Control::GetPaintRect();
//    if (!ui::UiRect::Intersect(paintRect, rcPaint, GetRect())) {
//        return;
//    }
//
//
//
//    Control::Paint(pRender, rcPaint);
//	Control::PaintImage(pRender,image.get(),L"",-1,nullptr,nullptr,);
//
//	pRender->DrawImage();
//
//
//
//}