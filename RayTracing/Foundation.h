#pragma once
#include <vector>
#include "algebra3.h"
#include "Shape.h"

using namespace std;

class Shape;

bool IsInShadow(vec3 testPos, vec3 lightPos, vector<Shape*>& shapes, vector<Shape*> exclude);
vec3 Reflect(const vec3 inVector, const vec3 normal);

struct HitInfo {
	Shape * hitObj;
	vec3 hitPos;

	HitInfo(Shape* obj, vec3 pos)
	{
		hitObj = obj;
		hitPos = pos;
	}
};

class Ray {
public:
	vec3 startPoint;
	vec3 direction;

	Ray(vec3 p, vec3 d)
	{
		startPoint = p;
		direction = d.normalize();
	}

	vec3 CastRay(vector<Shape*>& shapes, vec3 lightPos, vec3 eyePos);
	HitInfo BroadPhaseDetection(vector<Shape*>& shapes);
};

struct ViewInfo {
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



