#ifndef _ImageCropperControl_h
#define _ImageCropperControl_h




//图片裁剪器
class ImageCropperControl:public ui::SupportWeakCallback /*:public ui::Box*/
{

public:
	//ImageCropperControl(ui::Window* pWindow);
	//ImageCropperControl();



	void InitControls(ui::Window* pWindow);

	bool SetImageFromFile(const char* imageFilePath);
	//bool SetImageFromMemFile(uint8_t* imageFile);
	void SetAspectRatio(uint32_t wfactor, uint32_t hfactor);



	void SetZoomScale(float newScale);


	std::string WriteImage(const char* targetFolder,const char* fileNameWithoutExtension);


private:
	bool HandleEvent(const ui::EventArgs& e);
	bool OnMouseButtonDown(const ui::EventArgs& e);
	bool OnMouseMove(const ui::EventArgs& e);
	bool OnMouseButtonUp(const ui::EventArgs& e);
	//void OnMouseWheel(const ui::EventArgs& e);
	//true 放大，false缩小
	void ViewScale(bool up,float mouseX,float mouseY);



	void UpdateImage();


	//拖动状态
	ui::UiPoint downPos;
	ui::UiPointF downImageOff;
	ui::UiPoint curPos;
	bool buttonDown = false;
	bool draging = false;
	int32_t imgWidth=400;
	int32_t imgHeight=300;
	int32_t containerW;
	int32_t containerH;
	int32_t previewW;//用于计算预览的src坐标的中间值，与实际预览控件的大小无关
	int32_t previewH;
	uint32_t aspectRatioW=4;
	uint32_t aspectRatioH=3;

	float leftlimit;
	float rightlimit;
	float toplimit;
	float bottomlimit;

	//预览与导出的时候使用
	float srcLeft_Preview;
	float srcRight_Preview;
	float srcTop_Preview;
	float srcBottom_Preview;





	//图像状态
	ui::UiPointF realimageOff;
	ui::UiPointF imageOff;
	float scale=1.f;
	float scaleMin=1.f;
	



	//virtual void Paint(ui::IRender* pRender, const ui::UiRect& rcPaint)override;

	//std::unique_ptr<ui::Image> image;

	//一个控件有三个图像可同时绘制:背景、normal、fore
	ui::Box* cropperContainer;
	ui::Control* imgControl;
	ui::Control* imgclipMask;
	ui::Control* imgPreviewControl=nullptr;
	ui::Slider* zoomSlider =nullptr;
	std::string imgFilePath;

};









#endif