#pragma once

class Material {
public:
	unsigned char r, g, b;
	double Ka, Kd, Ks;
	double specularity;
	double reflectionRadio;
};
