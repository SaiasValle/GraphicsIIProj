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


// TODO: PART 2 STEP 6
#include "Trivial_VS.csh"
#include "Trivial_PS.csh"

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
	D3D11_VIEWPORT viewport;
	DXGI_SWAP_CHAIN_DESC swapDesc;

	// Other
	XTime clock;
	Camera camera;
	Scene scene;
	Object star;
	Object ground;

	// Shaders
	ID3D11VertexShader *VertShader	= nullptr;
	ID3D11PixelShader *PixelShader	= nullptr;
	
	// Star Buffers
	ID3D11Buffer *starVBuff			= nullptr;
	ID3D11Buffer *Starbuffer		= nullptr;
	D3D11_BUFFER_DESC StarbuffDesc;
	// Index buffer
	ID3D11Buffer *StarIndexbuff;
	D3D11_BUFFER_DESC StarIbuffDesc;

	// Ground Buffers
	ID3D11Buffer *GroundVBuff		= nullptr;
	ID3D11Buffer *Groundbuff		= nullptr;
	D3D11_BUFFER_DESC GndbuffDesc;
	// Index buffer
	ID3D11Buffer *GndIndexbuff;
	D3D11_BUFFER_DESC GndIbuffDesc;

	// Constant buffer
	ID3D11Buffer *Cbuffer;
	D3D11_BUFFER_DESC CbuffDesc;
	// Scene buffer
	ID3D11Buffer *Scenebuff;
	D3D11_BUFFER_DESC ScnebuffDesc;
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

public:
	struct SIMPLE_VERTEX
	{
		float x, y, z, w;
		float color[4];
	};
	SIMPLE_VERTEX starverts[12];
	SIMPLE_VERTEX groundverts[8];

	DEMO_APP(HINSTANCE hinst, WNDPROC proc);
	bool Run();
	bool ShutDown();
	void Resize(int width, int height);
	void SetProjectionMatrix();
	// Star/Ground
	void SetStarMat(float Time);
	void CreateStar();
	void CreateGround();
	// Set Buffers
	void SetBuffers();
	template <typename Type>
	void SetVertBuffer(ID3D11Buffer *vertbuff, vector<Type> verts);
	template <typename Type>
	void SetIndexBuffer(ID3D11Buffer *vertbuff, vector<Type> verts);
	// Sets Swapchain & Creates Viewport
	void SetSwapChain();
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
	
	// ViewPort & SwapChain
	SetSwapChain();
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
	CreateGround();
#endif

	// TODO: PART 2 STEP 3c
	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = starverts;
	data.SysMemPitch = NULL;
	data.SysMemSlicePitch = NULL;

	// Vertex buffer
	D3D11_BUFFER_DESC ObjbuffDesc;
	ZeroMemory(&ObjbuffDesc, sizeof(ObjbuffDesc));
	ObjbuffDesc.ByteWidth = sizeof(SIMPLE_VERTEX) * ARRAYSIZE(starverts);
	ObjbuffDesc.Usage = D3D11_USAGE_DEFAULT;
	ObjbuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	ObjbuffDesc.CPUAccessFlags = D3D11_USAGE_DEFAULT;
	ObjbuffDesc.StructureByteStride = 0;

	CHECK(device->CreateBuffer(&ObjbuffDesc, &data, &starVBuff));

	// Index buffer
	data.pSysMem = starindeces;
	ZeroMemory(&StarIbuffDesc, sizeof(StarIbuffDesc));
	StarIbuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	StarIbuffDesc.ByteWidth = sizeof(starindeces);
	StarIbuffDesc.CPUAccessFlags = NULL;
	StarIbuffDesc.Usage = D3D11_USAGE_IMMUTABLE;

	CHECK(device->CreateBuffer(&StarIbuffDesc, &data, &StarIndexbuff));

	// Vertex Buffers
	//CreateObjBuffs(starVBuff, starverts, starindeces, ARRAYSIZE(starverts));

	// Set Buffers (Index, Constant, etc.)
	SetBuffers();

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
	XMStoreFloat4x4(&scene.ViewMatrix, XMMatrixTranslation(0.0f, 0.0f, 5.0f));
}

//************************************************************
//************ EXECUTION *************************************
//************************************************************

bool DEMO_APP::Run()
{
	float time = (float)clock.TotalTime();
	// Set Star Matrix	
	SetStarMat(time);

	// Camera Movement
	camera.SetCameraMat(scene.ViewMatrix);
	camera.MoveCamera();
	camera.RotateCamera();
	scene.ViewMatrix = camera.GetCameraMat();

	// TODO: PART 1 STEP 7a
	devContext->OMSetRenderTargets(1, &RTV, DSV);

	// TODO: PART 1 STEP 7b
	devContext->RSSetViewports(1, &viewport);
	
	// TODO: PART 1 STEP 7c
	float color[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	devContext->ClearDepthStencilView(DSV, D3D11_CLEAR_DEPTH, 1, 0);
	devContext->ClearRenderTargetView(RTV, color);
	
	// Constant
	unsigned int size = sizeof(SEND_TO_VRAM);
	unsigned int offset = 0;
	D3D11_MAPPED_SUBRESOURCE map;
	devContext->Map(Cbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, &toShader, size);
	devContext->Unmap(Cbuffer, 0);
	devContext->VSSetConstantBuffers(0, 1, &Cbuffer);

	// Object (Star)
	size = sizeof(Object);
	devContext->Map(Starbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, &star, size);
	devContext->Unmap(Starbuffer, 0);
	devContext->VSSetConstantBuffers(1, 1, &Starbuffer);

	// Scene
	size = sizeof(Scene);
	devContext->Map(Scenebuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, &scene, size);
	devContext->Unmap(Scenebuff, 0);
	devContext->VSSetConstantBuffers(2, 1, &Scenebuff);

	// Star Bindings
	size = sizeof(SIMPLE_VERTEX);
	devContext->IASetVertexBuffers(0, 1, &starVBuff, &size, &offset);
	devContext->IASetIndexBuffer(StarIndexbuff, DXGI_FORMAT_R32_UINT, 0);

	// Shaders
	devContext->VSSetShader(VertShader, nullptr, 0);
	devContext->PSSetShader(PixelShader, nullptr, 0);

	// Topology
	devContext->IASetInputLayout(input);
	devContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Draw Star
	devContext->DrawIndexed(60, 0, 0);

	// Object (Ground)
	size = sizeof(Object);
	devContext->Map(Groundbuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, &ground, size);
	devContext->Unmap(Groundbuff, 0);
	devContext->VSSetConstantBuffers(1, 1, &Groundbuff);
	// Ground Bindings
	size = sizeof(SIMPLE_VERTEX);
	devContext->IASetVertexBuffers(0, 1, &GroundVBuff, &size, &offset);
	devContext->IASetIndexBuffer(GndIndexbuff, DXGI_FORMAT_R32_UINT, 0);
	// Draw Ground
	devContext->DrawIndexed(36, 0, 0);

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
	RELEASE(Cbuffer);
	RELEASE(Scenebuff);
	RELEASE(Zbuffer)
	RELEASE(StarIndexbuff);
	RELEASE(Starbuffer);
	RELEASE(starVBuff);
	RELEASE(GndIndexbuff);
	RELEASE(Groundbuff);
	RELEASE(GroundVBuff);

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

void DEMO_APP::SetSwapChain()
{
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
}
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
void DEMO_APP::SetStarMat(float Time)
{
	XMStoreFloat4x4(&star.WorldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&star.WorldMatrix, XMMatrixRotationY(Time));
}
void DEMO_APP::CreateStar()
{
	// TODO: PART 2 STEP 3a
	int index = 1;
	for (int i = 0; i < 360; i += 36)
	{
		// Position
		starverts[index].x = (float)cosf(i * 0.01745277777777777777777777777778f);
		starverts[index].y = (float)sinf(i * 0.01745277777777777777777777777778f);
		starverts[index].z = 0;
		starverts[index].w = 1.0f;
		// Color
		starverts[index].color[0] = 0.0f;
		starverts[index].color[1] = 0.0f;
		starverts[index].color[2] = 0.0f;
		starverts[index].color[3] = 1.0f;
		// Inner Vertices
		if (index % 2 == 0)
		{
			starverts[index].x /= 2;
			starverts[index].y /= 2;
			starverts[index].color[0] = 0.0f;
			starverts[index].color[1] = 0.0f;
			starverts[index].color[2] = 0.0f;
			starverts[index].color[3] = 1.0f;
		}
		++index;
	}
	starverts[0].x = 0.0f;
	starverts[0].y = 0.0f;
	starverts[0].z = 0.4f;
	starverts[0].w = 1.0f;
	starverts[11].x = 0.0f;
	starverts[11].y = 0.0f;
	starverts[11].z = -0.4f;
	starverts[11].w = 1.0f;
	starverts[0].color[0] = 1.0f;
	starverts[0].color[1] = 1.0f;
	starverts[0].color[2] = 0.0f;
	starverts[0].color[3] = 1.0f;
	starverts[11].color[0] = 1.0f;
	starverts[11].color[1] = 1.0f;
	starverts[11].color[2] = 0.0f;
	starverts[11].color[3] = 1.0f;
}
void DEMO_APP::CreateGround()
{
	// Ground Matrix
	XMStoreFloat4x4(&ground.WorldMatrix, XMMatrixIdentity());

	// Top Face
	groundverts[0].x = -16.0f;
	groundverts[0].y = -2.0f;
	groundverts[0].z = -16.0f;
	groundverts[0].w = 1.0f;

	groundverts[1].x = -16.0f;
	groundverts[1].y = -2.0f;
	groundverts[1].z = 16.0f;
	groundverts[1].w = 1.0f;

	groundverts[2].x = 16.0f;
	groundverts[2].y = -2.0f;
	groundverts[2].z = 16.0f;
	groundverts[2].w = 1.0f;

	groundverts[3].x = 16.0f;
	groundverts[3].y = -2.0f;
	groundverts[3].z = -16.0f;
	groundverts[3].w = 1.0f;

	// Bottom Face
	groundverts[4].x = -16.0f;
	groundverts[4].y = -3.0f;
	groundverts[4].z = -16.0f;
	groundverts[4].w = 1.0f;

	groundverts[5].x = -16.0f;
	groundverts[5].y = -3.0f;
	groundverts[5].z = 16.0f;
	groundverts[5].w = 1.0f;

	groundverts[6].x = 16.0f;
	groundverts[6].y = -3.0f;
	groundverts[6].z = 16.0f;
	groundverts[6].w = 1.0f;

	groundverts[7].x = 16.0f;
	groundverts[7].y = -3.0f;
	groundverts[7].z = -16.0f;
	groundverts[7].w = 1.0f;

	// Colors
	for (unsigned int i = 0; i < 4; i++)
	{
		for (unsigned int j = 0; j < 4; j++)
		{
			groundverts[i].color[j] = 0.6f;
			groundverts[i+4].color[j] = 0.2f;
		}
	}

	unsigned int indeces[] =
	{ 0, 1, 2,
	0, 2, 3,
	6, 5, 4,
	7, 6, 4,

	4, 1, 0,
	5, 4, 1,

	6, 2, 1,
	5, 6, 1,

	6, 7, 2,
	7, 3, 2,

	3, 7, 0,
	7, 4, 0};

	// TODO: PART 2 STEP 3c
	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = groundverts;
	data.SysMemPitch = NULL;
	data.SysMemSlicePitch = NULL;

	// Vertex buffer
	D3D11_BUFFER_DESC GndbuffDesc;
	ZeroMemory(&GndbuffDesc, sizeof(GndbuffDesc));
	GndbuffDesc.ByteWidth = sizeof(SIMPLE_VERTEX) * ARRAYSIZE(groundverts);
	GndbuffDesc.Usage = D3D11_USAGE_DEFAULT;
	GndbuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	GndbuffDesc.CPUAccessFlags = D3D11_USAGE_DEFAULT;
	GndbuffDesc.StructureByteStride = 0;

	CHECK(device->CreateBuffer(&GndbuffDesc, &data, &GroundVBuff));

	// Index buffer
	data.pSysMem = indeces;
	ZeroMemory(&GndIbuffDesc, sizeof(GndIbuffDesc));
	GndIbuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	GndIbuffDesc.ByteWidth = sizeof(indeces);
	GndIbuffDesc.CPUAccessFlags = NULL;
	GndIbuffDesc.Usage = D3D11_USAGE_IMMUTABLE;

	CHECK(device->CreateBuffer(&GndIbuffDesc, &data, &GndIndexbuff));
}
void DEMO_APP::SetBuffers()
{
	// Constant Buffer
	ZeroMemory(&CbuffDesc, sizeof(CbuffDesc));
	CbuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	CbuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	CbuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	CbuffDesc.ByteWidth = sizeof(SEND_TO_VRAM);

	CHECK(device->CreateBuffer(&CbuffDesc, nullptr, &Cbuffer));

	// Star Buffer
	ZeroMemory(&StarbuffDesc, sizeof(StarbuffDesc));
	StarbuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	StarbuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	StarbuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	StarbuffDesc.ByteWidth = sizeof(Object);

	CHECK(device->CreateBuffer(&StarbuffDesc, nullptr, &Starbuffer));
	
	// Ground Buffer
	ZeroMemory(&GndbuffDesc, sizeof(GndbuffDesc));
	GndbuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	GndbuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	GndbuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GndbuffDesc.ByteWidth = sizeof(Object);

	CHECK(device->CreateBuffer(&GndbuffDesc, nullptr, &Groundbuff));

	// Scene Buffer (View/Projection)
	ZeroMemory(&ScnebuffDesc, sizeof(ScnebuffDesc));
	ScnebuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ScnebuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	ScnebuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ScnebuffDesc.ByteWidth = sizeof(Scene);

	CHECK(device->CreateBuffer(&ScnebuffDesc, nullptr, &Scenebuff));

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

	CHECK(device->CreateTexture2D(&ZbuffDesc, 0, &Zbuffer));
	CHECK(device->CreateDepthStencilView(Zbuffer, nullptr, &DSV));
}

template<typename Type>
void DEMO_APP<Type>::SetVertBuffer(ID3D11Buffer *vertbuff, vector<Type> verts)
{
	// TODO: PART 2 STEP 3c
	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = verts;
	data.SysMemPitch = NULL;
	data.SysMemSlicePitch = NULL;

	// Vertex buffer
	D3D11_BUFFER_DESC ObjbuffDesc;
	ZeroMemory(&ObjbuffDesc, sizeof(ObjbuffDesc));
	ObjbuffDesc.ByteWidth = sizeof(SIMPLE_VERTEX) * numverts;
	ObjbuffDesc.Usage = D3D11_USAGE_DEFAULT;
	ObjbuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	ObjbuffDesc.CPUAccessFlags = D3D11_USAGE_DEFAULT;
	ObjbuffDesc.StructureByteStride = 0;

	CHECK(device->CreateBuffer(&ObjbuffDesc, &data, &vertbuff));

	// Index buffer
	data.pSysMem = indices;
	ZeroMemory(&StarIbuffDesc, sizeof(StarIbuffDesc));
	StarIbuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	StarIbuffDesc.ByteWidth = sizeof(indices);
	StarIbuffDesc.CPUAccessFlags = NULL;
	StarIbuffDesc.Usage = D3D11_USAGE_IMMUTABLE;

	CHECK(device->CreateBuffer(&StarIbuffDesc, &data, &StarIndexbuff));
}
