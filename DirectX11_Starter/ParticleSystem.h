#ifndef _PARTICLESYSTEM_H
#define _PARTICLESTSTEM_H

#include "Global.h"
#include "GameEntity.h"
#include "Material.h"
#include "ConstantBuffer.h"
#include "Mesh.h"
#include <Windows.h>
#include <d3d11.h>
#include <vector>

class ParticleSystem{
public:
	ParticleSystem(XMFLOAT4 position, XMFLOAT4 speed, ID3D11Device* dev, ID3D11DeviceContext* devCtx, Material* mat, int num_particles = 20);
	void drawParticleSystem(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, float time);
private:
	void drawSteamoutput(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, float time);
	void drawParticles(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, float time);
	ID3D11Device * device;
	ID3D11DeviceContext* deviceContext;
	std::vector<GameEntity*> particles;
	GameEntity* object;
	bool initialized;
	bool firstPass;
	bool secondPass;
	int numParticles;
};
#endif