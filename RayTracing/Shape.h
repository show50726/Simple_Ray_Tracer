#pragma once
#include <vector>
#include <algorithm>
#include <tuple>
#include <assert.h>
#include "algebra3.h"
#include "Foundation.h"
#include "Materials.h"

using namespace std;

class Ray;

class Shape {
public:
	Material material;
	tuple<bool, float> virtual HasIntersect(Ray ray) = 0;
	vec3 virtual GetNormal(vec3 position) = 0;
};

class Sphere : public Shape {
public:
	vec3 center;
	float radius;

	Sphere(vec3 c, float r, Material mat)
	{
		center = c;
		radius = r;
		material = mat;
	}

	tuple<bool, float> HasIntersect(Ray ray) override;

	vec3 GetNormal(vec3 position) override
	{
		vec3 dir = position - center;
		return dir.normalize();
	}
};

class Triangle : public Shape {
public:
	vector<vec3> vertices;

	Triangle(vector<vec3>& vert, Material mat, vec3 normal)
	{
		vertices = vert;
		_normal = normal;
		material = mat;
	}

	tuple<bool, float> HasIntersect(Ray ray) override;
	static void ReorderToCounterClockWise(vector<vec3>& vert, vec3 eyePos);

	vec3 GetNormal(vec3 position) override
	{
		return _normal;
	}

private:
	vec3 _normal;
};