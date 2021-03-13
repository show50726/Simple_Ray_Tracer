#pragma once
#include "algebra3.h"

class Ray {
public:
	vec3 startPoint;
	vec3 direction;

	Ray(vec3 p, vec3 d)
	{
		startPoint = p;
		direction = d.normalize();
	}
};
