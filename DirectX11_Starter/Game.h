#pragma once
#ifndef _GAME_H
#define _GAME_H
#include <vector>
#include "ConstantBuffer.h"
#include "ObjectLoader.h"
#include "Mesh.h"
#include "GameEntity.h"
#include "Material.h"
#include "ShaderProgram.h"
#include "SamplerState.h"
#include "Global.h"
#include "Camera.h"
#include "StateManager.h"
#include "FW1FontWrapper.h"
#include "Player.h"
#include <Audio.h>
#include "include/irrKlang.h"
#include <iostream>
#include "Projectile.h"
#include "GameTimer.h"
#include "Asteroid.h"
#include "Collectable.h"
#include "ParticleSystem.h"
#include "healthPickup.h"
#include "Nebula.h"
#include "State.h"
#include "QuadTree.h"
#include "RenderTexture.h"
using namespace DirectX;

#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

class Game{
public:
	Game(ID3D11Device& dev, ID3D11DeviceContext& devCxt, ID3D11DepthStencilView& depthStencilView, ID3D11RenderTargetView* renderTargetView);
	~Game(void);
	State *background;
	void initGame(SamplerState *samplerStates); // sets up the default parameters for the game
	void updateGame(float dt, StateManager *stateManager); // main update method for the game
	void drawGame(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, XMFLOAT3 gamePos, float time, wchar_t* state); // Main drawing method for the game
	void drawText(IFW1FontWrapper *pFontWrapper); // handles text rendering
	void DrawUI(float time, wchar_t* state);
	void reset(); // resets the game to the default state
	void handleCollision(StateManager *stateManager); // handles collisions between the player and an asteroid
	void getHealth(); //for health pickups
	void pickUp(); // for star pickups
	int hullIntegrity; // the current hull integrity (out of 100)
	ID3D11DepthStencilView& depthStencilView;
	ID3D11RenderTargetView* renderTargetView;
	RenderTexture renderTexture;
private:
	QuadTree quadTree;
	LightBufferType lighting;
	//sound engine for the project
	irrklang::ISoundEngine* engine;
	float p_time;
	bool positive;
	float c_time;
	int damageTimer;
	int resetDamage;
	bool isPlayerGreen;
	ID3D11Device& device;
	ID3D11DeviceContext& deviceContext;
	ShaderProgram* shaderProgram;
	State *pause;
	std::vector<SamplerState*>samplerStates;
	std::vector<Material*> materials; // The list of materials utilized by game entities

	Player* player; // the player character/ship
	Mesh* asteroid; // basic mesh used for the majority of in-game entities
	Mesh* playerm;
	Mesh* bulletm;
	Mesh* HPm;
	Mesh* Collm;
	//ObjectLoader *objectLoader;
	ConstantBufferLayout dataToSendToVSConstantBuffer;
	ParticleVertexShaderConstantBufferLayout dataToSendToGSConstantBuffer;
	LightBufferType dataToSendToLightConstantBuffer;
	CameraBufferType dataToSendToCameraConstantBuffer;

	Nebula *nebula;

	std::vector<ConstantBuffer*> constantBufferList;
	ParticleSystem *particle;
	std::vector<ParticleSystem*>stars;

	// The list of managers
	Projectile* projectileManager;
	Asteroid* asteroidManager;
	healthPickup* HPManager;
	Collectable* collManager;
	
	int shootingScore;
	int highScore1 = 0;
	int highScore2 = 0;
	int score;
	std::wstring scorep;
	std::wstring score1;
	std::wstring score2;

	std::vector<Particle*> particles; // Container for all the particles currently in the game
};
#endif