#include "Projectile.h"

//Constructor for Projectile object
Projectile::Projectile(ID3D11Device* dev, ID3D11DeviceContext* devCtx, vector<ConstantBuffer*> constantBufferList, ID3D11SamplerState* samplerState, Mesh* meshReference, Player* playerReference){
	// set up the lighting
	lighting.ambientColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	lighting.diffuseColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	lighting.lightDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
	lighting.specularColor = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	lighting.specularPower = 5.0f;

	device = dev;
	deviceContext = devCtx;
	sampler = samplerState;
	shaderProgram = new ShaderProgram(L"BulletVertexShader.cso", L"BulletPixelShader.cso", L"BulletGeometryShader.cso", device, constantBufferList);
	projectileMaterial = new Material(device, deviceContext, sampler, L"bullet.png", shaderProgram);
	player = playerReference;
	Vertex2 vertex[] = {
		{ { 0, 0, 1 },{ 0, 0, 1 },{ 0, 0 },{ 0,0,0 } },
		{{0,0.125,1}, {0,0,1}, {0, 0}, {0, 0, 0}},
		{ {0, 0.25, 1}, {0, 0, 1}, {0, 0}, {0,0,0}}
	};

	UINT index[] = { 0, 1, 2 };
	mesh = new Mesh(vertex, index, 3, device);
}


Projectile::~Projectile(){
	ReleaseMacro(device);
	ReleaseMacro(deviceContext);
	if (sampler){
		delete sampler;
		sampler = nullptr;
	}
	if (projectileMaterial){
		delete projectileMaterial;
		projectileMaterial = nullptr;
	}
	if (shaderProgram){
		delete shaderProgram;
		shaderProgram = nullptr;
	}
}


//Update projectile position each frame, check for user input that fires a projectile
void Projectile::update(float dt){

	// fires a projectile in response to the 'q' key
	if (GetAsyncKeyState('Q') & 0x8000){
		if (projectiles.size() < 1)
			fireProjectile();
	}

	// moves the projectile, cleans it up if it moves off screen
	if (projectiles.size() > 0)
	{
		for (int x = projectiles.size() - 1; x >= 0; x--)
		{
			projectiles[x]->translate(XMFLOAT3(dt*15,sin(dt*10)/10,0));
			ages[x] = dt*100;
			//._41 is the x value for the position matrix of game entities
			if (projectiles[x]->getPosition()._41 > SCREEN_WIDTH/26)
			{
				ages.erase(ages.begin() + x);
				projectiles.erase(projectiles.begin() + x);
			}

		}
	}

}

// Creates a new projectile at the player's position, scales it to 1/10th the original mesh size, and adds it to the list of active projectiles.
void Projectile::fireProjectile()
{
	float playerX = player->player->getPosition()._41;
	float playerY = player->player->getPosition()._42;

	ages.push_back(0);
	projectiles.push_back(new GameEntity(mesh, projectileMaterial));
	projectiles[projectiles.size() - 1]->scale(XMFLOAT3(0.1, 0.1, 0.1));
	projectiles[projectiles.size() - 1]->setPosition(XMFLOAT3(playerX, playerY, 0.0f));

}

//draw projectilesset
void Projectile::draw(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, XMFLOAT3 camPos, float age){
	for (unsigned int i = 0; i < projectiles.size(); i++){
		// The projectile is moved to compensate for the scaling operation (that would change its location)
		UINT offset = 0;
		UINT stride = projectiles[i]->g_mesh->sizeofvertex;
		deviceContext->IASetInputLayout(projectiles[i]->g_mat->shaderProgram->vsInputLayout);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		//set values that get passed to matrix constant buffer
		projectiles[i]->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer.world = projectiles[i]->getWorld();
		projectiles[i]->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer.view = viewMatrix;
		projectiles[i]->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer.projection = projectionMatrix;
		projectiles[i]->g_mat->shaderProgram->ConstantBuffers[3]->dataToSendToGSBuffer.age = ages[i];


		//matrix constant buffer
		deviceContext->UpdateSubresource(
			projectiles[i]->g_mat->shaderProgram->ConstantBuffers[0]->constantBuffer,
			0,
			NULL,
			&projectiles[i]->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer,
			0,
			0);

		deviceContext->UpdateSubresource(
			projectiles[i]->g_mat->shaderProgram->ConstantBuffers[3]->constantBuffer,
			0,
			NULL,
			&projectiles[i]->g_mat->shaderProgram->ConstantBuffers[3]->dataToSendToGSBuffer,
			0,
			0);
		

		deviceContext->IASetVertexBuffers(0, 1, &projectiles[i]->g_mesh->v_buffer, &stride, &offset);
		deviceContext->IASetIndexBuffer(projectiles[i]->g_mesh->i_buffer, DXGI_FORMAT_R32_UINT, 0);

		deviceContext->PSSetSamplers(0, 1, &projectiles[i]->g_mat->samplerState);
		deviceContext->PSSetShaderResources(0, 1, &projectiles[i]->g_mat->resourceView);


		deviceContext->GSSetShader(projectiles[i]->g_mat->shaderProgram->geometryShader, NULL, 0);

		// Set the current vertex and pixel shaders, as well the constant buffer for the vert shader
		deviceContext->VSSetShader(projectiles[i]->g_mat->shaderProgram->vertexShader, NULL, 0);
		deviceContext->VSSetConstantBuffers(0, 1, &projectiles[i]->g_mat->shaderProgram->ConstantBuffers[0]->constantBuffer); //set first constant vertex buffer-matrix
		deviceContext->PSSetShader(projectiles[i]->g_mat->shaderProgram->pixelShader, NULL, 0);
		deviceContext->GSSetConstantBuffers(0, 1, &projectiles[i]->g_mat->shaderProgram->ConstantBuffers[3]->constantBuffer);

		// Finally do the actual drawing
		deviceContext->DrawIndexed(
			projectiles[i]->g_mesh->m_size,	// The number of indices we're using in this draw
			0,
			0);
		// Move the projectile back to the position it was at before we compensated for the scaling operation.
		deviceContext->GSSetShader(NULL, NULL, 0);
	}
}
