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
					//cout << i << " " << j << " " << k << endl;

					auto newNode = new GridNode(shape);

					if (_nodeList[i][j][k] == NULL) {
						_nodeList[i][j][k] = newNode;
					}
					else {
						auto cur = _nodeList[i][j][k];
						while (cur->next != NULL) {
							cur = cur->next;
						}

						cur->next = newNode;
					}
				}
			}
		}
	}
}

vector<GridNode*> GridHash::FindClosestIntersectionNode(Ray& ray) {
	vector<GridNode*> list;

	// Find closest point if the start point of the ray is not inside the grid
	Ray curRay = ray;

	BoundingBox world = BoundingBox(_xBound[0], _xBound[1], _yBound[0], _yBound[1], _zBound[0], _zBound[1]);
	tuple<int, int, int> id;

	if (get<0>(id = FindClosestNode(curRay.startPoint)) == -1) {
		bool hit; float t;
		tie(hit, t) = world.HasIntersect(curRay);

		if (!hit)
			return {};

		t += 0.001f;
		curRay.startPoint = curRay.startPoint + t * curRay.direction;
	}

	while (get<0>(id = FindClosestNode(curRay.startPoint)) != -1) {
		int x, y, z;
		tie(x, y, z) = id;

		if(_nodeList[x][y][z] != NULL)
			list.push_back(_nodeList[x][y][z]);

		BoundingBox* boundingBox = _Grid[x][y][z];
		bool hit; float t;
		tie(hit, t) = boundingBox->HasIntersect(curRay);
		
		assert(hit == true);

		t += 0.05f;
		vec3 newPos = curRay.startPoint + t * curRay.direction;
		curRay.startPoint = newPos;
	}

	/* O(n^3) method
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
	*/

	return list;
}

void GridHash::_InitCells(vec2 xBound, vec2 yBound, vec2 zBound) {
	float xl = abs(xBound[1] - xBound[0]) / _xSize;
	float yl = abs(yBound[1] - yBound[0]) / _ySize;
	float zl = abs(zBound[1] - zBound[0]) / _zSize;

	_xLength = xl;
	_yLength = yl;
	_zLength = zl;

	for (int i = 0; i < _xSize; i++) {
		for (int j = 0; j < _ySize; j++) {
			for (int k = 0; k < _zSize; k++) {
				_Grid[i][j][k] = new BoundingBox(xBound[0]+xl*i, xBound[0]+xl*(i + 1), yBound[0]+yl*j, yBound[0]+yl*(j + 1), zBound[0]+zl*k, zBound[0]+zl*(k + 1));
			}
		}
	}
}

tuple<int, int, int> GridHash::FindClosestNode(vec3& pos) {
	int xIndex = (pos[0] - _xBound[0]) / _xLength;
	int yIndex = (pos[1] - _yBound[0]) / _yLength;
	int zIndex = (pos[2] - _zBound[0]) / _zLength;

	if (xIndex >= 0 && yIndex >= 0 && zIndex >= 0 && xIndex < _xSize && yIndex < _ySize && zIndex < _zSize)
		return make_tuple(xIndex, yIndex, zIndex);

	return make_tuple(-1, -1, -1);
}
