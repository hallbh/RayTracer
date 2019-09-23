#ifndef _BOUNDING_BOX_H
#define _BOUNDING_BOX_H

#include "Ray.h"
#include "rayHit.h"

class alignas(32) BoundingBox
{
public:
    float minMax[6];

    BoundingBox() = default;
    BoundingBox(Vec3 min, Vec3 max) 
		//: min(min), max(max) 
	{
		for (int i = 0; i < 3; i++)
		{
			minMax[i] = min[i];
			minMax[i+3] = max[i];
		}
	}
	BoundingBox(float vals[6])
	{
		for (int i = 0; i < 6; i++)
		{
			minMax[i] = vals[i];
		}
	}

    // bool hit(Ray ray, float startTime, rayHit &info)
	// {
	// 	return hit(this->minMax, ray, startTime, info);
	// }
	// void hit(rayBundle rays, float startTime, hitBundle &info)
	// {
	// 	hit(this->minMax, rays, startTime, info);
	// }
	static bool hit(const float minMax[6], Ray ray, float startTime, rayHit &info);
	static void hit(const float minMax[6], rayBundle ray, float startTime, hitBundle &record, bool *mask);
};

bool BoundingBox::hit(const float minMax[6], Ray ray, float startTime, rayHit &record)
{
    //we want to find the farthest entrace and closest exit to the box
	//if the exit is closer than the entrance, there is no hit
	float entrance = startTime;
	float exit = record.intersectionTime;
	Vec3 normal = Vec3(0);
	Vec3 inverseDir = ray.getInverseDirection();
	Vec3 org = ray.positionAtTime(0);
	
	for(int i=0; i<3; i++)
	{
		float slabA = minMax[i];
		float slabB = minMax[i+3];
		float invDir = inverseDir[i];
		float origin = org[i];
		
		float closestHit = (slabA - origin) * invDir;
		float farthestHit = (slabB - origin) * invDir;
		
		if(farthestHit < closestHit)
			std::swap(closestHit, farthestHit);
		
		bool foundNewEntrance = closestHit > entrance;
		entrance = foundNewEntrance ? closestHit : entrance;
		
		bool foundNewExit = farthestHit < exit;
		exit = foundNewExit ? farthestHit : exit;
		
		if (exit < entrance)
			return false;
		
#ifdef RENDER_LEAF_BBOX
		if(foundNewEntrance)
		{
            normal = Vec3(0);
			normal[i] = inverseDir[i] < 0 ? 1.0f : -1.0f;
		}
#endif
	}

#ifdef RENDER_LEAF_BBOX
    record.materialID = "";
    record.surfaceNormal = normal;
    record.intersectionTime = entrance;
    record.intersectionPoint = ray.positionAtTime(entrance);
#endif
    return true;
}

void BoundingBox::hit(const float minMax[6], rayBundle rays, float startTime, hitBundle &records, bool *mask)
{
    //we want to find the farthest entrace and closest exit to the box
	//if the exit is closer than the entrance, there is no hit
	
	// This should be 4, but someone is messing up and reading past the end of the array somehow
	// Upping size to 8 prevents stack smashing/segfaults
	Vec3 normals[8];

	float *entrances, *exits, *fdirs, *forgs;
	float temp[20];

	for (int i = 0; i < 4; i++)
	{
		if ((uintptr_t)(temp + i) % (uintptr_t)16UL == 0)
		{
			entrances = temp + i;
			exits = temp + i + 4;
			fdirs = temp + i + 8;
			forgs = temp + i + 12;
			break;
		}
	}

	for (int i = 0; i < 4; i++)
	{
		entrances[i] = startTime;
		exits[i] = records[i].intersectionTime;
		Vec3 dir = rays[i].getInverseDirection();
		Vec3 origin = rays[i].positionAtTime(0);
		for (int j = 0; j < 3; j++)
		{
			fdirs[i*4 + j] = dir[j];
			forgs[i*4 + j] = origin[j];
		}
	}
	
#pragma GCC ivdep
	for (int j = 0; j < 4; j++)
	{
		for(int i=0; i<3; i++)
		{
			float slabA = minMax[i];
			float slabB = minMax[i + 3];
			float invDir = fdirs[j*4 + i];
			float origin = forgs[j*4 + i];
			
			float closestHit = (slabA - origin) * invDir;
			float farthestHit = (slabB - origin) * invDir;
			
			bool swapped = (farthestHit < closestHit);
			if (swapped)
				std::swap(closestHit, farthestHit);
			
			bool foundNewEntrance = closestHit > entrances[j];
			entrances[j] = foundNewEntrance ? closestHit : entrances[j];
			
			bool foundNewExit = farthestHit < exits[j];
			exits[j] = foundNewExit ? farthestHit : exits[j];

			// mask[j] &= !(exits[j] < entrances[j]);
			
#ifdef RENDER_LEAF_BBOX
			if(foundNewEntrance)
			{
				normals[j] = Vec3(0);
				normals[j][i] = fdirs[j*4 + i] < 0 ? 1.0f : -1.0f;
			}
#endif
		}
	}

	for (int i = 0; i < 4; i++)
	{
		mask[i] &= !(exits[i] < entrances[i]);
	}

#ifdef RENDER_LEAF_BBOX
	for (int i = 0; i < 4; i++)
	{
		if (!mask[i])
			continue;
		records[i].materialID = "";
		records[i].surfaceNormal = normals[i];
		records[i].intersectionTime = entrances[i];
		records[i].intersectionPoint = rays[i].positionAtTime(entrances[i]);
	}
#endif 
}

#endif
