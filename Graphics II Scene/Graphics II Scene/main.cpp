// D3D LAB 1a "Line Land".
// Author: L.Norri CD DRX, FullSail University

// Introduction:
// Welcome to the first lab of the Direct3D Graphics Programming class.
// This is the ONLY guided lab in this course! 
// Future labs will demand the habits & foundation you develop right now!  
// It is CRITICAL that you follow each and every step. ESPECIALLY THE READING!!!

// TO BEGIN: Open the word document that acompanies this lab and start from the top.

//************************************************************
//************ INCLUDES & DEFINES ****************************
//************************************************************
#include "Camera.h"
#include "XTime.h"

// BEGIN PART 1
// TODO: PART 1 STEP 1a
#pragma comment(lib,"d3d11.lib")
#include <d3d11.h>

// TODO: PART 2 STEP 6
#include "Trivial_VS.csh"
#include "Trivial_PS.csh"

#define BACKBUFFER_WIDTH	500
#define BACKBUFFER_HEIGHT	500
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
	ID3D11Device *device			= nullptr;
	ID3D11DeviceContext *devContext = nullptr;
	ID3D11RenderTargetView *RTV		= nullptr;
	ID3D11DepthStencilView *DSV		= nullptr;
	ID3D11InputLayout *input		= nullptr;
	ID3D11Texture2D *image			= nullptr;
	IDXGISwapChain *swapchain		= nullptr;
	ID3D11Buffer *vertBuff			= nullptr;
	D3D11_VIEWPORT viewport;
	DXGI_SWAP_CHAIN_DESC swapDesc;
	XTime clock;


	// Shaders
	ID3D11VertexShader *VertShader	= nullptr;
	ID3D11PixelShader *PixelShader	= nullptr;
	
	// Buffers
	ID3D11Buffer *buffer = nullptr;
	D3D11_BUFFER_DESC buffDesc;

	ID3D11Buffer *Cbuffer;
	D3D11_BUFFER_DESC CbuffDesc;
	// Object Buffer
	ID3D11Buffer *Objbuffer;
	D3D11_BUFFER_DESC ObjbuffDesc;
	// Scene buffer
	ID3D11Buffer *Sbuffer;
	D3D11_BUFFER_DESC SbuffDesc;
	// Index buffer
	ID3D11Buffer *Ibuffer;
	D3D11_BUFFER_DESC IbuffDesc;
	// Z buffer
	ID3D11Texture2D *Zbuffer;
	D3D11_TEXTURE2D_DESC ZbuffDesc;

	// TODO: PART 3 STEP 2b
	struct SEND_TO_VRAM
	{
		float constantColor[4];
		float constantOffset[2];
		float padding[2];
	};
	// TODO: PART 3 STEP 4a
	SEND_TO_VRAM toShader;

	struct Scene
	{
		XMFLOAT4X4 ViewMatrix;
		XMFLOAT4X4 ProjectMatrix;
	};
	struct Object
	{
		XMFLOAT4X4 WorldMatrix;
	};

	Scene scene;
	Object obj;

public:
	struct SIMPLE_VERTEX
	{
		float x, y, z, w;
		float color[4];
	};
	SIMPLE_VERTEX star[12];
	unsigned int numVerts = 12;

	DEMO_APP(HINSTANCE hinst, WNDPROC proc);
	bool Run();
	bool ShutDown();
	void Resize(int width, int height);
	void SetProjectionMatrix();
	void SetObjMatrix(float Time);

	void CreateStar();
};

//************************************************************
//************ CREATION OF OBJECTS & RESOURCES ***************
//************************************************************

DEMO_APP::DEMO_APP(HINSTANCE hinst, WNDPROC proc)
{
	// ****************** BEGIN WARNING ***********************// 
	// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY! 
	application = hinst; 
	appWndProc = proc; 

	WNDCLASSEX  wndClass;
    ZeroMemory( &wndClass, sizeof( wndClass ) );
    wndClass.cbSize         = sizeof( WNDCLASSEX );             
    wndClass.lpfnWndProc    = appWndProc;						
    wndClass.lpszClassName  = L"DirectXApplication";            
	wndClass.hInstance      = application;		               
    wndClass.hCursor        = LoadCursor( NULL, IDC_ARROW );    
    wndClass.hbrBackground  = ( HBRUSH )( COLOR_WINDOWFRAME ); 
	//wndClass.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FSICON));
    RegisterClassEx( &wndClass );

	RECT window_size = { 0, 0, BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT };
	AdjustWindowRect(&window_size, WS_OVERLAPPEDWINDOW, false);

	window = CreateWindow(	L"DirectXApplication", L"Lab 1a Line Land",	WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME|WS_MAXIMIZEBOX), 
							CW_USEDEFAULT, CW_USEDEFAULT, window_size.right-window_size.left, window_size.bottom-window_size.top,					
							NULL, NULL,	application, this );												

    ShowWindow( window, SW_SHOW );
	//********************* END WARNING ************************//
	
	// TODO: PART 1 STEP 3a
	ZeroMemory(&swapDesc, sizeof(swapDesc));
	swapDesc.BufferDesc.Width = BACKBUFFER_WIDTH;
	swapDesc.BufferDesc.Height = BACKBUFFER_HEIGHT;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferCount = 1;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapDesc.OutputWindow = window;
	swapDesc.Windowed = true;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// TODO: PART 1 STEP 3b
	HRESULT hr = (D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG, 0, 0, D3D11_SDK_VERSION, &swapDesc, &swapchain, &device, nullptr, &devContext));
	
	// TODO: PART 1 STEP 4
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&image);
	device->CreateRenderTargetView(image, nullptr, &RTV);
	image->Release();

	// TODO: PART 1 STEP 5
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	// Setting Dimensions of the view
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = BACKBUFFER_WIDTH;
	viewport.Height = BACKBUFFER_HEIGHT;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;

	// Creating Star
#if 1
	unsigned int starindeces[60] =
	{ 0, 1, 2,
	0, 2, 3,
	0, 3, 4,
	0, 4, 5,
	0, 5, 6,
	0, 6, 7,
	0, 7, 8,
	0, 8, 9,
	0, 9, 10,
	0, 10, 1,
	11, 2, 1,
	11, 3, 2,
	11, 4, 3,
	11, 5, 4,
	11, 6, 5,
	11, 7, 6,
	11, 8, 7,
	11, 9, 8,
	11, 10, 9,
	11, 1, 10 };
	CreateStar();
#endif

    // TODO: PART 2 STEP 3c
	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = star;
	data.SysMemPitch = NULL;
	data.SysMemSlicePitch = NULL;

	// Vert buffer
	D3D11_BUFFER_DESC vertbuffDesc;
	ZeroMemory(&vertbuffDesc, sizeof(vertbuffDesc));
	vertbuffDesc.ByteWidth = sizeof(SIMPLE_VERTEX) * numVerts;
	vertbuffDesc.Usage = D3D11_USAGE_DEFAULT;
	vertbuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertbuffDesc.CPUAccessFlags = D3D11_USAGE_DEFAULT;
	vertbuffDesc.StructureByteStride = 0;

	CHECK(device->CreateBuffer(&vertbuffDesc, &data, &vertBuff));

	// Index buffer
	data.pSysMem = starindeces;
	ZeroMemory(&IbuffDesc, sizeof(IbuffDesc));
	IbuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IbuffDesc.ByteWidth = sizeof(starindeces);
	IbuffDesc.CPUAccessFlags = NULL;
	IbuffDesc.Usage = D3D11_USAGE_IMMUTABLE;

	CHECK(device->CreateBuffer(&IbuffDesc, &data, &Ibuffer));

	// Z buffer
	ZeroMemory(&ZbuffDesc, sizeof(ZbuffDesc));
	ZbuffDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	ZbuffDesc.Format = DXGI_FORMAT_D32_FLOAT;
	ZbuffDesc.Width = BACKBUFFER_WIDTH;
	ZbuffDesc.Height = BACKBUFFER_HEIGHT;
	ZbuffDesc.MipLevels = 1;
	ZbuffDesc.ArraySize = 1;
	ZbuffDesc.Usage = D3D11_USAGE_DEFAULT;
	ZbuffDesc.SampleDesc.Count = 1;
	ZbuffDesc.SampleDesc.Quality = 0;
	data.pSysMem = Zbuffer;
	data.SysMemPitch = NULL;
	data.SysMemSlicePitch = NULL;

	CHECK(device->CreateTexture2D(&ZbuffDesc, 0, &Zbuffer));
	CHECK(device->CreateDepthStencilView(Zbuffer, nullptr, &DSV));
	
	// TODO: PART 5 STEP 2b
	
	// TODO: PART 5 STEP 3
		
	// TODO: PART 2 STEP 5
	// ADD SHADERS TO PROJECT, SET BUILD OPTIONS & COMPILE

	// TODO: PART 2 STEP 7
	CHECK(device->CreateVertexShader(Trivial_VS, sizeof(Trivial_VS), nullptr, &VertShader));
	CHECK(device->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), nullptr, &PixelShader));

	// TODO: PART 2 STEP 8a
	D3D11_INPUT_ELEMENT_DESC vertLayout[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// TODO: PART 2 STEP 8b
	CHECK(device->CreateInputLayout(vertLayout, ARRAYSIZE(vertLayout), Trivial_VS, sizeof(Trivial_VS), &input));
	
	// Constant Buffer
	ZeroMemory(&CbuffDesc, sizeof(CbuffDesc));
	CbuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	CbuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	CbuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	CbuffDesc.ByteWidth = sizeof(SEND_TO_VRAM);

	CHECK(device->CreateBuffer(&CbuffDesc, nullptr, &Cbuffer));

	// Object buffer (World)
	ZeroMemory(&ObjbuffDesc, sizeof(ObjbuffDesc));
	ObjbuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ObjbuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	ObjbuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ObjbuffDesc.ByteWidth = sizeof(Object);

	CHECK(device->CreateBuffer(&ObjbuffDesc, nullptr, &Objbuffer));

	// Scene Buffer (View/Projection)
	ZeroMemory(&SbuffDesc, sizeof(SbuffDesc));
	SbuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	SbuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	SbuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	SbuffDesc.ByteWidth = sizeof(Scene);

	CHECK(device->CreateBuffer(&SbuffDesc, nullptr, &Sbuffer));

	// TODO: PART 3 STEP 4b
	toShader.constantOffset[0] = 0;
	toShader.constantOffset[1] = 0;

	toShader.constantColor[0] = 1;
	toShader.constantColor[1] = 1;
	toShader.constantColor[2] = 0;
	toShader.constantColor[3] = 1;

	// Set Projection Matrix
	SetProjectionMatrix();
	XMStoreFloat4x4(&scene.ViewMatrix, XMMatrixIdentity());
	//scene.ViewMatrix = Translation(Vertex(0, 0, 5, 1));
}

//************************************************************
//************ EXECUTION *************************************
//************************************************************

bool DEMO_APP::Run()
{
	float time = (float)clock.TotalTime();
	// Set World Matrix	
	SetObjMatrix(time);
	// TODO: PART 4 STEP 3
	
	// TODO: PART 4 STEP 5
	
	// END PART 4

	// TODO: PART 1 STEP 7a
	devContext->OMSetRenderTargets(1, &RTV, DSV);

	// TODO: PART 1 STEP 7b
	devContext->RSSetViewports(1, &viewport);
	
	// TODO: PART 1 STEP 7c
	float color[4] = { 0.0f, 0.125f, 0.6f, 1.0f };
	devContext->ClearDepthStencilView(DSV, D3D11_CLEAR_DEPTH, 1, 0);
	devContext->ClearRenderTargetView(RTV, color);

	// TODO: PART 5 STEP 4
	
	// TODO: PART 5 STEP 5
	
	// TODO: PART 5 STEP 6
	
	// TODO: PART 5 STEP 7
	
	// END PART 5
	
	// Constant
	unsigned int size = sizeof(SEND_TO_VRAM);
	unsigned int offset = 0;
	D3D11_MAPPED_SUBRESOURCE map;
	devContext->Map(Cbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, &toShader, size);
	devContext->Unmap(Cbuffer, 0);
	devContext->VSSetConstantBuffers(0, 1, &Cbuffer);

	// Object (world)
	size = sizeof(Object);
	devContext->Map(Objbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, &obj, size);
	devContext->Unmap(Objbuffer, 0);
	devContext->VSSetConstantBuffers(1, 1, &Objbuffer);

	// Scene
	size = sizeof(Scene);
	devContext->Map(Sbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, &scene, size);
	devContext->Unmap(Sbuffer, 0);
	devContext->VSSetConstantBuffers(2, 1, &Sbuffer);

	// Bindings
	size = sizeof(SIMPLE_VERTEX);
	devContext->IASetVertexBuffers(0, 1, &vertBuff, &size, &offset);
	devContext->IASetIndexBuffer(Ibuffer, DXGI_FORMAT_R32_UINT, 0);

	// Shaders
	devContext->VSSetShader(VertShader, nullptr, 0);
	devContext->PSSetShader(PixelShader, nullptr, 0);

	// Other stuff
	devContext->IASetInputLayout(input);
	devContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Draw
	devContext->DrawIndexed(60, 0, 0);

	// TODO: PART 1 STEP 8
	swapchain->Present(0, 0);

	clock.Signal();
	// END OF PART 1
	return true; 
}

//************************************************************
//************ DESTRUCTION ***********************************
//************************************************************
void DEMO_APP::Resize(int width, int height)
{
	RELEASE(RTV);
	devContext->ClearState();
}
bool DEMO_APP::ShutDown()
{
	// TODO: PART 1 STEP 6
	RELEASE(device);
	RELEASE(devContext);
	RELEASE(RTV);
	RELEASE(DSV);
	RELEASE(VertShader);
	RELEASE(PixelShader);
	RELEASE(input);
	RELEASE(swapchain);
	RELEASE(image);
	RELEASE(vertBuff);
	RELEASE(buffer);
	RELEASE(Cbuffer);
	RELEASE(Ibuffer);
	RELEASE(Sbuffer);
	RELEASE(Objbuffer);
	RELEASE(Zbuffer)

	UnregisterClass( L"DirectXApplication", application ); 
	return true;
}

//************************************************************
//************ WINDOWS RELATED *******************************
//************************************************************

// ****************** BEGIN WARNING ***********************// 
// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY!
	
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine,	int nCmdShow );						   
LRESULT CALLBACK WndProc(HWND hWnd,	UINT message, WPARAM wparam, LPARAM lparam );		
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE, LPTSTR, int )
{
	srand(unsigned int(time(0)));
	DEMO_APP myApp(hInstance,(WNDPROC)WndProc);	
    MSG msg; ZeroMemory( &msg, sizeof( msg ) );
    while ( msg.message != WM_QUIT && myApp.Run() )
    {	
	    if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        { 
            TranslateMessage( &msg );
            DispatchMessage( &msg ); 
        }
    }
	myApp.ShutDown(); 
	return 0; 
}
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    if(GetAsyncKeyState(VK_ESCAPE))
		message = WM_DESTROY;
    switch ( message )
    {
        case ( WM_DESTROY ): { PostQuitMessage( 0 ); }
        break;
    }
    return DefWindowProc( hWnd, message, wParam, lParam );
}
//********************* END WARNING ************************//
void DEMO_APP::SetProjectionMatrix()
{
	float zNear = 0.1f;
	float zFar = 100.0f;
	float Yscale = 1 / (tanf((65 >> 1) * (3.1415f / 180.0f)));
	float Xscale = Yscale * 1;

	XMStoreFloat4x4(&scene.ProjectMatrix, XMMatrixIdentity());
	scene.ProjectMatrix._11 = Xscale;
	scene.ProjectMatrix._22 = Yscale;
	scene.ProjectMatrix._33 = zFar / (zFar - zNear);
	scene.ProjectMatrix._34 = 1.0f;
	scene.ProjectMatrix._43 = -(zFar * zNear) / (zFar - zNear);
	scene.ProjectMatrix._44 = 0.0f;
}
void DEMO_APP::SetObjMatrix(float Time)
{
	// Row 1
	obj.WorldMatrix._11 = cos(Time);
	obj.WorldMatrix._12 = 0;
	obj.WorldMatrix._13 = sin(Time);
	obj.WorldMatrix._14 = 0;
	// Row 2
	obj.WorldMatrix._21 = 0;
	obj.WorldMatrix._22 = 1;
	obj.WorldMatrix._23 = 0;
	obj.WorldMatrix._24 = 0;
	// Row 3
	obj.WorldMatrix._31 = -sin(Time);
	obj.WorldMatrix._32 = 0;
	obj.WorldMatrix._33 = cos(Time);
	obj.WorldMatrix._34 = 0;
	// Row 4
	obj.WorldMatrix._41 = 0;
	obj.WorldMatrix._42 = 0;
	obj.WorldMatrix._43 = 5;
	obj.WorldMatrix._44 = 1;
}

void DEMO_APP::CreateStar()
{
	// TODO: PART 2 STEP 3a
	int index = 1;
	for (int i = 0; i < 360; i += 36)
	{
		// Position
		star[index].x = (float)cosf(i * 0.01745277777777777777777777777778f);
		star[index].y = (float)sinf(i * 0.01745277777777777777777777777778f);
		star[index].z = 0;
		star[index].w = 1.0f;
		// Color
		star[index].color[0] = 0.0f;
		star[index].color[1] = 0.4f;
		star[index].color[2] = 0.6f;
		star[index].color[3] = 1.0f;
		// Inner Vertices
		if (index % 2 == 0)
		{
			star[index].x /= 2;
			star[index].y /= 2;
			star[index].color[0] = 0.0;
			star[index].color[1] = 0.0;
			star[index].color[2] = 0.0;
			star[index].color[3] = 1.0;
		}
		++index;
	}
	star[0].x = 0.0f;
	star[0].y = 0.0f;
	star[0].z = 0.5f;
	star[0].w = 1.0f;
	star[11].x = 0.0f;
	star[11].y = 0.0f;
	star[11].z = -0.5f;
	star[11].w = 1.0f;
	star[0].color[0] = 0.0;
	star[0].color[1] = 0.0;
	star[0].color[2] = 0.0;
	star[0].color[3] = 1.0;
	star[11].color[0] = 0.0;
	star[11].color[1] = 0.0;
	star[11].color[2] = 0.0;
	star[11].color[3] = 1.0;
}