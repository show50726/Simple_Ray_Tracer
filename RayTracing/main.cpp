#include <iostream>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <thread>
#include <vector>
#include <fstream>      
#include "ImageIO.h"
#include "algebra3.h"
#include "Shape.h"
#include "BroadPhase.h"
#define PI 3.141592653589793

using namespace std;
using namespace chrono;

void ScanVector(ifstream& file, vec3& targetVector)
{
	file >> targetVector[0] >> targetVector[1] >> targetVector[2];
}

Pixel Vec2Pixel(vec3 color)
{
	color[0] = max(color[0], 0.0f);
	color[1] = max(color[1], 0.0f);
	color[2] = max(color[2], 0.0f);

	unsigned char r = static_cast<unsigned char>(255.999f * color[0]);
	unsigned char g = static_cast<unsigned char>(255.999f * color[1]);
	unsigned char b = static_cast<unsigned char>(255.999f * color[2]);

	return Pixel(r, g, b);
}

void UpdateBound(vec2& xBound, vec2& yBound, vec2& zBound, Shape* shape) {
	xBound[0] = min(xBound[0], shape->boundingBox.xMin);
	xBound[1] = max(xBound[1], shape->boundingBox.xMax);

	yBound[0] = min(yBound[0], shape->boundingBox.yMin);
	yBound[1] = max(yBound[1], shape->boundingBox.yMax);

	zBound[0] = min(zBound[0], shape->boundingBox.zMin);
	zBound[1] = max(zBound[1], shape->boundingBox.zMax);
}

int main()
{
	int W, H;
	ViewInfo view;
	vector<vec3> lightPos;
	vector<Material> materials;
	vector<Shape*> shapes;
	ColorImage image;
	vec2 xBound(1000000.0f, -1000000.0f), yBound(1000000.0f, -1000000.0f), zBound(1000000.0f, -1000000.0f);

	ifstream inputFile;
	char op;
	inputFile.open("Input_Suzanne.txt", ios::in);

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
			break;
		}
		case 'F':
		{
			inputFile >> view.fieldOfView;
			break;
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
			Shape* sphere = new Sphere(c, r, materials.back());
			UpdateBound(xBound, yBound, zBound, sphere);

			shapes.push_back(sphere);
			break;
		}
		case 'T':
		{
			vector<vec3> triangleVec(3);
			vec3 normal;
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					inputFile >> triangleVec[i][j];
				}
			}
			ScanVector(inputFile,normal);

			Shape* triangle = new Triangle(triangleVec, materials.back(), normal);
			UpdateBound(xBound, yBound, zBound, triangle);
			shapes.push_back(triangle);
			break;
		}
		case 'L':
		{
			vec3 light;
			ScanVector(inputFile, light);
			lightPos.push_back(light);
			cout << light[0] << endl;
			break;
		}
		case 'M':
		{
			Material mat;
			ScanVector(inputFile, mat.color);
			inputFile >> mat.Ka >> mat.Kd >> mat.Ks;
			inputFile >> mat.specularity >> mat.reflectionRadio;
			materials.push_back(mat);
			break;
		}
		default:
			cout << "Contains invalid operation!" << endl;
		}
	}
	
	auto t0 = high_resolution_clock::now();

	vector<vec3> cornerPos(3);
	vec3 rightVec = view.CalcRightVector();
	float halfWidth = tan(view.fieldOfView * PI / 180.0);
	float halfHeight = halfWidth / W * H;
	cornerPos[0] = view.eyePos + view.direction - rightVec * halfWidth + view.upVector * halfHeight;
	cornerPos[1] = view.eyePos + view.direction + rightVec * halfWidth + view.upVector * halfHeight;
	cornerPos[2] = view.eyePos + view.direction - rightVec * halfWidth - view.upVector * halfHeight;
	
	
	xBound[0] -= 1;
	xBound[1] += 1;
	yBound[0] -= 1;
	yBound[1] += 1;
	zBound[0] -= 1;
	zBound[1] += 1;
	
	BroadPhase* broadPhase = new SpatialHashBroadPhase(shapes, 3, 3, 3, xBound, yBound, zBound);
	// BroadPhase* broadPhase = new NSquareBroadPhase();

	auto t1 = high_resolution_clock::now();
	cout << "done hash construction in " << duration<double>(high_resolution_clock::now() - t0).count() << " s" << endl;
	//system("pause");

	vector<std::thread> threads;
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++)
		{
			threads.push_back(std::thread([&cornerPos, i, j, H, W, &view, &broadPhase, &shapes, &lightPos, &image] {
				vec3 x = (cornerPos[1] - cornerPos[0]) * (j + 1) / W;
				vec3 y = (cornerPos[2] - cornerPos[0]) * (i + 1) / H;
				vec3 screenPos = cornerPos[0] + x + y;

				vec3 dir = screenPos - view.eyePos;
				Ray ray = Ray(view.eyePos, dir, broadPhase);
				vec3 vecColor = ray.CastRay(shapes, lightPos[0], view.eyePos, 1.0f);
				Pixel color = Vec2Pixel(vecColor);

				image.writePixel(j, i, color);

			}));
			
		}
	}

	waitThreads(threads);

	image.outputPPM("RT.ppm");

	cout << "done ray tracing in " << duration<double>(high_resolution_clock::now() - t1).count() << " s" << endl;

	inputFile.close();

	system("pause");
}