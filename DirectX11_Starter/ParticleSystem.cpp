#include "ParticleSystem.h"
#include <iostream>

ParticleSystem::ParticleSystem(XMFLOAT4 position, XMFLOAT4 speed, ID3D11Device* dev, ID3D11DeviceContext* devCtx, Material* mat, int num_particles)
{
	device = dev;
	deviceContext = devCtx;
	initialized = false;
	firstPass = true;
	secondPass = true;

	UINT indices[1] = { 0 };
	Particle *p = new Particle;
	p->Position = position;
	p->speed = speed;

	object = new GameEntity(new Mesh(p, indices, 1, device), mat);
}

void ParticleSystem::drawParticleSystem(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, float time){
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
		this->drawSteamoutput(viewMatrix, projectionMatrix, time);
	}
	this->drawParticles(viewMatrix, projectionMatrix, time);
}

void ParticleSystem::drawSteamoutput(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, float time){
	UINT offset[] = { 0 };
	UINT *so_offset = 0;
	UINT stride = object->g_mesh->sizeofvertex;
	ID3D11Buffer* bufferArray[1] = { 0 };

	deviceContext->IASetInputLayout(object->g_mat->shaderProgram->soVSInputLayout);
	deviceContext->IASetVertexBuffers(0, 1,&object->g_mesh->init_buffer, &stride, offset);


	deviceContext->SOSetTargets(1,&object->g_mesh->so_buffer, 0);
	deviceContext->GSSetShader(object->g_mat->shaderProgram->streamOutputShader, NULL, 0);

	object->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer.world = object->getWorld();
	object->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer.view = viewMatrix;
	object->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer.projection = projectionMatrix;
	object->g_mat->shaderProgram->ConstantBuffers[3]->dataToSendToGSBuffer.age = time; 
	
	deviceContext->UpdateSubresource(
		object->g_mat->shaderProgram->ConstantBuffers[0]->constantBuffer,
		0,
		NULL,
		&object->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer,
		0,
		0);

	deviceContext->UpdateSubresource(
		object->g_mat->shaderProgram->ConstantBuffers[3]->constantBuffer,
		0,
		NULL,
		&object->g_mat->shaderProgram->ConstantBuffers[3]->dataToSendToGSBuffer,
		0,
		0);


	deviceContext->GSSetConstantBuffers(0, 1, &object->g_mat->shaderProgram->ConstantBuffers[0]->constantBuffer);

	deviceContext->VSSetShader(object->g_mat->shaderProgram->soVertexShader, NULL, 0);

	ID3D11DepthStencilState* depthState = nullptr;
	deviceContext->OMGetDepthStencilState(&depthState, 0);
	deviceContext->OMSetDepthStencilState(NULL, 0);

	deviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->PSSetShader(NULL, NULL, 0);



	if (firstPass){
		deviceContext->Draw(1, 0);
		firstPass = false;
		deviceContext->SOSetTargets(1, bufferArray, 0);
		std::swap(object->g_mesh->v_buffer, object->g_mesh->so_buffer);
	}

	deviceContext->GSSetShader(NULL, NULL, 0);


	deviceContext->OMSetDepthStencilState(depthState, 0);
}

void ParticleSystem::drawParticles(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, float time){
	UINT offset[] = { 0 };
	UINT *so_offset = 0;
	UINT stride = object->g_mesh->sizeofvertex;
	ID3D11Buffer* bufferArray[1] = { 0 };

	deviceContext->IASetInputLayout(object->g_mat->shaderProgram->vsInputLayout);
	deviceContext->IASetVertexBuffers(0, 1,&object->g_mesh->v_buffer, &stride, offset);


	deviceContext->SOSetTargets(1, bufferArray, 0);

	deviceContext->VSSetShader(object->g_mat->shaderProgram->vertexShader, NULL, 0);
	deviceContext->IASetIndexBuffer(object->g_mesh->i_buffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->PSSetSamplers(0, 1, &object->g_mat->samplerState);
	deviceContext->PSSetShaderResources(0, 1, &object->g_mat->resourceView);
	deviceContext->PSSetShader(object->g_mat->shaderProgram->pixelShader, NULL, 0);
	deviceContext->IASetVertexBuffers(0, 1, &object->g_mesh->v_buffer, &stride, offset);

	deviceContext->GSSetShader(object->g_mat->shaderProgram->geometryShader, NULL, 0);
	object->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer.world = object->getWorld();
	object->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer.view = viewMatrix;
	object->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer.projection = projectionMatrix;
	object->g_mat->shaderProgram->ConstantBuffers[3]->dataToSendToGSBuffer.age = time;

	deviceContext->UpdateSubresource(
		object->g_mat->shaderProgram->ConstantBuffers[0]->constantBuffer,
		0,
		NULL,
		&object->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer,
		0,
		0);

	deviceContext->UpdateSubresource(
		object->g_mat->shaderProgram->ConstantBuffers[3]->constantBuffer,
		0,
		NULL,
		&object->g_mat->shaderProgram->ConstantBuffers[3]->dataToSendToGSBuffer,
		0,
		0);


	deviceContext->GSSetConstantBuffers(0, 1, &object->g_mat->shaderProgram->ConstantBuffers[0]->constantBuffer);
	deviceContext->VSSetConstantBuffers(0, 1, &object->g_mat->shaderProgram->ConstantBuffers[3]->constantBuffer);

	deviceContext->DrawAuto();
	deviceContext->GSSetShader(NULL, NULL, 0);
}