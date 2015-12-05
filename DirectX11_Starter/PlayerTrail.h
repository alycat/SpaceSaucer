#ifndef PLAYERTRAIL_H
#define PLAYERTRAIL_H

#include "Global.h"
#include "GameEntity.h"
#include "Material.h"
#include "ConstantBuffer.h"
#include "Mesh.h"
#include <Windows.h>
#include <d3d11.h>
#include <vector>

class PlayerTrail{
public:
	PlayerTrail(XMFLOAT3 position, ID3D11Device* dev, ID3D11DeviceContext* devCtx, Material* mat);
	~PlayerTrail();
	void draw(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, float time, XMFLOAT3 position, XMFLOAT4 acceleration);
	GameEntity *object;
private:
protected:
	int age;
	ID3D11Device * device;
	ID3D11DeviceContext* deviceContext;
};
#endif