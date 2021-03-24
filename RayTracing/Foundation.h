#pragma once
#include "algebra3.h"
#include "Shape.h"

class Ray {
public:
	vec3 startPoint;
	vec3 direction;

	Ray(vec3 p, vec3 d)
	{
		startPoint = p;
		direction = d.normalize();
	}

	Shape* BroadPhaseDetection(vector<Shape*>& shapes)
	{
		float minT = 100010;
		Shape* candidate = NULL;
		for (auto shape : shapes)
		{
			bool intersect;
			float t;
			tie(intersect, t) = shape->IsIntersect(*this);

			if (intersect && t < minT)
			{
				candidate = shape;
			}
		}

		return candidate;
	}

	Pixel CastRay(vector<Shape*>& shapes)
	{
		
	}
};

class ViewInfo {
public:
	vec3 eyePos;
	vec3 direction;
	vec3 upVector;
	float fieldOfView;

	vec3 CalcRightVector()
	{
		vec3 v = upVector ^ direction;
		return v.normalize();
	}
};

