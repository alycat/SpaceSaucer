#ifndef EXPLOSIONEFFECT_H
#define EXPLOSIONEFFECT_H

#include "Global.h"
#include "Material.h"
#include "DirectXGame.h"
#include "GameEntity.h"
#include <d3d11.h>

class ExplosionEffect{
public:
	ExplosionEffect(XMFLOAT4 position, XMFLOAT4 speed, ID3D11Device* dev, ID3D11DeviceContext* devCtx, Material* mat);
	~ExplosionEffect();
	void draw(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, float time);
	void setPosition(XMFLOAT3 position);
	void translate(XMFLOAT3 translation);
	void rotate(XMFLOAT3 rotation);
protected:
private:
	ID3D11Device * device;
	ID3D11DeviceContext* deviceContext;
	GameEntity *object;
	void drawParticles(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, float time);
	void drawExplosion(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, float time);
	bool firstPass;
};

#endif