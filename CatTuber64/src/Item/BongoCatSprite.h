#ifndef _BongoCatSprite_h
#define _BongoCatSprite_h


#include<SDL3/SDL.h>
#include"Renderer/GlobalGraphicResourceManager.h"



class BongoCatSprite
{
public:
	BongoCatSprite();
	~BongoCatSprite();


	bool Avaliable() { return pTexture; };
	void SetUp(struct SDL_GPUTexture* texture,int w,int h);
	void SetPosition(float x,float y);
	void SetViewInfo(float canvasW,float canvasH,float windowW,float windowH);


	void Draw();


	

private:
	float canvasW;
	float canvasH;
	SDL_GPUTexture* pTexture=nullptr;
	SDL_FRect spriteRect = {};



private:
	//Graphic
	bool needUpdateVertex = true;
	rendering::VertexBuffer vertexbuffer;
	std::shared_ptr<rendering::IndexBuffer> indexbuffer;
	SDL_GPUShader* vs_sprite;
	SDL_GPUShader* ps_sprite;
};


#endif