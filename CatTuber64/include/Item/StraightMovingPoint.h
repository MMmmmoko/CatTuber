
#ifndef _StraightMovingPoint_h
#define _StraightMovingPoint_h

//直线运动点,通过设置点的起始位置和目标点， 点会按一定规则在一段时间内向目标点运动

#include <cstdint>

class StraightMovingPoint
{
public:
	void Update(float dt_second);
	void SetTarget(float x, float y, float z) { targetX = x; targetY = y; targetZ=z; };
	//设置起点后即开始运动
	void SetStartPointAndStartMove(float x,float y,float z);

	float GetX() { return currentX; };
	float GetY() { return currentY; };
	float GetZ() { return currentZ; };

	//设置为固定速度运行
	void SetFixSpeed(float v);

	enum MoveTypeWithFixTime
	{
		Fixspeed,
		Accelerate,
		Decelerates,
		AccelerateThenDecelerates,

	};
	//设置为固定完成时间运行
	void SetFixDuration(float duration,MoveTypeWithFixTime type);

private:
	float targetX;
	float targetY;
	float targetZ;

	float currentX;
	float currentY;
	float currentZ;

	float startX;
	float startY;
	float startZ;

	float v = 1.f;
	float vx;
	float vy;
	float vz;

	float timer = 0.f;
	float totalTime;//动作完成的总时间（duration）
	MoveTypeWithFixTime moveType;
	bool isFixTime=false;
};






#endif