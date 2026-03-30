#ifndef _BongoCatHand_h
#define _BongoCatHand_h


#include<SDL3/SDL.h>
#include<vector>
#include"Renderer/GlobalGraphicResourceManager.h"
class BongoCatHand
{

	public:
		BongoCatHand();
		~BongoCatHand();

		void SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
		void SetTextture(SDL_GPUTexture* cattex) { ptexture = cattex; };
		void SetPressPoint(float x, float y);

		void SetStartPoint(float startPointX, float startPointY, float startAngleCOS, float startAngleSIN);
		void SetEndPoint(float endPointX, float endPointY, float endAngleCos, float endAngleSin);
		void SetHandSize(float handWidth, float handLength);
		void SetOffset(float offsetX, float offsetY);



		//void SetViewport(float width, float height);


		void Update();
		void Draw();
		void GetMousePos(float& x, float& y)const;




	private:

		virtual void Update(float dt)noexcept {};


		void _AddCircle(float posx, float posy, float radius, float r, float g, float b, float a);
		struct
		{
			float x = 0;
			float y = 0;
		}pressPos, mousePos;


		struct HandPoint
		{
			float x;
			float y;
			float cos;
			float sin;
		};

		struct
		{
			HandPoint startPoint;
			HandPoint endPoint;
			float width;
			float length;
			float offsetX;
			float offsetY;
		}handparam;



		//float thickness = 7;//单位 像素

		std::vector<float> pss;//防止多次申请内存
		std::vector<float> pss2;

		bool needUpdate = true;
		bool isFirstUpdate = true;
		rendering::VertexBuffer vertexbuffer;

		struct SubData
		{
			float r;
			float g;
			float b;
			float a;
			float _1_width;//视口倒数
			float _1_height;//视口倒数
			float unuse;
			float unuse2;
		}subdata;
		
		//std::shared_ptr<c2d::bind::VertexConstantBuffer<SubData>> vertexCbuf;

		std::shared_ptr<rendering::IndexBuffer> indexbuffer;

		SDL_GPUViewport viewport;
		SDL_GPUShader* vsshader;
		SDL_GPUShader* vsshader_tex;
		SDL_GPUShader* psshader;
		SDL_GPUShader* psshader_tex;

		//struct
		//{
		//	SDL_GPUVertexInputState inputlayout = {};//后方的_vertexBufferDescription和_vertexAttr均是_vertexInputState内部指向的内容
		//	SDL_GPUVertexBufferDescription _vertexBufferDescription = {};//暂时应该每次渲染只需要一组顶点，后续再考虑这里改为数组
		//	SDL_GPUVertexAttribute _vertexAttr[2];//针对live2D开发的，目前live2D顶点只有xy、uv两组(没有颜色)
		//}inputlayout, inputlayout_tex;



		//std::shared_ptr<c2d::bind::InputLayout> inputlayout;
		//std::shared_ptr<c2d::bind::InputLayout> inputlayout_tex;




		SDL_GPUTexture* ptexture;
		std::shared_ptr<rendering::IndexBuffer> indexbuffer_tex;
		rendering::VertexBuffer vertexbuffer_tex;



		struct Vertex
		{
			float x;
			float y;
			float r;
			float g;
			float b;
			float a;
		};
		struct Vertex_Tex
		{
			float x;
			float y;
			float u;
			float v;
		};
		std::vector<Vertex>vertices;
		std::vector<Vertex_Tex>vertices_tex;




};


#endif