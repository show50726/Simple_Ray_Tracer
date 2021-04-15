#include "Foundation.h"

bool IsInShadow(vec3 testPos, vec3 lightPos, vector<Shape*>& shapes, vector<Shape*> exclude)
{
	Ray ray = Ray(testPos, lightPos - testPos);
	vector<Shape*> temp = GetExcludeVector(shapes, exclude);
	HitInfo hitInfo = ray.BroadPhaseDetection(temp);

	return hitInfo.hitObj != NULL;
}

vec3 Reflect(const vec3 inVector, const vec3 normal)
{
	vec3 v = inVector * normal * normal;
	vec3 h = inVector - v;
	v *= -1;

	return (v + h).normalize();
}

vector<Shape*> GetExcludeVector(vector<Shape*>& shapes, vector<Shape*> exclude)
{
	vector<Shape*> temp;

	for (auto shape : shapes)
	{
		if (find(exclude.begin(), exclude.end(), shape) != exclude.end())
			continue;

		temp.push_back(shape);
	}
	return temp;
}

HitInfo Ray::BroadPhaseDetection(vector<Shape*>& shapes)
{
	float minT = 100010.0f;
	Shape* candidate = NULL;
	for (auto shape : shapes)
	{
		bool intersect;
		float t;
		tie(intersect, t) = shape->HasIntersect(*this);

		if (intersect && t < minT)
		{
			candidate = shape;
			minT = t;
		}
	}

	if (candidate)
	{
		vec3 hitPos = startPoint + minT * direction;
		return HitInfo(candidate, hitPos);
	}
	
	return HitInfo(NULL, 0.0f);
}

vec3 Ray::CastRay(vector<Shape*>& shapes, vec3 lightPos, vec3 eyePos, float weight)
{
	HitInfo hitInfo = BroadPhaseDetection(shapes);
	Shape* shape = hitInfo.hitObj;
	vec3 hitPos = hitInfo.hitPos;
	
	if (shape == NULL)
	{
		return vec3(0.0, 0.0, 0.0);
	}

	vec3 hitNormal = shape->GetNormal(hitPos);
	
	if (IsInShadow(hitPos, lightPos, shapes, {shape}))
	{
		return shape->material.color * shape->material.Ka;
	}
	
	vec3 lightDir = (lightPos - hitPos).normalize();
	vec3 viewDir = (hitPos - eyePos).normalize();
	vec3 color = shape->material.CalcColor(hitNormal, lightDir, viewDir);

	vec3 subRayColor = vec3(0.0f, 0.0f, 0.0f);
	if (shape->material.reflectionRadio > 0.0f)
	{
		vec3 reflectDir = Reflect(direction, hitNormal);
		Ray subRay = Ray(hitPos, reflectDir);
		float newWeight = weight * shape->material.reflectionRadio;
		subRayColor += weight * shape->material.reflectionRadio * subRay.CastRay(shapes, lightPos, viewDir, newWeight);
	}

	return color + subRayColor;
}

