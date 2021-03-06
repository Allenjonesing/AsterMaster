#include "DirectXFramework.h"

DXGame::DXGame(void)
{
	m_bVsync		= false;
	xObject	= 0;
	xDevice	= 0;
	xBackCol	= D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	ZeroMemory(&x_3D, sizeof(x_3D));  

}

DXGame::~DXGame(void)
{
	Shutdown();
}

void DXGame::Init(HWND& hWnd, HINSTANCE& hInst, bool bWindowed)
{

	m_hWnd = hWnd;
	GameStates gamestate = GameStates::LOAD;
	partCount=0;

	InitFoundations( hWnd, hInst, bWindowed);
	InitCamera( hWnd, hInst);
	InitVertex( hWnd, hInst);
	InitFont( hWnd, hInst);
	InitSprite( hWnd, hInst);
	InitBrain();
	InitDirectInput( hWnd, hInst);
	InitFMOD( hWnd, hInst);
	result = system->playSound(FMOD_CHANNEL_FREE, sound[0], false, &channel[0]); 
	ERRCHECK(result);		

	//InitDShow( hWnd, hInst);
	//InitLight( hWnd, hInst);
//	InitMaterial( hWnd, hInst);
	//InitShader( hWnd, hInst);
	//InitMesh( hWnd, hInst);
}

void DXGame::InitVertex(HWND& hWnd, HINSTANCE& hInst)
{
	D3DVERTEXELEMENT9 decl[] =
	{
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	xDevice->CreateVertexDeclaration(decl, &xVdecl);

}

void DXGame::InitCamera(HWND& hWnd, HINSTANCE& hInst)
{
	eyePos	= D3DXVECTOR3(0.0f, 2.0f, -10.0f);	// Camera position
	lookAt	= D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// Pos. camera is viewing
	upVec		= D3DXVECTOR3(0.0f, 1.0f, 0.0f);	// Rotational orientation 

	D3DXMatrixLookAtLH(
		&viewMat,	// Returned viewMat
		&eyePos,	// Eye Position
		&lookAt,	// LookAt Position
		&upVec);	// Up Vector

	D3DXMatrixPerspectiveFovLH(
		&projMat,						// Returned ProjMat
		D3DXToRadian(65.0f),			// Field of View
		(float)m_width / (float)m_height,	// Aspect Ratio
		1.0f,							// Near Plane
		2000.0f);						// Far Plane

}
Eavoid DXGame::DecideBaye(AIBot bot, SpaceRock target)
{
		//determiene sets
	size2 = ( target.size > bot.brain.SMALLSIZE ) ? MEDSI : SMALL;
	if (size2==MEDSI)
		size2 = (  target.size > bot.brain.MEDSIZE ) ? BIG : MEDSI;

	speed2 = ( bot.ship.sprite.speed > bot.brain.SLOWSPEED) ? MEDSP : SLOW;
	if (speed2==MEDSP)
		speed2 = ( bot.ship.sprite.speed > bot.brain.MEDSPEED ) ? FAST : MEDSP;
	
	temp2 = (  target.temp > bot.brain.COLDTEMP) ? NORM : COLD;
	if (temp2==NORM)
		temp2 = ( target.temp > bot.brain.NORMTEMP ) ? HOT : NORM;

	//determine probabilties
	float avoidProb = 
		SizeProb( size2, AVOID ) *
		SpeedProb( speed2, AVOID ) * 
		TempProb( temp2, AVOID ) * 
		AvoidProb( AVOID );

	float collideProb = 
		SizeProb( size2, COLLIDE ) * 
		SpeedProb( speed2, COLLIDE ) * 
		TempProb( temp2, COLLIDE ) * 
		AvoidProb( COLLIDE );

	if ( avoidProb > collideProb )
		return AVOID;
	else
		return COLLIDE;
}


void DXGame::InitFoundations(HWND& hWnd, HINSTANCE& hInst, bool bWindowed)
{
		m_hWnd = hWnd;

	// Create the D3D Object
	xObject = Direct3DCreate9(D3D_SDK_VERSION);

	// Find the width and height of window using hWnd and GetClientRect()
	GetClientRect(hWnd, &screenRect);
	m_width = screenRect.right - screenRect.left;
	m_height = screenRect.bottom - screenRect.top;

	// Set D3D Device presentation parameters before creating the device
	x_3D.hDeviceWindow				= hWnd;										// Handle to the focus window
	x_3D.Windowed					= bWindowed;								// Windowed or Full-screen boolean
	x_3D.AutoDepthStencilFormat		= D3DFMT_D24S8;								// Format of depth/stencil buffer, 24 bit depth, 8 bit stencil
	x_3D.EnableAutoDepthStencil		= TRUE;										// Enables Z-Buffer (Depth Buffer)
	x_3D.BackBufferCount				= 1;										// Change if need of > 1 is required at a later date
	x_3D.BackBufferFormat			= bWindowed?D3DFMT_UNKNOWN:D3DFMT_X8R8G8B8;	// Back-buffer format, 8 bits for Red, Green, and Blue for each pixel
	x_3D.BackBufferHeight			= m_height;									// Make sure resolution is supported, use adapter modes
	x_3D.BackBufferWidth				= m_width;									// (Same as above)
	x_3D.SwapEffect					= D3DSWAPEFFECT_DISCARD;					// Discard back-buffer, must stay discard to support multi-sample
	x_3D.PresentationInterval		= m_bVsync ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE; // Present back-buffer immediately, unless V-Sync is on								
	x_3D.Flags						= D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;		// This flag should improve performance, if not set to NULL.
	x_3D.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;					// Full-screen refresh rate, use adapter modes or default
	x_3D.MultiSampleQuality			= 0;										// MSAA currently off, check documentation for support.
	x_3D.MultiSampleType				= D3DMULTISAMPLE_NONE;						// MSAA currently off, check documentation for support.

	DWORD deviceBehaviorFlags = 0;
	HR(xObject->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &xCaps));

	// Determine vertex processing mode
	if(xCaps.DevCaps & D3DCREATE_HARDWARE_VERTEXPROCESSING)
	{
		// Hardware vertex processing supported? (Video Card)
		deviceBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;	
	}
	else
	{
		// If not, use software (CPU)
		deviceBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING; 
	}
	
	if(xCaps.DevCaps & D3DDEVCAPS_PUREDEVICE && deviceBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING)
	{
		deviceBehaviorFlags |= D3DCREATE_PUREDEVICE;	
	}
	
	HR(xObject->CreateDevice(
		D3DADAPTER_DEFAULT,			// which adapter to use, set to primary
		D3DDEVTYPE_HAL,				// device type to use, set to hardware rasterization
		hWnd,						// handle to the focus window
		deviceBehaviorFlags,		// behavior flags
		&x_3D,					// presentation parameters
		&xDevice));			// returned device pointer

}
	
void DXGame::InitFont(HWND& hWnd, HINSTANCE& hInst)
{
		m_hWnd = hWnd;
		
	AddFontResourceEx(L"Delicious-Roman.otf", FR_PRIVATE, 0);

	// Load D3DXFont, each font style you want to support will need an ID3DXFont
	D3DXCreateFont(xDevice, 30, 0, FW_BOLD, 0, false, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Delicious-Roman"), &xFont);


}

void DXGame::InitSprite(HWND& hWnd, HINSTANCE& hInst)
{
		m_hWnd = hWnd;
	// Create a sprite object, note you will only need one for all 2D sprites
	D3DXCreateSprite(xDevice, &xSpr);

	D3DXCreateTextureFromFileEx(xDevice, L"Images/paddle.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &infos[0], 0, &textures[0]);
	
	D3DXCreateTextureFromFileEx(xDevice, L"Images/test2.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &infos[1], 0, &textures[1]);

	D3DXCreateTextureFromFileEx(xDevice, L"Images/testship.bmp", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &infos[2], 0, &textures[2]);

	D3DXCreateTextureFromFileEx(xDevice, L"Images/playButt.bmp", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &infos[3], 0, &textures[3]);

	D3DXCreateTextureFromFileEx(xDevice, L"Images/quitButt.bmp", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &infos[4], 0, &textures[4]);

	D3DXCreateTextureFromFileEx(xDevice, L"Images/aboutButt.bmp", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &infos[5], 0, &textures[5]);

	D3DXCreateTextureFromFileEx(xDevice, L"Images/partical.bmp", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &infos[6], 0, &textures[6]);

	D3DXCreateTextureFromFileEx(xDevice, L"Images/bullet.bmp", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &infos[7], 0, &textures[7]);

	D3DXCreateTextureFromFileEx(xDevice, L"Images/aster.bmp", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &infos[8], 0, &textures[8]);

	srand(timeGetTime());

		InitPlayer();
		InitOpponent();
		InitMenu();
		InitAster();

		score[0]=0;
		score[1]=0;

}

void DXGame::InitBrain()
{
}
void DXGame::InitAster()
{
	for(int i=0;i<MAX_ASTER;++i)
	{
		asterObj[i].sprite.position = D3DXVECTOR3(100+m_width, (INT(rand() %m_height))+100, 0.0f); // Random position within screen width and height

		asterObj[i].sprite.rotation = D3DXToRadian(0);
		asterObj[i].sprite.scale = INT(1+rand() % 3);
		asterObj[i].sprite.w =30*asterObj[i].sprite.scale ;
		asterObj[i].sprite.h =30*asterObj[i].sprite.scale ;
		asterObj[i].sprite.BBox = CRect(asterObj[i].sprite,asterObj[i].sprite.position.x- (asterObj[i].sprite.w/2), asterObj[i].sprite.position.x+ (asterObj[i].sprite.w/2),asterObj[i].sprite.position.y- asterObj[i].sprite.h, asterObj[i].sprite.position.y );
		asterObj[i].sprite.hspeed=((rand() % 250)-(rand() % 500))*0.5f;
		asterObj[i].sprite.vspeed=((rand() % 250)-(rand() % 500))*0.5f;
		asterObj[i].topAcc=1000.0f;	
		asterObj[i].m_width=m_width;
		asterObj[i].m_height=m_height;
		asterObj[i].off=0;
		asterObj[i].HP=2*asterObj[i].sprite.scale;
		asterObj[i].size=10*asterObj[i].sprite.scale;
		asterObj[i].temp=rand()%100;
		asterObj[i].scaredist=100*asterObj[i].sprite.scale;
		
		if(asterObj[i].temp<COLDTEMP2)
			asterObj[i].sprite.color = D3DCOLOR_ARGB(255, 0,0, 255); // Random color and alpha
		else if (asterObj[i].temp>NORMTEMP2)
			asterObj[i].sprite.color = D3DCOLOR_ARGB(255, 255,0, 0); // Random color and alpha
		else
			asterObj[i].sprite.color = D3DCOLOR_ARGB(255, 255,255, 255); // Random color and alpha
	}

}

void DXGame::InitMenu()
{
	for(int i=0;i<MAX_BUTT;++i)
	{
		buttObj[i].sprite.rotation = D3DXToRadian(0);
		buttObj[i].sprite.scale =3;
		buttObj[i].sprite.w =100*buttObj[i].sprite.scale;
		buttObj[i].sprite.h =50*buttObj[i].sprite.scale;
		buttObj[i].sprite.color = D3DCOLOR_ARGB(255, 255, 255,  255); // Random color and alpha
		buttObj[i].sprite.BBox = CRect(buttObj[0].sprite,buttObj[0].sprite.position.x- (buttObj[0].sprite.w/2), buttObj[0].sprite.position.x+ (buttObj[0].sprite.w/2),buttObj[0].sprite.position.y- buttObj[0].sprite.h, buttObj[0].sprite.position.y );
		buttObj[i].sprite.hspeed=0.0;
		buttObj[i].sprite.vspeed=0.0;
	}
	buttObj[0].sprite.position = D3DXVECTOR3(m_width/2, 200, 0.0f); // Random position within screen width and height
		buttObj[0].state=GameStates::GAME;
		buttObj[1].sprite.position = D3DXVECTOR3(m_width/2,500, 0.0f); 
		buttObj[1].state=GameStates::QUIT;

}

void DXGame::InitPlayer()
{
	//////////////////////////////////////////////////////////////////////////
	// Ship 1
	//////////////////////////////////////////////////////////////////////////
		//Ship *ppplayer = new Ship;
	
		ppplayer.sprite.position = D3DXVECTOR3(( m_width/2), (m_height/2), 0.0f); // Random position within screen width and height
		ppplayer.sprite.rotation = D3DXToRadian(180); // Random rotation
		ppplayer.sprite.w =50;
		ppplayer.sprite.h =50;
		ppplayer.sprite.scale =1;
		ppplayer.sprite.color = D3DCOLOR_ARGB(255,0, 255,0); // Random color and alpha
		ppplayer.sprite.BBox = CRect(ppplayer.sprite,ppplayer.sprite.position.x- (ppplayer.sprite.w/2), ppplayer.sprite.position.x+ (ppplayer.sprite.w/2),ppplayer.sprite.position.y- ppplayer.sprite.h, ppplayer.sprite.position.y );
		ppplayer.sprite.hspeed=0.0;
		ppplayer.sprite.vspeed=0.0;	
		ppplayer.type=1;	
		ppplayer.lbs=10;	
		ppplayer.accel=1000.0f;	
		ppplayer.topAcc=1000.0f;	
		ppplayer.m_width=m_width;
		ppplayer.m_height=m_height;
		ppplayer.canshoot=1.0;
		ppplayer.flashing=1.0;
		ppplayer.HP=300;
		ppplayer.bullet_Count=0;
	//	for (int i=0;i<MAX_BULL;++i)
			//	ppplayer.bullets.push (
			//	ppplayer.bullObj[i]
			//	); /* Add some values to the queue */


}

void DXGame::InitOpponent()
{
		//////////////////////////////////////////////////////////////////////////
	// Enemy 1
	//////////////////////////////////////////////////////////////////////////

		enemObj[0].ship.sprite.position = D3DXVECTOR3(( m_width/2), (m_height/10), 0.0f); // Random position within screen width and height
		enemObj[0].ship.sprite.rotation = D3DXToRadian(180); // Random rotation
		enemObj[0].ship.sprite.w =50;
		enemObj[0].ship.sprite.h =50;
		enemObj[0].ship.sprite.scale =1;
		enemObj[0].ship.sprite.color = D3DCOLOR_ARGB(255,0, 0,150+rand() % 105); // Random color and alpha
		enemObj[0].ship.sprite.BBox = CRect(enemObj[0].ship.sprite,enemObj[0].ship.sprite.position.x- (enemObj[0].ship.sprite.w/2), enemObj[0].ship.sprite.position.x+ (enemObj[0].ship.sprite.w/2),enemObj[0].ship.sprite.position.y- enemObj[0].ship.sprite.h, enemObj[0].ship.sprite.position.y );
		enemObj[0].ship.sprite.hspeed=0.0;
		enemObj[0].ship.sprite.vspeed=0.0;	
		enemObj[0].ship.type=1;	
		enemObj[0].ship.lbs=10;	
		enemObj[0].ship.HP=100;	
		enemObj[0].ship.type=4;
		enemObj[0].ship.accel=666.0f;	
		enemObj[0].ship.topAcc=300.0f;	
		enemObj[0].ship.m_width=m_width;
		enemObj[0].ship.m_height=m_height;
		enemObj[0].ship.canshoot=1.0f;
		enemObj[0].canchoose=1.0f;
		enemObj[0].canchase=1.0f;
		enemObj[0].daQuad=1;
		enemObj[0].lastQ=1;
		enemObj[0].Qlock=0;
		enemObj[0].closedist=100.0f;
		enemObj[0].fardist=400.0f;
		enemObj[0].override=0;//=200.0f;
		enemObj[0].frienddist=0.0f;
		enemObj[0].nearRock=0;
		enemObj[0].brain.thought=Think::DECIDE;	
		enemObj[0].diff=1;
		enemObj[0].friendx=0.0f;
		enemObj[0].friendy=0.0f;
		enemObj[0].RanAway=0;
		enemObj[0].clocklock=0.0f;
		enemObj[0].ship.bullet_Count=0;
	
	enemObj[0].brain.SMALLSIZE=0;	
	enemObj[0].brain.MEDSIZE=0;
	
	enemObj[0].brain.SLOWSPEED=0;	
	enemObj[0].brain.MEDSPEED=0;
	
	enemObj[0].brain.COLDTEMP=0;
	enemObj[0].brain.NORMTEMP=0;


		enemObj[1].ship.sprite.position = D3DXVECTOR3(( m_width/2), (m_height/10), 0.0f); // Random position within screen width and height
		enemObj[1].ship.sprite.rotation = D3DXToRadian(180); // Random rotation
		enemObj[1].ship.sprite.scale =1.4f;
		enemObj[1].ship.sprite.w =50*enemObj[1].ship.sprite.scale;
		enemObj[1].ship.sprite.h =50*enemObj[1].ship.sprite.scale;
		enemObj[1].ship.sprite.color = D3DCOLOR_ARGB(255,150+rand()%105,0, 0); // Random color and alpha
		enemObj[1].ship.sprite.BBox = CRect(enemObj[1].ship.sprite,enemObj[1].ship.sprite.position.x- (enemObj[1].ship.sprite.w/2), enemObj[1].ship.sprite.position.x+ (enemObj[1].ship.sprite.w/2),enemObj[1].ship.sprite.position.y- enemObj[1].ship.sprite.h, enemObj[1].ship.sprite.position.y );
		enemObj[1].ship.sprite.hspeed=0.0;
		enemObj[1].ship.sprite.vspeed=0.0;	
		enemObj[1].ship.type=1;	
		enemObj[1].ship.lbs=10;	
		enemObj[1].ship.HP=100;	
		enemObj[1].ship.type=4;
		enemObj[1].ship.accel=800.0f;	
		enemObj[1].ship.topAcc=350.0f;	
		enemObj[1].ship.m_width=m_width;
		enemObj[1].ship.m_height=m_height;
		enemObj[1].ship.canshoot=1.0f;
		enemObj[1].canchoose=1.0f;
		enemObj[1].canchase=1.0f;
		enemObj[1].daQuad=1;
		enemObj[1].lastQ=1;
		enemObj[1].Qlock=0;
		enemObj[1].closedist=100.0f;
		enemObj[1].fardist=300.0f;
		enemObj[1].frienddist=0.0f;
		enemObj[1].override=0;//=200.0f;
		enemObj[1].nearRock=0;
		enemObj[1].brain.thought=Think::DECIDE;	
		enemObj[1].diff=1;
		enemObj[1].friendx=0.0f;
		enemObj[1].friendy=0.0f;
		enemObj[1].RanAway=1;
		enemObj[1].clocklock=0.0f;
		enemObj[1].ship.bullet_Count=0;
	enemObj[1].brain.SMALLSIZE=0;	
	enemObj[1].brain.MEDSIZE=0;
	
	enemObj[1].brain.SLOWSPEED=0;	
	enemObj[1].brain.MEDSPEED=0;
	
	enemObj[1].brain.COLDTEMP=0;
	enemObj[1].brain.NORMTEMP=0;

}

void DXGame::InitLight(HWND& hWnd, HINSTANCE& hInst)
{
	//////////////////////////////////////////////////////////////////////////
	// Light
	//////////////////////////////////////////////////////////////////////////
	ZeroMemory(&light, sizeof(light));

	// Ambient light color emitted from this light
	light.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	// Diffuse light color emitted from this light
	light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	// Specular light color emitted from this light
	light.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	// Light Type (Point) Requires: Position, Range, Attenuation
	light.Type = D3DLIGHT_POINT;	// Point, alternatively D3DLIGHT_DIRECTIONAL or D3DLIGHT_SPOT
	// Light Position
	light.Position = D3DXVECTOR3(30.0f, 10.0f, -10.0f); 
	// Range of Light
	light.Range = 100.0f;
	// Light Attenuation
	light.Attenuation0 = 0.0f;	// Constant
	light.Attenuation1 = 0.05f;	// Linear
	light.Attenuation2 = 0.0f;	// Quadratic

}

void DXGame::InitMaterial(HWND& hWnd, HINSTANCE& hInst)
{
	
	//////////////////////////////////////////////////////////////////////////
	// Material (How light is reflected off of an object)
	//////////////////////////////////////////////////////////////////////////
	ZeroMemory(&material[0], sizeof(material[0]));

	material[0].Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);		// Ambient color reflected
	material[0].Diffuse = D3DXCOLOR(1.0f, 0.3f, 0.3f, 1.0f);		// Diffuse color reflected
	material[0].Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);		// Emissive color reflected
	material[0].Specular = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);		// Specular
	material[0].Power = 30.0f;									// Specular highlight intensity


	ZeroMemory(&material[1], sizeof(material[1]));

	material[1].Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);		// Ambient color reflected
	material[1].Diffuse = D3DXCOLOR(0.3f, 0.3f, 1.0f, 1.0f);		// Diffuse color reflected
	material[1].Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);		// Emissive color reflected
	material[1].Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);		// Specular
	material[1].Power = 30.0f;									// Specular highlight intensity



	ZeroMemory(&material[2], sizeof(material[2]));

	material[2].Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);		// Ambient color reflected
	material[2].Diffuse = D3DXCOLOR(0.3f, 1.0f, 0.3f, 1.0f);		// Diffuse color reflected
	material[2].Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);		// Emissive color reflected
	material[2].Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);		// Specular
	material[2].Power = 30.0f;									// Specular highlight intensity

	//////////////////////////////////////////////////////////////////////////
	// Texture
	//////////////////////////////////////////////////////////////////////////
	// Load Texture
	D3DXCreateTextureFromFile(xDevice, L"ground2.bmp", &otherTexture[0]);
	D3DXCreateTextureFromFile(xDevice, L"seafloor.bmp", &otherTexture[1]);
	D3DXCreateTextureFromFile(xDevice, L"floor.bmp", &otherTexture[2]);



}

void DXGame::InitShader(HWND& hWnd, HINSTANCE& hInst)
{
	
	//////////////////////////////////////////////////////////////////////////
	// Effect
	//////////////////////////////////////////////////////////////////////////
	// Load effect from file (Lab5.fx)	
#ifdef _DEBUG	// If Debug flag is set in compiler, use shader debug

	xEffErr = 0;
	D3DXCreateEffectFromFile(xDevice, L"Lab5.fx", 0, 0, D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION, 
		0, &xEffect, &xEffErr);
	if(xEffErr)
	{
		MessageBoxA(0, (char*)xEffErr->GetBufferPointer(), 0, 0);
	}

#else			// else do not use debug flag

	D3DXCreateEffectFromFile(xDevice, L"Lab5.fx", 0, 0, 0, 0, &xEffect,
		&xEffErr);
#endif

	// Specify technique by name, alternatively you could find the first valid tech
	xTech = xEffect->GetTechniqueByName("tech0");


	//////////////////////////////////////////////////////////////////////////
	// Set shader parameters that are the same for all objects
	//////////////////////////////////////////////////////////////////////////
	xEffect->SetFloatArray("eyePos", (float*)eyePos, 3);
	xEffect->SetFloatArray("lightPos", (float*)&light.Position, 3);
	xEffect->SetFloatArray("ambientLight", (float*)&light.Ambient, 3);
	xEffect->SetFloatArray("specularLight", (float*)&light.Specular, 3);
	xEffect->SetFloatArray("diffuseLight", (float*)&light.Diffuse, 3);
	xEffect->SetFloatArray("lightAttenuation", (float*)&D3DXVECTOR3(light.Attenuation0, light.Attenuation1, light.Attenuation2), 3);




}

void DXGame::InitMesh(HWND& hWnd, HINSTANCE& hInst)
{
	
	//////////////////////////////////////////////////////////////////////////
	// Init Meshes
	//////////////////////////////////////////////////////////////////////////
	D3DXCreateBox(xDevice, 1.0f, 1.0f, 1.0f,
	&xBox, &Abuff);

	D3DXLoadMeshFromX(
		L"DwarfWithEffectInstance.x", // file to load
		D3DXMESH_MANAGED, // flags for memory management
		xDevice, // DXD3D device
		&xAbuff, // adjacency buffer for mesh
		&xMats, // materials in mesh
		&xotherEff, // effects (such as lightin // similar to those in a .fx file	
		&xNumMat, // count of materials in file
		&xMesh); // reference to the mesh object

	//////////////////////////////////////////////////////////////////////////
	// Character 1
	//////////////////////////////////////////////////////////////////////////
	D3DXMATERIAL* d3dmats = (D3DXMATERIAL*)xMats->GetBufferPointer();

	 meshMats	= new D3DMATERIAL9[xNumMat] ;
	 meshTextures	= new LPDIRECT3DTEXTURE9[xNumMat] ;

	for(DWORD i = 0; i < xNumMat; ++i) 
	{
		// Place the materials into our mater array
		meshMats[i] =d3dmats[i].MatD3D;
		// D3DX does not Set the ambient color for a material
		// So we need to do this
		meshMats[i] .Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
		// Create our texture
		meshTextures[i] = NULL;
		if (d3dmats[i].pTextureFilename)
		{
			// Need to convert the texture filenames to Unicode string
			// just to be safe
			int len = 0;
			len = (int)strlen(d3dmats[i].pTextureFilename) + 1;
			wchar_t *ucString = new wchar_t[len] ;
			mbstowcs(ucString, d3dmats[i].pTextureFilename, len);
			LPCWSTR filename = (LPCWSTR)ucString;
			// Load the texture now that we have a valid filename
			D3DXCreateTextureFromFile(xDevice, filename, &meshTextures[i]);
			//D3DXCreateEffectFromFile(xDevice, filename, &xotherEff[i],);
			delete[] ucString;
		}
	}
	//xMats->Release();


}

void DXGame::InitDirectInput(HWND& hWnd, HINSTANCE& hInst)
{
		m_hWnd = hWnd;
			//////////////////////////////////////////////////////////////////////////
	// Initialize DirectInput
	//////////////////////////////////////////////////////////////////////////

	// Create the DI Object
	DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&xDIobj, NULL);

	// Initialize Keyboard
	xDIobj->CreateDevice(GUID_SysKeyboard, &xKeys, NULL);

	// Initialize Mouse
	xDIobj->CreateDevice(GUID_SysMouse, &xMouse, NULL);

	// Set up Keyboard
	xKeys->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	xKeys->SetDataFormat(&c_dfDIKeyboard);

	// Set up Mouse (c_dfDIMouse2 = 8 button mouse)
	xMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	xMouse->SetDataFormat(&c_dfDIMouse2);

	//bool Keydown init
	bool x_Kdown[256] = {0};

}

void DXGame::InitFMOD(HWND& hWnd, HINSTANCE& hInst)
{
			m_hWnd = hWnd;

	//////////////////////////////////////////////////////////////////////////
	// Create and Initialize FMOD
	//////////////////////////////////////////////////////////////////////////

	/*
	Create a System object and initialize.
	*/
	result = FMOD::System_Create(&system);
	ERRCHECK(result);
	result = system->getVersion(&version);
	ERRCHECK(result);
	if (version < FMOD_VERSION)
	{
		printf("Error! You are using an old version of FMOD %08x. This program requires %08x\n",
		version, FMOD_VERSION);
		//exit(-1);
	}
	result = system->getNumDrivers(&numdrivers);
	ERRCHECK(result);
	if (numdrivers == 0)
	{
		result = system->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
		ERRCHECK(result);
	}
	else
	{
		 
		result = system->getDriverCaps(0, &caps, 0, &speakermode);
		ERRCHECK(result);
		/*
		Set the user selected speaker mode.
		*/
		result = system->setSpeakerMode(speakermode);
		ERRCHECK(result);
		if (caps & FMOD_CAPS_HARDWARE_EMULATED)
		{
			/*
			The user has the 'Acceleration' slider set to off! This is really bad
			for latency! You might want to warn the user about this.
			*/
			result = system->setDSPBufferSize(1024, 10);
			ERRCHECK(result);
		}
				 
		result = system->getDriverInfo(0, name, 256, 0);
		ERRCHECK(result);
		
		if (strstr(name, "SigmaTel"))
		{
			/*
			Sigmatel sound devices crackle for some reason if the format is PCM 16bit.
			PCM floating point output seems to solve it.
			*/
			result = system->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0,0,
			FMOD_DSP_RESAMPLER_LINEAR);
			ERRCHECK(result);
		}
	}
	result = system->init(100, FMOD_INIT_NORMAL, 0);
	if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)
	{
		/*
		Ok, the speaker mode selected isn't supported by this soundcard. Switch it
		back to stereo...
		*/
		result = system->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
		ERRCHECK(result);
		/*
		... and re-init.
		*/
		result = system->init(100, FMOD_INIT_NORMAL, 0);
	}
	ERRCHECK(result);

	
	result = system->createSound("audio/tada.wav", FMOD_DEFAULT, 0, &sound[1]);		// FMOD_DEFAULT uses the defaults.  These are the same as FMOD_LOOP_OFF | FMOD_2D | FMOD_HARDWARE.
	if (result != FMOD_OK)   MessageBox(0, L"FMOD Failed to load tada :(", 0, 0);
		

	//////////////////////////////////////////////////////////////////////////
	//Assignment 2 Week 3
	//////////////////////////////////////////////////////////////////////////
	result = system->createStream("audio/Scientist 1.wav",FMOD_LOOP_NORMAL | FMOD_2D | FMOD_HARDWARE, 0, & sound[0]);		// FMOD_DEFAULT uses the defaults.  These are the same as FMOD_LOOP_OFF | FMOD_2D | FMOD_HARDWARE.
	if (result != FMOD_OK)   MessageBox(0, L"FMOD Failed to load  scientist :(", 0, 0);


	result = system->createSound("audio/chord.wav", FMOD_DEFAULT, 0, &sound[2]);		// FMOD_DEFAULT uses the defaults.  These are the same as FMOD_LOOP_OFF | FMOD_2D | FMOD_HARDWARE.
	if (result != FMOD_OK)   MessageBox(0, L"FMOD Failed to load chord :(", 0, 0);
	
	result = system->createSound("audio/ding.wav", FMOD_DEFAULT, 0, &sound[3]);		// FMOD_DEFAULT uses the defaults.  These are the same as FMOD_LOOP_OFF | FMOD_2D | FMOD_HARDWARE.
	if (result != FMOD_OK)   MessageBox(0, L"FMOD Failed to load ding :(", 0, 0);
	
	result = system->createSound("audio/jaguar.wav", FMOD_DEFAULT, 0, &sound[4]);		// FMOD_DEFAULT uses the defaults.  These are the same as FMOD_LOOP_OFF | FMOD_2D | FMOD_HARDWARE.
	if (result != FMOD_OK)   MessageBox(0, L"FMOD Failed to load jaguar :(", 0, 0);
		
	result = system->createSound("audio/swish.wav", FMOD_DEFAULT, 0, &sound[5]);		// FMOD_DEFAULT uses the defaults.  These are the same as FMOD_LOOP_OFF | FMOD_2D | FMOD_HARDWARE.
	if (result != FMOD_OK)   MessageBox(0, L"FMOD Failed to load swish :(", 0, 0);

	result = system->createSound("audio/shotgun.wav", FMOD_DEFAULT, 0, &sound[6]);		// FMOD_DEFAULT uses the defaults.  These are the same as FMOD_LOOP_OFF | FMOD_2D | FMOD_HARDWARE.
	if (result != FMOD_OK)   MessageBox(0, L"FMOD Failed to load shotgun :(", 0, 0);

	result = system->createSound("audio/break.wav", FMOD_DEFAULT, 0, &sound[7]);		// FMOD_DEFAULT uses the defaults.  These are the same as FMOD_LOOP_OFF | FMOD_2D | FMOD_HARDWARE.
	if (result != FMOD_OK)   MessageBox(0, L"FMOD Failed to load break :(", 0, 0);
	result = system->createSound("audio/damage2.ogg", FMOD_DEFAULT, 0, &sound[8]);		// FMOD_DEFAULT uses the defaults.  These are the same as FMOD_LOOP_OFF | FMOD_2D | FMOD_HARDWARE.
	if (result != FMOD_OK)   MessageBox(0, L"FMOD Failed to load damage2 :(", 0, 0);
	result = system->createSound("audio/smallexplode.wav", FMOD_DEFAULT, 0, &sound[9]);		// FMOD_DEFAULT uses the defaults.  These are the same as FMOD_LOOP_OFF | FMOD_2D | FMOD_HARDWARE.
	if (result != FMOD_OK)   MessageBox(0, L"FMOD Failed to load smexp :(", 0, 0);
	result = system->createSound("audio/explosion.wav", FMOD_DEFAULT, 0, &sound[10]);		// FMOD_DEFAULT uses the defaults.  These are the same as FMOD_LOOP_OFF | FMOD_2D | FMOD_HARDWARE.
	if (result != FMOD_OK)   MessageBox(0, L"FMOD Failed to load boom sound :(", 0, 0);
	result = system->createSound("audio/crunch.wav", FMOD_DEFAULT, 0, &sound[11]);		// FMOD_DEFAULT uses the defaults.  These are the same as FMOD_LOOP_OFF | FMOD_2D | FMOD_HARDWARE.
	if (result != FMOD_OK)   MessageBox(0, L"FMOD Failed to load crunch sound :(", 0, 0);
	result = system->createSound("audio/hit.wav", FMOD_DEFAULT, 0, &sound[12]);		// FMOD_DEFAULT uses the defaults.  These are the same as FMOD_LOOP_OFF | FMOD_2D | FMOD_HARDWARE.
	if (result != FMOD_OK)   MessageBox(0, L"FMOD Failed to load hit sound :(", 0, 0);
}

void DXGame::InitDShow(HWND& hWnd, HINSTANCE& hInst)
{
			m_hWnd = hWnd;
				//////////////////////////////////////////////////////////////////////////
	// Part 2 iLab
	//////////////////////////////////////////////////////////////////////////


	//CoInitialize(NULL); 

	//CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,   IID_IGraphBuilder, (void**)&pGraph);
	// Create the filter graph manager and query for interfaces. CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 	IID_IGraphBuilder, (void **)&pGraph); 
//	pGraph->QueryInterface(IID_IMediaControl, (void**)&pMedCtrl);
	//pGraph->QueryInterface(IID_IMediaEvent, (void**)&pEvent);
   
	// Build the graph. 
	//pGraph->RenderFile(L"/Video/Intro.wmv", NULL); 
	// Video Window
	//pMedCtrl->QueryInterface(IID_IVideoWindow, (void**)&pWindow);
	// Setup the window
	//pWindow->put_Owner((OAHWND)m_hWnd);
	// Set the style
	//pWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE);
	// Obtain the size of the window
	//RECT WinRect;
	//GetClientRect(m_hWnd, &WinRect);
	// Set the video size to the size of the window
	//pWindow->SetWindowPosition(WinRect.left, WinRect.top,  WinRect.right, WinRect.bottom);


	//VidPlay = 1;
	//pMedCtrl->Run(); // Run the graph. 
	


}

void DXGame::Update(float dt)
{
	for(int i=0;i<MAX_ENEMY;++i)
	{
		enemObj[i].dist = distToPlay(
		enemObj[i].ship.sprite.position.x, 
		enemObj[i].ship.sprite.position.y,
		ppplayer.sprite.position.x, 
		ppplayer.sprite.position.y);

		for(int k=0;k<MAX_ASTER;++k)
		{
			enemObj[i].dist3=enemObj[i].dist2;
			enemObj[i].dist2 =distToPlay(
				enemObj[i].ship.sprite.position.x, 
				enemObj[i].ship.sprite.position.y,
				asterObj[k].sprite.position.x, 
				asterObj[k].sprite.position.y);

		if (enemObj[i].dist2<asterObj[k].scaredist)
		{
			if(enemObj[i].dist2 < enemObj[i].dist3)
			{
				enemObj[i].neardist=enemObj[i].dist2;
				enemObj[i].nearRock=k;
			}
		}

	}




	enemObj[i].targ2x=asterObj[enemObj[i].nearRock].sprite.position.x;
	enemObj[i].targ2y=asterObj[enemObj[i].nearRock].sprite.position.y;
	enemObj[0].friendx=enemObj[1].ship.sprite.position.x;
	enemObj[0].friendy=enemObj[1].ship.sprite.position.y;
	enemObj[1].friendx=enemObj[0].ship.sprite.position.x;
	enemObj[1].friendy=enemObj[0].ship.sprite.position.y;
	
	if (enemObj[i].neardist<asterObj[enemObj[i].nearRock].scaredist)
	{
		enemObj[i].neardist=distToPlay(
				enemObj[i].ship.sprite.position.x, 
				enemObj[i].ship.sprite.position.y,
				asterObj[enemObj[i].nearRock].sprite.position.x, 
				asterObj[enemObj[i].nearRock].sprite.position.y);
	}


	enemObj[0].frienddist=distToPlay(
				enemObj[0].ship.sprite.position.x, 
				enemObj[0].ship.sprite.position.y,
				enemObj[1].ship.sprite.position.x, 
				enemObj[1].ship.sprite.position.y);
	enemObj[1].frienddist=distToPlay(
				enemObj[1].ship.sprite.position.x, 
				enemObj[1].ship.sprite.position.y,
				enemObj[0].ship.sprite.position.x, 
				enemObj[0].ship.sprite.position.y);

	//asterObj[enemObj[i].nearRock].sprite.color = D3DCOLOR_ARGB(255, rand() % 255, rand() % 255, rand() % 255); // Random color and alpha



	enemObj[i].targx=ppplayer.sprite.position.x;
	enemObj[i].targy=ppplayer.sprite.position.y;
	
	
	enemObj[i].brain.thought=decide(enemObj[i]);

	
	enemObj[i].act(dt);
	}
	UpdateCollisions(dt);
	UpdateInput(dt);
	UpdateAction(dt);
	UpdateFPS(dt);
	UpdateFMOD(dt);
}

GameStates DXGame::UpdateGUI(float dt)
{
	UpdateInput(dt);
	tempstate=UpdateMenuCollisions(dt);

	ClearBB();
	RenderMenu(dt);
	//HR(xSpr->End();)

	//RenderEffect(dt);
	//RenderMesh(dt);
	
	HR(xDevice->EndScene())
	HR(xDevice->Present(0, 0, 0, 0));


	UpdateFMOD(dt);
	return tempstate;
}

bool DXGame::PlayIntro(float dt)
{
	UpdateVideo(dt);
	UpdateInput(dt);

	if(buffer[DIK_RETURN] & 0x80) 
	{
		StopVid();
	} 
	if (!VidPlay)
		return 1;
	else
		return 0;
}

void DXGame::UpdateVideo(float dt)
{
	if (VidPlay)
	{
		pEvent->GetEvent(&evCode,& eventParam1, &eventParam2, 100.0f);
		if ((evCode==EC_COMPLETE)||(evCode==EC_ERRORABORT)||(evCode==EC_USERABORT ))	
		{
			VidPlay=0;
			pWindow->put_Visible(OAFALSE);
			pWindow->put_Owner((OAHWND)m_hWnd);
			pMedCtrl->Release();
			pWindow->Release();
				
			pEvent->Release();
			pEvent->FreeEventParams(evCode,eventParam1,eventParam2);
				

			pGraph->Release();
			result = system->playSound(FMOD_CHANNEL_FREE, sound[1], false, &channel[1]); 
			ERRCHECK(result);		

		}
		CoUninitialize();
	}

}

GameStates DXGame::UpdateMenuCollisions(float dt)
{
	for (int i = 0;i <MAX_BUTT; ++i )
	{
		GetCursorPos(&p);

		mx[1]=buttObj[i].sprite.position.x-(buttObj[0].sprite.w/3);
		my[1]=buttObj[i].sprite.position.y-(buttObj[0].sprite.h/3);
		mx[2]=buttObj[i].sprite.position.x+(buttObj[i].sprite.w/3);
		my[2]=buttObj[i].sprite.position.y+(buttObj[i].sprite.h/3);

		buttObj[i].sprite.BBox=CRect(buttObj[i].sprite,mx[1],mx[2],my[1],my[2]);

		if(mcollision(buttObj[i].sprite.BBox,p.x,p.y))
		{
			buttObj[i].sprite.color = D3DCOLOR_ARGB(255, rand() % 255, 255,  255); // Random color and alpha
			buttObj[i].hover=1;
		}
		else 
			buttObj[i].hover=0;
	if(mouseState.rgbButtons[0] & 0x80 && buttObj[i].hover==1)
		{
			return buttObj[i].state;
		}

	}	
	return GameStates::INTRO;
}

void DXGame::setUpCol(int i, int g)
{
	ppplayer.sprite.SCol.radius=ppplayer.sprite.w;
	ppplayer.sprite.SCol.vecCenter=
		D3DXVECTOR3(ppplayer.sprite.position.x
		,//-ppplayer.sprite.SCol.radius  ,  
		ppplayer.sprite.position.y
		,//-ppplayer.sprite.SCol.radius,
		0.0);

	enemObj[g].ship.sprite.SCol.radius=enemObj[g].ship.sprite.w/2;
	enemObj[g].ship.sprite.SCol.vecCenter
		=D3DXVECTOR3(enemObj[g].ship.sprite.position.x
		,//+enemObj[g].ship.sprite.SCol.radius  ,  
		enemObj[g].ship.sprite.position.y
		,//+enemObj[g].ship.sprite.SCol.radius,
		0.0);
	
	ppplayer.sprite.BBox 
					= CRect(
					ppplayer.sprite,
					ppplayer.sprite.position.x, 
					ppplayer.sprite.position.x
					+ppplayer.sprite.w,
					ppplayer.sprite.position.y, 
					ppplayer.sprite.position.y
					+ppplayer.sprite.h 
					);
				ppplayer.bullObj[i].sprite.BBox
					= CRect(ppplayer.bullObj[i].sprite,
					ppplayer.bullObj[i].sprite.position.x,
					ppplayer.bullObj[i].sprite.position.x
					+ppplayer.bullObj[i].sprite.w,
					ppplayer.bullObj[i].sprite.position.y, 
					ppplayer.bullObj[i].sprite.position.y
					+ppplayer.bullObj[i].sprite.h );
				
				
				enemObj[g].ship.sprite.BBox 
					= CRect(enemObj[g].ship.sprite,
					enemObj[g].ship.sprite.position.x, 
					enemObj[g].ship.sprite.position.x
					+enemObj[g].ship.sprite.w,
					enemObj[g].ship.sprite.position.y, 
					enemObj[g].ship.sprite.position.y
					+enemObj[g].ship.sprite.h);
				
				enemObj[g].ship.bullObj[i].sprite.BBox
					= CRect(enemObj[g].ship.bullObj[i].sprite,
					enemObj[g].ship.bullObj[i].sprite.position.x,
					enemObj[g].ship.bullObj[i].sprite.position.x
					+enemObj[g].ship.bullObj[i].sprite.w,
					enemObj[g].ship.bullObj[i].sprite.position.y, 
					enemObj[g].ship.bullObj[i].sprite.position.y
					+enemObj[g].ship.bullObj[i].sprite.h );
	ppplayer.bullObj[i].sprite.SCol.radius=ppplayer.sprite.w/2;
	ppplayer.bullObj[i].sprite.SCol.vecCenter
		=D3DXVECTOR3(ppplayer.bullObj[i].sprite.position.x
		,//+ppplayer.bullObj[i].sprite.SCol.radius  ,  
		ppplayer.bullObj[i].sprite.position.y
		,//+ppplayer.bullObj[i].sprite.SCol.radius,
		0.0);
	enemObj[g].ship.bullObj[i].sprite.SCol.radius=enemObj[g].ship.sprite.w/2;
	enemObj[g].ship.bullObj[i].sprite.SCol.vecCenter
		=D3DXVECTOR3(enemObj[g].ship.bullObj[i].sprite.position.x
		,//+enemObj[g].ship.bullObj[i].sprite.SCol.radius  ,  
		enemObj[g].ship.bullObj[i].sprite.position.y
		,//+enemObj[g].ship.bullObj[i].sprite.SCol.radius,
		0.0);

}

void DXGame::enemyhit(float dt,int i, int g)
{
				if (Scol(
					enemObj[g].ship.sprite.SCol,
					ppplayer.bullObj[i].sprite.SCol)==1  &&
					ppplayer.bullObj[i].dmg>0)
					//(collision(
					//enemObj[g].ship.sprite.BBox,
					//ppplayer.bullObj[i].sprite.BBox)==1  &&
					//ppplayer.bullObj[i].dmg>0)
				//	|| 
				{					
					explode(D3DCOLOR_ARGB(255,rand()%255,0,0),dt,3,enemObj[g].ship.sprite.position.x,enemObj[g].ship.sprite.position.y);
					//enemObj[g].ship.sprite.color = D3DCOLOR_ARGB(255, rand() % 255, rand() % 255, rand() % 255); // Random color and alpha
					ppplayer.bullObj[i].balloff=1;
					enemObj[g].ship.HP-=ppplayer.bullObj[i].dmg;
					result = system->playSound(FMOD_CHANNEL_FREE, sound[7], false, &channel[0]); 
					ERRCHECK(result);		
					/*if (ppplayer.bullObj[i].sprite.vspeed>0)
					result = system->playSound(FMOD_CHANNEL_FREE, sound[7], false, &channel[0]); 
					ERRCHECK(result);		
						ppplayer.bullObj[i].sprite.position.y-=3;
						ppplayer.bullObj[i].sprite.vspeed*=(-1.0);
					}
					else
					{
						ppplayer.bullObj[i].sprite.position.y+=3;
						ppplayer.bullObj[i].sprite.vspeed*=(-1.0);
					}*/
				}	

}

void DXGame::playerHit(float dt,int i, int g)
{

				if (Scol(
					ppplayer.sprite.SCol,
					enemObj[g].ship.bullObj[i].sprite.SCol)==1 &&
					enemObj[g].ship.bullObj[i].dmg>0)
				//if (Scol(
				//	ppplayer.sprite.SCol,
				//	enemObj[g].ship.bullObj[i].sprite.SCol)==1 &&
				//	enemObj[g].ship.bullObj[i].dmg>0)
				{
					explode( D3DCOLOR_ARGB(255,0,rand()%255,0),  dt,4,ppplayer.sprite.position.x,ppplayer.sprite.position.y);
					//ppplayer.sprite.color = D3DCOLOR_ARGB(255, rand() % 255, rand() % 255, rand() % 255); // Random color and alpha
					enemObj[g].ship.bullObj[i].balloff=1;
					ppplayer.HP-=enemObj[g].ship.bullObj[i].dmg;
				//	result = system->playSound(FMOD_CHANNEL_FREE, sound[7], false, &channel[0]); 
					ERRCHECK(result);		

				}
				/*
				if (collision(
					ppplayer.sprite.BBox,
					enemObj[g].ship.sprite.BBox)
					&&
					ppplayer.flashing>=0.2f)
				{
					ppplayer.sprite.color = D3DCOLOR_ARGB(255, rand() % 255, rand() % 255, rand() % 255); // Random color and alpha
					//enemObj[g].ship.bullObj[i].balloff=1;
					ppplayer.HP-=3;
					enemObj[g].ship.HP-=3;
					ppplayer.flashing=0.0f;
					result = system->playSound(FMOD_CHANNEL_FREE, sound[7], false, &channel[0]); 
					ERRCHECK(result);		

//					enemObj[g].ship.HP-=1;
				}	*/

}

void DXGame::Wreck(float dt,int i, int g)
{
				if (Scol(
					ppplayer.sprite.SCol,
					enemObj[g].ship.sprite.SCol)
					&&
					ppplayer.flashing>=0.2f)
				//if (collision(
				//	ppplayer.sprite.BBox,
				//	enemObj[g].ship.sprite.BBox)
				//	&&
				//	ppplayer.flashing>=0.2f)
				{
					explode(D3DCOLOR_ARGB(255, rand()%255,rand()%255,0), dt,8,ppplayer.sprite.position.x,ppplayer.sprite.position.y);
				//	ppplayer.sprite.color = D3DCOLOR_ARGB(255, rand() % 255, rand() % 255, rand() % 255); // Random color and alpha
					//enemObj[g].ship.bullObj[i].balloff=1;
					ppplayer.HP-=3;
					enemObj[g].ship.HP-=3;
					ppplayer.flashing=0.0f;
					result = system->playSound(FMOD_CHANNEL_FREE, sound[7], false, &channel[0]); 
					ERRCHECK(result);		

				//	explode(dt,4,ppplayer.sprite.position.x,ppplayer.sprite.position.y);

//					enemObj[g].ship.HP-=1;
				}	

}

void DXGame::asterCols(float dt,int i, int g, int k)
{
	asterObj[k].sprite.SCol.radius=
		asterObj[k].sprite.w;
	asterObj[k].sprite.SCol.vecCenter=
		D3DXVECTOR3(asterObj[k].sprite.position.x
		,//+asterObj[k].sprite.SCol.radius,
		asterObj[k].sprite.position.y
		,//+asterObj[k].sprite.SCol.radius,
		0.0);
			

	asterObj[k].sprite.BBox
					= CRect(asterObj[k].sprite,
					asterObj[k].sprite.position.x, 
					asterObj[k].sprite.position.x
					+asterObj[k].sprite.w,
					asterObj[k].sprite.position.y, 
					asterObj[k].sprite.position.y
					+asterObj[k].sprite.h );
	
				if (Scol(
					ppplayer.sprite.SCol,
					asterObj[k].sprite.SCol)
					&&
					ppplayer.flashing>=0.2f)
			//	if (collision(
			//		ppplayer.sprite.BBox,
			//		asterObj[k].sprite.BBox)
			//		&&
			//		ppplayer.flashing>=0.2f)
				{
					if(asterObj[k].temp>NORMTEMP2)
					{
						explode(D3DCOLOR_ARGB(200,rand()%255,0,0), dt,10,asterObj[k].sprite.position.x,asterObj[k].sprite.position.y);
						asterObj[k].Crumble();
						ppplayer.HP-=10;
						ppplayer.flashing=0.0f;
						result = system->playSound(FMOD_CHANNEL_FREE, sound[10], false, &channel[0]); 
						ERRCHECK(result);
					}
					else if(asterObj[k].temp<COLDTEMP2)
					{
						explode( D3DCOLOR_ARGB(200,0,0,rand()%255), dt,3,asterObj[k].sprite.position.x,asterObj[k].sprite.position.y);
						asterObj[k].Crumble();
						result = system->playSound(FMOD_CHANNEL_FREE, sound[11], false, &channel[0]); 
						ERRCHECK(result);
					}
					else if(asterObj[k].size>SMALLSIZE2)
					{
						explode(asterObj[k].sprite.color, dt,6,asterObj[k].sprite.position.x,asterObj[k].sprite.position.y);
						asterObj[k].Crumble();
						ppplayer.HP-=5;
						ppplayer.flashing=0.0f;
						result = system->playSound(FMOD_CHANNEL_FREE, sound[7], false, &channel[0]); 
						ERRCHECK(result);
					}
					else if(asterObj[k].size<SMALLSIZE2)
					{
						explode( D3DCOLOR_ARGB(200,0,0,rand()%255), dt,3,asterObj[k].sprite.position.x,asterObj[k].sprite.position.y);
						asterObj[k].Crumble();
						result = system->playSound(FMOD_CHANNEL_FREE, sound[11], false, &channel[0]); 
						ERRCHECK(result);
					}
							
				}

				if (Scol(
					enemObj[g].ship.sprite.SCol,
					asterObj[k].sprite.SCol)
					&&
					enemObj[g].ship.flashing>=0.2f)
				{
				//	enemObj[g].ship.sprite.color = D3DCOLOR_ARGB(255, rand() % 255, rand() % 255, rand() % 255); // Random color and alpha
						if(asterObj[k].temp>NORMTEMP2)
					{
						explode(D3DCOLOR_ARGB(200,rand()%255,0,0), dt,10,asterObj[k].sprite.position.x,asterObj[k].sprite.position.y);
						asterObj[k].Crumble();
						enemObj[g].ship.HP-=10;
						enemObj[g].ship.flashing=0.0f;

						enemObj[g].brain.NORMTEMP=NORMTEMP2;

						result = system->playSound(FMOD_CHANNEL_FREE, sound[10], false, &channel[0]); 
						ERRCHECK(result);
						
							//=	Att( BIG,	FAST,	HOT,	COLLIDE );
					}
					else if(asterObj[k].temp<COLDTEMP2)
					{
						explode( D3DCOLOR_ARGB(200,0,0,rand()%255), dt,3,asterObj[k].sprite.position.x,asterObj[k].sprite.position.y);
						asterObj[k].Crumble();
						
						enemObj[g].brain.COLDTEMP=COLDTEMP2;
						
						result = system->playSound(FMOD_CHANNEL_FREE, sound[11], false, &channel[0]); 
						ERRCHECK(result);

					}
					else if(asterObj[k].size>SMALLSIZE2)
					{
						explode(asterObj[k].sprite.color, dt,6,asterObj[k].sprite.position.x,asterObj[k].sprite.position.y);
						asterObj[k].Crumble();
						enemObj[g].ship.HP-=5;
						enemObj[g].ship.flashing=0.0f;

						enemObj[g].brain.SMALLSIZE=SMALLSIZE2;

						result = system->playSound(FMOD_CHANNEL_FREE, sound[7], false, &channel[0]); 
						ERRCHECK(result);
					}
					else if(asterObj[k].size<SMALLSIZE2) 
					{
						explode( D3DCOLOR_ARGB(200,0,0,rand()%255), dt,3,asterObj[k].sprite.position.x,asterObj[k].sprite.position.y);
						asterObj[k].Crumble();
						
						enemObj[g].brain.SMALLSIZE=SMALLSIZE2;
						
						result = system->playSound(FMOD_CHANNEL_FREE, sound[11], false, &channel[0]); 
						ERRCHECK(result);
					}

				
				}	
	
				if (Scol(
					ppplayer.bullObj[i].sprite.SCol,
					asterObj[k].sprite.SCol) &&
					ppplayer.bullObj[i].dmg>0)
				{
					//ppplayer.sprite.color = D3DCOLOR_ARGB(255, rand() % 255, rand() % 255, rand() % 255); // Random color and alpha
					explode( D3DCOLOR_ARGB(200,0,0,rand()%255), dt,3,asterObj[k].sprite.position.x,asterObj[k].sprite.position.y);
					
					//
					ppplayer.bullObj[i].balloff=1;
						asterObj[k].HP-=1;
					if (asterObj[k].HP<=0)
						asterObj[k].Crumble();
					//ppplayer.flashing=0.0f;
					
					
					result = system->playSound(FMOD_CHANNEL_FREE, sound[12], false, &channel[0]); 
					ERRCHECK(result);		
				}

				if (Scol(
					enemObj[g].ship.bullObj[i].sprite.SCol,
					asterObj[k].sprite.SCol) &&
					enemObj[g].ship.bullObj[i].dmg>0)
				{
					explode( D3DCOLOR_ARGB(200,0,0,rand()%255), dt,4,asterObj[k].sprite.position.x,asterObj[k].sprite.position.y);
					enemObj[g].ship.bullObj[i].balloff=1;
					asterObj[k].HP-=1;
					if (asterObj[k].HP<=0)
						asterObj[k].Crumble();

					result = system->playSound(FMOD_CHANNEL_FREE, sound[12], false, &channel[0]); 
					ERRCHECK(result);		

				}	

}

void DXGame::UpdateCollisions(float dt)
{
	for (int i = 0;i <MAX_BULL; ++i )
	{	
		for(int g=0;g<MAX_ENEMY;++g)
		{
				
			setUpCol(i,g);
			 enemyhit(dt, i,  g);
			 playerHit(dt, i,  g);
			 Wreck(dt, i,  g);

			 for (int k = 0;k<MAX_ASTER;k++)
				 asterCols(dt, i,  g,  k);
		}		
	}
}

void DXGame::UpdateInput(float dt)
{
	//////////////////////////////////////////////////////////////////////////
		// Get and Acquire Keyboard Input
		//////////////////////////////////////////////////////////////////////////

		// buffer - Stores our keyboard device state
	ZeroMemory(buffer, sizeof(buffer));
	
		// Get the input device state
	HRESULT hr;
	hr = xKeys->GetDeviceState( sizeof(buffer), (LPVOID)&buffer );
	
	if(FAILED(hr))
	{
		hr = xKeys->Acquire();
				
			// Device has probably been lost if failed, if so keep trying to get it until it’s found.
		while( hr == DIERR_INPUTLOST)
		{
			hr = xKeys->Acquire();
		}

			// If we failed for some other reason
		if(FAILED(hr))	return;

			// Read the device state again
		xKeys->GetDeviceState(sizeof(buffer), buffer);
	}

		//////////////////////////////////////////////////////////////////////////
		// Get and Acquire Mouse Input
		//////////////////////////////////////////////////////////////////////////
		// Stores our mouse state for an 8 button mouse.
	ZeroMemory(&mouseState, sizeof(mouseState));

		// Get the input device state
	hr = xMouse->GetDeviceState( sizeof(DIMOUSESTATE2), &mouseState );
	if(FAILED(hr))
	{
		hr = xMouse->Acquire();

			// Device has probably been lost if failed, if so keep trying to get it until it’s found.
		while( hr == DIERR_INPUTLOST)
		{
			hr = xMouse->Acquire();
		}

			// If we failed for some other reason
		if(FAILED(hr))	return;
		// Read the device state again
		xMouse->GetDeviceState(sizeof(DIMOUSESTATE2), &mouseState);
	}
}
 
void DXGame::StopVid()
{
				if (VidPlay)
			{
				pMedCtrl->Stop();
				
				VidPlay=0;
				pWindow->put_Visible(OAFALSE);
				pWindow->put_Owner((OAHWND)m_hWnd);
				pMedCtrl->Release();
				pWindow->Release();
				
				pEvent->Release();
				pEvent->FreeEventParams(evCode,eventParam1,eventParam2);
				

				pGraph->Release();

				}
}

void DXGame::UpdateAction(float dt)
{
		//////////////////////////////////////////////////////////////////////////
	// TODO: Any non-graphics related code can go here to be updated per frame
	//////////////////////////////////////////////////////////////////////////

	//if(buffer[DIK_RETURN] & 0x80) 
	//{
	//	StopVid();
	//} 


	//if (!VidPlay)
	//{
		//////////////////////////////////////////////////////////////////////////
		//	Keyboard Code Examples: [DIK (DirectInput Key) codes we translate]
	//	DIK_0 – DIK_9
	//	DIK_NUMPAD0 – DIK_NUMPAD9
	//	DIK_A – DIK_Z
	//	DIK_F1 – DIK_F12
	//	DIK_UP, DIK_DOWN, DIK_LEFT, DIK_RIGHT	// Arrow Keys
	//	DIK_SPACE, DIK_TAB, DIK_CAPITAL, DIK_LCONTROL (Left Ctrl Key), 
	//  DIK_RCONTROL (Right Ctrl Key), DIK_RETURN, DIK_LMENU (Left Alt Key), 
	//  DIK_LWIN (Left Windows Key), DIK_LSHIFT (Left Shift Key), etc.
	//	Complete list under Keyboard Device in the documentation.
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//	Mouse variables:  [MouseState2 structure supports 8 button mice]
	//	lX				-	X-axis mouse relative coordinates
	//	lY				-	Y-axis mouse relative coordinates
	//	lZ				-	Mouse wheel relative coordinates
	//	rgbButtons[8]	-	Array of 8 mouse buttons
	//
	//	Usually mouse hardware maps the button layout in a standard way for 
	//	the first 4 buttons, after that it depends on the mouse hardware layout
	//	rgbButtons[0]	-	Left Mouse Button
	//	rgbButtons[1]	-	Right Mouse Button
	//	rgbButtons[2]	-	Middle Mouse Button (click scroll wheel)
	//	rgbButtons[3]	-	Side Mouse Button 1
	//	rgbButtons[4]	-	Side Mouse Button 2
	//	rgbButtons[5]	-	Side Mouse Button 3
	//	rgbButtons[6]	-	Side Mouse Button 4
	//	rgbButtons[7]	-	Side Mouse Button 5
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
		x_Kdown[256] = buffer;
			
		ppplayer.Update(dt);
		for(int i=0;i<MAX_ENEMY;++i)
			enemObj[i].update(dt);
		for(int i=0;i<MAX_ASTER;++i)
			asterObj[i].Update(dt);
		for(int i=0;i<MAX_PART;++i)
			part[i].update(dt);
	//////////////////////////////////////////////////////////////////////////
	//	Act on Input
	//////////////////////////////////////////////////////////////////////////
		//paddleObj[0].position.x += mouseState.lX;
	// Keyboard
		if(buffer[DIK_W] & 0x80)
		{
		//bullObj[0].position.y -= 0.1f;
			ppplayer.Thrust(4,dt);
				//sprite.position.y -= 100.0f * dt;

		}
		if(buffer[DIK_A] & 0x80)
		{
			ppplayer.Thrust(1,dt);
			//ppplayer.position.x -=1000.0f * dt;
		}
		if(buffer[DIK_S] & 0x80)
		{
			ppplayer.Thrust(2,dt);
		}
		if(buffer[DIK_D] & 0x80)
		{
			ppplayer.Thrust(3,dt);
			//ppplayer.position.x +=1000.0f * dt;
		}
		if(mouseState.rgbButtons[0] & 0x80)
		{
			GetCursorPos(&p);
	if (ppplayer.bullet_Count>=0 &&
		ppplayer.bullet_Count<=30 &&
		ppplayer.canshoot>=0.3f)
			{
			result = system->playSound(FMOD_CHANNEL_FREE, sound[6], false, &channel[0]); 
			ERRCHECK(result);	
			ppplayer.Fire(dt,p.x,p.y);
			}//ppplayer.position.x +=1000.0f * dt;
		}
			
	// Mouse
			Updatebullets( dt);
	
		if (ppplayer.HP<=0)
	{
					MessageBox(0, L"AI Wins!", 0, 0);
					//balloff=[0,0,0,0,0];
enemObj[0].ship.HP=100;
enemObj[1].ship.HP=100;
ppplayer.HP=300;
	}
			
	if(enemObj[0].ship.HP<=0 ||
		enemObj[1].ship.HP<=0
		)
	{
					MessageBox(0, L"Player  Wins!", 0, 0);
					//balloff=0;
enemObj[1].ship.HP=100;
enemObj[0].ship.HP=100;
ppplayer.HP=300;

	}

	//////////////////////////////////////////////////////////////////////////
	// Assignment 2
	//////////////////////////////////////////////////////////////////////////
	// Keboard - Sprite 0 Movement
	/*	if(buffer[DIK_UP] & 0x80)
		{
			bullObj[0].position.y -= 0.3f;
		}*/
		/*if(buffer[DIK_LEFT] & 0x80)
		{
			paddleObj[0].position.x -= 3.3f;
		}*/
/*		if(buffer[DIK_DOWN] & 0x80)
		{
			bullObj[0].position.y += 0.3f;
		}*/
		/*if(buffer[DIK_RIGHT] & 0x80)
		{
			paddleObj[0].position.x += 3.3f;
		}*/
		
	// Mouse - Sprite 0 Rotation
		//bullObj[0].rotation +=.lX * 0.01f;


	// Sprite 0 clamping to screen space

	//}
		// Space bar to play sound effect 
	// If key is pressed, and the bool to detect if a key was pressed 
	// in a previous frame is false, then enter statement 
	
		
	/*
		
		if(buffer[DIK_SPACE] & 0x80) 
	{		
		if(!x_Kdown[DIK_SPACE] )
		{			
			x_Kdown[DIK_SPACE] = true;
			result = system->playSound(FMOD_CHANNEL_FREE, sound[1], false, &channel[0]); 
			ERRCHECK(result);		
		} 
	} 
	else 	
	{		
		if(x_Kdown[DIK_SPACE])
		{			
			x_Kdown[DIK_SPACE] = false;		
		}	
	}*/
	
	//////////////////////////////////////////////////////////////////////////
	// Assignment 2 -- Compressed
	//////////////////////////////////////////////////////////////////////////
	/*
	if(buffer[DIK_1] & 0x80) 
	{		if(!x_Kdown[DIK_1] )
		{			x_Kdown[DIK_1] = true;
			result = system->playSound(FMOD_CHANNEL_FREE, sound[2], false, &channel[1]); 
			ERRCHECK(result);		} 	} 
	else 	{		if(x_Kdown[DIK_1])
		{			x_Kdown[DIK_1] = false;		}	}
	
	if(buffer[DIK_2] & 0x80) 
	{		if(!x_Kdown[DIK_2] )
		{			x_Kdown[DIK_2] = true;
			result = system->playSound(FMOD_CHANNEL_FREE, sound[3], false, &channel[2]); 
			ERRCHECK(result);		} 	} 
	else 	{		if(x_Kdown[DIK_2])
		{			x_Kdown[DIK_2] = false;		}	}
	
	if(buffer[DIK_3] & 0x80) 
	{		if(!x_Kdown[DIK_3] )
		{			x_Kdown[DIK_3] = true;
			result = system->playSound(FMOD_CHANNEL_FREE, sound[4], false, &channel[3]); 
			ERRCHECK(result);		} 	} 
	else 	{		if(x_Kdown[DIK_3])
		{			x_Kdown[DIK_3] = false;		}	}

	if(buffer[DIK_4] & 0x80) 
	{		if(!x_Kdown[DIK_4] )
		{			x_Kdown[DIK_4] = true;
			result = system->playSound(FMOD_CHANNEL_FREE, sound[5], false, &channel[4]); 
			ERRCHECK(result);		} 	} 
	else 	{		if(x_Kdown[DIK_4])
		{			x_Kdown[DIK_4] = false;		}	}


		*/
};
void DXGame::Updatebullets( float dt)
{
		for (int i=0; i<MAX_BULL; i++)
		{
			//bullObj[0].position.x += mouseState.lX/(f+1);
		//	bullObj[0].position.y += mouseState.lY/(f+1);
			// Clamp sprite position to boundaries of the screen
		if(ppplayer.bullet_Count>=30)
			ppplayer.bullet_Count=0;
		if(ppplayer.bullObj[i].sprite.position.x <= 0)
			ppplayer.bullObj[i].balloff=1;
		if(ppplayer.bullObj[i].sprite.position.x >= m_width)
			ppplayer.bullObj[i].balloff=1;
		if(ppplayer.bullObj[i].sprite.position.y <= 0)
			ppplayer.bullObj[i].balloff=1;
		if(ppplayer.bullObj[i].sprite.position.y >= m_height)
			ppplayer.bullObj[i].balloff=1;
		ppplayer.bullObj[i].sprite.position.x += ppplayer.bullObj[i].sprite.hspeed*100.0f* dt;
		ppplayer.bullObj[i].sprite.position.y += ppplayer.bullObj[i].sprite.vspeed* 100.0f*dt;
		
		if (ppplayer.bullObj[i].balloff==1)
		{
			ppplayer.bullObj[i].balloff=0;
			ppplayer.bullObj[i].dmg=0;
			ppplayer.bullObj[i].sprite.position.x =10;
			ppplayer.bullObj[i].sprite.position.y =10;
			ppplayer.bullObj[i].sprite.color = D3DCOLOR_ARGB(0, 0, 0, 0); // Random color and alpha

			ppplayer.bullObj[i].sprite.hspeed=0;//(float(rand() % 300)-(float(rand() %600)))*100.0f* dt;
			ppplayer.bullObj[i].sprite.vspeed=0;//(float(rand() % 300)-(float(rand() %600)))*100.0f* dt;	
			//	ppplayer.bullets.pop ();
		}//
			//bullObj[0].position.x += mouseState.lX/(f+1);
		//	bullObj[0].position.y += mouseState.lY/(f+1);
		
		for(int k=0; k<MAX_ENEMY;++k)
		{
			if(enemObj[k].ship.bullet_Count>=30)
				enemObj[k].ship.bullet_Count=0;
		// Clamp sprite position to boundaries of the screen
		if(enemObj[k].ship.bullObj[i].sprite.position.x <= 0)
			enemObj[k].ship.bullObj[i].balloff=1;
		if(enemObj[k].ship.bullObj[i].sprite.position.x >= m_width)
			enemObj[k].ship.bullObj[i].balloff=1;
		if(enemObj[k].ship.bullObj[i].sprite.position.y <= 0)
			enemObj[k].ship.bullObj[i].balloff=1;
		if(enemObj[k].ship.bullObj[i].sprite.position.y >= m_height)
			enemObj[k].ship.bullObj[i].balloff=1;
		enemObj[k].ship.bullObj[i].sprite.position.x += enemObj[k].ship.bullObj[i].sprite.hspeed*100.0f* dt;
		enemObj[k].ship.bullObj[i].sprite.position.y += enemObj[k].ship.bullObj[i].sprite.vspeed* 100.0f*dt;
		
		if (enemObj[k].ship.bullObj[i].balloff==1)
		{
			enemObj[k].ship.bullObj[i].balloff=0;
			enemObj[k].ship.bullObj[i].dmg=0;
			enemObj[k].ship.bullObj[i].sprite.position.x =10;
			enemObj[k].ship.bullObj[i].sprite.position.y =10;
			enemObj[k].ship.bullObj[i].sprite.color = D3DCOLOR_ARGB(0, 0, 0, 0); // Random color and alpha


			enemObj[k].ship.bullObj[i].sprite.hspeed=0;//(float(rand() % 300)-(float(rand() %600)))*100.0f* dt;
			enemObj[k].ship.bullObj[i].sprite.vspeed=0;//(float(rand() % 300)-(float(rand() %600)))*100.0f* dt;	
			//	enemObj[k].ship.bullets.pop ();
		}//
	}
		}
}

void DXGame::ClearBB()
{
		// Clear the back buffer, call BeginScene()
	HR(xDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER , xBackCol, 1.0f, 0));
	HR(xDevice->BeginScene())
	// Call Sprite's Begin to start rendering 2D sprite objects
	HR(xSpr->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_DEPTH_FRONTTOBACK));
}

void DXGame::RenderMenu(float dt)
{


	D3DXMatrixIdentity(&transMat);
	D3DXMatrixIdentity(&scaleMat);
	D3DXMatrixIdentity(&rotMat);
	D3DXMatrixIdentity(&worldMat);
	
	for (int i = 0;i <MAX_BUTT; ++i )
	{
		D3DXMatrixScaling(&scaleMat, buttObj[i].sprite.scale, buttObj[i].sprite.scale, 0.0f);			// Scaling
		D3DXMatrixRotationZ(&rotMat, buttObj[i].sprite.rotation);	// Rotation on Z axis, value in radians, converting from degrees
		D3DXMatrixTranslation(&transMat, buttObj[i].sprite.position.x, buttObj[i].sprite.position.y, 0.0f);	// Translation
		D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
		D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);	// Multiply scale and translation, store in world
		// Set Transform
		xSpr->SetTransform(&worldMat);
		// Draw the texture with the sprite object
		xSpr->Draw(textures[i+3], 0, &D3DXVECTOR3(infos[i+3].Width , infos[2].Height, 0.0f),
			0, buttObj[i].sprite.color);
	}

	// End drawing 2D sprites
	xSpr->End();

	//////////////////////////////////////////////////////////////////////////
	// Draw Text
	//////////////////////////////////////////////////////////////////////////

	//RIGHT
	//TOP
	xFont->DrawText(0, L" Allen Jones\n =======\n Features:\nTime Based Movements\n Menu System\n  Player Controls\n Enemie with AI \n Winning/losing\n Targeting with mouse \nFSM Menu\n", -1, &screenRect, DT_TOP | DT_RIGHT | DT_NOCLIP, D3DCOLOR_ARGB(255, 255, 0, 0));
	//BOTTOM
	xFont->DrawText(0, L" Bugs:\n========\nOffeset Collsionsn\n\n", -1, &screenRect, DT_BOTTOM | DT_RIGHT | DT_NOCLIP, D3DCOLOR_ARGB(255, 255, 0, 0));
	
	//LEFT
	//TOP
	xFont->DrawText(0, L" AI Features:\n========\n  FSM Brain\n Distance calculation \n Bayenesian Learning \n Clockwise determination \n Quadrant Detection \n Circleing steering \n Collision Avoidence \n Regrouping behaviour \n", -1, &screenRect, DT_TOP | DT_LEFT | DT_NOCLIP, D3DCOLOR_ARGB(255, 255, 0, 0));
	//BOTTOM
	xFont->DrawText(0, L" Optional Support:\============\n 3D Meshes\n Intro Video\n Multi-Player \n ", -1, &screenRect, DT_BOTTOM | DT_LEFT | DT_NOCLIP, D3DCOLOR_ARGB(255, 255, 0, 0));
	
	
	//xFont->DrawText(0, L"First Game\n Bugs:\n========\nOffeset Collsions\nEnemy 'Circle' offset\nTime movements offset\n\n\n", -1, &screenRect, DT_TOP | DT_LEFT | DT_NOCLIP, D3DCOLOR_ARGB(255, 255, 0, 0));

}

void DXGame::UpdateFPS(float dt)
{
		//////////////////////////////////////////////////////////////////////////
	// FPS and milliseconds per frame
	//////////////////////////////////////////////////////////////////////////
	// Calculate Frames per Second
		m_time += dt;

		static int fpsCounter = 0;
		if(m_time >= 1.0f)
		{
			// fps update
			m_time = 0;
			m_FPS = fpsCounter;
			fpsCounter = 0;	

		// updated ms every ~1s fps counter is updated
			m_dt = dt * 1000;
		}
		else
		{
			++fpsCounter;
		}


}

void DXGame::UpdateFMOD(float dt)
{
		//////////////////////////////////////////////////////////////////////////
	// FMOD Updates
	//////////////////////////////////////////////////////////////////////////
	
	
	
	//iLab Instruction:
	//FMOD::System *system;
		system->update();// class "FMOD::System" has no member "Update"

}

void DXGame::RenderSprite(float dt)
{
		//////////////////////////////////////////////////////////////////////////
	// Matrix Transformations to control sprite position, scale, and rotate
	// Set these matrices for each object you want to render to the screen
	//////////////////////////////////////////////////////////////////////////
	/*D3DXMatrixIdentity(&transMat);
	D3DXMatrixIdentity(&scaleMat);
	D3DXMatrixIdentity(&rotMat);
	D3DXMatrixIdentity(&worldMat);

	D3DXMatrixScaling(&scaleMat, paddleObj[0].scale, paddleObj[0].scale, 0.0f);			// Scaling
	D3DXMatrixRotationZ(&rotMat, paddleObj[0].rotation);	// Rotation on Z axis, value in radians, converting from degrees
	D3DXMatrixTranslation(&transMat, paddleObj[0].position.x, paddleObj[0].position.y, 0.0f);	// Translation
	D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
	D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);	// Multiply scale and translation, store in world
	// Set Transform
	xSpr->SetTransform(&worldMat);

	// Draw the texture with the sprite object
	xSpr->Draw(textures[0], 0, &D3DXVECTOR3(infos[0].Width, infos[0].Height , 0.0f),
		0, paddleObj[0].color);

	D3DXMatrixScaling(&scaleMat, ppplayer.scale, paddleObj[1].scale, 0.0f);			// Scaling
	D3DXMatrixRotationZ(&rotMat, paddleObj[1].rotation);	// Rotation on Z axis, value in radians, converting from degrees
	D3DXMatrixTranslation(&transMat, paddleObj[1].position.x, paddleObj[1].position.y, 0.0f);	// Translation
	D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
	D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);	// Multiply scale and translation, store in world
	// Set Transform
	xSpr->SetTransform(&worldMat);

	// Draw the texture with the sprite object
	xSpr->Draw(textures[0], 0, &D3DXVECTOR3(infos[0].Width, infos[0].Height , 0.0f),
		0, paddleObj[1].color);*/
	for (int i = 0; i <MAX_ASTER; ++i)
	{
	D3DXMatrixScaling(&scaleMat, asterObj[i].sprite.scale, asterObj[i].sprite.scale, 0.0f);			// Scaling
		D3DXMatrixRotationZ(&rotMat, asterObj[i].sprite.rotation);	// Rotation on Z axis, value in radians, converting from degrees
		D3DXMatrixTranslation(&transMat, asterObj[i].sprite.position.x, asterObj[i].sprite.position.y, 0.0f);	// Translation
		D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
		D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);	// Multiply scale and translation, store in world

		//D3DXMATRIX newmat;
	//	D3DXMatrixTransformation2D(&newmat, NULL, NULL, NULL, &D3DXVECTOR2(1, 1),  asterObj[i].sprite.rotation, &D3DXVECTOR2( asterObj[i].sprite.position.x, asterObj[i].sprite.position.y));
		//worldMat *= newmat;
		//m_sprite->SetTransform(&mat);

		// Set Transform
		xSpr->SetTransform(&worldMat);

		// Draw the texture with the sprite object
		xSpr->Draw(textures[8], 0, &D3DXVECTOR3(infos[8].Width , infos[8].Height, 0.0f),
			0, asterObj[i].sprite.color);
	}
	for (int i = 0; i <MAX_PART; ++i)
	{
	D3DXMatrixScaling(&scaleMat,part[i].sprite.scale, part[i].sprite.scale, 0.0f);			// Scaling
		D3DXMatrixRotationZ(&rotMat,part[i].sprite.rotation);	// Rotation on Z axis, value in radians, converting from degrees
		D3DXMatrixTranslation(&transMat,part[i].sprite.position.x,part[i].sprite.position.y, 0.0f);	// Translation
		D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
		D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);	// Multiply scale and translation, store in world
		
		xSpr->SetTransform(&worldMat);
		xSpr->Draw(textures[6], 0, &D3DXVECTOR3(infos[6].Width , infos[8].Height, 0.0f),
			0,part[i].sprite.color);
	}
	//////////////////////////////////////////////////////////////////////////
	// Assignment 1
	//////////////////////////////////////////////////////////////////////////
for (int i=0;i<MAX_ENEMY;++i)
{
	D3DXMatrixScaling(&scaleMat, enemObj[i].ship.sprite.scale, enemObj[i].ship.sprite.scale, 0.0f);			// Scaling
		D3DXMatrixRotationZ(&rotMat, enemObj[i].ship.sprite.rotation);	// Rotation on Z axis, value in radians, converting from degrees
		D3DXMatrixTranslation(&transMat, enemObj[i].ship.sprite.position.x, enemObj[i].ship.sprite.position.y, 0.0f);	// Translation
		D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
		D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);	// Multiply scale and translation, store in world

		// Set Transform
		xSpr->SetTransform(&worldMat);

		// Draw the texture with the sprite object
		xSpr->Draw(textures[2], 0, &D3DXVECTOR3(infos[2].Width , infos[2].Height, 0.0f),
			0, enemObj[i].ship.sprite.color);

	for (int h = 0; h <MAX_BULL; ++h)
	{
	D3DXMatrixScaling(&scaleMat, enemObj[i].ship.bullObj[h].sprite.scale, enemObj[i].ship.bullObj[h].sprite.scale, 0.0f);			// Scaling
		D3DXMatrixRotationZ(&rotMat, enemObj[i].ship.bullObj[h].sprite.rotation);	// Rotation on Z axis, value in radians, converting from degrees
		D3DXMatrixTranslation(&transMat, enemObj[i].ship.bullObj[h].sprite.position.x, enemObj[i].ship.bullObj[h].sprite.position.y, 0.0f);	// Translation
		D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
		D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);	// Multiply scale and translation, store in world

		// Set Transform
		xSpr->SetTransform(&worldMat);

		// Draw the texture with the sprite object
		xSpr->Draw(textures[7], 0, &D3DXVECTOR3(infos[7].Width , infos[7].Height, 0.0f),
			0, enemObj[i].ship.bullObj[h].sprite.color);
	
	D3DXMatrixScaling(&scaleMat, ppplayer.bullObj[h].sprite.scale, ppplayer.bullObj[h].sprite.scale, 0.0f);			// Scaling
		D3DXMatrixRotationZ(&rotMat, ppplayer.bullObj[h].sprite.rotation);	// Rotation on Z axis, value in radians, converting from degrees
		D3DXMatrixTranslation(&transMat, ppplayer.bullObj[h].sprite.position.x, ppplayer.bullObj[h].sprite.position.y, 0.0f);	// Translation
		D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
		D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);	// Multiply scale and translation, store in world

		// Set Transform
		xSpr->SetTransform(&worldMat);

		// Draw the texture with the sprite object
		xSpr->Draw(textures[7], 0, &D3DXVECTOR3(infos[7].Width , infos[7].Height, 0.0f),
			0, ppplayer.bullObj[h].sprite.color);
	}
}
D3DXMatrixScaling(&scaleMat, ppplayer.sprite.scale, ppplayer.sprite.scale, 0.0f);			// Scaling
		D3DXMatrixRotationZ(&rotMat, ppplayer.sprite.rotation);	// Rotation on Z axis, value in radians, converting from degrees
		D3DXMatrixTranslation(&transMat, ppplayer.sprite.position.x, ppplayer.sprite.position.y, 0.0f);	// Translation
		D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
		D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);	// Multiply scale and translation, store in world

		// Set Transform
		xSpr->SetTransform(&worldMat);

		// Draw the texture with the sprite object
		xSpr->Draw(textures[2], 0, &D3DXVECTOR3(infos[2].Width , infos[2].Height, 0.0f),
			0, ppplayer.sprite.color);


}

void DXGame::RenderFont(float dt)
{
	//TOP RIGHT
	xFont->DrawText(0, L"Allen Jones", -1, &screenRect, DT_TOP | DT_RIGHT | DT_NOCLIP, D3DCOLOR_ARGB(255, 255, 0, 0));
	xFont->DrawText(0, L"\n\n State Key: { DECIDE, PURSUE, FLEE, AVOID, CIRCLE, ATTACK, FLOCK };", -1, &screenRect, DT_TOP | DT_RIGHT | DT_NOCLIP, D3DCOLOR_ARGB(255, 255, 0, 0));
	
	
	//BOTTOM RIGHT
	xFont->DrawText(0, L"Avoid Red or Big rocks\nUse WASD to move!\nUse the Mouse to aim & Fire\n First to 0 Health Loses!\n \n\n\n", -1, &screenRect, DT_BOTTOM | DT_RIGHT | DT_NOCLIP, D3DCOLOR_ARGB(255, 0, 0, 255));
	wchar_t buffer[400];
	
	//TOP LEFT

	swprintf_s(buffer, 400, L" FPS: %d   \n    Ms: %0.2f\n\n  Enemy 1 (BLUE)  Stats\n Health: %d\n State: %  d\nClockwise?  %d\n Cold temp: %  d\Norm Temp:  %d\n Small size: %  d\MedSize:  %d\n\n  Enemy 2 (RED) Stats\n Health: %d\n State: %  d\nClockwise?  %d\n Cold temp: %  d\Norm Temp:  %d\n Small size: %  d\MedSize:  %d\n", 
		m_FPS, m_dt,
		enemObj[0].ship.HP, 
		enemObj[0].brain.thought,
		enemObj[0].clockwise, 
		enemObj[0].brain.COLDTEMP,
		enemObj[0].brain.NORMTEMP, 
		enemObj[0].brain.SMALLSIZE,
		enemObj[0].brain.MEDSIZE,
		enemObj[1].ship.HP, 
		enemObj[1].brain.thought,
		enemObj[1].clockwise, 
		enemObj[1].brain.COLDTEMP,
		enemObj[1].brain.NORMTEMP, 
		enemObj[1].brain.SMALLSIZE,
		enemObj[1].brain.MEDSIZE);

	xFont->DrawText(0, buffer, -1, &screenRect, DT_TOP | DT_NOCLIP, D3DCOLOR_ARGB(255, 255, 0, 0));
	
	//swprintf_s(buffer, 64, L"\n\n\n Enemy 1\n Hspeed: %f \n Vspeed: %f \n", enemObj[0].ship.sprite.hspeed, enemObj[0].ship.sprite.vspeed);
	//xFont->DrawText(0, buffer, -1, &screenRect, DT_TOP | DT_NOCLIP, D3DCOLOR_ARGB(255, 255, 0, 0));
	//swprintf_s(buffer, 64, L"\n\n\n \n\n \n\n Enemy 2\n Hspeed: %f \n Vspeed: %f \n", enemObj[1].ship.sprite.hspeed, enemObj[1].ship.sprite.vspeed);
	//xFont->DrawText(0, buffer, -1, &screenRect, DT_TOP | DT_NOCLIP, D3DCOLOR_ARGB(255, 255, 0, 0));

	// BOTTOM LEFT
	//1+2 //3+4
	swprintf_s(buffer, 64, L" Player stats\n Health: %d\n\n \n", ppplayer.HP);
	xFont->DrawText(0, buffer, -1, &screenRect, DT_BOTTOM | DT_NOCLIP, D3DCOLOR_ARGB(255, 0, 255, 0));
/*	//5+6
	swprintf_s(buffer, 64, L"\n \n\n\n\n \n\n");
	xFont->DrawText(0, buffer, -1, &screenRect, DT_BOTTOM | DT_NOCLIP, D3DCOLOR_ARGB(255, 255, 0, 0));
	//7+8
	swprintf_s(buffer, 64, L"\n\n ");
	xFont->DrawText(0, buffer, -1, &screenRect, DT_TOP | DT_NOCLIP, D3DCOLOR_ARGB(255, 255, 0, 0));
	
	swprintf_s(buffer, 64, L"\n \n\n \n\n\n\n\n \n\n");
	xFont->DrawText(0, buffer, -1, &screenRect, DT_TOP | DT_NOCLIP, D3DCOLOR_ARGB(255, 255, 0, 0));*/
/*	swprintf_s(buffer, 64, L"\n \n\n \n\n\n\n \n\n");
	xFont->DrawText(0, buffer, -1, &screenRect, DT_TOP | DT_NOCLIP, D3DCOLOR_ARGB(255, 255, 0, 0));
	//9+10
	//swprintf_s(buffer, 64, L"Accel: %f\nEnemy 1\n Bullets: %d\n \n\n \n\n \n\n \n\n", enemObj[0].ship.accel,enemObj[0].ship.bullet_Count);
	//xFont->DrawText(0, buffer, -1, &screenRect, DT_BOTTOM | DT_NOCLIP, D3DCOLOR_ARGB(255, 255, 0, 0));
	
	//11+12
	swprintf_s(buffer, 64, L"\n Health: %d\n \n\n  \n\n \n\n \n\n \n\n", enemObj[1].ship.HP);
	xFont->DrawText(0, buffer, -1, &screenRect, DT_BOTTOM | DT_NOCLIP, D3DCOLOR_ARGB(255, 0, 0, 255));
	//13+14
	swprintf_s(buffer, 64, L"Enemy 2 Stats\n State: %  d\nClockwise?  %d\n \n\n \n\n \n\n \n\n \n\n \n\n \n\n", enemObj[1].brain.thought,enemObj[1].clockwise);
	xFont->DrawText(0, buffer, -1, &screenRect, DT_BOTTOM | DT_NOCLIP, D3DCOLOR_ARGB(255, 0, 0, 255));
	*///15+16
	//swprintf_s(buffer, 64, L"Accel: %f\nEnemy 2\n Bullets: %d\n \n\n \n\n \n\n \n\n \n\n \n\n \n\n \n\n", enemObj[1].ship.accel,enemObj[1].ship.bullet_Count);
	//xFont->DrawText(0, buffer, -1, &screenRect, DT_BOTTOM | DT_NOCLIP, D3DCOLOR_ARGB(255, 255, 0, 0));
	

}

void DXGame::Render(float dt)
{
	// Clear the back buffer, call BeginScene()
		ClearBB();


	RenderSprite(dt);
	HR(xSpr->End();)

	RenderFont(dt);

	//RenderEffect(dt);
	//RenderPrimMesh(dt);

	// EndScene, and Present the back buffer to the display buffer
	HR(xDevice->EndScene());
	HR(xDevice->Present(0, 0, 0, 0));

}

void DXGame::RenderEffect(float dt)
{
	//////////////////////////////////////////////////////////////////////////
	// Effect
	//////////////////////////////////////////////////////////////////////////
	// All drawn primitives are sharing the same shader, just pass in different
	// app variables to the shader and call CommitChanges() prior to Draw calls

	// Set the technique to use
	xEffect->SetTechnique(xTech);

	// Begin Effect
	numPasses = 0;
	xEffect->Begin(&numPasses, 0);

}

void DXGame::RenderPrimMesh(float dt)
{
	for(UINT i = 0; i < numPasses; ++i)
	{
		xEffect->BeginPass(i);
		//////////////////////////////////////////////////////////////////////////
		// Box
		//////////////////////////////////////////////////////////////////////////

		// Calculate Matrix Transform
		D3DXMatrixScaling(&scaleMat, 1.0f, 1.0f, 1.0f);			// Scaling
		D3DXMatrixRotationYawPitchRoll(&rotMat, timeGetTime() * -0.001f, timeGetTime() * -0.001f, timeGetTime() * -0.001f); // Rotation on Yaw, Pitch, and Roll
		D3DXMatrixTranslation(&transMat, -4.0f, -3.0f, 0.0f);		// Translation
		D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
		D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);	// Multiply scale and translation, store in world
		/*D3DXMatrixScaling(&scaleMat, enemObj[0].ship.sprite.scale, enemObj[0].ship.sprite.scale, 0.0f);			// Scaling
		D3DXMatrixRotationZ(&rotMat, enemObj[0].ship.sprite.rotation);	// Rotation on Z axis, value in radians, converting from degrees
		D3DXMatrixTranslation(&transMat, enemObj[0].ship.sprite.position.x, enemObj[0].ship.sprite.position.y, 0.0f);	// Translation
		D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
		D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);	// Multiply scale and translation, store in world
		*/

	
		// Set object specific effect parameters
		D3DXMatrixInverse(&WIT, 0, &worldMat);
		D3DXMatrixTranspose(&WIT, &WIT);
		xEffect->SetMatrix("worldViewProjMat", &(worldMat * viewMat * projMat));
		xEffect->SetMatrix("worldInverseTransposeMat", &WIT);
		xEffect->SetMatrix("worldMat", &worldMat);
		
		xEffect->SetValue("ambientMaterial", &material[0] .Ambient, sizeof(D3DXCOLOR));
		xEffect->SetValue("diffuseMaterial", &material[0] .Diffuse, sizeof(D3DXCOLOR));
		xEffect->SetValue("specularMaterial", &material[0].Specular, sizeof(D3DXCOLOR));
		xEffect->SetFloat("specularPower", material[0] .Power);
		xEffect->SetTexture("tex", otherTexture[0] );
		xEffect->SetBool("isTex", true);
		// After setting object specific parameters, commit changes
		xEffect->CommitChanges();
		// Draw the mesh subset
		xBox->DrawSubset(0);

		//End Pass
		xEffect->EndPass();
	}
	xEffect->End();
}

void DXGame::RenderMesh(float dt)
{
		for(UINT i = 0; i < numPasses; ++i)
	{
		xEffect->BeginPass(i);
		//////////////////////////////////////////////////////////////////////////
		// Character 1
		//////////////////////////////////////////////////////////////////////////

		// Calculate Matrix Transform
		D3DXMatrixScaling(&scaleMat, 6.0f, 6.0f, 6.0f);			// Scaling
		D3DXMatrixRotationYawPitchRoll(&rotMat, timeGetTime() * -0.001f,0.0f,0.0f); // Rotation on Yaw, Pitch, and Roll
		D3DXMatrixTranslation(&transMat, 2.0f, -3.0f, 0.0f);		// Translation
		D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
		D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);	// Multiply scale and translation, store in world
		
		// Set object specific effect parameters
		D3DXMatrixInverse(&WIT, 0, &worldMat);
		D3DXMatrixTranspose(&WIT, &WIT);
		xEffect->SetMatrix("worldViewProjMat", &(worldMat * viewMat * projMat));
		xEffect->SetMatrix("worldInverseTransposeMat", &WIT);
		xEffect->SetMatrix("worldMat", &worldMat);

	
		for ( DWORD q = 0; q < xNumMat; ++q) 
		{
			xEffect->SetValue("ambientMaterial",&meshMats[q] .Ambient, sizeof(D3DXCOLOR));
			xEffect->SetValue("diffuseMaterial",&meshMats[q] .Diffuse, sizeof(D3DXCOLOR));
			xEffect->SetValue("specularMaterial",&meshMats[q] .Specular, sizeof(D3DXCOLOR));
			xEffect->SetFloat("specularPower",meshMats[q] .Power);
			xEffect->SetTexture("tex", meshTextures[q]  );
			xEffect->SetBool("isTex", true);
			// After setting object specific parameters, commit changes
			xEffect->CommitChanges();
			// Draw the mesh subset
			xMesh->DrawSubset(q);
		}
		//End Pass
		xEffect->EndPass();
	}
	xEffect->End();
}

void DXGame::Shutdown()
{
	// Release COM objects in the opposite order they were created in
	SAFE_RELEASE(xEffect);
	SAFE_RELEASE(xEffErr); 

	//SAFE_RELEASE(xDIobj);	
	SAFE_RELEASE(xKeys);
	SAFE_RELEASE(xMouse);

	for (int i=0;i<MAX_TEXTURES;++i)
		SAFE_RELEASE(textures[i]);
		
	for (int i=0;i<3;++i)
		SAFE_RELEASE(otherTexture[i]);
	
	SAFE_RELEASE(xSpr);//Freed 6!!!

	SAFE_RELEASE(xBox); //Freed2!
	
	SAFE_RELEASE(xMesh);

	SAFE_RELEASE(xAbuff); 
	SAFE_RELEASE(xVbuff); 
	SAFE_RELEASE(xIbuff); 
	SAFE_RELEASE(xVdecl); 
		
	SAFE_RELEASE(xMats);//Freed2!
	SAFE_RELEASE(xotherEff);//Freed2!
	SAFE_RELEASE(Abuff);//Freed 2!
		

	SAFE_RELEASE(xFont);
	RemoveFontResourceEx(L"font/Delicious-Roman.otf", FR_PRIVATE, 0);
	
	//FMOD clean up
	system->release();

	SAFE_RELEASE(xDevice);
	SAFE_RELEASE(xObject); 
}

void DXGame::onLostDevice()
{
	xFont->OnLostDevice();
	xSpr->OnLostDevice();
}

void DXGame::onResetDevice()
{
	xFont->OnResetDevice();
	xSpr->OnResetDevice();
}

bool DXGame::isDeviceLost()
{
	HRESULT hr = xDevice->TestCooperativeLevel();
	if(hr == D3DERR_DEVICELOST)
	{
		Sleep(20);
		return true;
	}
	else if(hr == D3DERR_DRIVERINTERNALERROR)
	{
		MessageBox(0, L"Internal Driver Error.  This application must exit.", 0, 0);
		PostQuitMessage(0);
		return true;
	}
	else if(hr == D3DERR_DEVICENOTRESET) 
	{
		onLostDevice();
		xDevice->Reset(&x_3D);
		onResetDevice();

		return false;
	}
	else
	{
		return false;
	}
}

void DXGame::Ship::LightThrust(int direct, float dt)
{
switch (direct)
{
case 1:
	{
	this->sprite.hspeed-=(this->accel*dt)*0.66f;
	break;
	}
case 2:
	{
	this->sprite.vspeed+=(this->accel*dt)*0.66f;
	break;
	}
case 3:
	{
	this->sprite.hspeed+=(this->accel*dt)*0.66f;
	break;
	}
case 4:
	{
	this->sprite.vspeed-=(this->accel*dt)*0.66f;
	break;
	}
}
}

void DXGame::Ship::Thrust(int direct, float dt)
{
switch (direct)
{
case 1:
	{
	this->sprite.hspeed-=this->accel*dt;
	break;
	}
case 2:
	{
	this->sprite.vspeed+=this->accel*dt;
	break;
	}
case 3:
	{
	this->sprite.hspeed+=this->accel*dt;
	break;
	}
case 4:
	{
	this->sprite.vspeed-=this->accel*dt;
	break;
	}
}
}

void DXGame::AIBot::update( float dt)
{
	this->takebreak+=dt;
	this->canchoose+=dt;
	this->canchase+=dt;
	this->ship.Update(dt);
}

void DXGame::Ship::Update( float dt)
{
	this->canshoot+=dt;
	this->flashing+=dt;

	if (this->sprite.hspeed>this->sprite.vspeed)
		this->sprite.speed=this->sprite.hspeed;
	else
		this->sprite.speed=this->sprite.vspeed;


	this->sprite.position.x += this->sprite.hspeed * dt;
	this->sprite.position.y += this->sprite.vspeed * dt;

	if (this->sprite.hspeed>0.005f)
	{
		if (this->sprite.hspeed>this->topAcc)
			this->sprite.hspeed =this->topAcc;

		this->sprite.hspeed -=100.0f*dt;
	}
	else if (this->sprite.hspeed<-0.005f)
	{
		if (this->sprite.hspeed<(this->topAcc*(-1)))
			this->sprite.hspeed =(this->topAcc*(-1));
		
		this->sprite.hspeed +=100.0f*dt;
	}	
	else
		this->sprite.hspeed =0.0f;
	
		if (this->sprite.vspeed>0.005f)
	{
		if (this->sprite.vspeed>this->topAcc)
			this->sprite.vspeed =this->topAcc;
		
		this->sprite.vspeed -=100.0f*dt;
	}
	else if (this->sprite.vspeed<-0.005f)
	{
		if (this->sprite.vspeed<(this->topAcc*(-1)))
			this->sprite.vspeed =(this->topAcc*(-1));
		
		this->sprite.vspeed  +=100.0f*dt;
	}	
	else
		this->sprite.vspeed =0.0f;

	if (this->sprite.position.x>m_width)
	{
		this->sprite.position.x=m_width;
		this->sprite.hspeed=0;
	}
	if (this->sprite.position.y>m_height)
	{
		this->sprite.position.y=m_height;
		this->sprite.vspeed=0;
	}
	if (this->sprite.position.x<0)
	{
		this->sprite.position.x=0;
		this->sprite.hspeed=0;
	}
	if (this->sprite.position.y<0)
	{
		this->sprite.position.y=0;
		this->sprite.vspeed=0;
	}

}
void DXGame::Particle::update( float dt)
{

	this->sprite.position.x += this->sprite.hspeed * dt;
	this->sprite.position.y += this->sprite.vspeed * dt;
	//	this->sprite.rotation += D3DXToRadian(dt*5000);
	this->sprite.color = D3DCOLOR_ARGB(INT(this->life), 255, 255, 255); // Random color and alpha

	if (this->life<0.0f)
	{
			this->sprite.position.x =10;
			this->sprite.position.y =10;
			this->sprite.hspeed=0;//(float(rand() % 300)-(float(rand() %600)))*100.0f* dt;
			this->sprite.vspeed=0;//(float(rand() % 300)-(float(rand() %600)))*100.0f* dt;	
	}
	else
		life-=dt*300;
}void DXGame::SpaceRock::Crumble()
{
	this->off=1;
}
void DXGame::SpaceRock::Update( float dt)
{

	this->sprite.position.x += this->sprite.hspeed * dt;
	this->sprite.position.y += this->sprite.vspeed * dt;
	//	this->sprite.rotation += D3DXToRadian(dt*5000);

	if (this->sprite.position.x>(m_width+100) ||
		this->sprite.position.y>(m_height+100)||
		this->sprite.position.x<-100||
		this->sprite.position.y<-100)
		
		this->off=1;

	if (this->off)
	{
		float random=rand()%100;
		if( random>25)
		{	this->sprite.position = D3DXVECTOR3((INT(rand() % m_width)),100+m_height, 0.0f); // Random position within screen width and height
		if( random>50)
		{	this->sprite.position = D3DXVECTOR3(m_width+100, (INT(rand() %m_height)), 0.0f); // Random position within screen width and height
		if( random>75)
		{	this->sprite.position = D3DXVECTOR3(-100, (INT(rand() %m_height)), 0.0f); // Random position within screen width and height
		 }}}
		 else 
			this->sprite.position = D3DXVECTOR3((INT(rand() % m_width)), -100, 0.0f); // Random position within screen width and height


		this->sprite.scale = INT(1+rand() % 3);
		this->sprite.w=30*this->sprite.scale;
		this->sprite.h=30*this->sprite.scale;
		this->HP=2*this->sprite.scale;
		this->sprite.hspeed=((rand() % 250)-(rand() % 500))*0.5f;
		this->sprite.vspeed=((rand() % 250)-(rand() % 500))*0.5f;
		this->off=0;

		this->size=10*this->sprite.scale;
		this->temp=rand()%100;
		this->scaredist=100*this->sprite.scale;
		
		if(this->temp<COLDTEMP2)
			this->sprite.color = D3DCOLOR_ARGB(255, 0,0, 255); // Random color and alpha
		else if (this->temp>NORMTEMP2)
			this->sprite.color = D3DCOLOR_ARGB(255, 255,0, 0); // Random color and alpha
		else
			this->sprite.color = D3DCOLOR_ARGB(255, 255,255, 255); // Random color and alpha

	}
}

void DXGame::Ship::Fire(float dt, float targx, float targy)
{
	if (this->bullet_Count>=0 &&
		this->bullet_Count<=30 &&
		this->canshoot>=0.3f)
	{
		this->bullObj[bullet_Count].sprite.position = this->sprite.position; // Random rotation
		this->bullObj[bullet_Count].sprite.rotation = D3DXToRadian(rand() % 360); // Random rotation
		this->bullObj[bullet_Count].sprite.scale =1;
		this->bullObj[bullet_Count].sprite.w =20;
		this->bullObj[bullet_Count].sprite.h =20;
		this->bullObj[bullet_Count].sprite.color = D3DCOLOR_ARGB(255, 255,  255,  255); // Random color and alpha
 		this->bullObj[bullet_Count].sprite.BBox = XRect(this->bullObj[bullet_Count].sprite,this->bullObj[bullet_Count].sprite.position.x, this->bullObj[bullet_Count].sprite.position.x+ this->bullObj[bullet_Count].sprite.w,this->bullObj[bullet_Count].sprite.position.y, this->bullObj[bullet_Count].sprite.position.y+ this->bullObj[bullet_Count].sprite.h );
		this->bullObj[bullet_Count].sprite.hspeed= (targx-this->sprite.position.x)*50.0f* dt;
		this->bullObj[bullet_Count].sprite.vspeed= (targy-this->sprite.position.y)*50.0f* dt;	
		this->bullObj[bullet_Count].dmg=1;	
		
		++this->bullet_Count;
		
		this->canshoot=0.0f;

	}
}

void DXGame::explode( D3DCOLOR color, float dt,int size,int x, int y)
{

	for(int i=0;i<size;++i)
	{
		part[partCount].sprite.position.x=x; // Random rotation
		part[partCount].sprite.position.y=y; // Random rotation
		part[partCount].sprite.rotation = D3DXToRadian(rand() % 360); // Random rotation
		part[partCount].sprite.scale =1;
		part[partCount].sprite.color = color; // Random color and alpha
		part[partCount].sprite.hspeed= ((rand()%100)-(rand()%200))*1500.0f* dt;
		part[partCount].sprite.vspeed= ((rand()%100)-(rand()%200))*1500.0f* dt;
		part[partCount].life=150+rand()%100;	
		++partCount;
		if (partCount>=MAX_PART)
			partCount=0;

	}
}

void DXGame::AIBot::pursue(float dt)
{
	//this->ship.sprite.color = D3DCOLOR_ARGB(255,0, rand() % 255, rand() % 255); // Random color and alpha

	if (this->ship.sprite.position.x < this->targx)
	{
		this->ship.Thrust(3,dt);	
		this->ship.Thrust(3,dt);
	}
	else
	{
		this->ship.Thrust(1,dt);
		this->ship.Thrust(1,dt);
	}

	if (this->ship.sprite.position.y < this->targy)
	{
		this->ship.Thrust(2,dt);
		this->ship.Thrust(2,dt);
	}
	else
	{
		this->ship.Thrust(4,dt);
		this->ship.Thrust(4,dt);
	}
}

void DXGame::AIBot::flee(float dt)
{
	//this->ship.sprite.color = D3DCOLOR_ARGB(255,100+ rand() % 155,  rand() % 155,  rand() % 155); // Random color and alpha

	if (this->ship.sprite.position.x < this->targ2x)
		this->ship.Thrust(1,dt);
	else
		this->ship.Thrust(3,dt);


	if (this->ship.sprite.position.y < this->targ2y)
		this->ship.Thrust(4,dt);
	else
		this->ship.Thrust(2,dt);

	this->RanAway+=dt;
	if (this->RanAway>=0.45f)
	{
		this->override=1;
		this->targx=this->targ2x;
		this->targy=this->targ2y;
		this->attack(dt);
	}

}

void DXGame::AIBot::avoid(float dt)
{

	if (this->ship.sprite.position.x < this->targx)
	{
		if (this->ship.sprite.position.x < this->targ2x)
		{
		//this->ship.sprite.color = D3DCOLOR_ARGB(rand()%255,0, 0,0); // Random color and alpha
		//	this->ship.LightThrust(3,dt);
			if (this->ship.sprite.position.y < this->targ2y)
				this->ship.Thrust(2,dt);
			else
				this->ship.Thrust(4,dt);
		}
		else if (this->ship.sprite.position.x > this->targ2x)
			this->ship.Thrust(3,dt);
	}
	else 
	{
		if (this->ship.sprite.position.x < this->targ2x)
		{
			this->ship.Thrust(1,dt);
		}
		else 
		{
	//	this->ship.sprite.color = D3DCOLOR_ARGB(rand()%255,rand()%255, 0,0); // Random color and alpha
		//	this->ship.LightThrust(1,dt);
			if (this->ship.sprite.position.y < this->targ2y)
				this->ship.Thrust(4,dt);
			else
				this->ship.Thrust(2,dt);
		}
	}
	/*
	if (this->ship.sprite.position.y < this->targy)
	{
		if (this->ship.sprite.position.y < this->targ2y)
		{
		this->ship.sprite.color = D3DCOLOR_ARGB(rand()%255,0, rand()%255,0); // Random color and alpha
		//	this->ship.LightThrust(4,dt);
			if (this->ship.sprite.position.x < this->targ2x)
				this->ship.Thrust(3,dt);
			else
				this->ship.Thrust(1,dt);
		}
		else if (this->ship.sprite.position.y > this->targ2y)
			this->ship.Thrust(2,dt);
	}
	else
	{
		if (this->ship.sprite.position.y < this->targ2y)
		{
			this->ship.Thrust(2,dt);
		}
		else
		{
		this->ship.sprite.color = D3DCOLOR_ARGB(rand()%255,0, 0,rand()%255); // Random color and alpha
			//this->ship.LightThrust(4,dt);
			if (this->ship.sprite.position.x < this->targ2x)
				this->ship.Thrust(3,dt);
			else
				this->ship.Thrust(1,dt);
		}
	}*/
	/*
	if (this->ship.sprite.position.y < this->targy)
		if (this->ship.sprite.position.y < this->targ2y)
		{
			this->ship.LightThrust(2,dt);
			this->ship.Thrust(1,dt);
		}
		else
			this->ship.Thrust(4,dt);*/

}

void DXGame::AIBot::circle(float dt)
{
	if (this->canchase>=0.2)
		this->canchase=0.0f;
	this->lastQ=this->daQuad;
	this->daQuad = quadToPlay(
		this->ship.sprite.position.x, 
		this->ship.sprite.position.y,
		targx, 
		targy);

	if (this->clocklock==0)
	{
		this->clockwise=this->clockWise();
		this->clocklock=1;
	}

	//if (this->lastQ
	if (this->clockwise)
	{
	//this->ship.sprite.color = D3DCOLOR_ARGB(255,0, 0, rand() % 255); // Random color and alpha
switch (this->daQuad)
	{
	case 1:
		{
			if(this->ship.sprite.position.x > (this->targx-this->closedist))
				//left
			{
			//	
				this->ship.Thrust(1,dt);
			}
			else
			{
				//this->ship.sprite.color = D3DCOLOR_ARGB(255,0, rand() % 255, 0); // Random color and alpha
				this->ship.LightThrust(3,dt);
			}
			//if(this->ship.sprite.position.y > (this->targy+this->closedist))
				this->ship.Thrust(4,dt);
			//else
			//	this->ship.Thrust(2,dt);




			/*
			//RIGHT
			if (this->dist
				<200)
				this->ship.LightThrust(1,dt);
			else
			this->ship.LightThrust(3,dt);*/
			break;
		}

	case 2:
		{
			//if(this->ship.sprite.position.x < (this->targx-this->closedist))
				//left
				this->ship.Thrust(3,dt);
			//else
			//	this->ship.Thrust(1,dt);

			if(this->ship.sprite.position.y < (this->targy-this->closedist))
				{//left
				//this->ship.sprite.color = D3DCOLOR_ARGB(255,0, rand() % 255, 0); // Random color and alpha
					this->ship.LightThrust(2,dt);}
			else{
			//	this->ship.sprite.color = D3DCOLOR_ARGB(255,0, 0, rand() % 255); // Random color and alpha
				this->ship.Thrust(4,dt);
			}

			/*

			//RIGHT
				this->ship.Thrust(3,dt);
			//Down
			//RIGHT
			if (this->dist
				<200)
				this->ship.LightThrust(4,dt);
			else
			this->ship.LightThrust(2,dt);*/
			break;
		}

	case 3:
		{
			if(this->ship.sprite.position.x < (this->targx+this->closedist))
				{//left
				//this->ship.sprite.color = D3DCOLOR_ARGB(255,0, 0, rand() % 255); // Random color and alpha
				this->ship.Thrust(3,dt);}
			else
				{//this->ship.sprite.color = D3DCOLOR_ARGB(255,0, rand() % 255, 0); // Random color and alpha
				this->ship.LightThrust(1,dt);
			}
			//if(this->ship.sprite.position.y < (this->targy-this->closedist))
				//left
				this->ship.Thrust(2,dt);
			//else
			//	this->ship.Thrust(4,dt);
			/*


			//Down
				this->ship.Thrust(2,dt);
			//Left
			//RIGHT
			if (this->dist
				<200)
				this->ship.LightThrust(3,dt);
			else
			this->ship.LightThrust(1,dt);*/
			break;
		}

	case 4:
		{
			//if(this->ship.sprite.position.x > (this->targx+this->closedist))
				this->ship.Thrust(1,dt);
			///else
			//	this->ship.Thrust(3,dt);
			
			if(this->ship.sprite.position.y > (this->targy+this->closedist))
			{	//left
				//this->ship.sprite.color = D3DCOLOR_ARGB(255,0, rand() % 255, 0); // Random color and alpha
				this->ship.LightThrust(4,dt);}
			else
			{ 				//this->ship.sprite.color = D3DCOLOR_ARGB(255, 0, rand() % 255, 0); // Random color and alpha
// Random color and alpha
			this->ship.Thrust(2,dt);}
			
			
			/*
			
			//LEFT
				this->ship.Thrust(1,dt);
			//UP
			//RIGHT
			if (this->dist
				<200)
				this->ship.LightThrust(2,dt);
			else
			this->ship.LightThrust(4,dt);*/
			break;
		}
	/*case 5:
		{
			this->ship.sprite.color = D3DCOLOR_ARGB( 255,0, 0, rand()%255); // Random color and alpha
			
			if (this->lastQ==5)
			this->daQuad=this->lastQ;


			this->daQuad=this->lastQ;
			this->daQuad++;

			if (this->daQuad>4)
				this->daQuad-=4;

			this->Qlock=1;
			break;
		}*/

	}
	}
	else//CC
	{
			switch (this->daQuad)
	{
	case 1:
		{
			if(this->ship.sprite.position.y > (this->targy+this->closedist))
				//left
			{
			//	this->ship.sprite.color = D3DCOLOR_ARGB(255,0, rand() % 255, 0); // Random color and alpha
				this->ship.LightThrust(1,dt);
			}
			else
			{
				//this->ship.sprite.color = D3DCOLOR_ARGB(255,0, 0, rand() % 255); // Random color and alpha

				this->ship.Thrust(2,dt);
			}
			//if(this->ship.sprite.position.y > (this->targy+this->closedist))
				this->ship.Thrust(3,dt);
			//else
			//	this->ship.Thrust(2,dt);




			/*
			//RIGHT
			if (this->dist
				<200)
				this->ship.LightThrust(1,dt);
			else
			this->ship.LightThrust(3,dt);*/
			break;
		}

	case 2:
		{
			//if(this->ship.sprite.position.x < (this->targx-this->closedist))
				//left
				this->ship.Thrust(2,dt);
			//else
			//	this->ship.Thrust(1,dt);

			if(this->ship.sprite.position.x < (this->targx-this->closedist))
				{//left
			//	this->ship.sprite.color = D3DCOLOR_ARGB(255,0, rand() % 255, 0); // Random color and alpha
					this->ship.LightThrust(3,dt);}
			else{
			//	this->ship.sprite.color = D3DCOLOR_ARGB(255,0, 0, rand() % 255); // Random color and alpha
				this->ship.Thrust(1,dt);
			}

			/*

			//RIGHT
				this->ship.Thrust(3,dt);
			//Down
			//RIGHT
			if (this->dist
				<200)
				this->ship.LightThrust(4,dt);
			else
			this->ship.LightThrust(2,dt);*/
			break;
		}

	case 3:
		{
			if(this->ship.sprite.position.y < (this->targy-this->closedist))
				{//left
				//this->ship.sprite.color = D3DCOLOR_ARGB(255,0, rand() % 255, 0); // Random color and alpha
				this->ship.LightThrust(2,dt);
			}
			else
				{				
				//this->ship.sprite.color = D3DCOLOR_ARGB(255,0, 0, rand() % 255); // Random color and alpha
				this->ship.Thrust(4,dt);
			}
			//if(this->ship.sprite.position.y < (this->targy-this->closedist))
				//left
				this->ship.Thrust(1,dt);
			//else
			//	this->ship.Thrust(4,dt);
			/*


			//Down
				this->ship.Thrust(2,dt);
			//Left
			//RIGHT
			if (this->dist
				<200)
				this->ship.LightThrust(3,dt);
			else
			this->ship.LightThrust(1,dt);*/
			break;
		}

	case 4:
		{
			//if(this->ship.sprite.position.x > (this->targx+this->closedist))
				this->ship.Thrust(4,dt);
			///else
			//	this->ship.Thrust(3,dt);
			
			if(this->ship.sprite.position.x > (this->targx+this->closedist))
			{	//left
			//	this->ship.sprite.color = D3DCOLOR_ARGB(255,0, rand() % 255, 0); // Random color and alpha
				this->ship.LightThrust(1,dt);}
			else
			{ 		//		this->ship.sprite.color = D3DCOLOR_ARGB(255, 0, rand() % 255, 0); // Random color and alpha
// Random color and alpha
			this->ship.Thrust(3,dt);}
			
			
			/*
			
			//LEFT
				this->ship.Thrust(1,dt);
			//UP
			//RIGHT
			if (this->dist
				<200)
				this->ship.LightThrust(2,dt);
			else
			this->ship.LightThrust(4,dt);*/
			break;
		}
	/*case 5:
		{
			this->ship.sprite.color = D3DCOLOR_ARGB( 255,0, 0, rand()%255); // Random color and alpha
			
			if (this->lastQ==5)
			this->daQuad=this->lastQ;


			this->daQuad=this->lastQ;
			this->daQuad++;

			if (this->daQuad>4)
				this->daQuad-=4;

			this->Qlock=1;
			break;
		}*/

	}

		/*
	switch (this->daQuad)
	{
	case 1:
		{
			//right
			this->ship.Thrust(3,dt);
			//up
			//RIGHT
			if (this->dist
				<200)
				this->ship.LightThrust(2,dt);
			else
			this->ship.LightThrust(4,dt);
			break;
		}

	case 2:
		{
			//down 
				this->ship.Thrust(2,dt);
			//r
			//RIGHT
			if (this->dist
				<200)
				this->ship.LightThrust(1,dt);
			else
			this->ship.LightThrust(3,dt);
			break;
		}

	case 3:
		{
			//lwft
				this->ship.Thrust(1,dt);
			//down
			//RIGHT
			if (this->dist
				<200)
				this->ship.LightThrust(4,dt);
			else
			this->ship.LightThrust(2,dt);
			break;
		}

	case 4:
		{
			//up
				this->ship.Thrust(4,dt);
			//left
			//RIGHT
			if (this->dist
				<200)
				this->ship.LightThrust(3,dt);
			else
			this->ship.LightThrust(1,dt);
			break;
		}
	/*case 5:
		{
			this->ship.sprite.color = D3DCOLOR_ARGB( 255,0, 0, rand()%255); // Random color and alpha
			
			this->daQuad=this->lastQ;
			this->daQuad--;

			if (this->daQuad<0)
				this->daQuad+=4;

			this->Qlock=1;
			break;
		}*/

	//}
	}

}

void DXGame::AIBot::attack(float dt)
{
	if( this->takebreak>=3.0f ||
		this->override)
	{
		
		this->ship.Fire(dt,this->targx,this->targy);
		if (this->takebreak>=6.0f)
			this->takebreak=0.0f;
	}
}

void DXGame::AIBot::flock(float dt)
{
//this->ship.sprite.color = D3DCOLOR_ARGB(255,rand() % 255, 0, 0); // Random color and alpha

	if (this->ship.sprite.position.x < this->friendx)
		this->ship.Thrust(3,dt);
	else
		this->ship.Thrust(1,dt);


	if (this->ship.sprite.position.y < this->friendy)
		this->ship.Thrust(2,dt);
	else
		this->ship.Thrust(4,dt);

}

void DXGame::AIBot::act(float dt)
{	//if (this->clockWise())
			//this->ship.sprite.color = D3DCOLOR_ARGB( 255, rand()%166,0, 0); // Random color and alpha


	switch (this->brain.thought)
	{
	case Think::DECIDE:
		{
			break;
		}
	case Think::PURSUE:
		{
			this->RanAway=0;
			this->clocklock=0;
			this->override=0;
			this->pursue(dt);
			break;
		}
	case Think::FLEE:
		{

			this->clocklock=0;
			this->flee(dt);
			break;
		}
	case Think::AVOID:
		{

			this->RanAway=0;
			this->override=0;
			this->avoid(dt);
			break;
		}
	case Think::CIRCLE:
		{

			this->RanAway=0;
			this->override=0;
			this->circle(dt);
			break;
		}
	case Think::ATTACK:
		{

			this->RanAway=0;
			this->attack(dt);
			break;
		}
	case Think::FLOCK:
		{

			this->clocklock=0;
			this->RanAway=0;
			this->flock(dt);
			break;
		}
	}

}

void DXGame::AIBot::takeStim(float dt)
{

}

Think DXGame::decide(AIBot target)
{
		
	/*dist = distToPlay(
		target.ship.sprite.position.x, 
		target.ship.sprite.position.y,
		ppplayer.sprite.position.x, 
		ppplayer.sprite.position.y);
	*/
	target.toAvoid=DecideBaye(target, asterObj[target.nearRock]);

	if (target.neardist< asterObj[target.nearRock].scaredist &&
		target.neardist>0.0f &&
		target.toAvoid==AVOID)
		thunk = Think::FLEE;

	else if (target.fardist*2.0f<=target.frienddist)
		thunk = Think::FLOCK;

	else if (target.ship.canshoot>0.3f &&
		target.takebreak>3.0f)
		thunk = Think::ATTACK;
	else if (target.dist>target.fardist)
			
		thunk = Think::PURSUE;

	else
	{
		//ppplayer.sprite.color =D3DCOLOR_ARGB(255,0, rand() % 255, rand() % 255); // Random color and alpha
		thunk = Think::CIRCLE;
		
	}

	return thunk;
}

float DXGame::distToPlay(float x1, float y1, float x2, float y2)
{
	float distance;
	
	distance = sqrt(((x2 -x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1))); 

	return distance;
}

bool DXGame::AIBot::clockWise()
{
		float hh=this->ship.sprite.hspeed;
		if (hh<0.0f)
			hh*=(-1.0f);
		float vv=this->ship.sprite.vspeed;
		if (vv<0.0f)
			vv*=(-1.0f);

	switch (this->quadToPlay(this->ship.sprite.position.x,this->ship.sprite.position.y,targx,targy))
	{
	case 1:
		{
			if (hh>=vv)
			{
				if (this->ship.sprite.hspeed < 0.0f)
					return 1;
				else
					return 0;
			}
			else
			{
				if (this->ship.sprite.vspeed < 0.0f)
					return 1;
				else
					return 0;
			}

			break;
		}
	case 2:
		{
			if (hh>=vv)
			{
				if (this->ship.sprite.hspeed > 0.0f)
					return 1;
				else
					return 0;
			}
			else
			{
				if (this->ship.sprite.vspeed < 0.0f)
					return 1;
				else
					return 0;
			}
			break;
		}
	case 3:
		{
			if (hh>=vv)
			{
				if (this->ship.sprite.hspeed > 0.0f)
					return 1;
				else
					return 0;
			}
			else
			{
				if (this->ship.sprite.vspeed > 0.0f)
					return 1;
				else
					return 0;
			}
			break;
		}
	case 4:
		{
			if (hh>=vv)
			{
				if (this->ship.sprite.hspeed < 0.0f)
					return 1;
				else
					return 0;
			}
			else
			{
				if (this->ship.sprite.vspeed > 0.0f)
					return 1;
				else
					return 0;
			}
			break;
		}
	}


	/*if (this->daQuad ==1  )
		
		if (( this->ship.sprite.vspeed < 0))
		return 1;
	else if (this->daQuad ==2  && (this->ship.sprite.hspeed > 0) )
		return 1;
	else if (this->daQuad ==3  &&  ( this->ship.sprite.vspeed > 0))
		return 1;
	else if (this->daQuad ==4  &&  (this->ship.sprite.hspeed < 0 ))
		return 1;
	else
		return 0;*/
}

int DXGame::AIBot::quadToPlay(float x1, float y1, float x2, float y2)
{	//if (this->canchoose >=0.3f)
//{
if ((x2-x1)>=0)		//Quad's 1 & 2
	{
		if ((y2-y1)<=0)	//Quad's 1 & 4
		{
			//this->Qlock=0;//Quad 1
			return 1;
		}
		else if ((y2-y1)>=0)				//Quad's 2 & 3
		{
			//this->Qlock=0;//Quad 2
			return 2;
		}
	}
	else					//Quad's 3 & 4
	{
		if ((y2-y1)>0)	//Quad's 2 & 3
		{
			//this->Qlock=0;//Quad 3
			return 3;
		}
		else				//Quad's 1 & 4
		{
			//this->Qlock=0;//Quad 4
			return 4;
		}
	//this->canchoose=0.0f;
}
}

