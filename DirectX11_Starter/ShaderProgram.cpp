#include "ShaderProgram.h"
#include "Global.h"
#include <iostream>

ShaderProgram::ShaderProgram(wchar_t* vs_file, wchar_t* ps_file, ID3D11Device* dev, std::vector<ConstantBuffer*> constantBufferList){
	ID3DBlob* vsBlob;
	D3DReadFileToBlob(vs_file, &vsBlob);
	HRESULT hr_vertex = this->CreateInputLayoutDescFromShaderSignature(vsBlob, dev, &vsInputLayout);
	dev->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &vertexShader);
	ReleaseMacro(vsBlob);

	ID3DBlob* psBlob;
	D3DReadFileToBlob(ps_file, &psBlob);
	HRESULT hr_pixel = this->CreateInputLayoutDescFromShaderSignature(psBlob, dev, &psInputLayout);
	dev->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), NULL, &pixelShader);
	ReleaseMacro(psBlob);

	ConstantBuffers = constantBufferList;
}


ShaderProgram::ShaderProgram(wchar_t* vs_file, wchar_t* ps_file, wchar_t* gs_file, wchar_t* so_file, wchar_t* init_file, ID3D11Device* dev, std::vector<ConstantBuffer*> constantBufferList){
	D3D11_SO_DECLARATION_ENTRY desc[] =
	{
		{ 0, "SV_POSITION", 0, 0, 4, 0 },
		{ 0, "TEXCOORD", 0, 0, 4, 0 }
	};

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
		D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	ID3DBlob* vsBlob;
	D3DReadFileToBlob(vs_file, &vsBlob);
	HRESULT hr_vertex = this->CreateInputLayoutDescFromShaderSignature(vsBlob, dev, &vsInputLayout);
	//dev->CreateInputLayout(layout, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &vsInputLayout);
	dev->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &vertexShader);
	ReleaseMacro(vsBlob);

	ID3DBlob* psBlob;
	D3DReadFileToBlob(ps_file, &psBlob);
	HRESULT hr_pixel = this->CreateInputLayoutDescFromShaderSignature(psBlob, dev, &psInputLayout);
	dev->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), NULL, &pixelShader);
	ReleaseMacro(psBlob);




	ID3DBlob* soBlob;
	D3DReadFileToBlob(so_file, &soBlob);
	//dev->CreateInputLayout(layout, 2, soBlob->GetBufferPointer(), soBlob->GetBufferSize(), &soInputLayout);
	HRESULT hr_streamOut = this->CreateInputLayoutDescFromShaderSignature(soBlob, dev, &soInputLayout);
	HRESULT hr_steamOutputShader = dev->CreateGeometryShaderWithStreamOutput(soBlob->GetBufferPointer(), soBlob->GetBufferSize(), desc, 1, NULL, NULL, D3D11_SO_NO_RASTERIZED_STREAM, NULL, &streamOutputShader);
	UINT stride = sizeof(desc);
	//stride = sizeof(desc)*50;
	//hr_steamOutputShader = dev->CreateGeometryShaderWithStreamOutput(soBlob->GetBufferPointer(), soBlob->GetBufferSize(), desc, 1, &stride, 1, D3D11_SO_NO_RASTERIZED_STREAM, NULL, &streamOutputShader);
	hr_steamOutputShader = dev->CreateGeometryShaderWithStreamOutput(soBlob->GetBufferPointer(), soBlob->GetBufferSize(), desc, 2, NULL, 0, NULL, NULL, &streamOutputShader);
	if (FAILED(hr_steamOutputShader)){
		OutputDebugString(L"Failed To Create Stream output Shader\n");
		//dev->CreateGeometryShader(soBlob->GetBufferPointer(), soBlob->GetBufferSize(), NULL, &geometryShader);
	}

	ReleaseMacro(soBlob);

	ID3DBlob* gsBlob;
	D3DReadFileToBlob(gs_file, &gsBlob);
	//HRESULT hr_geometry = this->CreateInputLayoutDescFromShaderSignature(gsBlob, dev, &gsInputLayout);
	dev->CreateGeometryShader(gsBlob->GetBufferPointer(), gsBlob->GetBufferSize(), NULL, &geometryShader);
	//dev->CreateGeometryShaderWithStreamOutput(gsBlob->GetBufferPointer(), gsBlob->GetBufferSize(), desc2, 1, &stride2, 1, 0, NULL, &geometryShader);
	UINT bufferSize = gsBlob->GetBufferSize();
	ReleaseMacro(gsBlob);

	ID3DBlob *soVBlob;
	HRESULT hr_so_vertexShader = D3DReadFileToBlob(init_file, &soVBlob);
	//dev->CreateInputLayout(layout, 2, soVBlob->GetBufferPointer(), soVBlob->GetBufferSize(), &soVSInputLayout);
	this->CreateInputLayoutDescFromShaderSignature(soVBlob, dev, &soVSInputLayout);
	dev->CreateVertexShader(soVBlob->GetBufferPointer(), soVBlob->GetBufferSize(), NULL, &soVertexShader);


	ConstantBuffers = constantBufferList;
}

ShaderProgram::ShaderProgram(wchar_t* vs_file, wchar_t* ps_file, wchar_t* gs_file, wchar_t* so_file, ID3D11Device* dev, std::vector<ConstantBuffer*> constantBufferList){
	D3D11_SO_DECLARATION_ENTRY desc[] =
	{
		{ 0, "SV_POSITION", 0, 0, 4, 0 },
		{ 0, "COLOR", 0, 0, 4, 0 }
	};

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
		D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	ID3DBlob* vsBlob;
	D3DReadFileToBlob(vs_file, &vsBlob);
	HRESULT hr_vertex = this->CreateInputLayoutDescFromShaderSignature(vsBlob, dev, &vsInputLayout);
	//dev->CreateInputLayout(layout, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &vsInputLayout);
	dev->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &vertexShader);
	ReleaseMacro(vsBlob);

	ID3DBlob* psBlob;
	D3DReadFileToBlob(ps_file, &psBlob);
	HRESULT hr_pixel = this->CreateInputLayoutDescFromShaderSignature(psBlob, dev, &psInputLayout);
	dev->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), NULL, &pixelShader);
	ReleaseMacro(psBlob);

	


	ID3DBlob* soBlob;
	D3DReadFileToBlob(so_file, &soBlob);
	//dev->CreateInputLayout(layout, 2, soBlob->GetBufferPointer(), soBlob->GetBufferSize(), &soInputLayout);
	HRESULT hr_streamOut = this->CreateInputLayoutDescFromShaderSignature(soBlob, dev, &soInputLayout);
	HRESULT hr_steamOutputShader = dev->CreateGeometryShaderWithStreamOutput(soBlob->GetBufferPointer(), soBlob->GetBufferSize(), desc, 1, NULL, NULL, D3D11_SO_NO_RASTERIZED_STREAM, NULL, &streamOutputShader);
	UINT stride = sizeof(desc);
	//stride = sizeof(desc)*50;
	//hr_steamOutputShader = dev->CreateGeometryShaderWithStreamOutput(soBlob->GetBufferPointer(), soBlob->GetBufferSize(), desc, 1, &stride, 1, D3D11_SO_NO_RASTERIZED_STREAM, NULL, &streamOutputShader);
	hr_steamOutputShader = dev->CreateGeometryShaderWithStreamOutput(soBlob->GetBufferPointer(), soBlob->GetBufferSize(), desc,2, NULL, 0, NULL, NULL, &streamOutputShader);
	if (FAILED(hr_steamOutputShader)){
		OutputDebugString(L"Failed To Create Stream output Shader\n");
		//dev->CreateGeometryShader(soBlob->GetBufferPointer(), soBlob->GetBufferSize(), NULL, &geometryShader);
	}
	
	ReleaseMacro(soBlob);

	ID3DBlob* gsBlob;
	D3DReadFileToBlob(gs_file, &gsBlob);
	//HRESULT hr_geometry = this->CreateInputLayoutDescFromShaderSignature(gsBlob, dev, &gsInputLayout);
	dev->CreateGeometryShader(gsBlob->GetBufferPointer(), gsBlob->GetBufferSize(), NULL, &geometryShader);
	//dev->CreateGeometryShaderWithStreamOutput(gsBlob->GetBufferPointer(), gsBlob->GetBufferSize(), desc2, 1, &stride2, 1, 0, NULL, &geometryShader);
	UINT bufferSize = gsBlob->GetBufferSize();
	ReleaseMacro(gsBlob);

	ID3DBlob *soVBlob;
	HRESULT hr_so_vertexShader = D3DReadFileToBlob(L"StreamoutVertex.cso", &soVBlob);
	//dev->CreateInputLayout(layout, 2, soVBlob->GetBufferPointer(), soVBlob->GetBufferSize(), &soVSInputLayout);
	this->CreateInputLayoutDescFromShaderSignature(soVBlob, dev, &soVSInputLayout);
	dev->CreateVertexShader(soVBlob->GetBufferPointer(), soVBlob->GetBufferSize(), NULL, &soVertexShader);


	ConstantBuffers = constantBufferList;
}

ShaderProgram::ShaderProgram(wchar_t* vs_file, wchar_t* ps_file, wchar_t* gs_file, ID3D11Device* dev, std::vector<ConstantBuffer*> constantBufferList){
	ID3DBlob* vsBlob;
	D3DReadFileToBlob(vs_file, &vsBlob);
	HRESULT hr_vertex = this->CreateInputLayoutDescFromShaderSignature(vsBlob, dev, &vsInputLayout);
	//dev->CreateInputLayout(layout, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &vsInputLayout);
	dev->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &vertexShader);
	ReleaseMacro(vsBlob);

	ID3DBlob* psBlob;
	D3DReadFileToBlob(ps_file, &psBlob);
	HRESULT hr_pixel = this->CreateInputLayoutDescFromShaderSignature(psBlob, dev, &psInputLayout);
	dev->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), NULL, &pixelShader);
	ReleaseMacro(psBlob);

	ID3DBlob* gsBlob;
	D3DReadFileToBlob(gs_file, &gsBlob);
	HRESULT hr_geometry = this->CreateInputLayoutDescFromShaderSignature(gsBlob, dev, &gsInputLayout);
	HRESULT hr_shader = dev->CreateGeometryShader(gsBlob->GetBufferPointer(), gsBlob->GetBufferSize(), NULL, &geometryShader);
	if (!hr_shader){
		//char* str;// = std::wstring string(gs_file);
		OutputDebugString(gs_file);
	}
	//dev->CreateGeometryShaderWithStreamOutput(gsBlob->GetBufferPointer(), gsBlob->GetBufferSize(), desc2, 1, &stride2, 1, 0, NULL, &geometryShader);
	ReleaseMacro(gsBlob);

	ConstantBuffers = constantBufferList;
}

ShaderProgram::~ShaderProgram(void)
{
	/*ReleaseMacro(pixelShader);
	ReleaseMacro(vertexShader);
	ReleaseMacro(geometryShader);
	ReleaseMacro(streamOutputShader);
	ReleaseMacro(vsInputLayout);
	ReleaseMacro(psInputLayout);
	ReleaseMacro(gsInputLayout);
	ReleaseMacro(camInputLayout);*/
}


/**
*http://takinginitiative.wordpress.com/2011/12/11/directx-1011-basic-shader-reflection-automatic-input-layout-creation/
**/
HRESULT ShaderProgram::CreateInputLayoutDescFromShaderSignature(ID3DBlob* pShaderBlob, ID3D11Device* pD3DDevice, ID3D11InputLayout** pInputLayout)
{
	// Reflect shader info
	ID3D11ShaderReflection* pVertexShaderReflection = NULL;
	if (FAILED(D3DReflect(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pVertexShaderReflection)))
	{
		return S_FALSE;
	}

	// Get shader info
	D3D11_SHADER_DESC shaderDesc;
	pVertexShaderReflection->GetDesc(&shaderDesc);

	// Read input layout description from shader info
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for (uint32_t i = 0; i< shaderDesc.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		pVertexShaderReflection->GetInputParameterDesc(i, &paramDesc);

		// fill out input element desc
		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		// determine DXGI format
		if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if (paramDesc.Mask <= 3)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (paramDesc.Mask <= 7)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (paramDesc.Mask <= 15)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		//save element desc
		inputLayoutDesc.push_back(elementDesc);
	}

	// Try to create Input Layout
	HRESULT hr = pD3DDevice->CreateInputLayout(&inputLayoutDesc[0], inputLayoutDesc.size(), pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), pInputLayout);

	//Free allocation shader reflection memory
	pVertexShaderReflection->Release();
	return hr;
}