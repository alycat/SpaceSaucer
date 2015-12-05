#include "Collectable.h"
#include "Game.h"
#include "RenderTexture.h"

//Constructor for Collectable object
Collectable::Collectable(ID3D11Device* dev, ID3D11DeviceContext* devCtx, vector<ConstantBuffer*> constantBufferList, ID3D11SamplerState* samplerState, Mesh* meshReference, Player* playerReference, Game* gameReferencePassed, Material* material){
	// set up the lighting parameters
	lighting.ambientColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	lighting.diffuseColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	lighting.lightDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
	lighting.specularColor = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	lighting.specularPower = 5.0f;

	device = dev;
	deviceContext = devCtx;
	sampler = samplerState;

	shaderProgram = new ShaderProgram(L"CollectableVertexShader.cso", L"CollectablePixelShader.cso", L"CollectableGeometryShader.cso", device, constantBufferList);
	ShaderProgram* sparkleShader = new ShaderProgram(L"ExplosionVertexShader.cso", L"SparklePixelShader.cso", L"SparkleGeometryShader.cso", L"ExplosionEmitter.cso", L"IcosahedronVertexShader.cso", device, constantBufferList);
	sparkleMaterial = new Material(device, deviceContext, sampler, L"particle.png", sparkleShader);
	//collectableMaterial = new Material(device, deviceContext, sampler, L"bullet.jpg", shaderProgram);
	
	collectableMaterial = material;
	//collectableMaterial = new Material(gameReference->renderTexture.GetShaderResourceView(), sampler, shaderProgram);
	player = playerReference;
	mesh = meshReference;

	gameReference = gameReferencePassed;

	// populate the list of collectables, scale them to one tenth the size of the mesh and randomize their position (off the right side of the screen)
	for (int i = 0; i < 1; i++)
	{
		collectables.push_back(new GameEntity(meshReference, collectableMaterial));
		collectables[i]->scale(XMFLOAT3(0.1f, 0.1f, 0.1f));
		collectables[i]->setPosition(XMFLOAT3(((rand() % 60) + SCREEN_WIDTH/26), ((rand() % 40) - 19.0f), 0.0f));
	}

	collectables[0]->setPosition(XMFLOAT3(0, 5, 0));
}

Collectable::~Collectable(){
	ReleaseMacro(device);
	ReleaseMacro(deviceContext);
	if (sampler){
		delete sampler;
		sampler = nullptr;
	}
	if (collectableMaterial){
		delete collectableMaterial;
		collectableMaterial = nullptr;
	}
	if (shaderProgram){
		delete shaderProgram;
		shaderProgram = nullptr;
	}
}


//Update collectables positions each frame
void Collectable::update(float dt){

	//make bounding boxes
	BoundingBox *playerbb = new BoundingBox(XMFLOAT3(player->player->getPosition()._41, player->player->getPosition()._42, player->player->getPosition()._43),
		XMFLOAT3(2.0f, 2.0f, 0.0f));


	//moves collectables across screen (right to left) and respawns them when they leave the screen
	for (unsigned int i = 0; i < 1; i++){
		collectables[i]->translate(XMFLOAT3(-8.0f * dt, 0.0f, 0.0f));


		//._41 is the x value for the position matrix of game entities
		if (collectables[i]->getPosition()._41 < -SCREEN_WIDTH / 26)
			collectables[i]->setPosition(XMFLOAT3(SCREEN_WIDTH/26, (rand() % 40) - 19.0f, 0.0f));}

	// Tests for collisions between the collectables and the player
	for (int i = 0; i < 1; i++){
		BoundingBox *collectablebb = new BoundingBox(XMFLOAT3(collectables[i]->getPosition()._41, collectables[i]->getPosition()._42, collectables[i]->getPosition()._43),
			XMFLOAT3(2.0f, 2.0f, 0.0f));
		//check for intersections
		if (collectablebb->Intersects(*playerbb))
		{
				XMFLOAT4X4 matrix = collectables[i]->getPosition();
				XMFLOAT4 pos = XMFLOAT4(matrix._41*1.15, matrix._42*1.15, matrix._43, 1);
				collectables[i]->setPosition(XMFLOAT3(SCREEN_WIDTH / 26, (rand() % 40) - 19.0f, 0.0f));
				gameReference->pickUp();
				sparkles.push_back(new ExplosionEffect(pos, XMFLOAT4(0, 0, 0, 0), device, deviceContext, sparkleMaterial));
				sparkleTimer.push_back(3);

				//elimate spawning on each other
				for (int g = 0; g < 1; g++)
				{
					BoundingBox *collectablebb2 = new BoundingBox(XMFLOAT3(collectables[g]->getPosition()._41, collectables[g]->getPosition()._42, collectables[g]->getPosition()._43),
						XMFLOAT3(2.0f, 2.0f, 0.0f));
					if (collectablebb2->Intersects(*collectablebb))
					{
						
						collectables[i]->setPosition(XMFLOAT3(30.0f, (rand() % 40) - 19.0f, 0.0f));
					}
				}

		}

		break;
	}
}



//draw collectables
void Collectable::draw(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, XMFLOAT3 camPos){
	UINT offset = 0;
	UINT stride = sizeof(Vertex);

	for (int s = 0; s < sparkles.size(); ++s) {
		if (sparkleTimer[s] > 0) {
			sparkles[s]->draw(viewMatrix, projectionMatrix, sparkleTimer[s]);
			sparkleTimer[s] -= 0.2;
		}
	}

	for (unsigned int i = 0; i < collectables.size(); i++){

		//UINT offset = 0;
		stride = collectables[i]->g_mesh->sizeofvertex;
		// Set up the input assembler
		deviceContext->IASetInputLayout(collectables[i]->g_mat->shaderProgram->vsInputLayout);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		//set values that get passed to matrix constant buffer

		deviceContext->GSSetShader(collectables[i]->g_mat->shaderProgram->geometryShader, NULL, 0);
		collectables[i]->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer.world = collectables[i]->getWorld();
		collectables[i]->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer.view = viewMatrix;
		collectables[i]->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer.projection = projectionMatrix;

		
		//set values that get passed to lighting constant buffer
		collectables[i]->g_mat->shaderProgram->ConstantBuffers[1]->dataToSendToLightBuffer.ambientColor = lighting.ambientColor;
		collectables[i]->g_mat->shaderProgram->ConstantBuffers[1]->dataToSendToLightBuffer.diffuseColor = lighting.diffuseColor;
		collectables[i]->g_mat->shaderProgram->ConstantBuffers[1]->dataToSendToLightBuffer.lightDirection = lighting.lightDirection;
		collectables[i]->g_mat->shaderProgram->ConstantBuffers[1]->dataToSendToLightBuffer.specularColor = lighting.specularColor;
		collectables[i]->g_mat->shaderProgram->ConstantBuffers[1]->dataToSendToLightBuffer.specularPower = lighting.specularPower;
		//set values that get passed to camera constant buffer
		collectables[i]->g_mat->shaderProgram->ConstantBuffers[2]->dataToSendToCameraBuffer.cameraPosition = XMFLOAT3(SCREEN_WIDTH, SCREEN_HEIGHT, 0);
		collectables[i]->g_mat->shaderProgram->ConstantBuffers[2]->dataToSendToCameraBuffer.padding = 1.0f;

		//matrix constant buffer
		deviceContext->UpdateSubresource(
			collectables[i]->g_mat->shaderProgram->ConstantBuffers[0]->constantBuffer,
			0,
			NULL,
			&collectables[i]->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer,
			0,
			0);

		deviceContext->UpdateSubresource(
			collectables[i]->g_mat->shaderProgram->ConstantBuffers[2]->constantBuffer,
			0,
			NULL,
			&collectables[i]->g_mat->shaderProgram->ConstantBuffers[2]->dataToSendToCameraBuffer,
			0,
			0);
	
		deviceContext->IASetVertexBuffers(0, 1, &collectables[i]->g_mesh->v_buffer, &stride, &offset);
		deviceContext->IASetIndexBuffer(collectables[i]->g_mesh->i_buffer, DXGI_FORMAT_R32_UINT, 0);

		deviceContext->PSSetSamplers(0, 1, &collectables[i]->g_mat->samplerState);
		deviceContext->PSSetShaderResources(0, 1, &collectables[i]->g_mat->resourceView);

		deviceContext->GSSetConstantBuffers(0, 1, &collectables[i]->g_mat->shaderProgram->ConstantBuffers[0]->constantBuffer);
		// Set the current vertex and pixel shaders, as well the constant buffer for the vert shader
		deviceContext->VSSetShader(collectables[i]->g_mat->shaderProgram->vertexShader, NULL, 0);
		deviceContext->VSSetConstantBuffers(0, 1, &collectables[i]->g_mat->shaderProgram->ConstantBuffers[0]->constantBuffer); //set first constant vertex buffer-matrix
		
		deviceContext->PSSetShader(collectables[i]->g_mat->shaderProgram->pixelShader, NULL, 0);
		deviceContext->PSSetConstantBuffers(0, 1, &collectables[i]->g_mat->shaderProgram->ConstantBuffers[1]->constantBuffer); //set pixel constant buffer-light
		deviceContext->PSSetConstantBuffers(1, 1, &collectables[i]->g_mat->shaderProgram->ConstantBuffers[2]->constantBuffer); //set second constant vertex buffer-camera

		// Finally do the actual drawing
		deviceContext->DrawIndexed(
			collectables.at(i)->g_mesh->m_size,	// The number of indices we're using in this draw
			0,
			0);


		deviceContext->GSSetShader(NULL, NULL, 0);
	}
}
