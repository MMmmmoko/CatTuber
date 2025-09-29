#include<SDL3/SDL.h>
#include"Item/StraightMovingPoint.h"

void StraightMovingPoint::Update(float dt_second)
{
	timer += dt_second;

	if (timer > totalTime)
	{
		currentX = targetX;
		currentY = targetY;
		currentZ = targetZ;

		return;
	}


	if (isFixTime)
	{
		switch (moveType)
		{
		case StraightMovingPoint::Fixspeed:
		{
			currentX = startX + vx * timer;
			currentY = startY + vy * timer;
			currentZ = startZ + vz * timer;
			break;
		}
		case StraightMovingPoint::Accelerate:
		{
			//这种情景下vORa标识 加速度
			float tem = 0.5f * timer * timer;
			currentX = startX + vx * tem;
			currentY = startY + vy * tem;
			currentZ = startZ + vz * tem;
			break;
		}
		case StraightMovingPoint::Decelerates:
		{
			//currentX=startX+0.5*vx*titalTime^2 - 0.5*vx*(totalTime-timer)^2
			float tem = (totalTime+ totalTime-timer)*0.5f*timer;
			currentX = startX + vx * tem;
			currentY = startY + vy * tem;
			currentZ = startZ + vz * tem;
			break;
		} 
		case StraightMovingPoint::AccelerateThenDecelerates:
		{
			//先加速后减速
			//v=-at(T-t)求积  v速度 a常数
			//s(路程)=a*(-t^3/3+(T*t^2)/2) +C    (c:常数)
			//t=T时 s=a*T^3/6+C; t=0时s=C;
			//dS=a*T^3/6

			
			//VX=a

			float tem = timer * timer * (totalTime * 0.5f - timer * 0.333333333f);
			currentX = startX + vx * tem;
			currentY = startX + vy * tem;
			currentZ = startX + vz * tem;


			break;
		}
		default:
			break;
		}
	}
	else
	{
		//设置的固定速度
		currentX = startX + vx * timer;
		currentY = startY + vy * timer;
		currentZ = startZ + vz * timer;
	}




}

void StraightMovingPoint::SetStartPointAndStartMove(float x, float y, float z)
{
	startX = x;
	startY = y;
	startZ = z;

	float dx = targetX - x;
	float dy = targetY - y;
	float dz = targetZ - z;
	if (!isFixTime)
	{
		//如果设置的是固定速度
		totalTime = SDL_sqrtf(dx*dx+dy*dy+dz*dz)/v;
		vx = dx / totalTime;
		vy = dy / totalTime;
		vz = dz / totalTime;
		return;
	}

	switch (moveType)
	{
	case StraightMovingPoint::Fixspeed:
	{
		vx = dx / totalTime;
		vy = dy / totalTime;
		vz = dz / totalTime;

		break;
	}
	case StraightMovingPoint::Accelerate:
	case StraightMovingPoint::Decelerates:
	{
		float tem = 2.f / (totalTime * totalTime);
		vx = dx * tem;
		vy = dy * tem;
		vz = dz * tem;
		break;
	}
	case StraightMovingPoint::AccelerateThenDecelerates:
	{
		float tem=6.f/ (totalTime * totalTime* totalTime);

		vx = dx * tem;
		vy = dy * tem;
		vz = dz * tem;
		break;
	}
	default:
		break;
	}

}

void StraightMovingPoint::SetFixSpeed(float _v)
{
	isFixTime = false;
	v = _v;
}

void StraightMovingPoint::SetFixDuration(float duration, MoveTypeWithFixTime type)
{
	isFixTime = true;
	totalTime = duration;
	moveType = type;
}
