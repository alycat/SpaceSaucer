#include "ExplosionEffect.h"

ExplosionEffect::ExplosionEffect(XMFLOAT4 position, XMFLOAT4 speed, ID3D11Device* dev, ID3D11DeviceContext* devCtx, Material* mat){
	device = dev;
	deviceContext = devCtx;
	Particle particle[] = {
		{ position, speed }
	};
	UINT indices[] = { 0 };
	object = new GameEntity(new Mesh(particle, indices, 1, device), mat);
	//object->scale(XMFLOAT3(0.1, 0.1, 1));
	firstPass = true;
}
ExplosionEffect::~ExplosionEffect(){

}

void ExplosionEffect::setPosition(XMFLOAT3 position){
	//object->setPosition(position);
}

void ExplosionEffect::translate(XMFLOAT3 translation){
	//object->translate(translation);
}

void ExplosionEffect::rotate(XMFLOAT3 rotation){
	//object->rotate(rotation);
}

void ExplosionEffect::draw(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, float time){
	UINT offset = 0;
	UINT stride = sizeof(Vertex2);

	//object->setPosition(XMFLOAT3(position._41, position._42, position._43));
	//UINT offset = 0;
	stride = object->g_mesh->sizeofvertex;
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
		this->drawParticles(viewMatrix, projectionMatrix, time);
	}
	this->drawExplosion(viewMatrix, projectionMatrix, time);
}


void ExplosionEffect::drawParticles(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, float time){
//	object->setPosition(position);
	UINT offset[] = { 0 };
	UINT *so_offset = 0;
	UINT stride = object->g_mesh->sizeofvertex;
	ID3D11Buffer* bufferArray[1] = { 0 };

	object->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer.world = object->getWorld();
	object->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer.view = viewMatrix;
	object->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer.projection = projectionMatrix;
	object->g_mat->shaderProgram->ConstantBuffers[3]->dataToSendToGSBuffer.age = time;


	deviceContext->IASetInputLayout(object->g_mat->shaderProgram->soVSInputLayout);
	deviceContext->IASetVertexBuffers(0, 1, &object->g_mesh->init_buffer, &stride, offset);


	deviceContext->SOSetTargets(1, &object->g_mesh->so_buffer, 0);
	deviceContext->GSSetShader(object->g_mat->shaderProgram->streamOutputShader, NULL, 0);

	deviceContext->VSSetShader(object->g_mat->shaderProgram->soVertexShader, NULL, 0);

	ID3D11DepthStencilState* depthState = nullptr;
	deviceContext->OMGetDepthStencilState(&depthState, 0);
	deviceContext->OMSetDepthStencilState(NULL, 0);

	deviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->PSSetShader(NULL, NULL, 0);

	deviceContext->UpdateSubresource(
		object->g_mat->shaderProgram->ConstantBuffers[0]->constantBuffer,
		0,
		NULL,
		&object->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer,
		0,
		0);


	deviceContext->GSSetConstantBuffers(0, 1, &object->g_mat->shaderProgram->ConstantBuffers[0]->constantBuffer);

	if (firstPass){
		deviceContext->Draw(1, 0);
		firstPass = false;
		deviceContext->SOSetTargets(1, bufferArray, 0);
		std::swap(object->g_mesh->v_buffer, object->g_mesh->so_buffer);
	}

	deviceContext->GSSetShader(NULL, NULL, 0);


	deviceContext->OMSetDepthStencilState(depthState, 0);
}
void ExplosionEffect::drawExplosion(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, float time){
	UINT offset[] = { 0 };
	UINT *so_offset = 0;
	UINT stride = object->g_mesh->sizeofvertex;
	ID3D11Buffer* bufferArray[1] = { 0 };

	deviceContext->IASetInputLayout(object->g_mat->shaderProgram->vsInputLayout);
	deviceContext->IASetVertexBuffers(0, 1, &object->g_mesh->v_buffer, &stride, offset);


	deviceContext->SOSetTargets(1, bufferArray, 0);

	deviceContext->VSSetShader(object->g_mat->shaderProgram->vertexShader, NULL, 0);
	deviceContext->IASetIndexBuffer(object->g_mesh->i_buffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->PSSetSamplers(0, 1, &object->g_mat->samplerState);
	deviceContext->PSSetShaderResources(0, 1, &object->g_mat->resourceView);
	deviceContext->PSSetShaderResources(1, 1, &object->g_mat->resourceView2);
	deviceContext->PSSetShaderResources(2, 1, &object->g_mat->resourceView3);

	deviceContext->PSSetShader(object->g_mat->shaderProgram->pixelShader, NULL, 0);
	deviceContext->IASetVertexBuffers(0, 1, &object->g_mesh->v_buffer, &stride, offset);

	deviceContext->GSSetShader(object->g_mat->shaderProgram->geometryShader, NULL, 0);
	object->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer.world = object->getWorld();
	object->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer.view = viewMatrix;
	object->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer.projection = projectionMatrix;
	object->g_mat->shaderProgram->ConstantBuffers[3]->dataToSendToGSBuffer.age = time;
//	object->g_mat->shaderProgram->ConstantBuffers[3]->dataToSendToGSBuffer.position = XMFLOAT3(0, 0.1, 0);// XMFLOAT3(position._41, position._42, 1);//position;

	object->g_mat->shaderProgram->ConstantBuffers[1]->dataToSendToLightBuffer.ambientColor = XMFLOAT4(0, 0, 0, 1);
	object->g_mat->shaderProgram->ConstantBuffers[1]->dataToSendToLightBuffer.diffuseColor = XMFLOAT4(0.75, 0.5, 0.75, 1);//XMFLOAT4(0.54, 0.25, 0.27, 1);// XMFLOAT4(0.125, 0.125, 0.125, 1);
	object->g_mat->shaderProgram->ConstantBuffers[1]->dataToSendToLightBuffer.highlightColor = XMFLOAT4(0, 1, 1, 1);
	object->g_mat->shaderProgram->ConstantBuffers[1]->dataToSendToLightBuffer.lightDirection = XMFLOAT3(0, 0, -1);
	object->g_mat->shaderProgram->ConstantBuffers[1]->dataToSendToLightBuffer.specularColor = XMFLOAT4(0, 1, 1, 1);
	object->g_mat->shaderProgram->ConstantBuffers[1]->dataToSendToLightBuffer.specularPower = 5;

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

	deviceContext->UpdateSubresource(
		object->g_mat->shaderProgram->ConstantBuffers[1]->constantBuffer,
		0,
		NULL,
		&object->g_mat->shaderProgram->ConstantBuffers[1]->dataToSendToLightBuffer,
		0,
		0);


	deviceContext->VSSetConstantBuffers(0, 1, &object->g_mat->shaderProgram->ConstantBuffers[3]->constantBuffer);
	deviceContext->GSSetConstantBuffers(0, 1, &object->g_mat->shaderProgram->ConstantBuffers[0]->constantBuffer);
	deviceContext->PSSetConstantBuffers(0, 1, &object->g_mat->shaderProgram->ConstantBuffers[1]->constantBuffer);

	deviceContext->DrawAuto();
	deviceContext->GSSetShader(NULL, NULL, 0);
}