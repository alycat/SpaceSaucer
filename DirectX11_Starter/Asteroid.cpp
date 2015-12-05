#include "Asteroid.h"
#include "Game.h"

//Constructor for Asteroid object
Asteroid::Asteroid(ID3D11Device* dev, ID3D11DeviceContext* devCtx, vector<ConstantBuffer*> constantBufferList, ID3D11SamplerState* samplerState, Mesh* meshReference, Player* playerReference, Game* gameReferencePassed, QuadTree* quadTree){

	//set up the lighting parameters
	lighting.ambientColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	lighting.diffuseColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	lighting.lightDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
	lighting.specularColor = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	lighting.specularPower = 2.0f;

	device = dev;
	deviceContext = devCtx;
	sampler = samplerState;
	ShaderProgram* explosionShader = new ShaderProgram(L"ExplosionVertexShader.cso", L"ExplosionPixelShader.cso", L"SquareShader.cso", L"ExplosionEmitter.cso", L"IcosahedronVertexShader.cso", device, constantBufferList);
	explosionMaterial = new Material(device, deviceContext, sampler, L"explosion.png", explosionShader);
	shaderProgram = new ShaderProgram(L"AsteroidVertexShader.cso", L"AsteroidPixelShader.cso", L"AsteroidGeometryShader.cso", L"IcosahedronEmitter.cso", L"IcosahedronVertexShader.cso", device, constantBufferList);
	asteroidMaterial = new Material(device, deviceContext, sampler, L"asteroid.jpg", L"rock.jpg", L"alpha_map.png", shaderProgram);
	player = playerReference;
	mesh = meshReference;
	firstPass = true;
	gameReference = gameReferencePassed;

	m_quadTree = *quadTree;
		quadTree->build(quadTree);
	// populate the list of asteroids, scale them to one tenth the size of the mesh and randomize their position (off the right side of the screen)
	for (int i = 0; i < NUM_ASTEROIDS; i++)
	{
		XMFLOAT3 position = XMFLOAT3(((rand() % 60) + SCREEN_WIDTH/26), ((rand() % 40) - 19.0f), 0.0f);
		asteroids.push_back(new GameEntity(meshReference, asteroidMaterial, 2, 2));
		asteroids[i]->scale(XMFLOAT3(0.1f, 0.1f, 0.1f));
		asteroids[i]->setPosition(position);
		quadTree->insert(asteroids[i]);
		acceleration.push_back(XMFLOAT3(0, 0, 0));
		time.push_back(0);
		exploding.push_back(false);
	}
}

Asteroid::~Asteroid(){
	ReleaseMacro(device);
	ReleaseMacro(deviceContext);
	for (std::vector<GameEntity*>::iterator it = asteroids.begin(); it != asteroids.end(); ++it){
		if (*it){
			delete *it;
		}
	}
	asteroids.clear();
	if (sampler){
		delete sampler;
		sampler = nullptr;
	}
	if (asteroidMaterial){
		delete asteroidMaterial;
		asteroidMaterial = nullptr;
	}
	if (shaderProgram){
		delete shaderProgram;
		shaderProgram = nullptr;
	}
}


//Update asteroid positions each frame
void Asteroid::update(float dt, StateManager *stateManager){
	
	//make bounding boxes
	BoundingBox *playerbb = new BoundingBox(XMFLOAT3(player->player->getPosition()._41, player->player->getPosition()._42, player->player->getPosition()._43),
		XMFLOAT3(2.8f, 1.0f, 0.0f));

	// double boolean system used to ensure the same collision isn't registered multiple times.
	collision = L"Not Colliding";
	notColliding = false;
	
	//moves asteroids across screen (right to left) and respawns them when they leave the screen
	for (unsigned int i = 0; i < NUM_ASTEROIDS; i++)
	{
		BoundingBox *asteriodbb = new BoundingBox(XMFLOAT3(asteroids[i]->getPosition()._41, asteroids[i]->getPosition()._42, asteroids[i]->getPosition()._43),
			XMFLOAT3(2.8f, 1.0f, 2.0f));
		asteroids[i]->translate(XMFLOAT3(-8.0f * dt, 0.0f, 0.0f));
		//asteroids[i]->rotate(XMFLOAT3(0.0f, 0.0f, 0.0f));
		if (time[i] >= 1 && !exploding[i]) {
			XMFLOAT4X4 posMat = asteroids[i]->getPosition();
			XMFLOAT4 pos = XMFLOAT4(posMat._41, posMat._42, posMat._43, 1);
			explosionEffect.push_back(new ExplosionEffect(pos, XMFLOAT4(0, 0, 0, 0), device, deviceContext, explosionMaterial));
			explosionTimer.push_back(0);
			exploding[i] = true;
		}
		time[i] = time[i] > 0 ?  time[i]+=2 : 0;

		if (time[i] >= 100){
			time[i] = 0;
			exploding[i] = false;
			asteroids[i]->translate(XMFLOAT3(40.0f, (rand() % 40) - 19.0f, 0.0f));
		}


		//._41 is the x value for the position matrix of game entities
		if (asteroids[i]->getPosition()._41 < -SCREEN_WIDTH/26)
		{
			asteroids[i]->setPosition(XMFLOAT3(SCREEN_WIDTH / 26, (rand() % 40) - 19.0f, 0.0f));
			//helps elimate overlap
			QuadTree quad = *m_quadTree.getQuad(asteroids[i]);
			if (quad.getObjects().size() > 1)
			{
				asteroids[i]->setPosition(XMFLOAT3(SCREEN_WIDTH / 26, (rand() % 40) - 19.0f, 0.0f));
			}

/*			for (int g = 0; g < NUM_ASTEROIDS; g++)
			{
				BoundingBox *asteriodbb2 = new BoundingBox(XMFLOAT3(asteroids[g]->getPosition()._41, asteroids[g]->getPosition()._42, asteroids[g]->getPosition()._43),
					XMFLOAT3(4.0f, 4.0f, 0.0f));
				if (asteriodbb2->Intersects(*asteriodbb))
				{
					asteroids[i]->setPosition(XMFLOAT3(SCREEN_WIDTH / 26, (rand() % 40) - 19.0f, 0.0f));
				}
			}*/
		}
	}

	// Tests for collisions between the asteroids and the player, tells the game to handle the collision (for non-asteroid consequences) if one is found
/*	for (int i = 0; i < NUM_ASTEROIDS; i++)
	{
		BoundingBox *asteriodbb = new BoundingBox(XMFLOAT3(asteroids[i]->getPosition()._41, asteroids[i]->getPosition()._42, asteroids[i]->getPosition()._43),
			XMFLOAT3(2.6f, 1.0f, 0.0f));
		//check for intersections
		/*if (asteriodbb->Intersects(*playerbb))
		{
			notColliding = true;
			if (canTakeDamage && notColliding)
			{
				//asteroids[i]->setPosition(XMFLOAT3(30.0f, (rand() % 40) - 19.0f, 0.0f));
				canTakeDamage = false;
				gameReference->handleCollision(stateManager);

				//elimate spawning on each other
				for (int g = 0; g < NUM_ASTEROIDS; g++)
				{
					BoundingBox *asteriodbb2 = new BoundingBox(XMFLOAT3(asteroids[g]->getPosition()._41, asteroids[g]->getPosition()._42, asteroids[g]->getPosition()._43),
						XMFLOAT3(4.0f, 4.0f, 0.0f));
					if (asteriodbb2->Intersects(*asteriodbb))
					{
						//asteroids[i]->setPosition(XMFLOAT3(30.0f, (rand() % 40) - 19.0f, 0.0f));
						time[i] = 1;
						XMFLOAT4X4 posMat = asteroids[g]->getPosition();
						XMFLOAT4 pos = XMFLOAT4(posMat._41, posMat._42, posMat._43, 1);
						explosionEffect.push_back(new ExplosionEffect(pos, XMFLOAT4(0, 0, 0, 0), device, deviceContext, explosionMaterial));
						explosionTimer.push_back(0);
					}
				}

			}

			collision = L"Colliding";
			break;
		}
	}

	if (!notColliding){
		canTakeDamage = true;
	}*/

}



//draw asteroids
void Asteroid::draw(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, XMFLOAT3 camPos, float time){
	for (int e = 0; e < explosionEffect.size(); ++e){
		if (explosionTimer[e] < 2){
			explosionEffect[e]->draw(viewMatrix, projectionMatrix, explosionTimer[e]);
			explosionTimer[e] += 0.025;
		}
		else{
			//to do remove explosion effect
		}
		
	}
	UINT offset = 0;
	UINT stride = sizeof(Vertex2);

	for (unsigned int i = 0; i < NUM_ASTEROIDS; i++){
		//UINT offset = 0;
		stride = asteroids[i]->g_mesh->sizeofvertex;
		// Set up the input assembler
		ID3D11BlendState* blendState = nullptr;
		D3D11_BLEND_DESC blendDesc;
		blendDesc.AlphaToCoverageEnable = 0;
		blendDesc.IndependentBlendEnable = 0;
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		HRESULT hr_blend = device->CreateBlendState(&blendDesc, &blendState);

		deviceContext->OMSetBlendState(blendState, NULL, 0xffffffff);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		if (firstPass){
			this->drawIcosahedron(i);
		}
		this->drawAsteroids(viewMatrix, projectionMatrix, i, time);
	}
}

void Asteroid::drawIcosahedron(int index){
	UINT offset[] = { 0 };
	UINT *so_offset = 0;
	UINT stride = asteroids[index]->g_mesh->sizeofvertex;
	ID3D11Buffer* bufferArray[1] = { 0 };

	deviceContext->IASetInputLayout(asteroids[index]->g_mat->shaderProgram->soVSInputLayout);
	deviceContext->IASetVertexBuffers(0, 1, &asteroids[index]->g_mesh->init_buffer, &stride, offset);


	deviceContext->SOSetTargets(1, &asteroids[index]->g_mesh->so_buffer, 0);
	deviceContext->GSSetShader(asteroids[index]->g_mat->shaderProgram->streamOutputShader, NULL, 0);


	deviceContext->VSSetShader(asteroids[index]->g_mat->shaderProgram->soVertexShader, NULL, 0);

	ID3D11DepthStencilState* depthState = nullptr;
	deviceContext->OMGetDepthStencilState(&depthState, 0);
	deviceContext->OMSetDepthStencilState(NULL, 0);

	deviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->PSSetShader(NULL, NULL, 0);



	if (firstPass){
		deviceContext->Draw(1, 0);
		firstPass = false;
		deviceContext->SOSetTargets(1, bufferArray, 0);
		std::swap(asteroids[index]->g_mesh->v_buffer, asteroids[index]->g_mesh->so_buffer);
	}

	deviceContext->GSSetShader(NULL, NULL, 0);


	deviceContext->OMSetDepthStencilState(depthState, 0);
}


void Asteroid::drawAsteroids(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, int index, float time){
	UINT offset[] = { 0 };
	UINT *so_offset = 0;
	UINT stride = asteroids[index]->g_mesh->sizeofvertex;
	ID3D11Buffer* bufferArray[1] = { 0 };

	deviceContext->IASetInputLayout(asteroids[index]->g_mat->shaderProgram->vsInputLayout);
	deviceContext->IASetVertexBuffers(0, 1, &asteroids[index]->g_mesh->v_buffer, &stride, offset);


	deviceContext->SOSetTargets(1, bufferArray, 0);

	deviceContext->VSSetShader(asteroids[index]->g_mat->shaderProgram->vertexShader, NULL, 0);
	deviceContext->IASetIndexBuffer(asteroids[index]->g_mesh->i_buffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->PSSetSamplers(0, 1, &asteroids[index]->g_mat->samplerState);
	deviceContext->PSSetShaderResources(0, 1, &asteroids[index]->g_mat->resourceView);
	deviceContext->PSSetShaderResources(1, 1, &asteroids[index]->g_mat->resourceView2);
	deviceContext->PSSetShaderResources(2, 1, &asteroids[index]->g_mat->resourceView3);

	deviceContext->PSSetShader(asteroids[index]->g_mat->shaderProgram->pixelShader, NULL, 0);
	deviceContext->IASetVertexBuffers(0, 1, &asteroids[index]->g_mesh->v_buffer, &stride, offset);

	deviceContext->GSSetShader(asteroids[index]->g_mat->shaderProgram->geometryShader, NULL, 0);
	asteroids[index]->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer.world = asteroids[index]->getWorld();
	asteroids[index]->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer.view = viewMatrix;
	asteroids[index]->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer.projection = projectionMatrix;
	asteroids[index]->g_mat->shaderProgram->ConstantBuffers[3]->dataToSendToGSBuffer.age = this->time[index];
	asteroids[index]->g_mat->shaderProgram->ConstantBuffers[3]->dataToSendToGSBuffer.position = acceleration[index];

	asteroids[index]->g_mat->shaderProgram->ConstantBuffers[1]->dataToSendToLightBuffer.ambientColor = XMFLOAT4(0.75, 0.5, 0.75, 1);
	asteroids[index]->g_mat->shaderProgram->ConstantBuffers[1]->dataToSendToLightBuffer.diffuseColor = XMFLOAT4(0.75, 0.5, 0.75, 1);//XMFLOAT4(0.54, 0.25, 0.27, 1);// XMFLOAT4(0.125, 0.125, 0.125, 1);
	asteroids[index]->g_mat->shaderProgram->ConstantBuffers[1]->dataToSendToLightBuffer.highlightColor = XMFLOAT4(1, 1, 1, 1);
	asteroids[index]->g_mat->shaderProgram->ConstantBuffers[1]->dataToSendToLightBuffer.lightDirection = XMFLOAT3(0, 0, 1);
	asteroids[index]->g_mat->shaderProgram->ConstantBuffers[1]->dataToSendToLightBuffer.specularColor = XMFLOAT4(0, 1, 1, 1);
	asteroids[index]->g_mat->shaderProgram->ConstantBuffers[1]->dataToSendToLightBuffer.specularPower = 1;

	deviceContext->UpdateSubresource(
		asteroids[index]->g_mat->shaderProgram->ConstantBuffers[0]->constantBuffer,
		0,
		NULL,
		&asteroids[index]->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer,
		0,
		0);

	deviceContext->UpdateSubresource(
		asteroids[index]->g_mat->shaderProgram->ConstantBuffers[3]->constantBuffer,
		0,
		NULL,
		&asteroids[index]->g_mat->shaderProgram->ConstantBuffers[3]->dataToSendToGSBuffer,
		0,
		0);

	deviceContext->UpdateSubresource(
		asteroids[index]->g_mat->shaderProgram->ConstantBuffers[1]->constantBuffer,
		0,
		NULL,
		&asteroids[index]->g_mat->shaderProgram->ConstantBuffers[1]->dataToSendToLightBuffer,
		0,
		0);


	deviceContext->GSSetConstantBuffers(0, 1, &asteroids[index]->g_mat->shaderProgram->ConstantBuffers[0]->constantBuffer);
	deviceContext->VSSetConstantBuffers(0, 1, &asteroids[index]->g_mat->shaderProgram->ConstantBuffers[3]->constantBuffer);
	deviceContext->PSSetConstantBuffers(0, 1, &asteroids[index]->g_mat->shaderProgram->ConstantBuffers[1]->constantBuffer);

	deviceContext->DrawAuto();
	deviceContext->GSSetShader(NULL, NULL, 0);
}