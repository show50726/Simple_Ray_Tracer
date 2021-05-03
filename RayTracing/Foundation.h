#pragma once
#include <vector>
#include <thread>
#include "algebra3.h"
#include "Shape.h"
#include "BroadPhase.h"

using namespace std;

class Shape;
class BroadPhase;

bool IsInShadow(vec3 testPos, vec3 lightPos, vector<Shape*>& shapes, vector<Shape*> exclude);
vec3 Reflect(const vec3 inVector, const vec3 normal);
vector<Shape*> GetExcludeVector(vector<Shape*>& shapes, vector<Shape*> exclude);

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

	Ray(vec3 p, vec3 d, BroadPhase* broadPhase = NULL);

	vec3 CastRay(vector<Shape*>& shapes, vec3 lightPos, vec3 eyePos, float weight);
	HitInfo BroadPhaseDetection(vector<Shape*>& shapes);

private:
	BroadPhase * _broadPhase;
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

inline void waitThreads(vector<thread> &threads) {
	for (int i = 0; i < threads.size(); i++) {
		threads[i].join();
	}
}



