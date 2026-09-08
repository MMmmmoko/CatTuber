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

void MeshMapping::PrintBakeCode()
{
	//打印烘焙代码，使用代码可以将一个新的UnitRect rectMesh[5][5]赋值为当前的rectMesh[5][5]

	
	SDL_Log("{");
	for(int i=0;i<5;i++)
		for (int j = 0; j < 5; j++)
		{


			auto& curRect = rectMesh[i][j];
			SDL_Log("rectMesh[%d][%d].leftTopX=%ff;", i, j, curRect.leftTopX);
			SDL_Log("rectMesh[%d][%d].leftTopY=%ff;", i, j, curRect.leftTopY);
			SDL_Log("rectMesh[%d][%d].rightTopX=%ff;", i, j, curRect.rightTopX);
			SDL_Log("rectMesh[%d][%d].rightTopY=%ff;", i, j, curRect.rightTopY);
			SDL_Log("rectMesh[%d][%d].leftBottomX=%ff;", i, j, curRect.leftBottomX);
			SDL_Log("rectMesh[%d][%d].leftBottomY=%ff;", i, j, curRect.leftBottomY);
			SDL_Log("rectMesh[%d][%d].rightBottomX=%ff;", i, j, curRect.rightBottomX);
			SDL_Log("rectMesh[%d][%d].rightBottomY=%ff;", i, j, curRect.rightBottomY);
			SDL_Log("rectMesh[%d][%d].centerPointX=%ff;", i, j, curRect.centerPointX);
			SDL_Log("rectMesh[%d][%d].centerPointY=%ff;", i, j, curRect.centerPointY);
			SDL_Log("rectMesh[%d][%d].boundingBoxLeft=%ff;", i, j, curRect.boundingBoxLeft);
			SDL_Log("rectMesh[%d][%d].boundingBoxRight=%ff;", i, j, curRect.boundingBoxRight);
			SDL_Log("rectMesh[%d][%d].boundingBoxTop=%ff;", i, j, curRect.boundingBoxTop);
			SDL_Log("rectMesh[%d][%d].boundingBoxButtom=%ff;", i, j, curRect.boundingBoxButtom);

			for(int k=0;k<3;k++)
				for (int l = 0; l < 3; l++)
				{
					SDL_Log("rectMesh[%d][%d].H[%d][%d]=%ff;", i, j, k, l, curRect.H[k][l]);
				}
			SDL_Log("valid = true;");
			
		}

	SDL_Log("}");




}

void MeshMapping::SetUpAsOldHandLeft()
{
	//代码来自PrintBakeCode()烘培
	{
		rectMesh[0][0].leftTopX = -0.273062f;
		rectMesh[0][0].leftTopY = -0.472171f;
		rectMesh[0][0].rightTopX = -0.151366f;
		rectMesh[0][0].rightTopY = -0.517244f;
		rectMesh[0][0].leftBottomX = -0.230763f;
		rectMesh[0][0].leftBottomY = -0.431259f;
		rectMesh[0][0].rightBottomX = -0.112049f;
		rectMesh[0][0].rightBottomY = -0.473697f;
		rectMesh[0][0].centerPointX = -0.191810f;
		rectMesh[0][0].centerPointY = -0.473593f;
		rectMesh[0][0].boundingBoxLeft = -0.273062f;
		rectMesh[0][0].boundingBoxRight = -0.112049f;
		rectMesh[0][0].boundingBoxTop = -0.431259f;
		rectMesh[0][0].boundingBoxButtom = -0.517244f;
		rectMesh[0][0].H[0][0] = 9.700277f;
		rectMesh[0][0].H[0][1] = -10.029089f;
		rectMesh[0][0].H[0][2] = -2.086672f;
		rectMesh[0][0].H[1][0] = 10.052456f;
		rectMesh[0][0].H[1][1] = 27.141626f;
		rectMesh[0][0].H[1][2] = 15.560446f;
		rectMesh[0][0].H[2][0] = -0.078936f;
		rectMesh[0][0].H[2][1] = -1.199763f;
		rectMesh[0][0].H[2][2] = 1.000000f;
		rectMesh[0][1].leftTopX = -0.151366f;
		rectMesh[0][1].leftTopY = -0.517244f;
		rectMesh[0][1].rightTopX = -0.029671f;
		rectMesh[0][1].rightTopY = -0.562316f;
		rectMesh[0][1].leftBottomX = -0.112049f;
		rectMesh[0][1].leftBottomY = -0.473697f;
		rectMesh[0][1].rightBottomX = 0.006665f;
		rectMesh[0][1].rightBottomY = -0.516134f;
		rectMesh[0][1].centerPointX = -0.071605f;
		rectMesh[0][1].centerPointY = -0.517348f;
		rectMesh[0][1].boundingBoxLeft = -0.151366f;
		rectMesh[0][1].boundingBoxRight = 0.006665f;
		rectMesh[0][1].boundingBoxTop = -0.473697f;
		rectMesh[0][1].boundingBoxButtom = -0.562316f;
		rectMesh[0][1].H[0][0] = 10.045576f;
		rectMesh[0][1].H[0][1] = -9.069822f;
		rectMesh[0][1].H[0][2] = -3.170749f;
		rectMesh[0][1].H[1][0] = 9.796188f;
		rectMesh[0][1].H[1][1] = 26.449717f;
		rectMesh[0][1].H[1][2] = 15.163769f;
		rectMesh[0][1].H[2][0] = -0.059158f;
		rectMesh[0][1].H[2][1] = -1.119563f;
		rectMesh[0][1].H[2][2] = 1.000000f;
		rectMesh[0][2].leftTopX = -0.029671f;
		rectMesh[0][2].leftTopY = -0.562316f;
		rectMesh[0][2].rightTopX = 0.097573f;
		rectMesh[0][2].rightTopY = -0.590747f;
		rectMesh[0][2].leftBottomX = 0.006665f;
		rectMesh[0][2].leftBottomY = -0.516134f;
		rectMesh[0][2].rightBottomX = 0.133561f;
		rectMesh[0][2].rightBottomY = -0.544357f;
		rectMesh[0][2].centerPointX = 0.052032f;
		rectMesh[0][2].centerPointY = -0.553389f;
		rectMesh[0][2].boundingBoxLeft = -0.029671f;
		rectMesh[0][2].boundingBoxRight = 0.133561f;
		rectMesh[0][2].boundingBoxTop = -0.516134f;
		rectMesh[0][2].boundingBoxButtom = -0.590747f;
		rectMesh[0][2].H[0][0] = 6.996086f;
		rectMesh[0][2].H[0][1] = -5.504423f;
		rectMesh[0][2].H[0][2] = -2.887650f;
		rectMesh[0][2].H[1][0] = 4.281902f;
		rectMesh[0][2].H[1][1] = 19.164173f;
		rectMesh[0][2].H[1][2] = 10.903376f;
		rectMesh[0][2].H[2][0] = 0.002212f;
		rectMesh[0][2].H[2][1] = -0.078682f;
		rectMesh[0][2].H[2][2] = 1.000000f;
		rectMesh[0][3].leftTopX = 0.097573f;
		rectMesh[0][3].leftTopY = -0.590747f;
		rectMesh[0][3].rightTopX = 0.230363f;
		rectMesh[0][3].rightTopY = -0.602535f;
		rectMesh[0][3].leftBottomX = 0.133561f;
		rectMesh[0][3].leftBottomY = -0.544357f;
		rectMesh[0][3].rightBottomX = 0.268640f;
		rectMesh[0][3].rightBottomY = -0.558364f;
		rectMesh[0][3].centerPointX = 0.182534f;
		rectMesh[0][3].centerPointY = -0.574001f;
		rectMesh[0][3].boundingBoxLeft = 0.097573f;
		rectMesh[0][3].boundingBoxRight = 0.268640f;
		rectMesh[0][3].boundingBoxTop = -0.544357f;
		rectMesh[0][3].boundingBoxButtom = -0.602535f;
		rectMesh[0][3].H[0][0] = 4.509676f;
		rectMesh[0][3].H[0][1] = -3.498538f;
		rectMesh[0][3].H[0][2] = -2.506772f;
		rectMesh[0][3].H[1][0] = 1.228271f;
		rectMesh[0][3].H[1][1] = 13.836030f;
		rectMesh[0][3].H[1][2] = 8.053744f;
		rectMesh[0][3].H[2][0] = -0.150504f;
		rectMesh[0][3].H[2][1] = 0.539794f;
		rectMesh[0][3].H[2][2] = 1.000000f;
		rectMesh[0][4].leftTopX = 0.230363f;
		rectMesh[0][4].leftTopY = -0.602535f;
		rectMesh[0][4].rightTopX = 0.363154f;
		rectMesh[0][4].rightTopY = -0.614323f;
		rectMesh[0][4].leftBottomX = 0.268640f;
		rectMesh[0][4].leftBottomY = -0.558364f;
		rectMesh[0][4].rightBottomX = 0.403719f;
		rectMesh[0][4].rightBottomY = -0.572371f;
		rectMesh[0][4].centerPointX = 0.316469f;
		rectMesh[0][4].centerPointY = -0.586898f;
		rectMesh[0][4].boundingBoxLeft = 0.230363f;
		rectMesh[0][4].boundingBoxRight = 0.403719f;
		rectMesh[0][4].boundingBoxTop = -0.558364f;
		rectMesh[0][4].boundingBoxButtom = -0.614323f;
		rectMesh[0][4].H[0][0] = 4.209243f;
		rectMesh[0][4].H[0][1] = -3.647573f;
		rectMesh[0][4].H[0][2] = -3.167446f;
		rectMesh[0][4].H[1][0] = 1.207601f;
		rectMesh[0][4].H[1][1] = 13.603271f;
		rectMesh[0][4].H[1][2] = 7.918261f;
		rectMesh[0][4].H[2][0] = -0.144685f;
		rectMesh[0][4].H[2][1] = 0.562426f;
		rectMesh[0][4].H[2][2] = 1.000000f;
		rectMesh[1][0].leftTopX = -0.230763f;
		rectMesh[1][0].leftTopY = -0.431259f;
		rectMesh[1][0].rightTopX = -0.112049f;
		rectMesh[1][0].rightTopY = -0.473697f;
		rectMesh[1][0].leftBottomX = -0.188464f;
		rectMesh[1][0].leftBottomY = -0.390347f;
		rectMesh[1][0].rightBottomX = -0.072732f;
		rectMesh[1][0].rightBottomY = -0.430150f;
		rectMesh[1][0].centerPointX = -0.151002f;
		rectMesh[1][0].centerPointY = -0.431363f;
		rectMesh[1][0].boundingBoxLeft = -0.230763f;
		rectMesh[1][0].boundingBoxRight = -0.072732f;
		rectMesh[1][0].boundingBoxTop = -0.390347f;
		rectMesh[1][0].boundingBoxButtom = -0.473697f;
		rectMesh[1][0].H[0][0] = 9.757917f;
		rectMesh[1][0].H[0][1] = -10.088686f;
		rectMesh[1][0].H[0][2] = -2.099073f;
		rectMesh[1][0].H[1][0] = 9.500500f;
		rectMesh[1][0].H[1][1] = 26.576567f;
		rectMesh[1][0].H[1][2] = 13.653763f;
		rectMesh[1][0].H[2][0] = -0.060594f;
		rectMesh[1][0].H[2][1] = -1.223887f;
		rectMesh[1][0].H[2][2] = 1.000000f;
		rectMesh[1][1].leftTopX = -0.112049f;
		rectMesh[1][1].leftTopY = -0.473697f;
		rectMesh[1][1].rightTopX = 0.006665f;
		rectMesh[1][1].rightTopY = -0.516134f;
		rectMesh[1][1].leftBottomX = -0.072732f;
		rectMesh[1][1].leftBottomY = -0.430150f;
		rectMesh[1][1].rightBottomX = 0.043000f;
		rectMesh[1][1].rightBottomY = -0.469952f;
		rectMesh[1][1].centerPointX = -0.033779f;
		rectMesh[1][1].centerPointY = -0.472483f;
		rectMesh[1][1].boundingBoxLeft = -0.112049f;
		rectMesh[1][1].boundingBoxRight = 0.043000f;
		rectMesh[1][1].boundingBoxTop = -0.430150f;
		rectMesh[1][1].boundingBoxButtom = -0.516134f;
		rectMesh[1][1].H[0][0] = 10.121767f;
		rectMesh[1][1].H[0][1] = -9.138591f;
		rectMesh[1][1].H[0][2] = -3.194788f;
		rectMesh[1][1].H[1][0] = 9.274483f;
		rectMesh[1][1].H[1][1] = 25.944368f;
		rectMesh[1][1].H[1][2] = 13.328968f;
		rectMesh[1][1].H[2][0] = -0.041188f;
		rectMesh[1][1].H[2][1] = -1.142509f;
		rectMesh[1][1].H[2][2] = 1.000000f;
		rectMesh[1][2].leftTopX = 0.006665f;
		rectMesh[1][2].leftTopY = -0.516134f;
		rectMesh[1][2].rightTopX = 0.133561f;
		rectMesh[1][2].rightTopY = -0.544357f;
		rectMesh[1][2].leftBottomX = 0.043000f;
		rectMesh[1][2].leftBottomY = -0.469952f;
		rectMesh[1][2].rightBottomX = 0.169550f;
		rectMesh[1][2].rightBottomY = -0.497967f;
		rectMesh[1][2].centerPointX = 0.088194f;
		rectMesh[1][2].centerPointY = -0.507103f;
		rectMesh[1][2].boundingBoxLeft = 0.006665f;
		rectMesh[1][2].boundingBoxRight = 0.169550f;
		rectMesh[1][2].boundingBoxTop = -0.469952f;
		rectMesh[1][2].boundingBoxButtom = -0.544357f;
		rectMesh[1][2].H[0][0] = 6.996605f;
		rectMesh[1][2].H[0][1] = -5.504845f;
		rectMesh[1][2].H[0][2] = -2.887871f;
		rectMesh[1][2].H[1][0] = 4.250823f;
		rectMesh[1][2].H[1][1] = 19.113001f;
		rectMesh[1][2].H[1][2] = 9.836546f;
		rectMesh[1][2].H[2][0] = 0.002337f;
		rectMesh[1][2].H[2][1] = -0.078794f;
		rectMesh[1][2].H[2][2] = 1.000000f;
		rectMesh[1][3].leftTopX = 0.133561f;
		rectMesh[1][3].leftTopY = -0.544357f;
		rectMesh[1][3].rightTopX = 0.268640f;
		rectMesh[1][3].rightTopY = -0.558364f;
		rectMesh[1][3].leftBottomX = 0.169550f;
		rectMesh[1][3].leftBottomY = -0.497967f;
		rectMesh[1][3].rightBottomX = 0.306917f;
		rectMesh[1][3].rightBottomY = -0.514193f;
		rectMesh[1][3].centerPointX = 0.219667f;
		rectMesh[1][3].centerPointY = -0.528720f;
		rectMesh[1][3].boundingBoxLeft = 0.133561f;
		rectMesh[1][3].boundingBoxRight = 0.306917f;
		rectMesh[1][3].boundingBoxTop = -0.497967f;
		rectMesh[1][3].boundingBoxButtom = -0.558364f;
		rectMesh[1][3].H[0][0] = 4.545787f;
		rectMesh[1][3].H[0][1] = -3.526561f;
		rectMesh[1][3].H[0][2] = -2.526849f;
		rectMesh[1][3].H[1][0] = 1.468341f;
		rectMesh[1][3].H[1][1] = 14.159942f;
		rectMesh[1][3].H[1][2] = 7.511946f;
		rectMesh[1][3].H[2][0] = -0.141076f;
		rectMesh[1][3].H[2][1] = 0.534891f;
		rectMesh[1][3].H[2][2] = 1.000000f;
		rectMesh[1][4].leftTopX = 0.268640f;
		rectMesh[1][4].leftTopY = -0.558364f;
		rectMesh[1][4].rightTopX = 0.403719f;
		rectMesh[1][4].rightTopY = -0.572371f;
		rectMesh[1][4].leftBottomX = 0.306917f;
		rectMesh[1][4].leftBottomY = -0.514193f;
		rectMesh[1][4].rightBottomX = 0.444284f;
		rectMesh[1][4].rightBottomY = -0.530419f;
		rectMesh[1][4].centerPointX = 0.355890f;
		rectMesh[1][4].centerPointY = -0.543837f;
		rectMesh[1][4].boundingBoxLeft = 0.268640f;
		rectMesh[1][4].boundingBoxRight = 0.444284f;
		rectMesh[1][4].boundingBoxTop = -0.514193f;
		rectMesh[1][4].boundingBoxButtom = -0.572371f;
		rectMesh[1][4].H[0][0] = 4.252714f;
		rectMesh[1][4].H[0][1] = -3.685239f;
		rectMesh[1][4].H[0][2] = -3.200155f;
		rectMesh[1][4].H[1][0] = 1.446715f;
		rectMesh[1][4].H[1][1] = 13.951505f;
		rectMesh[1][4].H[1][2] = 7.401372f;
		rectMesh[1][4].H[2][0] = -0.135384f;
		rectMesh[1][4].H[2][1] = 0.557805f;
		rectMesh[1][4].H[2][2] = 1.000000f;
		rectMesh[2][0].leftTopX = -0.188464f;
		rectMesh[2][0].leftTopY = -0.390347f;
		rectMesh[2][0].rightTopX = -0.072732f;
		rectMesh[2][0].rightTopY = -0.430150f;
		rectMesh[2][0].leftBottomX = -0.146166f;
		rectMesh[2][0].leftBottomY = -0.349436f;
		rectMesh[2][0].rightBottomX = -0.033415f;
		rectMesh[2][0].rightBottomY = -0.386603f;
		rectMesh[2][0].centerPointX = -0.110194f;
		rectMesh[2][0].centerPointY = -0.389134f;
		rectMesh[2][0].boundingBoxLeft = -0.188464f;
		rectMesh[2][0].boundingBoxRight = -0.033415f;
		rectMesh[2][0].boundingBoxTop = -0.349436f;
		rectMesh[2][0].boundingBoxButtom = -0.430150f;
		rectMesh[2][0].H[0][0] = 9.820458f;
		rectMesh[2][0].H[0][1] = -10.153362f;
		rectMesh[2][0].H[0][2] = -2.112532f;
		rectMesh[2][0].H[1][0] = 8.946980f;
		rectMesh[2][0].H[1][1] = 26.014826f;
		rectMesh[2][0].H[1][2] = 11.841009f;
		rectMesh[2][0].H[2][0] = -0.040698f;
		rectMesh[2][0].H[2][1] = -1.250062f;
		rectMesh[2][0].H[2][2] = 1.000000f;
		rectMesh[2][1].leftTopX = -0.072732f;
		rectMesh[2][1].leftTopY = -0.430150f;
		rectMesh[2][1].rightTopX = 0.043000f;
		rectMesh[2][1].rightTopY = -0.469952f;
		rectMesh[2][1].leftBottomX = -0.033415f;
		rectMesh[2][1].leftBottomY = -0.386603f;
		rectMesh[2][1].rightBottomX = 0.079336f;
		rectMesh[2][1].rightBottomY = -0.423770f;
		rectMesh[2][1].centerPointX = 0.004047f;
		rectMesh[2][1].centerPointY = -0.427619f;
		rectMesh[2][1].boundingBoxLeft = -0.072732f;
		rectMesh[2][1].boundingBoxRight = 0.079336f;
		rectMesh[2][1].boundingBoxTop = -0.386603f;
		rectMesh[2][1].boundingBoxButtom = -0.469952f;
		rectMesh[2][1].H[0][0] = 10.204817f;
		rectMesh[2][1].H[0][1] = -9.213575f;
		rectMesh[2][1].H[0][2] = -3.221002f;
		rectMesh[2][1].H[1][0] = 8.750834f;
		rectMesh[2][1].H[1][1] = 25.444540f;
		rectMesh[2][1].H[1][2] = 11.581437f;
		rectMesh[2][1].H[2][0] = -0.021650f;
		rectMesh[2][1].H[2][1] = -1.167525f;
		rectMesh[2][1].H[2][2] = 1.000000f;
		rectMesh[2][2].leftTopX = 0.043000f;
		rectMesh[2][2].leftTopY = -0.469952f;
		rectMesh[2][2].rightTopX = 0.169550f;
		rectMesh[2][2].rightTopY = -0.497967f;
		rectMesh[2][2].leftBottomX = 0.079336f;
		rectMesh[2][2].leftBottomY = -0.423770f;
		rectMesh[2][2].rightBottomX = 0.205539f;
		rectMesh[2][2].rightBottomY = -0.451577f;
		rectMesh[2][2].centerPointX = 0.124356f;
		rectMesh[2][2].centerPointY = -0.460817f;
		rectMesh[2][2].boundingBoxLeft = 0.043000f;
		rectMesh[2][2].boundingBoxRight = 0.205539f;
		rectMesh[2][2].boundingBoxTop = -0.423770f;
		rectMesh[2][2].boundingBoxButtom = -0.497967f;
		rectMesh[2][2].H[0][0] = 6.997000f;
		rectMesh[2][2].H[0][1] = -5.505147f;
		rectMesh[2][2].H[0][2] = -2.888030f;
		rectMesh[2][2].H[1][0] = 4.219628f;
		rectMesh[2][2].H[1][1] = 19.061464f;
		rectMesh[2][2].H[1][2] = 8.776537f;
		rectMesh[2][2].H[2][0] = 0.002445f;
		rectMesh[2][2].H[2][1] = -0.078875f;
		rectMesh[2][2].H[2][2] = 1.000000f;
		rectMesh[2][3].leftTopX = 0.169550f;
		rectMesh[2][3].leftTopY = -0.497967f;
		rectMesh[2][3].rightTopX = 0.306917f;
		rectMesh[2][3].rightTopY = -0.514193f;
		rectMesh[2][3].leftBottomX = 0.205539f;
		rectMesh[2][3].leftBottomY = -0.451577f;
		rectMesh[2][3].rightBottomX = 0.345194f;
		rectMesh[2][3].rightBottomY = -0.470022f;
		rectMesh[2][3].centerPointX = 0.256800f;
		rectMesh[2][3].centerPointY = -0.483440f;
		rectMesh[2][3].boundingBoxLeft = 0.169550f;
		rectMesh[2][3].boundingBoxRight = 0.345194f;
		rectMesh[2][3].boundingBoxTop = -0.451577f;
		rectMesh[2][3].boundingBoxButtom = -0.514193f;
		rectMesh[2][3].H[0][0] = 4.580284f;
		rectMesh[2][3].H[0][1] = -3.553313f;
		rectMesh[2][3].H[0][2] = -2.546019f;
		rectMesh[2][3].H[1][0] = 1.710738f;
		rectMesh[2][3].H[1][1] = 14.482796f;
		rectMesh[2][3].H[1][2] = 6.921895f;
		rectMesh[2][3].H[2][0] = -0.132055f;
		rectMesh[2][3].H[2][1] = 0.530215f;
		rectMesh[2][3].H[2][2] = 1.000000f;
		rectMesh[2][4].leftTopX = 0.306917f;
		rectMesh[2][4].leftTopY = -0.514193f;
		rectMesh[2][4].rightTopX = 0.444284f;
		rectMesh[2][4].rightTopY = -0.530419f;
		rectMesh[2][4].leftBottomX = 0.345194f;
		rectMesh[2][4].leftBottomY = -0.470022f;
		rectMesh[2][4].rightBottomX = 0.484850f;
		rectMesh[2][4].rightBottomY = -0.488467f;
		rectMesh[2][4].centerPointX = 0.395311f;
		rectMesh[2][4].centerPointY = -0.500775f;
		rectMesh[2][4].boundingBoxLeft = 0.306917f;
		rectMesh[2][4].boundingBoxRight = 0.484850f;
		rectMesh[2][4].boundingBoxTop = -0.470022f;
		rectMesh[2][4].boundingBoxButtom = -0.530419f;
		rectMesh[2][4].H[0][0] = 4.294580f;
		rectMesh[2][4].H[0][1] = -3.721521f;
		rectMesh[2][4].H[0][2] = -3.231660f;
		rectMesh[2][4].H[1][0] = 1.689074f;
		rectMesh[2][4].H[1][1] = 14.299345f;
		rectMesh[2][4].H[1][2] = 6.834215f;
		rectMesh[2][4].H[2][0] = -0.126438f;
		rectMesh[2][4].H[2][1] = 0.553345f;
		rectMesh[2][4].H[2][2] = 1.000000f;
		rectMesh[3][0].leftTopX = -0.146166f;
		rectMesh[3][0].leftTopY = -0.349436f;
		rectMesh[3][0].rightTopX = -0.033415f;
		rectMesh[3][0].rightTopY = -0.386603f;
		rectMesh[3][0].leftBottomX = -0.103867f;
		rectMesh[3][0].leftBottomY = -0.308524f;
		rectMesh[3][0].rightBottomX = 0.005902f;
		rectMesh[3][0].rightBottomY = -0.343056f;
		rectMesh[3][0].centerPointX = -0.069386f;
		rectMesh[3][0].centerPointY = -0.346905f;
		rectMesh[3][0].boundingBoxLeft = -0.146166f;
		rectMesh[3][0].boundingBoxRight = 0.005902f;
		rectMesh[3][0].boundingBoxTop = -0.308524f;
		rectMesh[3][0].boundingBoxButtom = -0.386603f;
		rectMesh[3][0].H[0][0] = 9.888229f;
		rectMesh[3][0].H[0][1] = -10.223411f;
		rectMesh[3][0].H[0][2] = -2.127104f;
		rectMesh[3][0].H[1][0] = 8.391415f;
		rectMesh[3][0].H[1][1] = 25.456085f;
		rectMesh[3][0].H[1][2] = 10.121798f;
		rectMesh[3][0].H[2][0] = -0.019076f;
		rectMesh[3][0].H[2][1] = -1.278431f;
		rectMesh[3][0].H[2][2] = 1.000000f;
		rectMesh[3][1].leftTopX = -0.033415f;
		rectMesh[3][1].leftTopY = -0.386603f;
		rectMesh[3][1].rightTopX = 0.079336f;
		rectMesh[3][1].rightTopY = -0.423770f;
		rectMesh[3][1].leftBottomX = 0.005902f;
		rectMesh[3][1].leftBottomY = -0.343056f;
		rectMesh[3][1].rightBottomX = 0.115671f;
		rectMesh[3][1].rightBottomY = -0.377588f;
		rectMesh[3][1].centerPointX = 0.041873f;
		rectMesh[3][1].centerPointY = -0.382755f;
		rectMesh[3][1].boundingBoxLeft = -0.033415f;
		rectMesh[3][1].boundingBoxRight = 0.115671f;
		rectMesh[3][1].boundingBoxTop = -0.343056f;
		rectMesh[3][1].boundingBoxButtom = -0.423770f;
		rectMesh[3][1].H[0][0] = 10.295482f;
		rectMesh[3][1].H[0][1] = -9.295457f;
		rectMesh[3][1].H[0][2] = -3.249628f;
		rectMesh[3][1].H[1][0] = 8.224786f;
		rectMesh[3][1].H[1][1] = 24.950565f;
		rectMesh[3][1].H[1][2] = 9.920794f;
		rectMesh[3][1].H[2][0] = -0.000343f;
		rectMesh[3][1].H[2][1] = -1.194841f;
		rectMesh[3][1].H[2][2] = 1.000000f;
		rectMesh[3][2].leftTopX = 0.079336f;
		rectMesh[3][2].leftTopY = -0.423770f;
		rectMesh[3][2].rightTopX = 0.205539f;
		rectMesh[3][2].rightTopY = -0.451577f;
		rectMesh[3][2].leftBottomX = 0.115671f;
		rectMesh[3][2].leftBottomY = -0.377588f;
		rectMesh[3][2].rightBottomX = 0.241527f;
		rectMesh[3][2].rightBottomY = -0.405187f;
		rectMesh[3][2].centerPointX = 0.160518f;
		rectMesh[3][2].centerPointY = -0.414531f;
		rectMesh[3][2].boundingBoxLeft = 0.079336f;
		rectMesh[3][2].boundingBoxRight = 0.241527f;
		rectMesh[3][2].boundingBoxTop = -0.377588f;
		rectMesh[3][2].boundingBoxButtom = -0.451577f;
		rectMesh[3][2].H[0][0] = 6.997437f;
		rectMesh[3][2].H[0][1] = -5.505491f;
		rectMesh[3][2].H[0][2] = -2.888211f;
		rectMesh[3][2].H[1][0] = 4.188469f;
		rectMesh[3][2].H[1][1] = 19.010059f;
		rectMesh[3][2].H[1][2] = 7.723608f;
		rectMesh[3][2].H[2][0] = 0.002560f;
		rectMesh[3][2].H[2][1] = -0.078966f;
		rectMesh[3][2].H[2][2] = 1.000000f;
		rectMesh[3][3].leftTopX = 0.205539f;
		rectMesh[3][3].leftTopY = -0.451577f;
		rectMesh[3][3].rightTopX = 0.345194f;
		rectMesh[3][3].rightTopY = -0.470022f;
		rectMesh[3][3].leftBottomX = 0.241527f;
		rectMesh[3][3].leftBottomY = -0.405187f;
		rectMesh[3][3].rightBottomX = 0.383471f;
		rectMesh[3][3].rightBottomY = -0.425851f;
		rectMesh[3][3].centerPointX = 0.293933f;
		rectMesh[3][3].centerPointY = -0.438159f;
		rectMesh[3][3].boundingBoxLeft = 0.205539f;
		rectMesh[3][3].boundingBoxRight = 0.383471f;
		rectMesh[3][3].boundingBoxTop = -0.405187f;
		rectMesh[3][3].boundingBoxButtom = -0.470022f;
		rectMesh[3][3].H[0][0] = 4.613414f;
		rectMesh[3][3].H[0][1] = -3.579021f;
		rectMesh[3][3].H[0][2] = -2.564438f;
		rectMesh[3][3].H[1][0] = 1.955392f;
		rectMesh[3][3].H[1][1] = 14.805103f;
		rectMesh[3][3].H[1][2] = 6.283731f;
		rectMesh[3][3].H[2][0] = -0.123389f;
		rectMesh[3][3].H[2][1] = 0.525727f;
		rectMesh[3][3].H[2][2] = 1.000000f;
		rectMesh[3][4].leftTopX = 0.345194f;
		rectMesh[3][4].leftTopY = -0.470022f;
		rectMesh[3][4].rightTopX = 0.484850f;
		rectMesh[3][4].rightTopY = -0.488467f;
		rectMesh[3][4].leftBottomX = 0.383471f;
		rectMesh[3][4].leftBottomY = -0.425851f;
		rectMesh[3][4].rightBottomX = 0.525415f;
		rectMesh[3][4].rightBottomY = -0.446515f;
		rectMesh[3][4].centerPointX = 0.434733f;
		rectMesh[3][4].centerPointY = -0.457714f;
		rectMesh[3][4].boundingBoxLeft = 0.345194f;
		rectMesh[3][4].boundingBoxRight = 0.525415f;
		rectMesh[3][4].boundingBoxTop = -0.425851f;
		rectMesh[3][4].boundingBoxButtom = -0.488467f;
		rectMesh[3][4].H[0][0] = 4.334802f;
		rectMesh[3][4].H[0][1] = -3.756390f;
		rectMesh[3][4].H[0][2] = -3.261934f;
		rectMesh[3][4].H[1][0] = 1.934443f;
		rectMesh[3][4].H[1][1] = 14.646446f;
		rectMesh[3][4].H[1][2] = 6.216391f;
		rectMesh[3][4].H[2][0] = -0.117839f;
		rectMesh[3][4].H[2][1] = 0.549065f;
		rectMesh[3][4].H[2][2] = 1.000000f;
		rectMesh[4][0].leftTopX = -0.103867f;
		rectMesh[4][0].leftTopY = -0.308524f;
		rectMesh[4][0].rightTopX = 0.005902f;
		rectMesh[4][0].rightTopY = -0.343056f;
		rectMesh[4][0].leftBottomX = -0.061568f;
		rectMesh[4][0].leftBottomY = -0.267612f;
		rectMesh[4][0].rightBottomX = 0.045219f;
		rectMesh[4][0].rightBottomY = -0.299509f;
		rectMesh[4][0].centerPointX = -0.028578f;
		rectMesh[4][0].centerPointY = -0.304675f;
		rectMesh[4][0].boundingBoxLeft = -0.103867f;
		rectMesh[4][0].boundingBoxRight = 0.045219f;
		rectMesh[4][0].boundingBoxTop = -0.267612f;
		rectMesh[4][0].boundingBoxButtom = -0.343056f;
		rectMesh[4][0].H[0][0] = 9.962626f;
		rectMesh[4][0].H[0][1] = -10.300352f;
		rectMesh[4][0].H[0][2] = -2.143115f;
		rectMesh[4][0].H[1][0] = 7.834099f;
		rectMesh[4][0].H[1][1] = 24.902319f;
		rectMesh[4][0].H[1][2] = 8.496654f;
		rectMesh[4][0].H[2][0] = 0.004577f;
		rectMesh[4][0].H[2][1] = -1.309574f;
		rectMesh[4][0].H[2][2] = 1.000000f;
		rectMesh[4][1].leftTopX = 0.005902f;
		rectMesh[4][1].leftTopY = -0.343056f;
		rectMesh[4][1].rightTopX = 0.115671f;
		rectMesh[4][1].rightTopY = -0.377588f;
		rectMesh[4][1].leftBottomX = 0.045219f;
		rectMesh[4][1].leftBottomY = -0.299509f;
		rectMesh[4][1].rightBottomX = 0.152006f;
		rectMesh[4][1].rightBottomY = -0.331406f;
		rectMesh[4][1].centerPointX = 0.079700f;
		rectMesh[4][1].centerPointY = -0.337890f;
		rectMesh[4][1].boundingBoxLeft = 0.005902f;
		rectMesh[4][1].boundingBoxRight = 0.152006f;
		rectMesh[4][1].boundingBoxTop = -0.299509f;
		rectMesh[4][1].boundingBoxButtom = -0.377588f;
		rectMesh[4][1].H[0][0] = 10.394330f;
		rectMesh[4][1].H[0][1] = -9.384683f;
		rectMesh[4][1].H[0][2] = -3.280821f;
		rectMesh[4][1].H[1][0] = 7.695571f;
		rectMesh[4][1].H[1][1] = 24.462063f;
		rectMesh[4][1].H[1][2] = 8.346438f;
		rectMesh[4][1].H[2][0] = 0.022937f;
		rectMesh[4][1].H[2][1] = -1.224606f;
		rectMesh[4][1].H[2][2] = 1.000000f;
		rectMesh[4][2].leftTopX = 0.115671f;
		rectMesh[4][2].leftTopY = -0.377588f;
		rectMesh[4][2].rightTopX = 0.241527f;
		rectMesh[4][2].rightTopY = -0.405187f;
		rectMesh[4][2].leftBottomX = 0.152006f;
		rectMesh[4][2].leftBottomY = -0.331406f;
		rectMesh[4][2].rightBottomX = 0.277516f;
		rectMesh[4][2].rightBottomY = -0.358797f;
		rectMesh[4][2].centerPointX = 0.196680f;
		rectMesh[4][2].centerPointY = -0.368245f;
		rectMesh[4][2].boundingBoxLeft = 0.115671f;
		rectMesh[4][2].boundingBoxRight = 0.277516f;
		rectMesh[4][2].boundingBoxTop = -0.331406f;
		rectMesh[4][2].boundingBoxButtom = -0.405187f;
		rectMesh[4][2].H[0][0] = 6.997920f;
		rectMesh[4][2].H[0][1] = -5.505871f;
		rectMesh[4][2].H[0][2] = -2.888410f;
		rectMesh[4][2].H[1][0] = 4.157345f;
		rectMesh[4][2].H[1][1] = 18.958742f;
		rectMesh[4][2].H[1][2] = 6.677719f;
		rectMesh[4][2].H[2][0] = 0.002679f;
		rectMesh[4][2].H[2][1] = -0.079070f;
		rectMesh[4][2].H[2][2] = 1.000000f;
		rectMesh[4][3].leftTopX = 0.241527f;
		rectMesh[4][3].leftTopY = -0.405187f;
		rectMesh[4][3].rightTopX = 0.383471f;
		rectMesh[4][3].rightTopY = -0.425851f;
		rectMesh[4][3].leftBottomX = 0.277516f;
		rectMesh[4][3].leftBottomY = -0.358797f;
		rectMesh[4][3].rightBottomX = 0.421748f;
		rectMesh[4][3].rightBottomY = -0.381680f;
		rectMesh[4][3].centerPointX = 0.331066f;
		rectMesh[4][3].centerPointY = -0.392878f;
		rectMesh[4][3].boundingBoxLeft = 0.241527f;
		rectMesh[4][3].boundingBoxRight = 0.421748f;
		rectMesh[4][3].boundingBoxTop = -0.358797f;
		rectMesh[4][3].boundingBoxButtom = -0.425851f;
		rectMesh[4][3].H[0][0] = 4.645296f;
		rectMesh[4][3].H[0][1] = -3.603748f;
		rectMesh[4][3].H[0][2] = -2.582157f;
		rectMesh[4][3].H[1][0] = 2.202165f;
		rectMesh[4][3].H[1][1] = 15.126945f;
		rectMesh[4][3].H[1][2] = 5.597354f;
		rectMesh[4][3].H[2][0] = -0.115063f;
		rectMesh[4][3].H[2][1] = 0.521398f;
		rectMesh[4][3].H[2][2] = 1.000000f;
		rectMesh[4][4].leftTopX = 0.383471f;
		rectMesh[4][4].leftTopY = -0.425851f;
		rectMesh[4][4].rightTopX = 0.525415f;
		rectMesh[4][4].rightTopY = -0.446515f;
		rectMesh[4][4].leftBottomX = 0.421748f;
		rectMesh[4][4].leftBottomY = -0.381680f;
		rectMesh[4][4].rightBottomX = 0.565980f;
		rectMesh[4][4].rightBottomY = -0.404563f;
		rectMesh[4][4].centerPointX = 0.474154f;
		rectMesh[4][4].centerPointY = -0.414652f;
		rectMesh[4][4].boundingBoxLeft = 0.383471f;
		rectMesh[4][4].boundingBoxRight = 0.565980f;
		rectMesh[4][4].boundingBoxTop = -0.381680f;
		rectMesh[4][4].boundingBoxButtom = -0.446515f;
		rectMesh[4][4].H[0][0] = 4.373475f;
		rectMesh[4][4].H[0][1] = -3.789880f;
		rectMesh[4][4].H[0][2] = -3.291025f;
		rectMesh[4][4].H[1][0] = 2.182635f;
		rectMesh[4][4].H[1][1] = 14.992790f;
		rectMesh[4][4].H[1][2] = 5.547714f;
		rectMesh[4][4].H[2][0] = -0.109568f;
		rectMesh[4][4].H[2][1] = 0.544953f;
		rectMesh[4][4].H[2][2] = 1.000000f; 
		valid = true;
	}
}

void MeshMapping::SetUpAsOldHandRight()
{
	//代码来自PrintBakeCode()烘培
	{
		rectMesh[0][0].leftTopX = -0.531362f;
		rectMesh[0][0].leftTopY = -0.382026f;
		rectMesh[0][0].rightTopX = -0.454392f;
		rectMesh[0][0].rightTopY = -0.411150f;
		rectMesh[0][0].leftBottomX = -0.513333f;
		rectMesh[0][0].leftBottomY = -0.342848f;
		rectMesh[0][0].rightBottomX = -0.428597f;
		rectMesh[0][0].rightBottomY = -0.372804f;
		rectMesh[0][0].centerPointX = -0.481921f;
		rectMesh[0][0].centerPointY = -0.377207f;
		rectMesh[0][0].boundingBoxLeft = -0.531362f;
		rectMesh[0][0].boundingBoxRight = -0.428597f;
		rectMesh[0][0].boundingBoxTop = -0.342848f;
		rectMesh[0][0].boundingBoxButtom = -0.411150f;
		rectMesh[0][0].H[0][0] = 5.105824f;
		rectMesh[0][0].H[0][1] = -2.349577f;
		rectMesh[0][0].H[0][2] = 1.815442f;
		rectMesh[0][0].H[1][0] = 3.956549f;
		rectMesh[0][0].H[1][1] = 10.456588f;
		rectMesh[0][0].H[1][2] = 6.097053f;
		rectMesh[0][0].H[2][0] = 0.519907f;
		rectMesh[0][0].H[2][1] = 0.735338f;
		rectMesh[0][0].H[2][2] = 1.000000f;
		rectMesh[0][1].leftTopX = -0.454392f;
		rectMesh[0][1].leftTopY = -0.411150f;
		rectMesh[0][1].rightTopX = -0.377422f;
		rectMesh[0][1].rightTopY = -0.440274f;
		rectMesh[0][1].leftBottomX = -0.428597f;
		rectMesh[0][1].leftBottomY = -0.372804f;
		rectMesh[0][1].rightBottomX = -0.343861f;
		rectMesh[0][1].rightBottomY = -0.402760f;
		rectMesh[0][1].centerPointX = -0.401068f;
		rectMesh[0][1].centerPointY = -0.406747f;
		rectMesh[0][1].boundingBoxLeft = -0.454392f;
		rectMesh[0][1].boundingBoxRight = -0.343861f;
		rectMesh[0][1].boundingBoxTop = -0.372804f;
		rectMesh[0][1].boundingBoxButtom = -0.440274f;
		rectMesh[0][1].H[0][0] = 5.190828f;
		rectMesh[0][1].H[0][1] = -3.491838f;
		rectMesh[0][1].H[0][2] = 0.923003f;
		rectMesh[0][1].H[1][0] = 4.102691f;
		rectMesh[0][1].H[1][1] = 10.842841f;
		rectMesh[0][1].H[1][2] = 6.322267f;
		rectMesh[0][1].H[2][0] = 0.514198f;
		rectMesh[0][1].H[2][1] = 0.692061f;
		rectMesh[0][1].H[2][2] = 1.000000f;
		rectMesh[0][2].leftTopX = -0.377422f;
		rectMesh[0][2].leftTopY = -0.440274f;
		rectMesh[0][2].rightTopX = -0.272715f;
		rectMesh[0][2].rightTopY = -0.477719f;
		rectMesh[0][2].leftBottomX = -0.343861f;
		rectMesh[0][2].leftBottomY = -0.402760f;
		rectMesh[0][2].rightBottomX = -0.238183f;
		rectMesh[0][2].rightBottomY = -0.437500f;
		rectMesh[0][2].centerPointX = -0.308045f;
		rectMesh[0][2].centerPointY = -0.439563f;
		rectMesh[0][2].boundingBoxLeft = -0.377422f;
		rectMesh[0][2].boundingBoxRight = -0.238183f;
		rectMesh[0][2].boundingBoxTop = -0.402760f;
		rectMesh[0][2].boundingBoxButtom = -0.477719f;
		rectMesh[0][2].H[0][0] = 8.755401f;
		rectMesh[0][2].H[0][1] = -7.832935f;
		rectMesh[0][2].H[0][2] = -0.144154f;
		rectMesh[0][2].H[1][0] = 8.145636f;
		rectMesh[0][2].H[1][1] = 22.777601f;
		rectMesh[0][2].H[1][2] = 13.102727f;
		rectMesh[0][2].H[2][0] = 0.432090f;
		rectMesh[0][2].H[2][1] = -0.686372f;
		rectMesh[0][2].H[2][2] = 1.000000f;
		rectMesh[0][3].leftTopX = -0.272715f;
		rectMesh[0][3].leftTopY = -0.477719f;
		rectMesh[0][3].rightTopX = -0.140271f;
		rectMesh[0][3].rightTopY = -0.523485f;
		rectMesh[0][3].leftBottomX = -0.238183f;
		rectMesh[0][3].leftBottomY = -0.437500f;
		rectMesh[0][3].rightBottomX = -0.111564f;
		rectMesh[0][3].rightBottomY = -0.477025f;
		rectMesh[0][3].centerPointX = -0.190683f;
		rectMesh[0][3].centerPointY = -0.478932f;
		rectMesh[0][3].boundingBoxLeft = -0.272715f;
		rectMesh[0][3].boundingBoxRight = -0.111564f;
		rectMesh[0][3].boundingBoxTop = -0.437500f;
		rectMesh[0][3].boundingBoxButtom = -0.523485f;
		rectMesh[0][3].H[0][0] = 23.437145f;
		rectMesh[0][3].H[0][1] = -20.123598f;
		rectMesh[0][3].H[0][2] = -3.221753f;
		rectMesh[0][3].H[1][0] = 23.063370f;
		rectMesh[0][3].H[1][1] = 66.743965f;
		rectMesh[0][3].H[1][2] = 38.174576f;
		rectMesh[0][3].H[2][0] = 0.392044f;
		rectMesh[0][3].H[2][1] = -5.883790f;
		rectMesh[0][3].H[2][2] = 1.000000f;
		rectMesh[0][4].leftTopX = -0.140271f;
		rectMesh[0][4].leftTopY = -0.523485f;
		rectMesh[0][4].rightTopX = -0.007828f;
		rectMesh[0][4].rightTopY = -0.569251f;
		rectMesh[0][4].leftBottomX = -0.111564f;
		rectMesh[0][4].leftBottomY = -0.477025f;
		rectMesh[0][4].rightBottomX = 0.015055f;
		rectMesh[0][4].rightBottomY = -0.516551f;
		rectMesh[0][4].centerPointX = -0.061152f;
		rectMesh[0][4].centerPointY = -0.521578f;
		rectMesh[0][4].boundingBoxLeft = -0.140271f;
		rectMesh[0][4].boundingBoxRight = 0.015055f;
		rectMesh[0][4].boundingBoxTop = -0.477025f;
		rectMesh[0][4].boundingBoxButtom = -0.569251f;
		rectMesh[0][4].H[0][0] = 19.725075f;
		rectMesh[0][4].H[0][1] = -12.188336f;
		rectMesh[0][4].H[0][2] = -3.613544f;
		rectMesh[0][4].H[1][0] = 16.986450f;
		rectMesh[0][4].H[1][1] = 49.157585f;
		rectMesh[0][4].H[1][2] = 28.115955f;
		rectMesh[0][4].H[2][0] = 0.449485f;
		rectMesh[0][4].H[2][1] = -3.818698f;
		rectMesh[0][4].H[2][2] = 1.000000f;
		rectMesh[1][0].leftTopX = -0.513333f;
		rectMesh[1][0].leftTopY = -0.342848f;
		rectMesh[1][0].rightTopX = -0.428597f;
		rectMesh[1][0].rightTopY = -0.372804f;
		rectMesh[1][0].leftBottomX = -0.495304f;
		rectMesh[1][0].leftBottomY = -0.303669f;
		rectMesh[1][0].rightBottomX = -0.402802f;
		rectMesh[1][0].rightBottomY = -0.334457f;
		rectMesh[1][0].centerPointX = -0.460009f;
		rectMesh[1][0].centerPointY = -0.338445f;
		rectMesh[1][0].boundingBoxLeft = -0.513333f;
		rectMesh[1][0].boundingBoxRight = -0.402802f;
		rectMesh[1][0].boundingBoxTop = -0.303669f;
		rectMesh[1][0].boundingBoxButtom = -0.372804f;
		rectMesh[1][0].H[0][0] = 5.134015f;
		rectMesh[1][0].H[0][1] = -2.362556f;
		rectMesh[1][0].H[0][2] = 1.825463f;
		rectMesh[1][0].H[1][0] = 4.078840f;
		rectMesh[1][0].H[1][1] = 11.537816f;
		rectMesh[1][0].H[1][2] = 6.049520f;
		rectMesh[1][0].H[2][0] = 0.491722f;
		rectMesh[1][0].H[2][1] = 0.760291f;
		rectMesh[1][0].H[2][2] = 1.000000f;
		rectMesh[1][1].leftTopX = -0.428597f;
		rectMesh[1][1].leftTopY = -0.372804f;
		rectMesh[1][1].rightTopX = -0.343861f;
		rectMesh[1][1].rightTopY = -0.402760f;
		rectMesh[1][1].leftBottomX = -0.402802f;
		rectMesh[1][1].leftBottomY = -0.334457f;
		rectMesh[1][1].rightBottomX = -0.310299f;
		rectMesh[1][1].rightBottomY = -0.365245f;
		rectMesh[1][1].centerPointX = -0.371390f;
		rectMesh[1][1].centerPointY = -0.368817f;
		rectMesh[1][1].boundingBoxLeft = -0.428597f;
		rectMesh[1][1].boundingBoxRight = -0.310299f;
		rectMesh[1][1].boundingBoxTop = -0.334457f;
		rectMesh[1][1].boundingBoxButtom = -0.402760f;
		rectMesh[1][1].H[0][0] = 5.188372f;
		rectMesh[1][1].H[0][1] = -3.490188f;
		rectMesh[1][1].H[0][2] = 0.922565f;
		rectMesh[1][1].H[1][0] = 4.205334f;
		rectMesh[1][1].H[1][1] = 11.895641f;
		rectMesh[1][1].H[1][2] = 6.237134f;
		rectMesh[1][1].H[2][0] = 0.484935f;
		rectMesh[1][1].H[2][1] = 0.717685f;
		rectMesh[1][1].H[2][2] = 1.000000f;
		rectMesh[1][2].leftTopX = -0.343861f;
		rectMesh[1][2].leftTopY = -0.402760f;
		rectMesh[1][2].rightTopX = -0.238183f;
		rectMesh[1][2].rightTopY = -0.437500f;
		rectMesh[1][2].leftBottomX = -0.310299f;
		rectMesh[1][2].leftBottomY = -0.365245f;
		rectMesh[1][2].rightBottomX = -0.203650f;
		rectMesh[1][2].rightBottomY = -0.397282f;
		rectMesh[1][2].centerPointX = -0.273998f;
		rectMesh[1][2].centerPointY = -0.400697f;
		rectMesh[1][2].boundingBoxLeft = -0.343861f;
		rectMesh[1][2].boundingBoxRight = -0.203650f;
		rectMesh[1][2].boundingBoxTop = -0.365245f;
		rectMesh[1][2].boundingBoxButtom = -0.437500f;
		rectMesh[1][2].H[0][0] = 8.767778f;
		rectMesh[1][2].H[0][1] = -7.843987f;
		rectMesh[1][2].H[0][2] = -0.144349f;
		rectMesh[1][2].H[1][0] = 7.562515f;
		rectMesh[1][2].H[1][1] = 23.004555f;
		rectMesh[1][2].H[1][2] = 11.865759f;
		rectMesh[1][2].H[2][0] = 0.443035f;
		rectMesh[1][2].H[2][1] = -0.696198f;
		rectMesh[1][2].H[2][2] = 1.000000f;
		rectMesh[1][3].leftTopX = -0.238183f;
		rectMesh[1][3].leftTopY = -0.437500f;
		rectMesh[1][3].rightTopX = -0.111564f;
		rectMesh[1][3].rightTopY = -0.477025f;
		rectMesh[1][3].leftBottomX = -0.203650f;
		rectMesh[1][3].leftBottomY = -0.397282f;
		rectMesh[1][3].rightBottomX = -0.082856f;
		rectMesh[1][3].rightBottomY = -0.430566f;
		rectMesh[1][3].centerPointX = -0.159063f;
		rectMesh[1][3].centerPointY = -0.435593f;
		rectMesh[1][3].boundingBoxLeft = -0.238183f;
		rectMesh[1][3].boundingBoxRight = -0.082856f;
		rectMesh[1][3].boundingBoxTop = -0.397282f;
		rectMesh[1][3].boundingBoxButtom = -0.477025f;
		rectMesh[1][3].H[0][0] = 24.590050f;
		rectMesh[1][3].H[0][1] = -21.113510f;
		rectMesh[1][3].H[0][2] = -3.380238f;
		rectMesh[1][3].H[1][0] = 20.688721f;
		rectMesh[1][3].H[1][1] = 66.276489f;
		rectMesh[1][3].H[1][2] = 33.923676f;
		rectMesh[1][3].H[2][0] = 0.679815f;
		rectMesh[1][3].H[2][1] = -6.339134f;
		rectMesh[1][3].H[2][2] = 1.000000f;
		rectMesh[1][4].leftTopX = -0.111564f;
		rectMesh[1][4].leftTopY = -0.477025f;
		rectMesh[1][4].rightTopX = 0.015055f;
		rectMesh[1][4].rightTopY = -0.516551f;
		rectMesh[1][4].leftBottomX = -0.082856f;
		rectMesh[1][4].leftBottomY = -0.430566f;
		rectMesh[1][4].rightBottomX = 0.037938f;
		rectMesh[1][4].rightBottomY = -0.463850f;
		rectMesh[1][4].centerPointX = -0.035356f;
		rectMesh[1][4].centerPointY = -0.471998f;
		rectMesh[1][4].boundingBoxLeft = -0.111564f;
		rectMesh[1][4].boundingBoxRight = 0.037938f;
		rectMesh[1][4].boundingBoxTop = -0.430566f;
		rectMesh[1][4].boundingBoxButtom = -0.516551f;
		rectMesh[1][4].H[0][0] = 20.625103f;
		rectMesh[1][4].H[0][1] = -12.744462f;
		rectMesh[1][4].H[0][2] = -3.778419f;
		rectMesh[1][4].H[1][0] = 15.208511f;
		rectMesh[1][4].H[1][1] = 48.720436f;
		rectMesh[1][4].H[1][2] = 24.937599f;
		rectMesh[1][4].H[2][0] = 0.660712f;
		rectMesh[1][4].H[2][1] = -4.075715f;
		rectMesh[1][4].H[2][2] = 1.000000f;
		rectMesh[2][0].leftTopX = -0.495304f;
		rectMesh[2][0].leftTopY = -0.303669f;
		rectMesh[2][0].rightTopX = -0.402802f;
		rectMesh[2][0].rightTopY = -0.334457f;
		rectMesh[2][0].leftBottomX = -0.477275f;
		rectMesh[2][0].leftBottomY = -0.264491f;
		rectMesh[2][0].rightBottomX = -0.377006f;
		rectMesh[2][0].rightBottomY = -0.296111f;
		rectMesh[2][0].centerPointX = -0.438097f;
		rectMesh[2][0].centerPointY = -0.299682f;
		rectMesh[2][0].boundingBoxLeft = -0.495304f;
		rectMesh[2][0].boundingBoxRight = -0.377006f;
		rectMesh[2][0].boundingBoxTop = -0.264491f;
		rectMesh[2][0].boundingBoxButtom = -0.334457f;
		rectMesh[2][0].H[0][0] = 5.158546f;
		rectMesh[2][0].H[0][1] = -2.373845f;
		rectMesh[2][0].H[0][2] = 1.834186f;
		rectMesh[2][0].H[1][0] = 4.200518f;
		rectMesh[2][0].H[1][1] = 12.620497f;
		rectMesh[2][0].H[1][2] = 5.912994f;
		rectMesh[2][0].H[2][0] = 0.467214f;
		rectMesh[2][0].H[2][1] = 0.781983f;
		rectMesh[2][0].H[2][2] = 1.000000f;
		rectMesh[2][1].leftTopX = -0.402802f;
		rectMesh[2][1].leftTopY = -0.334457f;
		rectMesh[2][1].rightTopX = -0.310299f;
		rectMesh[2][1].rightTopY = -0.365245f;
		rectMesh[2][1].leftBottomX = -0.377006f;
		rectMesh[2][1].leftBottomY = -0.296111f;
		rectMesh[2][1].rightBottomX = -0.276737f;
		rectMesh[2][1].rightBottomY = -0.327731f;
		rectMesh[2][1].centerPointX = -0.341711f;
		rectMesh[2][1].centerPointY = -0.330886f;
		rectMesh[2][1].boundingBoxLeft = -0.402802f;
		rectMesh[2][1].boundingBoxRight = -0.276737f;
		rectMesh[2][1].boundingBoxTop = -0.296111f;
		rectMesh[2][1].boundingBoxButtom = -0.365245f;
		rectMesh[2][1].H[0][0] = 5.186208f;
		rectMesh[2][1].H[0][1] = -3.488728f;
		rectMesh[2][1].H[0][2] = 0.922182f;
		rectMesh[2][1].H[1][0] = 4.309257f;
		rectMesh[2][1].H[1][1] = 12.947193f;
		rectMesh[2][1].H[1][2] = 6.066061f;
		rectMesh[2][1].H[2][0] = 0.459635f;
		rectMesh[2][1].H[2][1] = 0.739850f;
		rectMesh[2][1].H[2][2] = 1.000000f;
		rectMesh[2][2].leftTopX = -0.310299f;
		rectMesh[2][2].leftTopY = -0.365245f;
		rectMesh[2][2].rightTopX = -0.203650f;
		rectMesh[2][2].rightTopY = -0.397282f;
		rectMesh[2][2].leftBottomX = -0.276737f;
		rectMesh[2][2].leftBottomY = -0.327731f;
		rectMesh[2][2].rightBottomX = -0.169118f;
		rectMesh[2][2].rightBottomY = -0.357063f;
		rectMesh[2][2].centerPointX = -0.239951f;
		rectMesh[2][2].centerPointY = -0.361830f;
		rectMesh[2][2].boundingBoxLeft = -0.310299f;
		rectMesh[2][2].boundingBoxRight = -0.169118f;
		rectMesh[2][2].boundingBoxTop = -0.327731f;
		rectMesh[2][2].boundingBoxButtom = -0.397282f;
		rectMesh[2][2].H[0][0] = 8.780564f;
		rectMesh[2][2].H[0][1] = -7.855434f;
		rectMesh[2][2].H[0][2] = -0.144562f;
		rectMesh[2][2].H[1][0] = 6.978802f;
		rectMesh[2][2].H[1][1] = 23.232468f;
		rectMesh[2][2].H[1][2] = 10.651069f;
		rectMesh[2][2].H[2][0] = 0.454249f;
		rectMesh[2][2].H[2][1] = -0.706301f;
		rectMesh[2][2].H[2][2] = 1.000000f;
		rectMesh[2][3].leftTopX = -0.203650f;
		rectMesh[2][3].leftTopY = -0.397282f;
		rectMesh[2][3].rightTopX = -0.082856f;
		rectMesh[2][3].rightTopY = -0.430566f;
		rectMesh[2][3].leftBottomX = -0.169118f;
		rectMesh[2][3].leftBottomY = -0.357063f;
		rectMesh[2][3].rightBottomX = -0.054148f;
		rectMesh[2][3].rightBottomY = -0.384107f;
		rectMesh[2][3].centerPointX = -0.127443f;
		rectMesh[2][3].centerPointY = -0.392254f;
		rectMesh[2][3].boundingBoxLeft = -0.203650f;
		rectMesh[2][3].boundingBoxRight = -0.054148f;
		rectMesh[2][3].boundingBoxTop = -0.357063f;
		rectMesh[2][3].boundingBoxButtom = -0.430566f;
		rectMesh[2][3].H[0][0] = 26.044806f;
		rectMesh[2][3].H[0][1] = -22.362574f;
		rectMesh[2][3].H[0][2] = -3.580206f;
		rectMesh[2][3].H[1][0] = 18.240372f;
		rectMesh[2][3].H[1][1] = 66.197235f;
		rectMesh[2][3].H[1][2] = 30.013605f;
		rectMesh[2][3].H[2][0] = 1.042973f;
		rectMesh[2][3].H[2][1] = -6.913706f;
		rectMesh[2][3].H[2][2] = 1.000000f;
		rectMesh[2][4].leftTopX = -0.082856f;
		rectMesh[2][4].leftTopY = -0.430566f;
		rectMesh[2][4].rightTopX = 0.037938f;
		rectMesh[2][4].rightTopY = -0.463850f;
		rectMesh[2][4].leftBottomX = -0.054148f;
		rectMesh[2][4].leftBottomY = -0.384107f;
		rectMesh[2][4].rightBottomX = 0.060821f;
		rectMesh[2][4].rightBottomY = -0.411150f;
		rectMesh[2][4].centerPointX = -0.009561f;
		rectMesh[2][4].centerPointY = -0.422418f;
		rectMesh[2][4].boundingBoxLeft = -0.082856f;
		rectMesh[2][4].boundingBoxRight = 0.060821f;
		rectMesh[2][4].boundingBoxTop = -0.384107f;
		rectMesh[2][4].boundingBoxButtom = -0.463850f;
		rectMesh[2][4].H[0][0] = 21.741787f;
		rectMesh[2][4].H[0][1] = -13.434465f;
		rectMesh[2][4].H[0][2] = -3.982986f;
		rectMesh[2][4].H[1][0] = 13.369568f;
		rectMesh[2][4].H[1][1] = 48.520290f;
		rectMesh[2][4].H[1][2] = 21.998936f;
		rectMesh[2][4].H[2][0] = 0.922712f;
		rectMesh[2][4].H[2][1] = -4.394601f;
		rectMesh[2][4].H[2][2] = 1.000000f;
		rectMesh[3][0].leftTopX = -0.477275f;
		rectMesh[3][0].leftTopY = -0.264491f;
		rectMesh[3][0].rightTopX = -0.377006f;
		rectMesh[3][0].rightTopY = -0.296111f;
		rectMesh[3][0].leftBottomX = -0.459246f;
		rectMesh[3][0].leftBottomY = -0.225313f;
		rectMesh[3][0].rightBottomX = -0.351211f;
		rectMesh[3][0].rightBottomY = -0.257765f;
		rectMesh[3][0].centerPointX = -0.416185f;
		rectMesh[3][0].centerPointY = -0.260920f;
		rectMesh[3][0].boundingBoxLeft = -0.477275f;
		rectMesh[3][0].boundingBoxRight = -0.351211f;
		rectMesh[3][0].boundingBoxTop = -0.225313f;
		rectMesh[3][0].boundingBoxButtom = -0.296111f;
		rectMesh[3][0].H[0][0] = 5.180042f;
		rectMesh[3][0].H[0][1] = -2.383739f;
		rectMesh[3][0].H[0][2] = 1.841829f;
		rectMesh[3][0].H[1][0] = 4.321664f;
		rectMesh[3][0].H[1][1] = 13.704237f;
		rectMesh[3][0].H[1][2] = 5.687271f;
		rectMesh[3][0].H[2][0] = 0.445721f;
		rectMesh[3][0].H[2][1] = 0.801011f;
		rectMesh[3][0].H[2][2] = 1.000000f;
		rectMesh[3][1].leftTopX = -0.377006f;
		rectMesh[3][1].leftTopY = -0.296111f;
		rectMesh[3][1].rightTopX = -0.276737f;
		rectMesh[3][1].rightTopY = -0.327731f;
		rectMesh[3][1].leftBottomX = -0.351211f;
		rectMesh[3][1].leftBottomY = -0.257765f;
		rectMesh[3][1].rightBottomX = -0.243176f;
		rectMesh[3][1].rightBottomY = -0.290217f;
		rectMesh[3][1].centerPointX = -0.312033f;
		rectMesh[3][1].centerPointY = -0.292956f;
		rectMesh[3][1].boundingBoxLeft = -0.377006f;
		rectMesh[3][1].boundingBoxRight = -0.243176f;
		rectMesh[3][1].boundingBoxTop = -0.257765f;
		rectMesh[3][1].boundingBoxButtom = -0.327731f;
		rectMesh[3][1].H[0][0] = 5.184347f;
		rectMesh[3][1].H[0][1] = -3.487482f;
		rectMesh[3][1].H[0][2] = 0.921849f;
		rectMesh[3][1].H[1][0] = 4.414269f;
		rectMesh[3][1].H[1][1] = 13.997886f;
		rectMesh[3][1].H[1][2] = 5.809137f;
		rectMesh[3][1].H[2][0] = 0.437556f;
		rectMesh[3][1].H[2][1] = 0.759183f;
		rectMesh[3][1].H[2][2] = 1.000000f;
		rectMesh[3][2].leftTopX = -0.276737f;
		rectMesh[3][2].leftTopY = -0.327731f;
		rectMesh[3][2].rightTopX = -0.169118f;
		rectMesh[3][2].rightTopY = -0.357063f;
		rectMesh[3][2].leftBottomX = -0.243176f;
		rectMesh[3][2].leftBottomY = -0.290217f;
		rectMesh[3][2].rightBottomX = -0.134585f;
		rectMesh[3][2].rightBottomY = -0.316845f;
		rectMesh[3][2].centerPointX = -0.205904f;
		rectMesh[3][2].centerPointY = -0.322964f;
		rectMesh[3][2].boundingBoxLeft = -0.276737f;
		rectMesh[3][2].boundingBoxRight = -0.134585f;
		rectMesh[3][2].boundingBoxTop = -0.290217f;
		rectMesh[3][2].boundingBoxButtom = -0.357063f;
		rectMesh[3][2].H[0][0] = 8.793555f;
		rectMesh[3][2].H[0][1] = -7.867058f;
		rectMesh[3][2].H[0][2] = -0.144777f;
		rectMesh[3][2].H[1][0] = 6.394300f;
		rectMesh[3][2].H[1][1] = 23.460884f;
		rectMesh[3][2].H[1][2] = 9.458405f;
		rectMesh[3][2].H[2][0] = 0.465721f;
		rectMesh[3][2].H[2][1] = -0.716606f;
		rectMesh[3][2].H[2][2] = 1.000000f;
		rectMesh[3][3].leftTopX = -0.169118f;
		rectMesh[3][3].leftTopY = -0.357063f;
		rectMesh[3][3].rightTopX = -0.054148f;
		rectMesh[3][3].rightTopY = -0.384107f;
		rectMesh[3][3].leftBottomX = -0.134585f;
		rectMesh[3][3].leftBottomY = -0.316845f;
		rectMesh[3][3].rightBottomX = -0.025441f;
		rectMesh[3][3].rightBottomY = -0.337647f;
		rectMesh[3][3].centerPointX = -0.095823f;
		rectMesh[3][3].centerPointY = -0.348915f;
		rectMesh[3][3].boundingBoxLeft = -0.169118f;
		rectMesh[3][3].boundingBoxRight = -0.025441f;
		rectMesh[3][3].boundingBoxTop = -0.316845f;
		rectMesh[3][3].boundingBoxButtom = -0.384107f;
		rectMesh[3][3].H[0][0] = 27.938015f;
		rectMesh[3][3].H[0][1] = -23.988173f;
		rectMesh[3][3].H[0][2] = -3.840472f;
		rectMesh[3][3].H[1][0] = 15.685287f;
		rectMesh[3][3].H[1][1] = 66.682426f;
		rectMesh[3][3].H[1][2] = 26.462494f;
		rectMesh[3][3].H[2][0] = 1.515522f;
		rectMesh[3][3].H[2][1] = -7.661445f;
		rectMesh[3][3].H[2][2] = 1.000000f;
		rectMesh[3][4].leftTopX = -0.054148f;
		rectMesh[3][4].leftTopY = -0.384107f;
		rectMesh[3][4].rightTopX = 0.060821f;
		rectMesh[3][4].rightTopY = -0.411150f;
		rectMesh[3][4].leftBottomX = -0.025441f;
		rectMesh[3][4].leftBottomY = -0.337647f;
		rectMesh[3][4].rightBottomX = 0.083704f;
		rectMesh[3][4].rightBottomY = -0.358450f;
		rectMesh[3][4].centerPointX = 0.016234f;
		rectMesh[3][4].centerPointY = -0.372839f;
		rectMesh[3][4].boundingBoxLeft = -0.054148f;
		rectMesh[3][4].boundingBoxRight = 0.083704f;
		rectMesh[3][4].boundingBoxTop = -0.337647f;
		rectMesh[3][4].boundingBoxButtom = -0.411150f;
		rectMesh[3][4].H[0][0] = 23.163372f;
		rectMesh[3][4].H[0][1] = -14.312883f;
		rectMesh[3][4].H[0][2] = -4.243417f;
		rectMesh[3][4].H[1][0] = 11.444360f;
		rectMesh[3][4].H[1][1] = 48.653206f;
		rectMesh[3][4].H[1][2] = 19.307711f;
		rectMesh[3][4].H[2][0] = 1.256203f;
		rectMesh[3][4].H[2][1] = -4.800559f;
		rectMesh[3][4].H[2][2] = 1.000000f;
		rectMesh[4][0].leftTopX = -0.459246f;
		rectMesh[4][0].leftTopY = -0.225313f;
		rectMesh[4][0].rightTopX = -0.351211f;
		rectMesh[4][0].rightTopY = -0.257765f;
		rectMesh[4][0].leftBottomX = -0.441217f;
		rectMesh[4][0].leftBottomY = -0.186134f;
		rectMesh[4][0].rightBottomX = -0.325416f;
		rectMesh[4][0].rightBottomY = -0.219418f;
		rectMesh[4][0].centerPointX = -0.394273f;
		rectMesh[4][0].centerPointY = -0.222157f;
		rectMesh[4][0].boundingBoxLeft = -0.459246f;
		rectMesh[4][0].boundingBoxRight = -0.325416f;
		rectMesh[4][0].boundingBoxTop = -0.186134f;
		rectMesh[4][0].boundingBoxButtom = -0.257765f;
		rectMesh[4][0].H[0][0] = 5.199035f;
		rectMesh[4][0].H[0][1] = -2.392476f;
		rectMesh[4][0].H[0][2] = 1.848582f;
		rectMesh[4][0].H[1][0] = 4.442348f;
		rectMesh[4][0].H[1][1] = 14.788818f;
		rectMesh[4][0].H[1][2] = 5.372238f;
		rectMesh[4][0].H[2][0] = 0.426729f;
		rectMesh[4][0].H[2][1] = 0.817829f;
		rectMesh[4][0].H[2][2] = 1.000000f;
		rectMesh[4][1].leftTopX = -0.351211f;
		rectMesh[4][1].leftTopY = -0.257765f;
		rectMesh[4][1].rightTopX = -0.243176f;
		rectMesh[4][1].rightTopY = -0.290217f;
		rectMesh[4][1].leftBottomX = -0.325416f;
		rectMesh[4][1].leftBottomY = -0.219418f;
		rectMesh[4][1].rightBottomX = -0.209614f;
		rectMesh[4][1].rightBottomY = -0.252703f;
		rectMesh[4][1].centerPointX = -0.282354f;
		rectMesh[4][1].centerPointY = -0.255026f;
		rectMesh[4][1].boundingBoxLeft = -0.351211f;
		rectMesh[4][1].boundingBoxRight = -0.209614f;
		rectMesh[4][1].boundingBoxTop = -0.219418f;
		rectMesh[4][1].boundingBoxButtom = -0.290217f;
		rectMesh[4][1].H[0][0] = 5.182701f;
		rectMesh[4][1].H[0][1] = -3.486373f;
		rectMesh[4][1].H[0][2] = 0.921557f;
		rectMesh[4][1].H[1][0] = 4.520118f;
		rectMesh[4][1].H[1][1] = 15.047732f;
		rectMesh[4][1].H[1][2] = 5.466290f;
		rectMesh[4][1].H[2][0] = 0.418121f;
		rectMesh[4][1].H[2][1] = 0.776204f;
		rectMesh[4][1].H[2][2] = 1.000000f;
		rectMesh[4][2].leftTopX = -0.243176f;
		rectMesh[4][2].leftTopY = -0.290217f;
		rectMesh[4][2].rightTopX = -0.134585f;
		rectMesh[4][2].rightTopY = -0.316845f;
		rectMesh[4][2].leftBottomX = -0.209614f;
		rectMesh[4][2].leftBottomY = -0.252703f;
		rectMesh[4][2].rightBottomX = -0.100053f;
		rectMesh[4][2].rightBottomY = -0.276626f;
		rectMesh[4][2].centerPointX = -0.171857f;
		rectMesh[4][2].centerPointY = -0.284098f;
		rectMesh[4][2].boundingBoxLeft = -0.243176f;
		rectMesh[4][2].boundingBoxRight = -0.100053f;
		rectMesh[4][2].boundingBoxTop = -0.252703f;
		rectMesh[4][2].boundingBoxButtom = -0.316845f;
		rectMesh[4][2].H[0][0] = 8.806891f;
		rectMesh[4][2].H[0][1] = -7.878992f;
		rectMesh[4][2].H[0][2] = -0.144997f;
		rectMesh[4][2].H[1][0] = 5.809055f;
		rectMesh[4][2].H[1][1] = 23.690060f;
		rectMesh[4][2].H[1][2] = 8.287880f;
		rectMesh[4][2].H[2][0] = 0.477442f;
		rectMesh[4][2].H[2][1] = -0.727158f;
		rectMesh[4][2].H[2][2] = 1.000000f;
		rectMesh[4][3].leftTopX = -0.134585f;
		rectMesh[4][3].leftTopY = -0.316845f;
		rectMesh[4][3].rightTopX = -0.025441f;
		rectMesh[4][3].rightTopY = -0.337647f;
		rectMesh[4][3].leftBottomX = -0.100053f;
		rectMesh[4][3].leftBottomY = -0.276626f;
		rectMesh[4][3].rightBottomX = 0.003267f;
		rectMesh[4][3].rightBottomY = -0.291188f;
		rectMesh[4][3].centerPointX = -0.064203f;
		rectMesh[4][3].centerPointY = -0.305576f;
		rectMesh[4][3].boundingBoxLeft = -0.134585f;
		rectMesh[4][3].boundingBoxRight = 0.003267f;
		rectMesh[4][3].boundingBoxTop = -0.276626f;
		rectMesh[4][3].boundingBoxButtom = -0.337647f;
		rectMesh[4][3].H[0][0] = 30.502359f;
		rectMesh[4][3].H[0][1] = -26.189941f;
		rectMesh[4][3].H[0][2] = -4.192966f;
		rectMesh[4][3].H[1][0] = 12.966415f;
		rectMesh[4][3].H[1][1] = 68.030273f;
		rectMesh[4][3].H[1][2] = 23.300110f;
		rectMesh[4][3].H[2][0] = 2.155686f;
		rectMesh[4][3].H[2][1] = -8.674258f;
		rectMesh[4][3].H[2][2] = 1.000000f;
		rectMesh[4][4].leftTopX = -0.025441f;
		rectMesh[4][4].leftTopY = -0.337647f;
		rectMesh[4][4].rightTopX = 0.083704f;
		rectMesh[4][4].rightTopY = -0.358450f;
		rectMesh[4][4].leftBottomX = 0.003267f;
		rectMesh[4][4].leftBottomY = -0.291188f;
		rectMesh[4][4].rightBottomX = 0.106587f;
		rectMesh[4][4].rightBottomY = -0.305750f;
		rectMesh[4][4].centerPointX = 0.042030f;
		rectMesh[4][4].centerPointY = -0.323259f;
		rectMesh[4][4].boundingBoxLeft = -0.025441f;
		rectMesh[4][4].boundingBoxRight = 0.106587f;
		rectMesh[4][4].boundingBoxTop = -0.291188f;
		rectMesh[4][4].boundingBoxButtom = -0.358450f;
		rectMesh[4][4].H[0][0] = 25.033657f;
		rectMesh[4][4].H[0][1] = -15.468532f;
		rectMesh[4][4].H[0][2] = -4.586037f;
		rectMesh[4][4].H[1][0] = 9.391751f;
		rectMesh[4][4].H[1][1] = 49.275387f;
		rectMesh[4][4].H[1][2] = 16.876633f;
		rectMesh[4][4].H[2][0] = 1.694999f;
		rectMesh[4][4].H[2][1] = -5.334645f;
		rectMesh[4][4].H[2][2] = 1.000000f;
		valid = true;
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
