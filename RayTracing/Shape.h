#pragma once
#include <vector>
#include <algorithm>
#include <tuple>
#include "algebra3.h"
#include "Foundation.h"
#include "Materials.h"

using namespace std;

class Shape {
public:
	Material* material;
	tuple<bool, float> virtual IsIntersect(Ray ray) = 0;
	vec3 virtual GetNormal(vec3 position) = 0;
};

class Sphere : public Shape {
public:
	vec3 center;
	float radius;

	Sphere(vec3 c, float r, Material* mat = NULL)
	{
		center = c;
		radius = r;
	}

	tuple<bool, float> IsIntersect(Ray ray) override
	{
		float a = ray.direction.length2();
		float b = 2.0f * (ray.startPoint * ray.direction - center * ray.direction);
		float c = (ray.startPoint - center).length2() - radius * radius;

		float delta = b * b - 4.0f * a * c;

		if (delta < 0)
			return make_tuple(false, 0);

		float sd = sqrt(delta);
		float r1 = (-b + sd) / (2 * a);
		float r2 = (-b - sd) / (2 * a);

		if (r1 < 0 && r2 < 0)
			return make_tuple(false, 0);

		if (r1 < 0)
			return make_tuple(true, r2);
	
		if (r2 < 0)
			return make_tuple(true, r1);

		return make_tuple(true, min(r1, r2));
	}

	vec3 GetNormal(vec3 position) override
	{
		vec3 dir = position - center;
		return dir.normalize();
	}
};

class Triangle : public Shape {
public:
	vector<vec3> vertices;

	Triangle(vector<vec3>& vert, Material* mat = NULL)
	{
		vertices = vert;
		_normal = _CalcNormal();
	}

	tuple<bool, float> IsIntersect(Ray ray) override
	{
		vec3 d1 = vertices[1] - vertices[0];
		vec3 d2 = vertices[2] - vertices[0];

		mat3 A;
		A[0][0] = d1[0];
		A[0][1] = d2[0];
		A[0][2] = -ray.direction[0];

		A[1][0] = d1[1];
		A[1][1] = d2[1];
		A[1][2] = -ray.direction[1];

		A[2][0] = d1[2];
		A[2][1] = d2[2];
		A[2][2] = -ray.direction[2];

		vec3 b;
		b[0] = ray.startPoint[0] - vertices[0][0];
		b[1] = ray.startPoint[1] - vertices[0][1];
		b[2] = ray.startPoint[2] - vertices[0][2];

		vec3 x = A.inverse() * b;

		if (x[0] < 0 || x[0] > 1)
			return make_tuple(false, 0);

		if (x[1] < 0 || x[1] > 1)
			return make_tuple(false, 0);

		if (x[2] > 0 && x[1] + x[0] <= 1)
			return make_tuple(true, x[2]);

		return make_tuple(false, 0);
	}

	vec3 GetNormal(vec3 position) override
	{
		return _normal;
	}

private:
	vec3 _normal;

	vec3 _CalcNormal()
	{
		vec3 d1 = vertices[1] - vertices[0];
		vec3 d2 = vertices[2] - vertices[0];
		vec3 dir = d1 ^ d2;
		return dir.normalize();
	}
};