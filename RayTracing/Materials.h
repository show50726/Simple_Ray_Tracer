#pragma once
#include <iostream>
#include <algorithm>
#include "algebra3.h"

using namespace std;

struct Material {
public:
	vec3 color;
	float Ka, Kd, Ks;
	float specularity;
	float reflectionRadio;

	vec3 CalcColor(vec3 normal, vec3 lightDir, vec3 viewDir) const;

private:
	vec3 _CalcDiffuse(vec3 normal, vec3 lightDir) const;
	vec3 _CalcSpecular(vec3 normal, vec3 lightDir, vec3 viewDir) const;
};
