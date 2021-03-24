#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <fstream>      
#include "ImageIO.h"
#include "algebra3.h"
#include "Shape.h"
#define PI 3.141592653589793

using namespace std;

void ScanVector(ifstream& file, vec3& targetVector)
{
	file >> targetVector[0] >> targetVector[1] >> targetVector[2];
}

int main()
{
	int W, H;
	ViewInfo view;
	vector<vec3> lightPos;
	vector<Material> materials;
	vector<Shape*> shapes;
	ColorImage image;

	ifstream inputFile;
	char op;
	inputFile.open("input.txt", ios::in);

	if (!inputFile)
	{
		cout << "File not found" << endl;
		return 0;
	}
	
	while (inputFile >> op) {
		switch (op) {
		case 'E':
		{
			ScanVector(inputFile, view.eyePos);
			break;
		}
		case 'V':
		{
			ScanVector(inputFile, view.direction);
			ScanVector(inputFile, view.upVector);
		}
		case 'R':
		{
			inputFile >> W >> H;
			image.init(W, H);
			break;
		}
		case 'S':
		{
			vec3 c;
			float r;
			inputFile >> c[0] >> c[1] >> c[2] >> r;
			Shape* sphere = new Sphere(c, r, &materials.back());
			shapes.push_back(sphere);
			break;
		}
		case 'T':
		{
			vector<vec3> triangleVec(3);
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					inputFile >> triangleVec[i][j];
				}
			}
			Shape* triangle = new Triangle(triangleVec, &materials.back());
			shapes.push_back(triangle);
			break;
		}
		case 'L':
		{
			vec3 light;
			ScanVector(inputFile, light);
			lightPos.push_back(light);
		}
		case 'M':
		{
			Material mat;
			inputFile >> mat.r >> mat.g >> mat.b;
			inputFile >> mat.Ka >> mat.Kd >> mat.Ks;
			inputFile >> mat.specularity >> mat.reflectionRadio;
			materials.push_back(mat);
		}
		default:
			cout << "Contains invalid operation!" << endl;
		}
	}

	vector<vec3> cornerPos(3);
	vec3 rightVec = view.CalcRightVector();
	float halfWidth = tan(view.fieldOfView * PI / 180.0);
	float halfHeight = halfWidth / W * H;
	cornerPos[0] = view.eyePos + view.direction - rightVec * halfWidth + view.upVector * halfHeight;
	cornerPos[1] = view.eyePos + view.direction + rightVec * halfWidth + view.upVector * halfHeight;
	cornerPos[2] = view.eyePos + view.direction - rightVec * halfWidth - view.upVector * halfHeight;


	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++)
		{
			vec3 x = (cornerPos[1] - cornerPos[0]) * (j + 1) / W;
			vec3 y = (cornerPos[2] - cornerPos[0]) * (i + 1) / H;
			vec3 screenPos = cornerPos[0] + x + y;

			vec3 dir = screenPos - view.eyePos;
			Ray ray = Ray(view.eyePos, dir);

			Pixel color = {0, 0, 0};
			Shape* hitShape = ray.BroadPhaseDetection(shapes);

			image.writePixel(j, i, color);
		}
	}

	image.outputPPM("RT.ppm");
	inputFile.close();
}