#include "Player.h"

//Constructor for player object
//Params(device, deviceContext, vector of constantbuffers, sampler state, mesh)
Player::Player(ID3D11Device* dev, ID3D11DeviceContext* devCtx, vector<ConstantBuffer*> constantBufferList, ID3D11SamplerState* samplerState, Mesh* mesh){
	lighting.ambientColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	lighting.diffuseColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	lighting.lightDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
	lighting.specularColor = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	lighting.specularPower = 5.0f;
	playerColor = XMFLOAT4(0, 1, 1, 1);
	y_acceleration = 0.0f;
	y_velocity = 0.0f;
	prev_y_velocity = 0.0f;
	x_velocity = 0.0f;
	x_acceleration = 0.0f;
	time = 0.0f;
	prevYDir = 0.0f;
	device = dev;
	deviceContext = devCtx;
	sampler = samplerState;
	health = 10;
	shaderProgram = new ShaderProgram(L"MultiTexVertexShader.cso", L"MultiTexPixelShader.cso", device, constantBufferList);
	shipMaterial = new Material(device, deviceContext, sampler, L"", L"night.jpg", L"Butterfly.png", shaderProgram);

	player = new GameEntity(mesh, shipMaterial, 1, 1);
	player->translate(XMFLOAT3(0.0f, 0.0f, 0.0f));
	player->scale(XMFLOAT3(0.1f, 0.1f, 0.1f));

	ShaderProgram* trailProgram = new ShaderProgram(L"TrailVertexShader.cso", L"TrailPixelShader.cso", L"TrailGeometryShader.cso", device, constantBufferList);
	Material* trailMaterial = new Material(device, deviceContext, sampler, L"night.jpg", trailProgram);
	XMFLOAT3 pos = { 0, 0, 1};
	trail = new PlayerTrail(pos, device, deviceContext, trailMaterial);
	age = 0;
	inc = 0.001;
}

GameEntity* Player::getPlayer() {
	return player;
}

Player::~Player(){
	ReleaseMacro(device);
	ReleaseMacro(deviceContext);
	if (player){
		delete player;
		player = nullptr;
	}
	if (sampler){
		delete sampler;
		sampler = nullptr;
	}
	if (shipMaterial){
		delete shipMaterial;
		shipMaterial = nullptr;
	}
	if (shaderProgram){
		delete shaderProgram;
		shaderProgram = nullptr;
	}
}

//Decrement Player's health
void Player::takeDamage(){
	if (health > 0){
		health--;
	}
}

//return ten times player's health
int Player::returnHealth(){
	return health * 10;
}


//Set Player's health ->Percent goes in, player's health goes from 1-10
//Ex: new_health is 100, health will set to 10.
void Player::setHealth(int new_health){
	int tempH = new_health / 10;
	health = max(0, min(tempH, 10));
}

//Update player position based on user input
void Player::update(float dt){
	time++;
//	age += dt * inc;
	float new_y_velocity = -2.0f - (4.9f * dt);
	new_y_velocity = cos(time / 50)*3;
//	new_y_velocity = 0;
	prevYDir = 0.0f;
	float new_x_velocity = 1.0f;
	player->translate(XMFLOAT3(0.0f, (new_y_velocity)*dt/1.5, 0.0f));

	if (GetAsyncKeyState('D') & 0x8000){
		player->translate(XMFLOAT3(5.0f * dt, 0.0f, 0.0f));
		new_x_velocity += 5.0f;
	}
	if (GetAsyncKeyState('A') & 0x8000){
		player->translate(XMFLOAT3(-5.0f * dt, 0.0f, 0.0f));
		new_x_velocity -= 5.0f;
	}
	if (GetAsyncKeyState('W') & 0x8000){
		player->translate(XMFLOAT3(0.0f, 5.0f * dt, 0.0f));
		new_y_velocity += 5.0f;
		prevYDir = 5.0f;
	}
	if (GetAsyncKeyState('S') & 0x8000){
		player->translate(XMFLOAT3(0.0f, -5.0f * dt, 0.0f));
		new_y_velocity -= 5.0f;
		prevYDir = -5.0f;
	//	trail->object->translate(XMFLOAT3(0.0f, -5.0f * dt, 0.0f));
	}
	y_acceleration = (new_y_velocity - y_velocity);
	y_velocity = new_y_velocity;
	x_acceleration = (new_x_velocity - x_velocity)/dt;
	x_velocity += new_x_velocity;
}

//draw player game entity
void Player::draw(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, XMFLOAT3 camPos, float time){
	if (abs(age) > 0.5){
		inc *= -1;
	}
	age = time - age;
	XMFLOAT4X4 pos = player->getPosition();
	XMFLOAT3 position = XMFLOAT3{pos._41, pos._42, 0};
	XMFLOAT4 acceleration = XMFLOAT4{ 0, prev_y_velocity, 0, 0 };
	trail->draw(viewMatrix, projectionMatrix, age, position, acceleration);
	prev_y_velocity = y_velocity;
//	age = time;
	age+=inc;
	UINT offset = 0;
	UINT stride = player->g_mesh->sizeofvertex;
	deviceContext->IASetInputLayout(player->g_mat->shaderProgram->vsInputLayout);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//set values that get passed to matrix constant buffer
	player->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer.world = player->getWorld();
	player->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer.view = viewMatrix;
	player->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer.projection = projectionMatrix;

	//set values that get passed to lighting constant buffer
	player->g_mat->shaderProgram->ConstantBuffers[1]->dataToSendToLightBuffer.ambientColor = lighting.ambientColor;
	player->g_mat->shaderProgram->ConstantBuffers[1]->dataToSendToLightBuffer.diffuseColor = lighting.diffuseColor;
	player->g_mat->shaderProgram->ConstantBuffers[1]->dataToSendToLightBuffer.lightDirection = lighting.lightDirection;
	player->g_mat->shaderProgram->ConstantBuffers[1]->dataToSendToLightBuffer.specularColor = lighting.specularColor;
	player->g_mat->shaderProgram->ConstantBuffers[1]->dataToSendToLightBuffer.specularPower = lighting.specularPower;
	player->g_mat->shaderProgram->ConstantBuffers[1]->dataToSendToLightBuffer.highlightColor = playerColor;
	//set values that get passed to camera constant buffer
	player->g_mat->shaderProgram->ConstantBuffers[2]->dataToSendToCameraBuffer.cameraPosition = camPos;
	player->g_mat->shaderProgram->ConstantBuffers[2]->dataToSendToCameraBuffer.padding = 1.0f;

	//matrix constant buffer
	deviceContext->UpdateSubresource(
		player->g_mat->shaderProgram->ConstantBuffers[0]->constantBuffer,
		0,
		NULL,
		&player->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer,
		0,
		0);

	//camera constant buffer 
	deviceContext->UpdateSubresource(
		player->g_mat->shaderProgram->ConstantBuffers[2]->constantBuffer,
		0,
		NULL,
		&player->g_mat->shaderProgram->ConstantBuffers[2]->dataToSendToCameraBuffer,
		0,
		0);
	//light constant buffer
	deviceContext->UpdateSubresource(
		player->g_mat->shaderProgram->ConstantBuffers[1]->constantBuffer,
		0,
		NULL,
		&player->g_mat->shaderProgram->ConstantBuffers[1]->dataToSendToLightBuffer,
		0,
		0);

	deviceContext->IASetVertexBuffers(0, 1, &player->g_mesh->v_buffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(player->g_mesh->i_buffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->PSSetSamplers(0, 1, &player->g_mat->samplerState);
	deviceContext->PSSetShaderResources(0, 1, &player->g_mat->resourceView);
	deviceContext->PSSetShaderResources(1, 1, &player->g_mat->resourceView2);
	deviceContext->PSSetShaderResources(2, 1, &player->g_mat->resourceView3);


	// Set the current vertex and pixel shaders, as well the constant buffer for the vert shader
	deviceContext->VSSetShader(player->g_mat->shaderProgram->vertexShader, NULL, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &player->g_mat->shaderProgram->ConstantBuffers[0]->constantBuffer); //set first constant vertex buffer-matrix
	deviceContext->VSSetConstantBuffers(1, 1, &player->g_mat->shaderProgram->ConstantBuffers[2]->constantBuffer); //set second constant vertex buffer-camera
	deviceContext->PSSetShader(player->g_mat->shaderProgram->pixelShader, NULL, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &player->g_mat->shaderProgram->ConstantBuffers[1]->constantBuffer); //set pixel constant buffer-light

	// Finally do the actual drawing
	deviceContext->DrawIndexed(
		player->g_mesh->m_size,	// The number of indices we're using in this draw
		0,
		0);
}

//simple reset
void Player::reset()
{
	player->setPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
}

void Player::drawText(IFW1FontWrapper *pFontWrapper){

}