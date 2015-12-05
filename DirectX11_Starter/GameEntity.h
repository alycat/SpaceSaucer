#ifndef _GAMEENTITY_H
#define _GAMEENTITY_H

#include "Mesh.h"
#include "Material.h"
#include "ConstantBuffer.h"


class GameEntity{
public:
	GameEntity(Mesh* mesh, Material* mat, float width = 1, float height = 1);
	~GameEntity(void);
	void clearTransforms(void);
	XMFLOAT4X4 getWorld(void);
	XMFLOAT4X4 getRotation(void);
	XMFLOAT4X4 getPosition(void);
	XMFLOAT3 getPositionVector(void);
    XMFLOAT4X4 getScale(void);
	BoundingBox getBoundingBox();
	void setScale(XMFLOAT4X4 scale);
	void scale(XMFLOAT3 scale);
	void translate(XMFLOAT3 translate);
	void rotate(XMFLOAT3 rotate);
	void setPosition(XMFLOAT3 pos);

	Mesh* g_mesh;
	Material* g_mat;
protected:
	XMFLOAT4X4 worldMatrix;
	XMFLOAT4X4 rotationMatrix;
	XMFLOAT4X4 positionMatrix;
	XMFLOAT4X4 scaleMatrix;
	float m_height;
	float m_width;
};
#endif