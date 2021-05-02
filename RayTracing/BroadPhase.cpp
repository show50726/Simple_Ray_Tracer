#include "BroadPhase.h"

HitInfo NSquareBroadPhase::BroadPhaseDetection(Ray ray, vector<Shape*> shapes) {
	float minT = 100010.0f;
	Shape* candidate = NULL;
	for (auto shape : shapes)
	{
		bool hit;
		float t;
		tie(hit, t) = shape->HasIntersect(ray);

		if (hit && t < minT)
		{
			candidate = shape;
			minT = t;
		}
	}

	if (candidate)
	{
		vec3 hitPos = ray.startPoint + minT * ray.direction;
		return HitInfo(candidate, hitPos);
	}

	return HitInfo(NULL, 0.0f);
}

HitInfo SpatialHashBroadPhase::BroadPhaseDetection(Ray ray, vector<Shape*> shapes) {
	vector<GridNode*> gridNodes = _grid->FindClosestIntersectionNode(ray);
	Shape* candidate = NULL;
	float minT = 100000010;

	for (auto gridNode : gridNodes) {
		GridNode* curNode = gridNode;

		while (curNode != NULL) {
			bool hit;
			float t;
			tie(hit, t) = curNode->shape->HasIntersect(ray);

			if (hit && t < minT && find(shapes.begin(), shapes.end(), curNode->shape) != shapes.end()) {
				candidate = curNode->shape;
				minT = t;;
			}

			curNode = curNode->next;
		}
	}

	if (candidate) {
		vec3 hitPos = ray.startPoint + minT * ray.direction;
		return HitInfo(candidate, hitPos);
	}

	return HitInfo(NULL, 0.0f);
}

void GridHash::Insert(Shape* shape) {
	for (int i = 0; i < _xSize; i++) {
		for (int j = 0; j < _ySize; j++) {
			for (int k = 0; k < _zSize; k++) {
				auto box = _Grid[i][j][k];
				if (shape->HasIntersectionWithBox(box)) {
					cout << i << " " << j << " " << k << endl;
					auto newNode = new GridNode(shape);

					if (_nodeList[i][j][k] == NULL) {
						_nodeList[i][j][k] = newNode;
					}
					else {
						int l = 1;
						auto cur = _nodeList[i][j][k];
						while (cur->next != NULL) {
							l++;
							cur = cur->next;
						}
						cout << "length: " << l << endl;
						cur->next = newNode;
					}
				}
			}
		}
	}
}

vector<GridNode*> GridHash::FindClosestIntersectionNode(Ray& ray) {
	vector<GridNode*> list;
	for (int i = 0; i < _xSize; i++) {
		for (int j = 0; j < _ySize; j++) {
			for (int k = 0; k < _zSize; k++) {
				auto box = _Grid[i][j][k];
				bool hit;
				float t;
				tie(hit, t) = box->HasIntersect(ray);
				
				if (_nodeList[i][j][k] != NULL) {
					list.push_back(_nodeList[i][j][k]);
				}
			}
		}
	}
	return list;
}

void GridHash::_InitCells(vec2 xBound, vec2 yBound, vec2 zBound) {
	float xl = abs(xBound[1] - xBound[0]) / _xSize;
	float yl = abs(yBound[1] - yBound[0]) / _ySize;
	float zl = abs(zBound[1] - zBound[0]) / _zSize;

	for (int i = 0; i < _xSize; i++) {
		for (int j = 0; j < _ySize; j++) {
			for (int k = 0; k < _zSize; k++) {
				_Grid[i][j][k] = new BoundingBox(xBound[0]+xl*i, xBound[0]+xl*(i + 1), yBound[0]+yl*j, yBound[0]+yl*(j + 1), zBound[0]+zl*k, zBound[0]+zl*(k + 1));
			}
		}
	}
}
