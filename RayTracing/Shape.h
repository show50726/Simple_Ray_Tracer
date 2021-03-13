#pragma once
#include <vector>
#include "algebra3.h"
#include "Foundation.h"

using namespace std;

class Shape {
public:
	bool virtual IsIntersect(Ray ray)=0;
};

class Sphere : public Shape {
public:
	vec3 center;
	float radius;

	Sphere(vec3 c, float r)
	{
		center = c;
		radius = r;
	}

	bool IsIntersect(Ray ray) override 
	{
		float a = ray.direction.length2();
		float b = 2.0f * (ray.startPoint * ray.direction - center * ray.direction);
		float c = (ray.startPoint - center).length2() - radius * radius;

		float delta = b * b - 4.0f * a * c;

		if (delta < 0)
			return false;

		float sd = sqrt(delta);
		float r1 = (-b + sd) / (2 * a);
		float r2 = (-b - sd) / (2 * a);


		return (r1 >= 0 || r2 >= 0);
	}
};

class Triangle : public Shape {
public:
	vector<vec3> vertices;
	
	Triangle(vector<vec3>& vert)
	{
		vertices = vert;
	}

	bool IsIntersect(Ray ray) override 
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
			return false;

		if (x[1] < 0 || x[1] > 1) 
			return false;

		if (x[2] > 0 && x[1] + x[0] <= 1)
			return true;

		return false;
	}
};