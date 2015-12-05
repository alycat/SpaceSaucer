#ifndef _GLOBAL_H
#define _GLOBAL_H
#define ReleaseMacro(x) { if(x){ x->Release(); x = 0; } }
#define new new(1, __FILE__, __LINE__)
#include <DirectXMath.h>
#define NUM_ASTEROIDS 5
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#include "SimpleMath.h"
using namespace DirectX;

// Vertex struct for triangles
struct Vertex
{
	XMFLOAT3 Position;
	XMFLOAT4 Color;
	XMFLOAT2 UVs;

};

struct Vertex2{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 UVs;
	XMFLOAT3 Tangent;
};

struct Triangle{
	int index[3];
};

struct Phong{
	XMFLOAT3 Position;
	XMFLOAT4 Color;
	XMFLOAT2 uv;
	XMFLOAT3 Normal;
	XMFLOAT3 lightPos;
};

//Vector struct for our camera class
struct Vector
{
	float x;
	float y;
	float z;
};

struct ConstantBufferLayout
{
	XMFLOAT4X4 world;
	XMFLOAT4X4 view;
	XMFLOAT4X4 projection;
};


struct ParticleVertexShaderConstantBufferLayout
{
	float age;
	XMFLOAT3 position;
	XMFLOAT4 acceleration;
};

//Camera Constant Buffer Data Layout
struct CameraBufferType{
	XMFLOAT3 cameraPosition;
	float padding;
};

//Light Constant Buffer Data Layout
struct LightBufferType{
	XMFLOAT4 ambientColor;
	XMFLOAT4 diffuseColor;
	XMFLOAT3 lightDirection;
	float specularPower;
	XMFLOAT4 specularColor;
	XMFLOAT4 highlightColor;
};

struct Particle{
	XMFLOAT4 Position;
	XMFLOAT4 speed;
};

static Vertex2* initialVertex()
{
	Vertex2 init[] = {
		{ { 0, 0, 0 } ,  { 0, 0, 0 } , {  0, 0 }}, { {0,0,0} }
	};
	return init;
}

#endif