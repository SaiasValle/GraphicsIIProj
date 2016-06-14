#pragma once
#include "Model.h"
#include "XTime.h"
#include "DDSTextureLoader.h"
#include <atlbase.h>

// Shaders
#include "Trivial_VS.csh"
#include "Trivial_PS.csh"
#include "Skybox_VS.csh"
#include "Skybox_PS.csh"

#define RELEASE(point) { if(point) { point->Release(); point = nullptr; } }
#define CHECK(HR) { assert(HR == S_OK); }
//************************************************************
//************ SIMPLE WINDOWS APP CLASS **********************
//************************************************************

class DEMO_APP
{
	HINSTANCE						application;
	WNDPROC							appWndProc;
	HWND							window;
	// TODO: PART 1 STEP 2
public:

	ID3D11Device *device = nullptr;
	ID3D11DeviceContext *devContext;
	ID3D11RenderTargetView *RTV = nullptr;
	ID3D11DepthStencilView *DSV = nullptr;
	ID3D11InputLayout *input = nullptr;
	ID3D11Texture2D *image = nullptr;
	IDXGISwapChain *swapchain = nullptr;
	ID3D11RasterizerState *RastState = nullptr;
	ID3D11RasterizerState *RastStatetoggled = nullptr;
	D3D11_VIEWPORT viewport;
	D3D11_VIEWPORT viewportR;
	DXGI_SWAP_CHAIN_DESC swapDesc;

	// Skybox
	Object Skybox;
	ID3D11InputLayout *Skyboxinput = nullptr;
	ID3D11Texture2D *Skytexture = nullptr;
	ID3D11ShaderResourceView *SkySRV = nullptr;
	ID3D11DepthStencilView *SkyDSV = nullptr;
	ID3D11Texture2D *SkyZbuffer = nullptr;
	ID3D11Buffer *SkyVbuffer = nullptr;
	ID3D11Buffer *SkyCbuffer = nullptr;
	ID3D11Buffer *SkyIbuffer = nullptr;

	// Other
	XTime clock;
	Camera camera;
	Scene scene;
	Object star;
	Object ground;

	// Shaders
	ID3D11VertexShader *VertShader = nullptr;
	ID3D11PixelShader *PixelShader = nullptr;
	ID3D11VertexShader *SkyboxVShader = nullptr;
	ID3D11PixelShader *SkyboxPShader = nullptr;

	// Star Buffers
	ID3D11Buffer *StarVbuffer = nullptr;
	ID3D11Buffer *StarCbuffer = nullptr;
	ID3D11Buffer *StarIndexbuff;

	// Ground Buffers
	ID3D11Buffer *GroundVbuff = nullptr;
	ID3D11Buffer *GroundCbuff = nullptr;
	ID3D11Buffer *GndIndexbuff;

	// Scene buffer
	ID3D11Buffer *SceneCbuffer;
	// Z buffer
	ID3D11Texture2D *Zbuffer;

	struct SIMPLE_VERTEX
	{
		float x, y, z, w;
		float color[4];
		float uvw[3];
		float normal[3];
	};
	vector<SIMPLE_VERTEX> starverts;
	vector<SIMPLE_VERTEX> groundverts;
	vector<SIMPLE_VERTEX> skyboxverts;


	DEMO_APP(HINSTANCE hinst, WNDPROC proc);
	bool Run();
	bool ShutDown();
	void Resize(int width, int height);
	void SetProjectionMatrix();
	// Scene Getters/Mutators
	ID3D11Buffer* GetSceneCBuffer() { return SceneCbuffer; }
	Scene GetScene() { return scene; }
	// Star/Ground
	void SetStarMat(float Time);
	void CreateStar();
	void CreateGround();
	void CreateSkybox();
	// Set Buffers
	void Initialize();
	template <typename Type>
	void SetVertBuffer(ID3D11Buffer **vertbuff, vector<Type> verts);
	template <typename Type>
	void SetIndexBuffer(ID3D11Buffer **indexbuff, vector<Type> verts);
	// Sets Swapchain & Creates Viewport
	void SetSwapChain();
	// Draw
	void Draw(Object *obj, ID3D11Buffer **vertbuff, ID3D11Buffer *indexbuff, ID3D11Buffer *constbuff, unsigned int numindices);
};