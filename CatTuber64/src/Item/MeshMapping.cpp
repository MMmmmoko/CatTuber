#include "Item/MeshMapping.h"


//高斯消元求解
static void gaussian_elimination(float* input, int n)
{
	float* A = input;
	int i = 0;
	int j = 0;
	//m=8rows,n=9cols
	int m = n - 1;
	while (i < m && j < n)
	{
		int maxi = i;
		for (int k = i + 1; k < m; k++)
		{
			if (SDL_fabs(A[k * n + j]) > SDL_fabs(A[maxi * n + j]))
			{
				maxi = k;
			}
		}
		if (A[maxi * n + j] != 0)
		{
			if (i != maxi)
			{
				for (int k = 0; k < n; k++)
				{
					float aux = A[i * n + k];
					A[i * n + k] = A[maxi * n + k];
					A[maxi * n + k] = aux;
				}
			}
			//主行归一化
			float A_ij = A[i * n + j];
			for (int k = 0; k < n; k++)
			{
				A[i * n + k] /= A_ij;
			}


			for (int u = i + 1; u < m; u++)
			{
				float A_uj = A[u * n + j];
				for (int k = 0; k < n; k++)
				{
					A[u * n + k] -= A_uj * A[i * n + k];
				}
			}
			i++;
		}
		j++;
	}
	for (int i = m - 2; i >= 0; i--)
	{
		for (int j = i + 1; j < n - 1; j++)
		{
			A[i * n + m] -= A[i * n + j] * A[j * n + m];
		}
	}
}

static inline float cross(float uX, float uY, float vX, float vY)
{
	return uX * vY - uY * vX;
}
//求解一个点是否在一个四边形内
//static inline void pointInTriangle(float pX, float pY,
//	float p1X, float p1Y, float p2X, float p2Y, float p3X, float p3Y)
//{
//	v0X =
//}

static inline bool pointInQuad(float pX, float pY, 
	float p1X,float p1Y, float p2X, float p2Y,float p3X,float p3Y,float p4X,float p4Y)
{
	float v0X = p2X - p1X;
	float v0Y = p2Y - p1Y;

	float v1X = p3X - p2X;
	float v1Y = p3Y - p2Y;

	float v2X = p4X - p3X;
	float v2Y = p4Y - p3Y;

	float v3X = p1X - p4X;
	float v3Y = p1Y - p4Y;


	float OP1X = pX - p1X;
	float OP1Y = pY - p1Y;

	float OP2X = pX - p2X;
	float OP2Y = pY - p2Y;

	float OP3X = pX - p3X;
	float OP3Y = pY - p3Y;

	float OP4X = pX - p4X;
	float OP4Y = pY - p4Y;

	float c1 = cross(v0X, v0Y, OP1X, OP1Y);
	float c2 = cross(v1X, v1Y, OP2X, OP2Y);
	float c3 = cross(v2X, v2Y, OP3X, OP3Y);
	float c4 = cross(v3X, v3Y, OP4X, OP4Y);
	// 全部同号或为零 => 在形内
	return (c1 >= 0.f && c2 >= 0.f && c3 >= 0.f&& c4>=0.f) ||
		(c1 <= 0.f && c2 <= 0.f && c3 <= 0.f && c4 <= 0.f);

}
void MeshMapping::UnitRect::_CalcHomographyMat()
{
	//计算单应矩阵
	//取左上映射点位的u=0、v=0

	//为了便于编写代码，将点放进数组内
	struct __FPOINT
	{
		float x;
		float y;
	};
	__FPOINT src[4] = { {leftTopX,leftTopY},{rightTopX,rightTopY},{leftBottomX,leftBottomY},{rightBottomX,rightBottomY} };
	__FPOINT dst[4] = { {0.f,0.f},{1.f,0.f},{0.f,1.f},{1.f,1.f} };


	//__FPOINT src[4] = { {0.f,0.f},{0.f,1.f},{1.f,0.f},{1.f,1.f} };
	//__FPOINT dst[4] = { {0.f,0.f},{0.f,1.f},{1.f,0.f},{2.f,2.f} };





	//列举8x9方程组
	float P[8][9] =
	{
	   { -src[0].x, -src[0].y, -1, 0, 0, 0, src[0].x * dst[0].x, src[0].y * dst[0].x, -dst[0].x },//H11
	{ 0, 0, 0, -src[0].x, -src[0].y, -1, src[0].x * dst[0].y, src[0].y * dst[0].y, -dst[0].y },//H12

	{ -src[1].x, -src[1].y, -1, 0, 0, 0, src[1].x * dst[1].x, src[1].y * dst[1].x, -dst[1].x },//H13
	{ 0, 0, 0, -src[1].x, -src[1].y, -1, src[1].x * dst[1].y, src[1].y * dst[1].y,- dst[1].y },//H21

	{ -src[2].x, -src[2].y, -1, 0, 0, 0, src[2].x * dst[2].x, src[2].y * dst[2].x, -dst[2].x },//H22
	{ 0, 0, 0, -src[2].x, -src[2].y, -1, src[2].x * dst[2].y, src[2].y * dst[2].y, -dst[2].y },//H23

	{ -src[3].x, -src[3].y, -1, 0, 0, 0, src[3].x * dst[3].x, src[3].y * dst[3].x, -dst[3].x },//H31
	{ 0, 0, 0, -src[3].x, -src[3].y, -1, src[3].x * dst[3].y, src[3].y * dst[3].y, -dst[3].y }//H32
	};

	gaussian_elimination(&P[0][0], 9);

	H[0][0] = P[0][8];
	H[0][1] = P[1][8];
	H[0][2] = P[2][8];

	H[1][0] = P[3][8];
	H[1][1] = P[4][8];
	H[1][2] = P[5][8];

	H[2][0] = P[6][8];
	H[2][1] = P[7][8];
	H[2][2] = 1.f;



}


void MeshMapping::GetParamValueAtPos(float posX, float posY, float* ParamValueX, float* ParamValueY)
{
	for (int row = 0; row < 5; row++)
	{
		for (int column = 0; column < 5; column++)
		{
			auto& curRect = rectMesh[row][column];

			//进行快速包络检查
			//注意模型空间中Y轴向上
			if (posX< curRect.boundingBoxLeft || posX>curRect.boundingBoxRight
				|| posY< curRect.boundingBoxButtom || posY>curRect.boundingBoxTop)
			{
				//包络检查不通过
				continue;
			}

			//进行精确检查
			//
			if (!pointInQuad(posX, posY, 
				curRect.leftTopX, curRect.leftTopY,
				curRect.rightTopX, curRect.rightTopY,
				curRect.rightBottomX, curRect.rightBottomY,
				curRect.leftBottomX, curRect.leftBottomY
				))
			{
				//点不在格子内
				continue;
			}

			//点在当前格内
			{
			//H mul Pos
				auto& H = curRect.H;
				float u=(H[0][0] * posX + H[0][1] * posY + H[0][2])/ (H[2][0] * posX + H[2][1] * posY + H[2][2]);
				float v=(H[1][0] * posX + H[1][1] * posY + H[1][2])/ (H[2][0] * posX + H[2][1] * posY + H[2][2]);

				//参数范围为-1~1，总长2，切分为5格的话 每格步0.4
				float startParamValueX = -1.f + 0.4f * column;
				//float endParamValueX = startParamValueX+0.4f;
				//格子给的顺序是 1 2 3 4 5
				//格子给的顺序是 6 7 8 9 10
				//在建模中Y参数的方向 是 上为正 所以是Y方向上到下是1到-1
				float startParamValueY = 1.f - 0.4f * row;
				//float endParamValueY = startParamValueY-0.4f;
			

				//UV是0~1、左上到右下、Y向下为正
				float resultValueX = startParamValueX + 0.4f * u;
				float resultValueY = startParamValueY - 0.4f * v;
				*ParamValueX = resultValueX;
				*ParamValueY = resultValueY;

				return;
			}


		}
	}

	//点不在任何一个格子中
	//检测点与哪个格子距离最近
	float mindis;
	int minRowIndex;
	int minColumnIndex;

	//计算最近的方格
	//OPTIMIZE 优化：可以根据pos的方向来减少检测的方格数量
	{
		//对于第1行
		{
			{
				auto& curRect = rectMesh[0][0];
				float dX = curRect.centerPointX - posX;
				float dY = curRect.centerPointY - posY;
				mindis = dX * dX + dY * dY;
				minRowIndex = 0;
				minColumnIndex = 0;
			}
			for (int column = 1; column < 5; column++)
			{
				auto& curRect = rectMesh[0][column];
				float dX = curRect.centerPointX - posX;
				float dY = curRect.centerPointY - posY;
				float dis = dX * dX + dY * dY;
				if (dis < mindis)
				{
					mindis = dis;
					minRowIndex = 0;
					minColumnIndex = column;
				}
			}
		}
		//对于第2~4行
		for (int row = 1; row < 4; row++)
		{
			{
				auto& curRect = rectMesh[row][0];
				float dX = curRect.centerPointX - posX;
				float dY = curRect.centerPointY - posY;
				float dis = dX * dX + dY * dY;
				if (dis < mindis)
				{
					mindis = dis;
					minRowIndex = row;
					minColumnIndex = 0;
				}
			}
			{
				auto& curRect = rectMesh[row][4];
				float dX = curRect.centerPointX - posX;
				float dY = curRect.centerPointY - posY;
				float dis = dX * dX + dY * dY;
				if (dis < mindis)
				{
					mindis = dis;
					minRowIndex = row;
					minColumnIndex = 4;
				}
			}
		}
		//对于第5行
		for (int column = 0; column < 5; column++)
		{
			auto& curRect = rectMesh[4][column];
			float dX = curRect.centerPointX - posX;
			float dY = curRect.centerPointY - posY;
			float dis = dX * dX + dY * dY;
			if (dis < mindis)
			{
				mindis = dis;
				minRowIndex = 4;
				minColumnIndex = column;
			}
		}
	}

	//找到了最近的矩形
	{
		//H mul Pos
		auto& curRect = rectMesh[minRowIndex][minColumnIndex];
		int row = minRowIndex;
		int column = minColumnIndex;


		auto& H = curRect.H;
		float u = (H[0][0] * posX + H[0][1] * posY + H[0][2]) / (H[2][0] * posX + H[2][1] * posY + H[2][2]);
		float v = (H[1][0] * posX + H[1][1] * posY + H[1][2]) / (H[2][0] * posX + H[2][1] * posY + H[2][2]);

		//参数范围为-1~1，总长2，切分为5格的话 每格步0.4
		float startParamValueX = -1.f + 0.4f * column;
		//float endParamValueX = startParamValueX+0.4f;
		//格子给的顺序是 1 2 3 4 5
		//格子给的顺序是 6 7 8 9 10
		//在建模中Y参数的方向 是 上为正 所以是Y方向上到下是1到-1
		float startParamValueY = 1.f - 0.4f * row;
		//float endParamValueY = startParamValueY-0.4f;


		//UV是0~1、左上到右下、Y向下为正
		float resultValueX = startParamValueX + 0.4f * u;
		float resultValueY = startParamValueY - 0.4f * v;
		*ParamValueX = resultValueX;
		*ParamValueY = resultValueY;

		return;
	}

}


void MeshMapping::BuildMeshMapping(IModel* _model, HandPosHandle hand, ParamHandle handParamPosX, ParamHandle handParamPosY, ParamHandle handParamPosZ)
{
//填充结构体
	//按参数方格的左上到右下计算，  所以Y向上为正
	//左上
	float paramPosX[6][6];
	float paramPosY[6][6];


	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			//i行 j列
			float curParamX = -1.f + j * 0.4f;
			float curParamY = 1.f - i * 0.4f ;
			_model->SetParamValue(handParamPosZ, 0.f, true);
			_model->SetParamValue(handParamPosX, curParamX, true);
			_model->SetParamValue(handParamPosY, curParamY, true);
			_model->Update(0);
			_model->GetHandPosFromHandle(hand, &paramPosX[i][j], &paramPosY[i][j]);
		}
	}

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			//i行 j列

			auto& curRect = rectMesh[i][j];

			curRect.leftTopX = paramPosX[i][j];
			curRect.leftTopY = paramPosY[i][j];

			curRect.rightTopX = paramPosX[i][j + 1];
			curRect.rightTopY = paramPosY[i][j + 1];

			curRect.leftBottomX = paramPosX[i + 1][j];
			curRect.leftBottomY = paramPosY[i + 1][j];

			curRect.rightBottomX = paramPosX[i+1][j+1];
			curRect.rightBottomY = paramPosY[i+1][j+1];

			curRect.centerPointX = (curRect.leftTopX + curRect.rightTopX + curRect.leftBottomX + curRect.rightBottomX)/4.f;
			curRect.centerPointY = (curRect.leftTopY + curRect.rightTopY + curRect.leftBottomY + curRect.rightBottomY)/4.f;

#define _RECT_MIN(a,b,c,d) SDL_min(SDL_min(a,b), SDL_min(c,d))
#define _RECT_MAX(a,b,c,d) SDL_max(SDL_max(a,b), SDL_max(c,d))

			curRect.boundingBoxLeft = _RECT_MIN(curRect.leftTopX, curRect.rightTopX, curRect.leftBottomX, curRect.rightBottomX);
			curRect.boundingBoxRight = _RECT_MAX(curRect.leftTopX, curRect.rightTopX, curRect.leftBottomX, curRect.rightBottomX);
			curRect.boundingBoxTop = _RECT_MAX(curRect.leftTopY, curRect.rightTopY, curRect.leftBottomY, curRect.rightBottomY);
			curRect.boundingBoxButtom = _RECT_MIN(curRect.leftTopY, curRect.rightTopY, curRect.leftBottomY, curRect.rightBottomY);

			curRect._CalcHomographyMat();
		}
	}

	valid = true;

}
