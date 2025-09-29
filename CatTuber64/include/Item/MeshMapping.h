
#ifndef _MeshMapping_h
#define _MeshMapping_h

//网格映射
#include <Model/IModel.h>
#include<glm/glm.hpp>
class MeshMapping
{
public:
	void BuildMeshMapping(IModel* _model,HandPosHandle hand, ParamHandle handParamPosX, ParamHandle handParamPosY, ParamHandle handParamPosZ);
	void Clear();
	bool Valid() { return valid; };

	//点落在的矩形中执行单应变换
	void GetParamValueAtPos(float posX,float posY,float* ParamValueX,float* ParamValueY);
private:
	bool valid = false;
	

	struct UnitRect
	{
		//点坐标  Y轴向上
		float leftTopX;
		float leftTopY;

		float rightTopX;
		float rightTopY;

		float leftBottomX;
		float leftBottomY;

		float rightBottomX;
		float rightBottomY;


		//中心点, 用于目标点在网格之外时 使用最近距离的点进行映射
		float centerPointX;
		float centerPointY;
	
		//包络快速检测
		float boundingBoxLeft;
		float boundingBoxRight;
		float boundingBoxTop;
		float boundingBoxButtom;


		//单应变换矩阵
		float H[3][3];
		void _CalcHomographyMat();//计算单应矩阵
	};

	UnitRect rectMesh[5][5];


};






#endif