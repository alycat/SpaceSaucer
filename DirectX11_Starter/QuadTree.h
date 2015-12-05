#ifndef QUADTREE_H
#define QUADTREE_H
#include "Global.h"
#include "GameEntity.h"
#include <DirectXMath.h>
#include <vector>

using namespace DirectX;

class QuadTree{
public:
	QuadTree();
	QuadTree(BoundingBox bb);
	~QuadTree();
	QuadTree& operator=(const QuadTree &copy);
	void build(QuadTree* tree);
	void insert(GameEntity* object);
	QuadTree* getQuad(GameEntity* ge);
	BoundingBox getBoundingBox();
	void setBoundingBox(BoundingBox boundingBox);
	std::vector<GameEntity*> getObjects();
	float Height();
	float Width();
	XMFLOAT2 Center();
protected:
private:
	QuadTree* upper_left;
	QuadTree* upper_right;
	QuadTree* lower_left;
	QuadTree* lower_right;
	BoundingBox boundingBox;
	QuadTree *getQuad(BoundingBox bb, QuadTree* quad);
	std::vector<GameEntity*> objects;
};
#endif