#include "QuadTree.h"

QuadTree::QuadTree() {
	upper_left = nullptr;
	upper_right = nullptr;
	lower_left = nullptr;
	lower_right = nullptr;
	boundingBox = BoundingBox(XMFLOAT3(0, 0, 0), XMFLOAT3(1000, 1000, 1000));
}

QuadTree::QuadTree(BoundingBox bb) {
	upper_left = nullptr;
	upper_right = nullptr;
	lower_left = nullptr;
	lower_right = nullptr;
	boundingBox = bb;
}

QuadTree::~QuadTree() {
	if (upper_left) {
		delete upper_left;
	}
	upper_left = nullptr;
	if (upper_right) {
		delete upper_right;
	}
	upper_right = nullptr;
	if (lower_left) {
		delete lower_left;
	}
	lower_left = nullptr;
	if (lower_right) {
		delete lower_right;
	}
	lower_right = nullptr;
}

QuadTree& QuadTree::operator=(const QuadTree &copy) {
	this->upper_left = copy.upper_left;
	this->upper_right = copy.upper_right;
	this->lower_left = copy.lower_left;
	this->lower_right = copy.lower_right;
	memcpy(&this->boundingBox, &copy.boundingBox, sizeof(BoundingBox));
	return *this;
}

BoundingBox QuadTree::getBoundingBox() {
	return boundingBox;
}

std::vector<GameEntity*> QuadTree::getObjects() {
	return objects;
}

float QuadTree::Height() {
	return boundingBox.Extents.y;
}

float QuadTree::Width() {
	return boundingBox.Extents.x;
}

XMFLOAT2 QuadTree::Center() {
	return XMFLOAT2(boundingBox.Center.x, boundingBox.Center.y);
}

void QuadTree::setBoundingBox(BoundingBox boundingBox) {
	this->boundingBox = boundingBox;
}

void QuadTree::build(QuadTree *tree){
	if (tree->getObjects().size() > 3) {
		float height = tree->Height();
		float width = tree->Width();
		XMFLOAT2 center = tree->Center();
		tree->upper_left = new QuadTree(BoundingBox(XMFLOAT3(center.x - width / 2, center.y - height / 2, 1), XMFLOAT3(width / 2, height / 2, 1000)));
		tree->upper_right = new QuadTree(BoundingBox(XMFLOAT3(center.x + width / 2, center.y - height / 2, 1), XMFLOAT3(width / 2, height / 2, 1000)));
		tree->lower_left = new QuadTree(BoundingBox(XMFLOAT3(center.x - width / 2, center.y + height / 2, 1), XMFLOAT3(width / 2, height / 2, 1000)));
		tree->lower_right = new QuadTree(BoundingBox(XMFLOAT3(center.x + width / 2, center.y + height / 2, 1), XMFLOAT3(width / 2, height / 2, 1000)));

		for each(GameEntity* obj in tree->getObjects()) {
			if (tree->upper_left->getBoundingBox().Contains(obj->getBoundingBox())) {
				tree->upper_left->insert(obj);
			}
			else if (tree->upper_right->getBoundingBox().Contains(obj->getBoundingBox())) {
				tree->upper_right->insert(obj);
			}
			else if (tree->lower_left->getBoundingBox().Contains(obj->getBoundingBox())) {
				tree->lower_left->insert(obj);
			}
			else {
				tree->lower_right->insert(obj);
			}

			build(tree->upper_left);
			build(tree->upper_right);
			build(tree->lower_left);
			build(tree->lower_right);
		}
	}
}

QuadTree* QuadTree::getQuad(BoundingBox bb, QuadTree* tree) {
	if (tree->getBoundingBox().Contains(bb)) {
		if (tree->upper_left) {
			if (tree->upper_left->getBoundingBox().Contains(bb)) {
				return getQuad(bb, tree->upper_left);
			}
		}
		else if (tree->upper_right) {
			if (tree->upper_right->getBoundingBox().Contains(bb)) {
				return getQuad(bb, tree->upper_right);
			}
		}
		else if (tree->lower_left) {
			if (tree->lower_left->getBoundingBox().Contains(bb)) {
				return getQuad(bb, tree->lower_left);
			}
		}
		else if (tree->lower_right) {
			if (tree->lower_right->getBoundingBox().Contains(bb)) {
				return getQuad(bb, tree->lower_right);
			}
		}
		return tree;
	}
	return nullptr;
}

QuadTree* QuadTree::getQuad(GameEntity* ge) {
	return getQuad(ge->getBoundingBox(), this);
}

void QuadTree::insert(GameEntity *gameEntity) {
	objects.push_back(gameEntity);
}