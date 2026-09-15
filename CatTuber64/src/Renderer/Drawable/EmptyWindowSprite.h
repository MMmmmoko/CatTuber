#ifndef _EmptyWindowSprite_h
#define _EmptyWindowSprite_h



#include<SDL3/SDL.h>
#include <memory>
#include"Renderer/Bindable/IndexBuffer.h"
#include"Renderer/Bindable/VertexBuffer.h"
//窗口透明且无内容的时候显示内容
//暂时为灰色半透明圆角矩形
//不能用矩形类代替这个，因为滤镜作用全屏没有坐标非实体概念，矩形是有坐标大小的实体
class EmptyWindowSprite
{

public:
	//EmptyWindowSprite();
	~EmptyWindowSprite() { CleanUp(); };


	bool Avaliable() { return vs_filterVS; };
	void SetUp();
	void CleanUp();


	void Draw(int renderW,int renderH);







private:




	std::shared_ptr<rendering::IndexBuffer> filterIndexbuffer;
	std::shared_ptr<rendering::VertexBuffer> filterVertexbuffer;



	SDL_GPUShader* vs_filterVS=nullptr;
	SDL_GPUShader* ps_emptyWindowPS=nullptr;







};















#endif