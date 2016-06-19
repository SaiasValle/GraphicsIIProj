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
#include "DEMO_APP.h"
#include <thread>
#include <mutex>

//************************************************************
//************ CREATION OF OBJECTS & RESOURCES ***************
//************************************************************
float trans = 0.0f;

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
	// Creating Star, Ground, Skybox
	CreateStar();
	CreateGround();
	CreateSkybox();
	// Models
	moon.LoadFromFile("moon1.obj", device);
	moon.ScaleModel(XMFLOAT3(0.24f, 0.24f, 0.24f));

	vette.LoadFromFile("corvette.obj", device);
	vette.RotateModel(XMFLOAT3(0.0f, -2.6f, 0.0f));
	vette.ScaleModel(XMFLOAT3(0.48f, 0.48f, 0.48f));
	vette.TranslateModel(XMFLOAT3(-10.5f, -2.0f, 11.0f));

	koenigsegg.LoadFromFile("koenigsegg.obj", device);
	koenigsegg.RotateModel(XMFLOAT3(0.0f, -0.9f, 0.0f));
	koenigsegg.TranslateModel(XMFLOAT3(-2.0f, -1.8f, 10.0f));

	// Multi-threading
	thread thread1, thread2, thread3;
	thread1 = thread(CreateDDSTextureFromFile, device, L"vette.dds", nullptr, vette.GetSRV(),0);
	thread2 = thread(CreateDDSTextureFromFile, device, L"MoonMap.dds", nullptr, moon.GetSRV(), 0);
	thread3 = thread(CreateDDSTextureFromFile, device, L"carbonfiber.dds", nullptr, koenigsegg.GetSRV(), 0);
	thread1.join();
	thread2.join();
	thread3.join();

	// Set Buffers (Zbuffer, Constant, etc.)
	Initialize();
	InitializeLights();

	// Standard Vertex Layout
	D3D11_INPUT_ELEMENT_DESC vertLayout[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	// Skybox Vertex Layout
	D3D11_INPUT_ELEMENT_DESC SkyboxVLayout[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UVW", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	// ADD SHADERS TO PROJECT, SET BUILD OPTIONS & COMPILE
	// Create Shaders
	CHECK(device->CreateVertexShader(Trivial_VS, sizeof(Trivial_VS), nullptr, &VertShader));
	CHECK(device->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), nullptr, &PixelShader));
	// Sky
	CHECK(device->CreateVertexShader(Skybox_VS, sizeof(Skybox_VS), nullptr, &SkyboxVShader));
	CHECK(device->CreatePixelShader(Skybox_PS, sizeof(Skybox_PS), nullptr, &SkyboxPShader));
	// Lighting
	CHECK(device->CreatePixelShader(Lights_PS, sizeof(Lights_PS), nullptr, &LightingPS));

	// Create Input Layouts
	CHECK(device->CreateInputLayout(vertLayout, ARRAYSIZE(vertLayout), Trivial_VS, sizeof(Trivial_VS), &input));
	CHECK(device->CreateInputLayout(SkyboxVLayout, ARRAYSIZE(SkyboxVLayout), Skybox_VS, sizeof(Skybox_VS), &Skyboxinput));

	// Textures
	CHECK(CreateDDSTextureFromFile(device, L"City.dds", nullptr, &SkySRV));
	CHECK(CreateDDSTextureFromFile(device, L"ground.dds", nullptr, &GroundSRV));

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
	// Set Star Matrix	
	SetStarMat((float)clock.TotalTime());

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
	float color[4] = { 0.4f, 0.1f, 0.1f, 1.0f };
	devContext->ClearDepthStencilView(DSV, D3D11_CLEAR_DEPTH, 1, 0);
	devContext->ClearRenderTargetView(RTV, color);

	devContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	// Constant
	unsigned int size;
	unsigned int offset = 0;
	D3D11_MAPPED_SUBRESOURCE map;

	// Scene
	size = sizeof(Scene);
	devContext->Map(SceneCbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, &scene, size);
	devContext->Unmap(SceneCbuffer, 0);
	devContext->VSSetConstantBuffers(1, 1, &SceneCbuffer);

	// Skybox
#if 1
	// Shaders
	devContext->VSSetShader(SkyboxVShader, nullptr, 0);
	devContext->PSSetShader(SkyboxPShader, nullptr, 0);
	// Input Layout
	devContext->IASetInputLayout(Skyboxinput);
	// Texture
	devContext->PSSetShaderResources(0, 1, &SkySRV);

	XMMATRIX caminverse = XMMatrixInverse(0, XMLoadFloat4x4(&camera.GetCameraMat()));

	Skybox.WorldMatrix._41 = caminverse.r[3].m128_f32[0];
	Skybox.WorldMatrix._42 = caminverse.r[3].m128_f32[1];
	Skybox.WorldMatrix._43 = caminverse.r[3].m128_f32[2];

	size = sizeof(Object);
	devContext->Map(SkyCbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, &Skybox, size);
	devContext->Unmap(SkyCbuffer, 0);
	devContext->VSSetConstantBuffers(0, 1, &SkyCbuffer);
	// Skybox Bindings
	size = sizeof(SIMPLE_VERTEX);
	devContext->IASetVertexBuffers(0, 1, &SkyVbuffer, &size, &offset);
	devContext->IASetIndexBuffer(SkyIbuffer, DXGI_FORMAT_R32_UINT, 0);
	// Draw Skybox
	devContext->RSSetState(RastStatetoggled);
	devContext->DrawIndexed(36, 0, 0);
	devContext->RSSetState(RastState);
	devContext->ClearDepthStencilView(DSV, D3D11_CLEAR_DEPTH, 1, 0);
#endif
	// Standard Shaders
	devContext->VSSetShader(VertShader, nullptr, 0);
	devContext->PSSetShader(LightingPS, nullptr, 0);
	// Standard Input Layout
	devContext->IASetInputLayout(input);
	// Lighting
#if 1
	// Point Light Constant Buffer
	size = sizeof(PointLight);
	devContext->Map(PlightCbuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, &Plight, size);
	devContext->Unmap(PlightCbuff, 0);
	devContext->PSSetConstantBuffers(1, 1, &PlightCbuff);
	// Direction Light Constant Buffer
	size = sizeof(DirectionalLight);
	devContext->Map(DlightCbuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, &Dlight, size);
	devContext->Unmap(DlightCbuff, 0);
	devContext->PSSetConstantBuffers(2, 1, &DlightCbuff);
	// Spot Light Constant Buffer
	size = sizeof(SpotLight);
	devContext->Map(SlightCbuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, &Slight, size);
	devContext->Unmap(SlightCbuff, 0);
	devContext->PSSetConstantBuffers(3, 1, &SlightCbuff);

	ToggleLights();
	MoveLights();
#endif
	devContext->PSSetShader(PixelShader, nullptr, 0);
	// Object (Star)
#if 1
	size = sizeof(Object);
	devContext->Map(StarCbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, &star, size);
	devContext->Unmap(StarCbuffer, 0);
	devContext->VSSetConstantBuffers(0, 1, &StarCbuffer);
	// Star Bindings
	size = sizeof(SIMPLE_VERTEX);
	devContext->IASetVertexBuffers(0, 1, &StarVbuffer, &size, &offset);
	devContext->IASetIndexBuffer(StarIndexbuff, DXGI_FORMAT_R32_UINT, 0);
	// Draw Star
	devContext->DrawIndexed(60, 0, 0);
#endif
	// Object (Ground)
#if 1
	devContext->PSSetShader(LightingPS, nullptr, 0);
	// Texture
	devContext->PSSetShaderResources(0, 1, &GroundSRV);

	size = sizeof(Object);
	devContext->Map(GroundCbuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, &ground, size);
	devContext->Unmap(GroundCbuff, 0);
	devContext->VSSetConstantBuffers(0, 1, &GroundCbuff);
	// Ground Bindings
	size = sizeof(SIMPLE_VERTEX);
	devContext->IASetVertexBuffers(0, 1, &GroundVbuff, &size, &offset);
	devContext->IASetIndexBuffer(GndIndexbuff, DXGI_FORMAT_R32_UINT, 0);
	// Draw Ground
	devContext->DrawIndexed(12, 0, 0);
#endif
	// Models
	vette.Draw(devContext);
	koenigsegg.Draw(devContext);
	if (Plight.color.w > 0.0f)
		moon.Draw(devContext);

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
	RELEASE(RastState);
	RELEASE(VertShader);
	RELEASE(PixelShader);
	RELEASE(SkyboxVShader);
	RELEASE(SkyboxPShader);
	RELEASE(LightingPS);
	RELEASE(input);
	RELEASE(swapchain);
	RELEASE(image);
	RELEASE(SceneCbuffer);
	RELEASE(Zbuffer)
	RELEASE(StarIndexbuff);
	RELEASE(StarCbuffer);
	RELEASE(StarVbuffer);
	RELEASE(GndIndexbuff);
	RELEASE(GroundCbuff);
	RELEASE(GroundVbuff);
	RELEASE(GroundSRV);
	RELEASE(SkyZbuffer);
	RELEASE(SkyVbuffer);
	RELEASE(SkyCbuffer);
	RELEASE(SkyIbuffer);
	RELEASE(Skyboxinput);
	RELEASE(SkySRV);
	RELEASE(RastState);
	RELEASE(RastStatetoggled);
	RELEASE(PlightCbuff);
	RELEASE(DlightCbuff);
	RELEASE(SlightCbuff);

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
	CHECK(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG, 0, 0, D3D11_SDK_VERSION, &swapDesc, &swapchain, &device, nullptr, &devContext));

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
	// Set Vertices
	int index = 1;
	SIMPLE_VERTEX currvert;
	// Back Vertex
	currvert.x = 0.0f;
	currvert.y = 0.0f;
	currvert.z = 0.4f;
	currvert.w = 1.0f;
	currvert.color[0] = 1.0f;
	currvert.color[1] = 1.0f;
	currvert.color[2] = 0.0f;
	currvert.color[3] = 1.0f;
	currvert.normal[0] = 0.0f;
	currvert.normal[1] = 0.0f;
	currvert.normal[2] = 1.0f;
	starverts.push_back(currvert);

	for (int i = 0; i < 360; i += 36, index++)
	{
		// Position
		currvert.x = (float)cosf(i * 0.01745277777777777777777777777778f);
		currvert.y = (float)sinf(i * 0.01745277777777777777777777777778f);
		currvert.z = 0;
		currvert.w = 1.0f;
		// Color
		currvert.color[0] = 0.0f;
		currvert.color[1] = 0.0f;
		currvert.color[2] = 0.0f;
		currvert.color[3] = 1.0f;
		currvert.normal[0] = 0.0f;
		currvert.normal[1] = 1.0f;
		currvert.normal[2] = 0.0f;
		// Inner Vertices
		if (index % 2 == 0)
		{
			currvert.x /= 2;
			currvert.y /= 2;
			currvert.color[0] = 0.0f;
			currvert.color[1] = 0.0f;
			currvert.color[2] = 0.0f;
			currvert.color[3] = 1.0f;
			currvert.normal[0] = 0.0f;
			currvert.normal[1] = -1.0f;
			currvert.normal[2] = 0.0f;
		}
		starverts.push_back(currvert);
	}

	// Front Vertex
	currvert.x = 0.0f;
	currvert.y = 0.0f;
	currvert.z = -0.4f;
	currvert.w = 1.0f;
	currvert.color[0] = 1.0f;
	currvert.color[1] = 1.0f;
	currvert.color[2] = 0.0f;
	currvert.color[3] = 1.0f;
	currvert.normal[0] = 0.0f;
	currvert.normal[1] = 0.0f;
	currvert.normal[2] = -1.0f;
	starverts.push_back(currvert);

	vector<unsigned int> starindeces =
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

	// Vertex Buffers
	SetVertBuffer(&StarVbuffer, starverts);
	// Index Buffers
	SetIndexBuffer(&StarIndexbuff, starindeces);
}
void DEMO_APP::CreateGround()
{
	// Ground Matrix
	XMStoreFloat4x4(&ground.WorldMatrix, XMMatrixIdentity());

	SIMPLE_VERTEX currvert;
	// Top Face
	currvert.x = -16.0f;
	currvert.y = -2.0f;
	currvert.z = -16.0f;
	currvert.w = 1.0f;
	currvert.uv[0] = 0;
	currvert.uv[1] = 1;
	currvert.normal[0] = 0.0f;
	currvert.normal[1] = 1.0f;
	currvert.normal[2] = 0.0f;
	groundverts.push_back(currvert);

	currvert.x = -16.0f;
	currvert.y = -2.0f;
	currvert.z = 16.0f;
	currvert.w = 1.0f;
	currvert.uv[0] = 0;
	currvert.uv[1] = 0;
	currvert.normal[0] = 0.0f;
	currvert.normal[1] = 1.0f;
	currvert.normal[2] = 0.0f;
	groundverts.push_back(currvert);

	currvert.x = 16.0f;
	currvert.y = -2.0f;
	currvert.z = 16.0f;
	currvert.w = 1.0f;
	currvert.uv[0] = 1;
	currvert.uv[1] = 0;
	currvert.normal[0] = 0.0f;
	currvert.normal[1] = 1.0f;
	currvert.normal[2] = 0.0f;
	groundverts.push_back(currvert);

	currvert.x = 16.0f;
	currvert.y = -2.0f;
	currvert.z = -16.0f;
	currvert.w = 1.0f;
	currvert.uv[0] = 1;
	currvert.uv[1] = 1;
	currvert.normal[0] = 0.0f;
	currvert.normal[1] = 1.0f;
	currvert.normal[2] = 0.0f;
	groundverts.push_back(currvert);

	for (int i = 0; i < 4; i++)
	{
		currvert = groundverts[i];
		currvert.normal[1] = -1.0f;
		groundverts.push_back(currvert);
	}
#if 0
	// Bottom Face
	currvert.x = -16.0f;
	currvert.y = -3.0f;
	currvert.z = -16.0f;
	currvert.w = 1.0f;
	currvert.uv[0] = 0;
	currvert.uv[1] = 1;
	groundverts.push_back(currvert);

	currvert.x = -16.0f;
	currvert.y = -3.0f;
	currvert.z = 16.0f;
	currvert.w = 1.0f;
	groundverts.push_back(currvert);

	currvert.x = 16.0f;
	currvert.y = -3.0f;
	currvert.z = 16.0f;
	currvert.w = 1.0f;
	groundverts.push_back(currvert);

	currvert.x = 16.0f;
	currvert.y = -3.0f;
	currvert.z = -16.0f;
	currvert.w = 1.0f;
	groundverts.push_back(currvert);

	vector<unsigned int> indeces =
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
#endif
	// Colors
	for (unsigned int i = 0; i < 4; i++)
	{
		for (unsigned int j = 0; j < 4; j++)
		{
			groundverts[i].color[j] = 0.6f;
		}
	}
	vector<unsigned int> indeces =
	{ 0, 1, 2,
	0, 2, 3,
	4, 7, 6,
	6, 5, 4 };

	SetVertBuffer(&GroundVbuff, groundverts);
	SetIndexBuffer(&GndIndexbuff, indeces);
}
void DEMO_APP::CreateSkybox()
{
	XMStoreFloat4x4(&Skybox.WorldMatrix, XMMatrixIdentity());

	SIMPLE_VERTEX currvert;
	// Top Face
	currvert.x = -24.0f;
	currvert.y = 24.0f;
	currvert.z = -24.0f;
	currvert.w = 1.0f;

	skyboxverts.push_back(currvert);

	currvert.x = -24.0f;
	currvert.y = 24.0f;
	currvert.z = 24.0f;
	currvert.w = 1.0f;
	skyboxverts.push_back(currvert);

	currvert.x = 24.0f;
	currvert.y = 24.0f;
	currvert.z = 24.0f;
	currvert.w = 1.0f;
	skyboxverts.push_back(currvert);

	currvert.x = 24.0f;
	currvert.y = 24.0f;
	currvert.z = -24.0f;
	currvert.w = 1.0f;
	skyboxverts.push_back(currvert);

	// Bottom Face
	currvert.x = -24.0f;
	currvert.y = -24.0f;
	currvert.z = -24.0f;
	currvert.w = 1.0f;
	skyboxverts.push_back(currvert);

	currvert.x = -24.0f;
	currvert.y = -24.0f;
	currvert.z = 24.0f;
	currvert.w = 1.0f;
	skyboxverts.push_back(currvert);

	currvert.x = 24.0f;
	currvert.y = -24.0f;
	currvert.z = 24.0f;
	currvert.w = 1.0f;
	skyboxverts.push_back(currvert);

	currvert.x = 24.0f;
	currvert.y = -24.0f;
	currvert.z = -24.0f;
	currvert.w = 1.0f;
	skyboxverts.push_back(currvert);

	// Colors
	for (unsigned int i = 0; i < 8; i++)
	{
		for (unsigned int j = 0; j < 4; j++)
		{
			skyboxverts[i].color[1] = 1.0f;
		}
	}

	vector<unsigned int> indeces =
	{ 0, 1, 2,
	2, 3, 0,
	4, 5, 6,
	6, 7, 4,
	0, 3, 7,
	7, 4, 0,
	1, 0, 4,
	4, 5, 1,
	1, 2, 6,
	6, 5, 1,
	3, 2, 6,
	6, 7, 3 };

	SetVertBuffer(&SkyVbuffer, skyboxverts);
	SetIndexBuffer(&SkyIbuffer, indeces);
}
void DEMO_APP::Initialize()
{
	// Raster State (Normal)
	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(rasterDesc));
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.DepthBias = 0;
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.ScissorEnable = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.AntialiasedLineEnable = false;
	
	CHECK(device->CreateRasterizerState(&rasterDesc, &RastState));
	
	// Raster State (Toggled)
	rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(rasterDesc));
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.DepthBias = 0;
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.ScissorEnable = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.AntialiasedLineEnable = false;
	
	CHECK(device->CreateRasterizerState(&rasterDesc, &RastStatetoggled));

	// Skybox Constant Buffer
	SetConstBuffer(&SkyCbuffer, Skybox);

	// Star Constant Buffer
	SetConstBuffer(&StarCbuffer, star);
	
	// Ground Constant Buffer
	SetConstBuffer(&GroundCbuff, ground);

	// Scene Constant Buffer (View/Projection)
	SetConstBuffer(&SceneCbuffer, scene);
	
	// Lighting Constant Buffer
	SetConstBuffer(&PlightCbuff, Plight);
	SetConstBuffer(&DlightCbuff, Dlight);
	SetConstBuffer(&SlightCbuff, Slight);

	// Z buffer
	D3D11_TEXTURE2D_DESC ZbuffDesc;
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
void DEMO_APP::InitializeLights()
{
	// Point Light
	Plight.color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	Plight.position = XMFLOAT4(0.0f, -1.0f, 5.0f, 0.0f);
	// Directional Light
	Dlight.color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	Dlight.direction = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	// Spot Light
	Slight.color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	Slight.position = XMFLOAT4(0.0f, 8.0f, 0.0f, 0.0f);
	Slight.direction = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	Slight.spotRatio.x = 0.98f;
	Slight.spotRatio.y = 0.9f;
}
void DEMO_APP::ToggleLights()
{
	// Point Light
	if (GetAsyncKeyState('1') & 0x1)
	{
		if (Plight.color.x <= 0.0f)
		{
			Plight.color = XMFLOAT4(0.6f, 0.6f, 1.0f, 1.0f);
		}
		else if (Plight.color.x >= 0.1f)
			Plight.color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	// Directional Light
	if (GetAsyncKeyState('2') & 0x1)
	{
		if (Dlight.color.w <= 0.0f)
			Dlight.color = XMFLOAT4(0.6f, 0.6f, 0.4f, 1.0f);
		else if (Dlight.color.w >= 0.1f)
			Dlight.color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	// Spot Light
	if (GetAsyncKeyState('3') & 0x1)
	{
		if (Slight.color.w <= 0.0f)
			Slight.color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
		else if (Slight.color.w >= 0.1f)
			Slight.color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}
}
void DEMO_APP::MoveLights()
{
	// Point Light
#if 1
	if (GetAsyncKeyState('I'))
	{
		Plight.position.z += 0.02f;
	}
	if (GetAsyncKeyState('K'))
	{
		Plight.position.z += -0.02f;
	}
	if (GetAsyncKeyState('J'))
	{
		Plight.position.x += -0.02f;
	}
	if (GetAsyncKeyState('L'))
	{
		Plight.position.x += 0.02f;
	}
	if (GetAsyncKeyState('U'))
	{
		Plight.position.y += 0.02f;
	}
	if (GetAsyncKeyState('O'))
	{
		Plight.position.y -= 0.02f;
	}
	if (GetAsyncKeyState('P'))
	{
		Plight.position = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	}
	moon.GetWorldMatrix()->_41 = Plight.position.x;
	moon.GetWorldMatrix()->_42 = Plight.position.y;
	moon.GetWorldMatrix()->_43 = Plight.position.z;
#endif
	// Directional Light
#if 1
	trans += 0.0002f;
	if (trans > 2.0f)
	{
		trans = -1.9f;
	}
	else if (trans < -2.0f)
	{
		trans = 1.0f;
	}
	Dlight.direction = XMFLOAT4(trans, -1.0f, 0.0f, 0.0f);
#endif
	// Spot Light
#if 1
	XMFLOAT4X4 camin;
	XMStoreFloat4x4(&camin, XMMatrixInverse(0, XMLoadFloat4x4(&camera.GetCameraMat())));

	Slight.position.x = camin._41;
	Slight.position.y = camin._42;
	Slight.position.z = camin._43;

	Slight.direction.x = camin._31;
	Slight.direction.y = camin._32;
	Slight.direction.z = camin._33;
#endif
}
void DEMO_APP::Draw(Object *obj, ID3D11Buffer **vertbuff, ID3D11Buffer *indexbuff, ID3D11Buffer *constbuff, unsigned int numindices)
{
	unsigned int size = 0;
	unsigned int offset = 0;
	D3D11_MAPPED_SUBRESOURCE map;

	// Object 
	size = sizeof(Object);
	devContext->Map(constbuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, &obj, size);
	devContext->Unmap(constbuff, 0);
	devContext->VSSetConstantBuffers(1, 1, &constbuff);

	// Bindings
	size = sizeof(SIMPLE_VERTEX);
	devContext->IASetVertexBuffers(0, 1, vertbuff, &size, &offset);
	devContext->IASetIndexBuffer(indexbuff, DXGI_FORMAT_R32_UINT, 0);

	// Draw Ground
	devContext->DrawIndexed(numindices, 0, 0);
}

template<typename Type>
void DEMO_APP::SetVertBuffer(ID3D11Buffer **vertbuff, vector<Type> verts)
{
	// TODO: PART 2 STEP 3c
	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = verts.data();
	data.SysMemPitch = NULL;
	data.SysMemSlicePitch = NULL;

	// Vertex buffer
	D3D11_BUFFER_DESC ObjbuffDesc;
	ZeroMemory(&ObjbuffDesc, sizeof(ObjbuffDesc));
	ObjbuffDesc.ByteWidth = sizeof(Type) * verts.size();
	ObjbuffDesc.Usage = D3D11_USAGE_DEFAULT;
	ObjbuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	ObjbuffDesc.CPUAccessFlags = D3D11_USAGE_DEFAULT;
	ObjbuffDesc.StructureByteStride = 0;

	CHECK(device->CreateBuffer(&ObjbuffDesc, &data, vertbuff));
}
template<typename Type>
void DEMO_APP::SetIndexBuffer(ID3D11Buffer **indexBuff, vector<Type> indices)
{
	// Index buffer
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = indices.data();
	data.SysMemPitch = NULL;
	data.SysMemSlicePitch = NULL;

	D3D11_BUFFER_DESC IbuffDesc;
	ZeroMemory(&IbuffDesc, sizeof(IbuffDesc));
	IbuffDesc.ByteWidth = sizeof(Type) * indices.size();
	IbuffDesc.Usage = D3D11_USAGE_IMMUTABLE;
	IbuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IbuffDesc.CPUAccessFlags = D3D11_USAGE_DEFAULT;
	IbuffDesc.StructureByteStride = 0;

	CHECK(device->CreateBuffer(&IbuffDesc, &data, indexBuff));
}
template <typename Type>
void DEMO_APP::SetConstBuffer(ID3D11Buffer **constbuff, Type size)
{
	// Constant Buffer
	D3D11_BUFFER_DESC ConstbuffDesc;
	ZeroMemory(&ConstbuffDesc, sizeof(ConstbuffDesc));
	ConstbuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstbuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	ConstbuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ConstbuffDesc.ByteWidth = sizeof(Type);

	CHECK(device->CreateBuffer(&ConstbuffDesc, nullptr, constbuff));
}
