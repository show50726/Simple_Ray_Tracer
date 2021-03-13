#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <fstream>      
#include "ImageIO.h"
#include "algebra3.h"
#include "Shape.h"

using namespace std;

int main()
{
	vec3 eyePos;
	vector<vec3> cornerPos(4);
	int W, H;
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
			inputFile >> eyePos[0] >> eyePos[1] >> eyePos[2];
			break;
		}
		case 'O':
		{
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 3; j++) {
					inputFile >> cornerPos[i][j];
				}
			}
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
			Shape* sphere = new Sphere(c, r);
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
			Shape* triangle = new Triangle(triangleVec);
			shapes.push_back(triangle);
			break;
		}
		default:
			cout << "Contains invalid operation!" << endl;
		}
	}

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++)
		{
			vec3 x = (cornerPos[1] - cornerPos[0]) * (j + 1) / W;
			vec3 y = (cornerPos[2] - cornerPos[0]) * (i + 1) / H;
			vec3 screenPos = cornerPos[0] + x + y;

			vec3 dir = screenPos - eyePos;
			Ray ray = Ray(eyePos, dir);

			Pixel color = {0, 0, 0};
			for (auto shape : shapes)
			{
				if (shape->IsIntersect(ray))
				{
					color = { 255, 255, 255 };
					break;
				}
			}

			image.writePixel(j, i, color);
		}
	}

	image.outputPPM("RT.ppm");
	inputFile.close();
}