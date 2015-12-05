#include "Game.h"
#include "WICTextureLoader.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "SimpleMath.h"

std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
std::unique_ptr<DirectX::SpriteFont> spriteFont;

Game::Game(ID3D11Device& dev, ID3D11DeviceContext& devCxt, ID3D11DepthStencilView& depthStencilView, ID3D11RenderTargetView* renderTargetView):
	device(dev), deviceContext(devCxt), depthStencilView(depthStencilView), renderTargetView(renderTargetView)
{
	positive = true;
	damageTimer = 0;
	resetDamage = 0;
	isPlayerGreen = true;
}

Game::~Game(void){
}

void Game::initGame(SamplerState *samplerStates){
	renderTexture.Initialize(&device, SCREEN_WIDTH, SCREEN_HEIGHT);
	p_time = 0;
	//sound effect engine
	engine = irrklang::createIrrKlangDevice();

	// Set up lighting parameters
	lighting.ambientColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	lighting.diffuseColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	lighting.lightDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
	lighting.specularColor = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	lighting.specularPower = 5.0f;

	// Set initial hull integrity to full (100%)
	hullIntegrity = 100;
	shootingScore = 0;

	constantBufferList.push_back(new ConstantBuffer(dataToSendToVSConstantBuffer, &device)); //create matrix constant buffer
	constantBufferList.push_back(new ConstantBuffer(dataToSendToLightConstantBuffer, &device));//create light constant buffer
	constantBufferList.push_back(new ConstantBuffer(dataToSendToCameraConstantBuffer, &device)); //create camera constant buffer
	constantBufferList.push_back(new ConstantBuffer(dataToSendToGSConstantBuffer, &device)); //create geometry constant buffer

	//create shader program-Params(vertex shader, pixel shader, device, constant buffers)
	shaderProgram = new ShaderProgram(L"FlatVertexShader.cso", L"FlatPixelShader.cso", &device, constantBufferList);

	ShaderProgram *collectableShader = new ShaderProgram(L"CollectableVertexShader.cso", L"CollectablePixelShader.cso", L"CollectableGeometryShader.cso", &device, constantBufferList);
	ShaderProgram* textureProgram = new ShaderProgram(L"NebulaVertexTexture.cso", L"NebulaPixelTexture.cso", &device, constantBufferList);
	ShaderProgram* geoShader = new ShaderProgram(L"GeometryVertexShader.cso", L"GeometryPixelShader.cso", L"GeometryShader.cso", L"GeometryShaderStreamOutput.cso", &device, constantBufferList);
	ShaderProgram* nebulaShader = new ShaderProgram(L"NebulaVertexTexture.cso", L"NebulaPixelTexture.cso", L"NebulaGeometryShader.cso", &device, constantBufferList);
	ObjectLoader* objectLoader[4];
	for (int i = 0; i < 4; ++i){
		objectLoader[i] = new ObjectLoader(&device);
	}
//	objectLoader = new ObjectLoader(device);
	asteroid = objectLoader[0]->LoadModel("asteroid.obj");
	playerm = objectLoader[1]->LoadModel("pyramid.obj");
	bulletm = objectLoader[2]->LoadModel("energy.obj");
	Mesh* backMesh = objectLoader[3]->LoadModel("fullscreenQuad.obj");
	ID3D11SamplerState* sample = nullptr;
	SamplerState* samplerState = new SamplerState(sample);
	samplerState->createSamplerState(&device);

	ObjectLoader *bgObject = new ObjectLoader(&device);
	ShaderProgram *pauseShaderProgram = new ShaderProgram(L"PostProcessVertexShader.cso", L"PostProcessPixelShader.cso", &device, constantBufferList);
	ShaderProgram* backgroundShaderProgram = new ShaderProgram(L"FlatVertexShader.cso", L"FlatPixelShader.cso", &device, constantBufferList);
	Material *backMat = new Material(&device, &deviceContext, samplerStates->sampler, L"star.png", shaderProgram);
	Material *rendMat = new Material(renderTexture.GetShaderResourceView(), samplerStates->sampler, backgroundShaderProgram);
	Material *pauseMat = new Material(renderTexture.GetShaderResourceView(), samplerStates->sampler, pauseShaderProgram);
	background = new State(&device, &deviceContext, rendMat, backMesh);
	pause = new State(&device, &deviceContext, pauseMat, backMesh);
	HPm = bulletm;
	Vertex2 collectableVertices[] = {
		{ { 0, 0, 1 }, { 0, 0, 1 }, { 0, 0 }, {0,0,0} }
	};

	Particle icosahedronParticle[] = {
		{ { 0, 0, 1, 1 }, { 0, 0, 0, 0 } }
	};
	UINT coll_i[] = { 0 };
	Collm = new Mesh(collectableVertices, coll_i, 1, &device);//collObject->LoadModel("star.obj");
	Mesh* icosahedron = new Mesh(icosahedronParticle, coll_i, 1, &device);
//	ID3D11SamplerState* sample = nullptr;

	// Set up the object loader and load the main menu



	//Create the matierials used by the myriad game entities
	materials.push_back(new Material(&device, &deviceContext, samplerStates->sampler, L"spaceShipTexture.jpg", shaderProgram));
	materials.push_back(new Material(&device, &deviceContext, samplerStates->sampler, L"asteroid.jpg", shaderProgram));
	materials.push_back(new Material(&device, &deviceContext, samplerStates->sampler, L"star.png", shaderProgram));
	materials.push_back(new Material(&device, &deviceContext, samplerStates->sampler, L"energy.png", shaderProgram));
	materials.push_back(new Material(&device, &deviceContext, samplerStates->sampler, L"background.jpg", textureProgram));
	materials.push_back(new Material(&device, &deviceContext, samplerStates->sampler, L"bullet.jpg", shaderProgram));
	materials.push_back(new Material(&device, &deviceContext, samplerStates->sampler, L"particle.png", geoShader));
	materials.push_back(new Material(&device, &deviceContext, samplerStates->sampler, L"perlin.png", nebulaShader));



	for (float i = 0; i < 50; i++){
		stars.push_back(new ParticleSystem(XMFLOAT4(-2.0f, -1.0f, 2.0f, 1.0f), XMFLOAT4(0.0005f, 0.0005f, 0.5f, 0.5f), &device, &deviceContext, materials[6], 1));
	}
	player = new Player(&device, &deviceContext, constantBufferList, samplerStates->sampler, playerm);

	Vertex2 nebVert []= {
		{ { 0, 0, 2 }, { 0, 0, 1 }, { 0, 0 }, { 0, 0, 0 } }

	};

	nebula = new Nebula(nebVert, &device, &deviceContext, materials[7], 1);


	//Set up the two backgrounds
	// Create the managers
	projectileManager = new Projectile(&device, &deviceContext, constantBufferList, samplerStates->sampler, Collm, player);
	asteroidManager = new Asteroid(&device, &deviceContext, constantBufferList, samplerStates->sampler, icosahedron, player, this, &quadTree);
	HPManager = new healthPickup(&device, &deviceContext, constantBufferList, samplerStates->sampler, HPm, player, this);
	Material* collectableMat = new Material(renderTexture.GetShaderResourceView(), samplerStates->sampler, collectableShader);
	collManager = new Collectable(&device, &deviceContext, constantBufferList, samplerStates->sampler, Collm, player, this, collectableMat);

	spriteBatch.reset(new DirectX::SpriteBatch(&deviceContext));
	spriteFont.reset(new DirectX::SpriteFont(&device, L"Font.spritesheet"));
}

// Main update function for the game
void Game::updateGame(float dt, StateManager *stateManager)
{
	quadTree.build(&quadTree);
	float step = 0.02;
	if (damageTimer > 0){
		player->playerColor = XMFLOAT4(min(player->playerColor.x + step, 1), max(player->playerColor.y - step, 0), max(player->playerColor.z - step, 0), player->playerColor.w);
		damageTimer-=step;
	}
	else if(resetDamage > 0){
		player->playerColor = XMFLOAT4(max(player->playerColor.x - step, 0), min(player->playerColor.y + step, 1), min(player->playerColor.z + step, 1), player->playerColor.w);
		resetDamage-=step;
		isPlayerGreen = true;
	}
	// Call the different entity manager's update functions
	player->update(dt);
	projectileManager->update(dt);
	asteroidManager->update(dt, stateManager);
	HPManager->update(dt);
	collManager->update(dt);

	for (int i = 0; i < projectileManager->projectiles.size(); ++i) {
		GameEntity* p = projectileManager->projectiles[i];
		QuadTree* projQuad = quadTree.getQuad(p);
		for each(GameEntity* as in projQuad->getObjects()) {
			if (as->getBoundingBox().Contains(p->getBoundingBox())) {
				int index = find(asteroidManager->asteroids.begin(), asteroidManager->asteroids.end(), as) - asteroidManager->asteroids.begin(); 
				projectileManager->projectiles.erase(projectileManager->projectiles.begin() + i);
				asteroidManager->acceleration[index] = XMFLOAT3(1, 0, 0);
				asteroidManager->time[index] = 1;
			}
		}
	}


	QuadTree *quad = quadTree.getQuad(player->getPlayer());
	for each(GameEntity* ge in quad->getObjects()) {
		if (player->getPlayer()->getBoundingBox().Intersects(ge->getBoundingBox())) {
			int index = find(asteroidManager->asteroids.begin(), asteroidManager->asteroids.end(), ge) - asteroidManager->asteroids.begin();
			if (asteroidManager->time[index] < 1) {
				asteroidManager->acceleration[index] = XMFLOAT3(1, 0, 0);
				asteroidManager->time[index] = 1;
				handleCollision(stateManager);
			}
		}
	}
}

// Handles collisions between the player and an asteroid
void Game::handleCollision(StateManager *stateManager)
{
	if (damageTimer <= 0 && isPlayerGreen){
		damageTimer = resetDamage = 100;
		isPlayerGreen = false;
	}
	
	// Drop the hull integrity by 10% due to the collision
	hullIntegrity -= 10;

	// start the sound engine with default parameters
	engine->play2D("Explosion.wav", false);

	//Trigger a game loss
	if (hullIntegrity <= 0)
	{
		// State 5 is the "game loss" state that triggers the game over screen to show
		damageTimer = resetDamage = 0;
		isPlayerGreen = true;
		player->playerColor = XMFLOAT4(0, 1, 1, 1);
		stateManager->setState(5);
		reset();
	}
}

// Handles collisions between the player and an HPUp
void Game::getHealth()
{
	// Raise the hull intergerty by 30 due to pick up
	hullIntegrity += 30;

	// start the sound engine with default parameters
	engine->play2D("energy.wav", false);

	//MAke sure health doesn't exceed 100
	if (hullIntegrity >= 100)
	{
		hullIntegrity = 100;
	}
}

void Game::pickUp()
{

	shootingScore += 500;
	engine->play2D("Laser_Shot.mp3", false);

}


// Method where all the actual drawing occurs
void Game::drawGame(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, XMFLOAT3 camPos, float time, wchar_t* state)
{
	float clear[4] = { 0, 0, 0, 0 };
	deviceContext.ClearRenderTargetView(renderTargetView, clear);
	deviceContext.ClearDepthStencilView(
		&depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);


	renderTexture.SetRenderTarget(&deviceContext, &depthStencilView);
	renderTexture.ClearRenderTarget(&deviceContext, &depthStencilView, 0.0, 0.0, 0.0, 0.0);

	c_time = time;
	UINT offset = 0;
	UINT stride = sizeof(Vertex);
	float max = 40;
	float age = positive ? time - p_time : max - (time - p_time);
	if (abs(age) > max || age <= 0)
	{
		positive = !positive;
		p_time = time;
	}
	

	nebula->draw(viewMatrix, projectionMatrix, time/2);

	for (int i = 0; i < stars.size(); i++){
		stars[i]->drawParticleSystem(viewMatrix, projectionMatrix, (cos(time/2) + 1)*3 + (i * 0.5));
	}
	
	// Call the corresponding draw methods for the different entity managers
	
	player->draw(viewMatrix, projectionMatrix, camPos, age*0.025);
	asteroidManager->draw(viewMatrix, projectionMatrix, camPos, age);

	HPManager->draw(viewMatrix, projectionMatrix, camPos);
	projectileManager->draw(viewMatrix, projectionMatrix, camPos, age);
	DrawUI(time, state);
	(&deviceContext)->OMSetRenderTargets(1, &renderTargetView, &depthStencilView);
	background->draw(viewMatrix, projectionMatrix);

	collManager->draw(viewMatrix, projectionMatrix, camPos);

	if (state == L"Pause") {
	//	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
		pause->draw(viewMatrix, projectionMatrix);
	}
}

void Game::DrawUI(float time, wchar_t* state)
{

	std::wstring pi = std::to_wstring(hullIntegrity);
	const WCHAR* szName = pi.c_str();

	score = int(time / 2) + shootingScore;
	scorep = std::to_wstring(score);
	const WCHAR* szScore = scorep.c_str();

	//hs1
	score1 = std::to_wstring(highScore1);
	const WCHAR* szScore1 = score1.c_str();

	//hs2
	score2 = std::to_wstring(highScore2);
	const WCHAR* szScore2 = score2.c_str();
	
	spriteBatch->Begin();
	if (state == L"Game")
	{
		//Draw Sprites and fonts
		spriteFont->DrawString(spriteBatch.get(), L"Health: ", DirectX::SimpleMath::Vector2(15, 25));
		spriteFont->DrawString(spriteBatch.get(), szName, DirectX::SimpleMath::Vector2(160, 25), Colors::LawnGreen);

		spriteFont->DrawString(spriteBatch.get(), L"Score: ", DirectX::SimpleMath::Vector2(15, 55));
		spriteFont->DrawString(spriteBatch.get(), szScore, DirectX::SimpleMath::Vector2(144, 55), Colors::LawnGreen);

		if (hullIntegrity <= 30)
		{
			spriteFont->DrawString(spriteBatch.get(), szName, DirectX::SimpleMath::Vector2(160, 25), Colors::Red);
		}
	}

		if (state == L"Pause")
		{
		/*	spriteFont->DrawString(spriteBatch.get(), L"Health: ", DirectX::SimpleMath::Vector2(15, 25));
			spriteFont->DrawString(spriteBatch.get(), szName, DirectX::SimpleMath::Vector2(160, 25));

			spriteFont->DrawString(spriteBatch.get(), L"Score: ", DirectX::SimpleMath::Vector2(15, 55));
			spriteFont->DrawString(spriteBatch.get(), szScore, DirectX::SimpleMath::Vector2(144, 55));

			spriteFont->DrawString(spriteBatch.get(), L"Pause", DirectX::SimpleMath::Vector2(800 - 150, 25));

			spriteFont->DrawString(spriteBatch.get(), L"High Scores:", DirectX::SimpleMath::Vector2(300, 25));
			
			if (highScore1 >= highScore2 && highScore2 >= score)
			{
				spriteFont->DrawString(spriteBatch.get(), szScore1, DirectX::SimpleMath::Vector2(400, 50));
				spriteFont->DrawString(spriteBatch.get(), szScore2, DirectX::SimpleMath::Vector2(400, 75));
				spriteFont->DrawString(spriteBatch.get(), szScore, DirectX::SimpleMath::Vector2(400, 100), Colors::Gold);
			}

			if (highScore2 >= highScore1 && highScore1 >= score)
			{
				spriteFont->DrawString(spriteBatch.get(), szScore2, DirectX::SimpleMath::Vector2(400, 50));
				spriteFont->DrawString(spriteBatch.get(), szScore1, DirectX::SimpleMath::Vector2(400, 75));
				spriteFont->DrawString(spriteBatch.get(), szScore, DirectX::SimpleMath::Vector2(400, 100), Colors::Gold);
			}

			if (highScore1 >= score && score >= highScore2)
			{
				spriteFont->DrawString(spriteBatch.get(), szScore1, DirectX::SimpleMath::Vector2(400, 50));
				spriteFont->DrawString(spriteBatch.get(), szScore, DirectX::SimpleMath::Vector2(400, 75), Colors::Gold);
				spriteFont->DrawString(spriteBatch.get(), szScore2, DirectX::SimpleMath::Vector2(400, 100));
			}

			if (highScore2 >= score && score >= highScore1)
			{
				spriteFont->DrawString(spriteBatch.get(), szScore2, DirectX::SimpleMath::Vector2(400, 50));
				spriteFont->DrawString(spriteBatch.get(), szScore, DirectX::SimpleMath::Vector2(400, 75), Colors::Gold);
				spriteFont->DrawString(spriteBatch.get(), szScore1, DirectX::SimpleMath::Vector2(400, 100));
			}

			if (score >= highScore2 && highScore2 >= highScore1)
			{
				spriteFont->DrawString(spriteBatch.get(), szScore, DirectX::SimpleMath::Vector2(400, 50), Colors::Gold);
				spriteFont->DrawString(spriteBatch.get(), szScore2, DirectX::SimpleMath::Vector2(400, 75));
				spriteFont->DrawString(spriteBatch.get(), szScore1, DirectX::SimpleMath::Vector2(400, 100));
			}

			if (score >= highScore1 && highScore1 >= highScore2)
			{
				spriteFont->DrawString(spriteBatch.get(), szScore, DirectX::SimpleMath::Vector2(400, 50), Colors::Gold);
				spriteFont->DrawString(spriteBatch.get(), szScore1, DirectX::SimpleMath::Vector2(400, 75));
				spriteFont->DrawString(spriteBatch.get(), szScore2, DirectX::SimpleMath::Vector2(400, 100));
			}*/

		}
	spriteBatch->End();
	
}

//resets the game after lose condition
void Game::reset()
{
	if (highScore2 > score)
		highScore1 = score;
	if (highScore2 < score)
		highScore2 = score;

	// reset hull integrity to full
	hullIntegrity = 100;
	shootingScore = 0;

	//reset player 
	player->reset();

	//reset asteroids to a random area off the right side of the screen
	for (int i = 0; i < NUM_ASTEROIDS; i++)
	{
		asteroidManager->asteroids[i]->setPosition(XMFLOAT3(((rand() % 60) + 30), ((rand() % 40) - 19.0f), 0.0f));
	}

	//reset HP to a random area off the right side of the screen
	for (int i = 0; i < 1; i++)
	{
		HPManager->HPUp[i]->setPosition(XMFLOAT3(((rand() % 60) + 30), ((rand() % 40) - 19.0f), 0.0f));
	}

	//reset HP to a random area off the right side of the screen
	for (int i = 0; i < 1; i++)
	{
		collManager->collectables[i]->setPosition(XMFLOAT3(((rand() % 60) + 30), ((rand() % 40) - 19.0f), 0.0f));
	}

	//reset projectiles
	if (projectileManager->projectiles.size() > 0)
	{
		for (int x = projectileManager->projectiles.size() - 1; x >= 0; x--)
		{
			projectileManager->projectiles.erase(projectileManager->projectiles.begin() + x);
		}
	}
}

