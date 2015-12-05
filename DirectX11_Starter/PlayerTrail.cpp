#include "PlayerTrail.h"

PlayerTrail::PlayerTrail(XMFLOAT3 position, ID3D11Device* dev, ID3D11DeviceContext* devCtx, Material* mat){
	
	Vertex2 v[] = { 
		{
			{ 0, 0, 1 },
			{ 0, 0, 1 },
			{ 0, 0 },
			{ 0, 0, 0 } 
		}
	};
	device = dev;
	deviceContext = devCtx;
	UINT indices[1] = { 0 };
	Mesh *trail = new Mesh(v, indices, 1, device);
	object = new GameEntity(trail, mat);
	age = 0;
}

PlayerTrail::~PlayerTrail(){

}

void PlayerTrail::draw(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, float time, XMFLOAT3 position, XMFLOAT4 acceleration){
	UINT offset = 0;
	//UINT offset = 0
	UINT stride = object->g_mesh->sizeofvertex;
	// Set up the input assembler
	deviceContext->IASetInputLayout(object->g_mat->shaderProgram->vsInputLayout);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	//set values that get passed to matrix constant buffer

	deviceContext->GSSetShader(object->g_mat->shaderProgram->geometryShader, NULL, 0);
	object->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer.world = object->getWorld();
	object->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer.view = viewMatrix;
	object->g_mat->shaderProgram->ConstantBuffers[0]->dataToSendToConstantBuffer.projection = projectionMatrix;

	object->g_mat->shaderProgram->ConstantBuffers[3]->dataToSendToGSBuffer.age = time;
	object->g_mat->shaderProgram->ConstantBuffers[3]->dataToSendToGSBuffer.position = position;
	object->g_mat->shaderProgram->ConstantBuffers[3]->dataToSendToGSBuffer.acceleration = acceleration;
	//matrix constant buffer
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

	deviceContext->IASetVertexBuffers(0, 1, &object->g_mesh->v_buffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(object->g_mesh->i_buffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->PSSetSamplers(0, 1, &object->g_mat->samplerState);
	deviceContext->PSSetShaderResources(0, 1, &object->g_mat->resourceView);



	// Set the current vertex and pixel shaders, as well the constant buffer for the vert shader
	deviceContext->VSSetShader(object->g_mat->shaderProgram->vertexShader, NULL, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &object->g_mat->shaderProgram->ConstantBuffers[3]->constantBuffer); //set first constant vertex buffer-matrix
	deviceContext->GSSetConstantBuffers(0, 1, &object->g_mat->shaderProgram->ConstantBuffers[0]->constantBuffer); //set second constant vertex buffer-camera
	deviceContext->PSSetShader(object->g_mat->shaderProgram->pixelShader, NULL, 0);
	//deviceContext->PSSetConstantBuffers(0, 1, &object->g_mat->shaderProgram->ConstantBuffers[1]->constantBuffer); //set pixel constant buffer-light

	// Finally do the actual drawing
	deviceContext->DrawIndexed(
		object->g_mesh->m_size,	// The number of indices we're using in this draw
		0,
		0);


	deviceContext->GSSetShader(NULL, NULL, 0);
}