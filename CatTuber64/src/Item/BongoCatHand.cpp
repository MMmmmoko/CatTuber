#include<SDL3/SDL.h>
#include"Renderer/GlobalGraphicResourceManager.h"
#include"AppContext.h"
#include"BongoCatHand.h"

void BongoCatHand::SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	subdata.r = float(r) / 255;
	subdata.g = float(g) / 255;
	subdata.b = float(b) / 255;
	subdata.a = float(a) / 255;
	//vertexCbuf->Update(*pC2D, subdata);
	needUpdate = true;
}

void BongoCatHand::SetPressPoint(float x, float y)
{
	if (pressPos.x == x && pressPos.y == y)
	{
		return;
	}
	needUpdate = true;
	pressPos.x = x;
	pressPos.y = y;
}

void BongoCatHand::SetStartPoint(float startPointX, float startPointY, float startAngleCOS, float startAngleSIN)
{
	if (handparam.startPoint.x == startPointX && handparam.startPoint.y == startPointY
		&& handparam.startPoint.cos == startAngleCOS && handparam.startPoint.sin == startAngleSIN)
	{
		//如果数值不变 返回
		return;
	}
	//数值有变化
	needUpdate = true;
	handparam.startPoint.x = startPointX;
	handparam.startPoint.y = startPointY;
	handparam.startPoint.cos = startAngleCOS;
	handparam.startPoint.sin = startAngleSIN;
}

void BongoCatHand::SetEndPoint(float endPointX, float endPointY, float endAngleCos, float endAngleSin)
{
	if (handparam.endPoint.x == endPointX && handparam.endPoint.y == endPointY
		&& handparam.endPoint.cos == endAngleCos && handparam.endPoint.sin == endAngleSin)
	{
		//如果数值不变 返回
		return;
	}
	//数值有变化
	needUpdate = true;
	handparam.endPoint.x = endPointX;
	handparam.endPoint.y = endPointY;
	handparam.endPoint.cos = endAngleCos;
	handparam.endPoint.sin = endAngleSin;
}

void BongoCatHand::SetHandSize(float handWidth, float handLength)
{
	if (handparam.width == handWidth && handparam.length == handLength)
	{
		return;
	}
	needUpdate = true;
	handparam.width = handWidth;
	handparam.length = handLength;
}

void BongoCatHand::SetOffset(float offsetX, float offsetY)
{
	if (handparam.offsetX == offsetX && handparam.offsetY == offsetY)
	{
		return;
	}
	needUpdate = true;
	handparam.offsetX = offsetX;
	handparam.offsetY = offsetY;
}

//void BongoCatHand::SetViewport(float width, float height)
//{
//	if (width > 0 && height > 0)
//	{
//		subdata._1_width = 1 / width;
//		subdata._1_height = 1 / height;
//
//		//vertexCbuf->Update(*pC2D, subdata);
//		needUpdate = true;
//	}
//}



static std::tuple<float, float> bezier(float ratio, std::vector<float>& points, int length)
{
	float fact[22] = { 0.001f, 0.001f, 0.002f, 0.006f, 0.024f, 0.12f, 0.72f, 5.04f, 40.32f, 362.88f, 3628.8f, 39916.8f, 479001.6f, 6227020.8f, 87178291.2f, 1307674368.0f, 20922789888.0f, 355687428096.0f, 6402373705728.0f, 121645100408832.0f, 2432902008176640.0f, 51090942171709440.0f };
	int nn = (length / 2) - 1;
	float xx = 0.f;
	float yy = 0.f;

	for (int point = 0; point <= nn; point++)
	{
		float tmp = fact[nn] / (fact[point] * fact[nn - point]) * powf(ratio,static_cast<float>( point)) * powf(1 - ratio, static_cast<float>(nn - point));
		xx += points[2 * point] * tmp;
		yy += points[2 * point + 1] * tmp;
	}
	return std::make_tuple(xx / 1000.f, yy / 1000.f);
}


void BongoCatHand::Update()
{
	if (needUpdate)
	{
		needUpdate = false;
		pss.clear();
		pss2.clear();
		vertices.clear();

		//生成数组
		{

			const auto& x = pressPos.x;
			const auto& y = pressPos.y;
			const auto& xstart = handparam.startPoint.x;
			const auto& ystart = handparam.startPoint.y;
			const auto& handwidth = handparam.width;
			const auto& handlength = handparam.length;
			const auto& xend = handparam.endPoint.x;
			const auto& yend = handparam.endPoint.y;
			const auto& cosend = handparam.endPoint.cos;
			const auto& sinend = handparam.endPoint.sin;


			// initializing pss and pss2 (kuvster's magic)
			int oof = 6;
			pss = { handparam.startPoint.x, handparam.startPoint.y };

			float dist = hypot(handparam.startPoint.x - pressPos.x, handparam.startPoint.y - pressPos.y);

			float centreleft0 = handparam.startPoint.x + handparam.startPoint.cos * dist * 0.5f;
			float centreleft1 = handparam.startPoint.y + handparam.startPoint.sin * dist * 0.5f;
			for (int i = 1; i < oof; i++)
			{
				float p0;
				float p1;
				std::vector<float> bez = { xstart, ystart, centreleft0, centreleft1, x, y };
				std::tie(p0, p1) = bezier(float(i) / oof, bez, 6);
				pss.push_back(p0);
				pss.push_back(p1);
			}
			pss.push_back(x);
			pss.push_back(y);
			float a = y - centreleft1;
			float b = centreleft0 - x;
			float le = hypot(a, b);
			a = x + a / le * handwidth;
			b = y + b / le * handwidth;
			int a1 = static_cast<int>(xend);
			int a2 = static_cast<int>(yend);
			dist = hypot(a1 - a, a2 - b);
			float centreright0 = a1 + cosend * dist / 2;
			float centreright1 = a2 + sinend * dist / 2;
			int push = static_cast<int>(handlength);
			float s = x - centreleft0;
			float t = y - centreleft1;
			le = hypot(s, t);
			s *= push / le;
			t *= push / le;
			float s2 = a - centreright0;
			float t2 = b - centreright1;
			le = hypot(s2, t2);
			s2 *= push / le;
			t2 *= push / le;
			for (int i = 1; i < oof; i++)
			{
				float p0;
				float p1;
				std::vector<float> bez = { x, y, x + s, y + t, a + s2, b + t2, a, b };
				std::tie(p0, p1) = bezier(float(i) / oof, bez, 8);
				pss.push_back(p0);
				pss.push_back(p1);
			}
			pss.push_back(a);
			pss.push_back(b);
			for (int i = oof - 1; i > 0; i--)
			{
				float p0;
				float p1;
				std::vector<float> bez = { float(a1), float(a2), centreright0, centreright1, a, b };
				std::tie(p0, p1) = bezier(float(i) / oof, bez, 6);
				pss.push_back(p0);
				pss.push_back(p1);
			}
			pss.push_back(static_cast<float>(a1));
			pss.push_back(static_cast<float>(a2));
			mousePos.x = (a + x) / 2 - 52 - 15 + handparam.offsetX;
			mousePos.y = (b + y) / 2 - 34 + 5 + handparam.offsetY;


			const int iter = 25;

			pss2 = { pss[0] , pss[1] };
			for (int i = 1; i < iter; i++)
			{
				float p0;
				float p1;
				std::tie(p0, p1) = bezier(float(i) / iter, pss, 38);
				pss2.push_back(p0);
				pss2.push_back(p1);
			}
			pss2.push_back(pss[36]);
			pss2.push_back(pss[37]);



			for (int i = 0; i < 26; i++)
			{
				pss2[i * 2] += handparam.offsetX;
				pss2[i * 2 + 1] += handparam.offsetY;
			}
		}
		//生成顶点
		{


			float shad = 77.f / 255;
			//sf::VertexArray edge(sf::TriangleStrip, 52);


			float width = 7;

			_AddCircle(pss2[0], pss2[1], width * 0.5f, subdata.r, subdata.g, subdata.b, subdata.a* shad);


			for (int i = 0; i < 50; i += 2)
			{
				float vec0 = pss2[i] - pss2[i + 2];
				float vec1 = pss2[i + 1] - pss2[i + 3];
				float dist = hypotf(vec0, vec1);
				vertices.push_back({ pss2[i] + vec1 / dist * width / 2, pss2[i + 1] - vec0 / dist * width / 2 ,subdata.r,subdata.g,subdata.b,subdata.a * shad });
				vertices.push_back({ pss2[i] - vec1 / dist * width / 2, pss2[i + 1] + vec0 / dist * width / 2 ,subdata.r,subdata.g,subdata.b,subdata.a * shad });

				width -= 0.08f;//??
			}
			float vec0 = pss2[50] - pss2[48];
			float vec1 = pss2[51] - pss2[49];
			float dist = hypotf(vec0, vec1);

			vertices.push_back({ pss2[50] - vec1 / dist * width / 2, pss2[51] + vec0 / dist * width / 2 ,subdata.r,subdata.g,subdata.b,subdata.a * shad });
			vertices.push_back({ pss2[50] + vec1 / dist * width / 2, pss2[51] - vec0 / dist * width / 2,subdata.r,subdata.g,subdata.b,subdata.a * shad });


			_AddCircle(pss2[50], pss2[51], width * 0.5f, subdata.r, subdata.g, subdata.b, subdata.a* shad);


			// drawing second arm arc
			//sf::VertexArray edge2(sf::TriangleStrip, 52);
			width = 6;
			_AddCircle(pss2[0], pss2[1], width * 0.5f, subdata.r, subdata.g, subdata.b, subdata.a);

			for (int i = 0; i < 50; i += 2)
			{
				vec0 = pss2[i] - pss2[i + 2];
				vec1 = pss2[i + 1] - pss2[i + 3];
				float dist = hypotf(vec0, vec1);
				vertices.push_back({ pss2[i] + vec1 / dist * width / 2, pss2[i + 1] - vec0 / dist * width / 2 , subdata.r, subdata.g, subdata.b, subdata.a });
				vertices.push_back({ pss2[i] - vec1 / dist * width / 2, pss2[i + 1] + vec0 / dist * width / 2, subdata.r, subdata.g, subdata.b, subdata.a });
				width -= 0.08f;
			}
			vec0 = pss2[50] - pss2[48];
			vec1 = pss2[51] - pss2[49];

			dist = hypotf(vec0, vec1);
			vertices.push_back({ pss2[50] - vec1 / dist * width / 2, pss2[51] + vec0 / dist * width / 2 , subdata.r, subdata.g, subdata.b, subdata.a });
			vertices.push_back({ pss2[50] + vec1 / dist * width / 2, pss2[51] - vec0 / dist * width / 2, subdata.r, subdata.g, subdata.b, subdata.a });

			_AddCircle(pss2[50], pss2[51], width * 0.5f, subdata.r, subdata.g, subdata.b, subdata.a);

			//if (isFirstUpdate)
			//{
			//	vertexbuffer.reset(new c2d::bind::VertexBuffer(*pC2D, inputlayout->GetLayout(), vertices, true));
			//	AddBind(vertexbuffer);
			//}
			//else
			//{
			//	vertexbuffer->UpdateVertex(vertices);
			//}
			vertexbuffer.UpdateVertex(vertices.data(),sizeof(Vertex)* vertices.size());


			//手纹理的顶点
			if (isFirstUpdate)
			{
				isFirstUpdate = false;
				needUpdate = true;//不知道为什么关闭l2d后要鼠标移动了才显示



				for (int i = 0; i < 13; i++)
				{
					vertices_tex.push_back({ pss2[i * 2],pss2[i * 2 + 1], 0,0 });
					vertices_tex.push_back({ pss2[50 - i * 2],pss2[50 - i * 2 + 1], 0,0 });
				}

				float left = vertices_tex[0].x;
				float right = vertices_tex[0].x;
				float top = vertices_tex[0].y;
				float bottom = vertices_tex[0].y;
				//计算点的矩形
				for (auto& x : vertices_tex)
				{
					if (x.x < left)left = x.x;
					else if (x.x > right)right = x.x;
					if (x.y < top)top = x.y;
					else if (x.y > bottom)bottom = x.y;
				}
				//设置纹理坐标
				for (auto& x : vertices_tex)
				{
					x.u = (x.x - left) / (right - left);
					x.v = (x.y - top) / (bottom - top);
				}

				vertexbuffer_tex.UpdateVertex(vertices_tex.data(),sizeof(Vertex_Tex)* vertices_tex.size());

				//vertexbuffer_tex.reset(new c2d::bind::VertexBuffer(*pC2D, inputlayout_tex->GetLayout(), vertices_tex.size()));




			}
			else
			{
				for (int i = 0; i < 13; i++)
				{
					vertices_tex[2 * i].x = pss2[i * 2];
					vertices_tex[2 * i].y = pss2[i * 2 + 1];

					vertices_tex[2 * i + 1].x = pss2[50 - i * 2];
					vertices_tex[2 * i + 1].y = pss2[50 - i * 2 + 1];


				}

				vertexbuffer_tex.UpdateVertex(vertices_tex.data(), sizeof(Vertex_Tex)* vertices_tex.size());
				//vertexbuffer_tex->UpdateVertex(vertices_tex);
			}

		}

	}


}


void BongoCatHand::Draw()
{

	//先绑定不需要改变的Bindable
//BindBindable(*pC2D);
	//for (auto& bind : bindableVec)
	//{
	//	bind->Bind(*pC2D);
	//}
	auto pContext = AppContext::GetSDL3RenderContext();
	pContext->SetTopology(SDL_GPU_PRIMITIVETYPE_TRIANGLELIST);
	pContext->SetFragmentSampler(SDL3Sampler::Sampler_Normal);

	//pContext->SetVertexConstantBuffer(0, 1, &vertexCbuf);
	//pContext->SetVertexUniformData(1, &subdata, sizeof(subdata));
	//vertexCbuf;

	pContext->SetBlendState(SDL3Blend::Blend_Normal);
	pContext->SetCullMode(SDL3Cull::Cull_None);



	//绑定资源以绘制手的纹理
	pContext->SetVertexShader(vsshader_tex);
	vertexbuffer_tex.Bind();
	pContext->SetFragmentShader(psshader_tex);
	SDL_GPUTextureSamplerBinding texBinding = {};
	texBinding.sampler = pContext->GetFragmentSampler();
	texBinding.texture = ptexture;
	pContext->SetFragmentTextureResources(0,1,&texBinding);
	indexbuffer_tex->Bind();
	pContext->DrawIndexed(indexbuffer_tex->GetCount());



	pContext->SetVertexShader(vsshader);
	vertexbuffer.Bind();
	pContext->SetFragmentShader(psshader);
	indexbuffer->Bind();
	pContext->DrawIndexed(indexbuffer->GetCount());


}

void BongoCatHand::GetMousePos(float& x, float& y) const
{
	x = mousePos.x;
	y = mousePos.y;
}

void BongoCatHand::_AddCircle(float posx, float posy, float radius, float r, float g, float b, float a)
{
	vertices.push_back({ posx,posy + radius,r,g,b,a });
	vertices.push_back({ posx - radius * 0.7071067f,posy + radius * 0.7071067f,r,g,b,a });
	vertices.push_back({ posx + radius * 0.7071067f,posy + radius * 0.7071067f,r,g,b,a });//0.7071067f=√2/2
	vertices.push_back({ posx - radius,posy,r,g,b,a });
	vertices.push_back({ posx + radius,posy,r,g,b,a });
	vertices.push_back({ posx - radius * 0.7071067f,posy - radius * 0.7071067f,r,g,b,a });//0.7071067f=√2/2
	vertices.push_back({ posx + radius * 0.7071067f,posy - radius * 0.7071067f,r,g,b,a });//0.7071067f=√2/2
	vertices.push_back({ posx,posy - radius,r,g,b,a });
}






inline void _AddTrangleStrip_IndexVec(std::vector<unsigned short>& indices, int index, int count)
{
	//将三角带顶点，以三角列表的形式添加到索引数组中
	//偶数时添加 i  i+1   i+2;
	//奇数时添加 i  i+2   i+1;

	bool b_even = true;
	for (int i = index; i < index + count - 2; i++)
	{
		if (b_even)
		{
			indices.push_back(i);
			indices.push_back(i + 1);
			indices.push_back(i + 2);
		}
		else
		{
			indices.push_back(i);
			indices.push_back(i + 2);
			indices.push_back(i + 1);
		}

		b_even = !b_even;
	}
}




BongoCatHand::BongoCatHand()
{





	vertices.reserve(136);






	vertexbuffer.PushLayout(SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2);//xy
	vertexbuffer.PushLayout(SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4);//rgba

	vertexbuffer_tex.PushLayout(SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2);//xy
	vertexbuffer_tex.PushLayout(SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2);//uv


	//auto layout_handLine = c2d::Dvtx::VertexLayout{}
	//	.Append(c2d::Dvtx::VertexLayout::Position2D)
	//	.Append(c2d::Dvtx::VertexLayout::Float4Color);
	//auto layout_handTex = c2d::Dvtx::VertexLayout{}
	//	.Append(c2d::Dvtx::VertexLayout::Position2D)
	//	.Append(c2d::Dvtx::VertexLayout::Texture2D);








	vsshader = GlobalGraphicResourceManager::GetIns().GetShaderByName("BongoCatHandVS");
	vsshader_tex = GlobalGraphicResourceManager::GetIns().GetShaderByName("BongoCatHandTexVS");
	psshader = GlobalGraphicResourceManager::GetIns().GetShaderByName("BongoCatHandPS");
	psshader_tex = GlobalGraphicResourceManager::GetIns().GetShaderByName("BongoCatHandTexPS");






	indexbuffer = GlobalGraphicResourceManager::GetIns().GetGlobalBindable<rendering::IndexBuffer>("BongoCatHandIndex");
	
	std::vector<unsigned short>indices;

	if (!indexbuffer)
	{

		indices.reserve((8 - 2) * 3 * 4 + (52 - 2) * 3 * 2);

		//BCM的手的顶点数为52+52+8*4=136
		_AddTrangleStrip_IndexVec(indices, 0, 8);
		_AddTrangleStrip_IndexVec(indices, 8, 52);
		_AddTrangleStrip_IndexVec(indices, 60, 8);
		_AddTrangleStrip_IndexVec(indices, 68, 8);
		_AddTrangleStrip_IndexVec(indices, 76, 52);
		_AddTrangleStrip_IndexVec(indices, 128, 8);
		indexbuffer = std::make_shared<rendering::IndexBuffer>();
		indexbuffer->UpdateIndices(indices.data(),SDL_GPU_INDEXELEMENTSIZE_16BIT, static_cast<uint32_t>(indices.size()));

		GlobalGraphicResourceManager::StoreBindable("BongoCatHandIndex", indexbuffer);
	}

	indexbuffer_tex = GlobalGraphicResourceManager::GetIns().GetGlobalBindable<rendering::IndexBuffer>("BongoCatHandTexIndex");

	if (!indexbuffer_tex)
	{
		indices.clear();
		_AddTrangleStrip_IndexVec(indices, 0, 26);
		indexbuffer_tex= std::make_shared<rendering::IndexBuffer>();
		indexbuffer_tex->UpdateIndices(indices.data(), SDL_GPU_INDEXELEMENTSIZE_16BIT, static_cast<uint32_t>(indices.size()));


		GlobalGraphicResourceManager::StoreBindable("BongoCatHandTexIndex", indexbuffer_tex);
	}





	//AddBind(vertexbuffer);
	//viewport.reset(new c2d::bind::Viewport(100,100));
	//AddBind(viewport);






	//AddBind(indexbuffer);



	//纹理部分
	//vertexbuffer_tex.reset(new c2d::bind::VertexBuffer(c2d, vertices_tex));
	//纹理部分的多边形有26个顶点

}

BongoCatHand::~BongoCatHand()
{
}

