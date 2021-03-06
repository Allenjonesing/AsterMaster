#pragma once
#include <stdio.h>
#pragma comment(lib, "winmm.lib")


#include <iostream>
#include <conio.h>
using namespace std;
#include <d3d9.h>
#include <d3dx9.h>
#include <DxErr.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dxerr.lib")
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include "fmod.hpp"
#include "fmod_errors.h"
#pragma comment(lib, "Fmodex_vc.lib")
#include "dshow.h" 
#if defined(DEBUG) | defined(_DEBUG)
	#ifndef D3D_DEBUG_INFO
		#define D3D_DEBUG_INFO
	#endif

	#pragma comment(lib, "d3dx9d.lib")
#else
	#pragma comment(lib, "d3dx9.lib")
#endif

// Macro to release COM objects fast and safely
#define SAFE_RELEASE(x) if(x){x->Release(); x = 0;}

// HRESULT macro that prints an error window
#ifndef HR
#define HR(x)												\
		{                                                   \
		HRESULT hr = x;									    \
		if(FAILED(hr))									    \
			{                                               \
				MessageBox(m_hWnd, DXGetErrorDescription(hr), DXGetErrorString(hr), MB_OK);	/*DXTraceA(__FILE__, __LINE__, hr, DXGetErrorStringA(hr), TRUE);*/	    \
			}                                               \
		}                                                   
#endif

#define MAX_PLAYERS  1
#define MAX_ENEMY  2

#define MAX_ASTER 25

#define MAX_SHIPS  15
#define MAX_BULL  30


#define MAX_PART 20
#define MAX_TEXTURES  9

#define MAX_SOUNDS  13
#define MAX_CHAN  4

#define MAX_BUTT  2

#define ATTNUM sizeof( Braint ) / sizeof( Att )

#define COLDTEMP2 20
#define NORMTEMP2 80
#define SMALLSIZE2 10
#define MEDSIZE2 20
 
enum class GameStates { LOAD, 
	INTRO, MENU, GAME, QUIT};
enum class Think { DECIDE, 
	PURSUE, FLEE, AVOID, CIRCLE, 
	ATTACK, FLOCK };
enum Esize
{ SMALL,
	MEDSI,
	BIG };
//speed
enum Espeed
{ SLOW,
	MEDSP,
	FAST };
//temperature
enum Etemp
{ COLD,
	NORM,
	HOT };

enum Eavoid
{ AVOID,
	COLLIDE };

struct Att
{
	Esize	size;
	Espeed	speed;
	Etemp	temp;
	Eavoid	avoid;
	//self declaration
	//Att(void);

	struct Att( Esize si, Espeed sp, Etemp tp, Eavoid av )
	{
		size = si;
		speed = sp;
		temp = tp;
		avoid = av;
	}
};
	const Att 	 Braint[] =
{
	Att( SMALL,	SLOW,	COLD,	COLLIDE ),//Always collide with a frozen rock
	Att( SMALL,	SLOW,	NORM,	COLLIDE ),//Use size and speed to determine
	Att( SMALL,	SLOW,	HOT,	AVOID ),//always avoid a hot rock
	Att( SMALL,	MEDSP,	COLD,	COLLIDE ),
	Att( SMALL,	MEDSP,	NORM,	COLLIDE ),
	Att( SMALL,	MEDSP,	HOT,	AVOID ),
	Att( SMALL,	FAST,	COLD,	COLLIDE ),
	Att( SMALL,	FAST,	NORM,	COLLIDE ),
	Att( SMALL,	FAST,	HOT,	AVOID ),
	Att( MEDSI,	SLOW,	COLD,	COLLIDE ),
	Att( MEDSI,	SLOW,	NORM,	AVOID ),
	Att( MEDSI,	SLOW,	HOT,	AVOID ),
	Att( MEDSI,	MEDSP,	COLD,	COLLIDE ),
	Att( MEDSI,	MEDSP,	NORM,	COLLIDE ),
	Att( MEDSI,	MEDSP,	HOT,	AVOID ),
	Att( MEDSI,	FAST,	COLD,	COLLIDE ),
	Att( MEDSI,	FAST,	NORM,	COLLIDE ),
	Att( MEDSI,	FAST,	HOT,	AVOID ),
	Att( BIG,	SLOW,	COLD,	COLLIDE ),
	Att( BIG,	SLOW,	NORM,	AVOID ),
	Att( BIG,	SLOW,	HOT,	AVOID ),
	Att( BIG,	MEDSP,	COLD,	COLLIDE ),
	Att( BIG,	MEDSP,	NORM,	AVOID ),
	Att( BIG,	MEDSP,	HOT,	AVOID ),
	Att( BIG,	FAST,	COLD,	COLLIDE ),
	Att( BIG,	FAST,	NORM,	AVOID ),
	Att( BIG,	FAST,	HOT,	AVOID ),
};

class DXGame
{
	//////////////////////////////////////////////////////////////////////////
	// Application Variables
	//////////////////////////////////////////////////////////////////////////

	HWND				m_hWnd;			// Handle to the window
	bool				m_bVsync;		// Boolean for vertical syncing
	int					m_width;		// Window width
	int					m_height;		// Window height
	RECT				screenRect;		// RECT for the entire size of window

	float				m_time;		// Time to render current frame
	float				m_ptime;		// Time to render previous frame
	int					m_FPS;			// Frames per second
	float				m_dt;	// ms per frame

	int score[MAX_PLAYERS];
	//////////////////////////////////////////////////////////////////////////
	// Direct3D Variables
	//////////////////////////////////////////////////////////////////////////
	IDirect3D9*			  xObject;	// Direct3D 9 Object
	IDirect3DDevice9*	  xDevice;	// Direct3D 9 Device
	D3DPRESENT_PARAMETERS x_3D;		// Device Presentation Parameters
	D3DCAPS9			  xCaps;	// Device Capabilities
	D3DXCOLOR			  xBackCol; // Back-buffer Clear Color
	//////////////////////////////////////////////////////////////////////////
	// View and Projection Matrices
	//////////////////////////////////////////////////////////////////////////
	// Set these 3 component to initialize the view matrix (camera)
	D3DXVECTOR3 eyePos;
	D3DXVECTOR3 lookAt;
	D3DXVECTOR3 upVec;

	// View matrix
	D3DXMATRIX viewMat;

	// Projection matrix
	D3DXMATRIX projMat;

//////////////////////////////////////////////////////////////////////////
	// Lighting, Material, Textures
	//////////////////////////////////////////////////////////////////////////
	D3DLIGHT9			light;
	D3DMATERIAL9		material[3];
	IDirect3DTexture9*	otherTexture[3];

	//////////////////////////////////////////////////////////////////////////
	// Font Variables
	//////////////////////////////////////////////////////////////////////////
	ID3DXFont*			xFont;		// Font Object

	//////////////////////////////////////////////////////////////////////////
	// brain Variables
	//////////////////////////////////////////////////////////////////////////
	Esize size2;
	Espeed speed2;
	Etemp temp2;

	float size3, speed3, temp3;

	//IDirect3DTexture9*	textures[1];	// Second sprite image
	//D3DXIMAGE_INFO		infos[1];	// File details of 2nd texture
// File details of 2nd texture
struct TSphere
{
    D3DXVECTOR3 vecCenter;
    float radius;
};

	//Boundind Box Struct
	struct BRect
	{
		float x1, x2, y1, y2;
	}	;
	
	// A structure of sprite information to use for sprites, so we can initialize
	// values for each sprite in the init, and use them in the render() function
	struct SprObj
	{
		D3DXVECTOR3 position;	// Translation
		float rotation;			// Z rotation
		float scale;			// Uniform scaling (x and y value the same)
		D3DCOLOR color;		
		float  w, h;
		BRect BBox;
		TSphere SCol;
		float hspeed, vspeed, speed;
   };


	struct SpaceRock
	{
		SprObj sprite;\
		float scaredist;
		int size;
		int HP;
		int temp;
		void Crumble();
		void Update(float dt);
		float topAcc;

		bool off;

		int					m_width;		// Window width
		int					m_height;		// Window height
	};

	struct Bullet
	{
		SprObj sprite;
		int type;
		int dmg;
	bool balloff;
	};

	struct Ship
	{
	//	Ship(void) { Ship *ppplayer = new Ship;	}

		SprObj sprite;
		Bullet					bullObj[MAX_BULL]; 
		
		BRect XRect( SprObj sprite, float xx1, float xx2, float yy1, float yy2 )
{
    sprite.BBox.x1 = xx1;
    sprite.BBox.x2 = xx2;
    sprite.BBox.y1 = yy1;
	sprite.BBox.y2 = yy2;
	return sprite.BBox;
}

//queue <Bullet> bullets; /* Declare a queue */
		//Bullet restack;// = this->bullets.front();

		int type;
		int lbs;
		float accel;
		int HP;
		float topAcc;

		int					m_width;		// Window width
		int					m_height;		// Window height
		int					bullet_Count;

		float canshoot;
		float flashing;

		void Thrust(int direct, float dt);
		void LightThrust(int direct, float dt);
		void Update(float dt);
		void Fire(float dt, float targx, float targy) ;

	};

 /*	case 1:
			//UP
			//Left
	case 2:
			//RIGHT
			//uo
	case 3:
			//Down
			//right
	case 4:
			//LEFT
			//down
*/
	
	struct MenuButt
	{
		SprObj sprite;
		bool hover;
		GameStates state;

	};
	struct Brain
	{
	int SMALLSIZE;	
	int MEDSIZE;
	
	int SLOWSPEED;	
	int MEDSPEED;
	
	int COLDTEMP;
	int NORMTEMP;
		Think thought;

	};

	Think thunk;
	float dist;
	struct AIBot
	{
		Brain brain;
		Ship ship;
		Eavoid toAvoid;

		int diff;
		float targx;
		float targy;
		float targ2x;
		float targ2y;
		float friendx;
		float friendy;

		float takebreak;
		float canchoose;
		float canchase;
		bool clocklock;
		bool override;
		float RanAway;

		int	lastQ;
		int	daQuad;
		bool clockwise;
		int learncount;

		int nearRock;
		float dist;
		float frienddist;
		float dist2;
		float dist3;
		float neardist;
		bool Qlock;
		float closedist;
		float fardist;

		void act(float dt);
		void takeStim(float dt);
		void pursue(float dt);
		void flee(float dt);
		void avoid(float dt);
		void circle(float dt);
		void attack(float dt);
		void flock(float dt);
		int quadToPlay(float x1, float y1, float x2, float y2);
		bool clockWise();
		void update(float dt);
	
	};
	//Attribute containing 4 components


	struct Particle
	{
		SprObj sprite;
		
		
		void update(float dt);
		
		float life;
	};



	ID3DXSprite*			xSpr;	// Sprite Object
	Ship					ppplayer;

	IDirect3DTexture9*		textures[MAX_TEXTURES];		// Texture Object for a sprite
	D3DXIMAGE_INFO			infos[MAX_TEXTURES];	// File details of a texture
	Ship					shipObj[MAX_SHIPS];
	MenuButt				buttObj[MAX_BUTT];
	AIBot					enemObj[MAX_ENEMY];
	SpaceRock				asterObj[MAX_ASTER];
	Particle part[MAX_PART];
	int						partCount;

	float mx[MAX_BUTT];
	float my[MAX_BUTT];
	//SprObj bullObj[0][6] ;//= new SprObj;	// Array of ball sprite values to be used
	//SprObj paddleObj[2];
	//SprObj paddleObj[1];

	//////////////////////////////////////////////////////////////////////////
	// DirectInput
	//////////////////////////////////////////////////////////////////////////
	IDirectInput8*		 xDIobj;	// DirectInput Object
	IDirectInputDevice8* xKeys;	// Keyboard Device
	IDirectInputDevice8* xMouse;	// Mouse Device 
	char buffer[256];
		DIMOUSESTATE2 mouseState;

		POINT p;

	FMOD::System* system;
	FMOD_RESULT       result;
	unsigned int     version;
	int               numdrivers;
	FMOD_SPEAKERMODE speakermode;
	FMOD_CAPS         caps;
	char             name[256];
	
	FMOD::Sound* sound[MAX_SOUNDS];
	FMOD::Channel* channel[MAX_CHAN];
	
	bool x_Kdown[256];
	//////////////////////////////////////////////////////////////////////////
	// DirectShow
	//////////////////////////////////////////////////////////////////////////
	IGraphBuilder *pGraph; 
	IMediaControl *pMedCtrl;
	IMediaEvent* pEvent; 
	IVideoWindow *pWindow;
	long evCode;
	LONG_PTR eventParam1, eventParam2;
	bool VidPlay;
	//////////////////////////////////////////////////////////////////////////
	// Effects
	//////////////////////////////////////////////////////////////////////////
	ID3DXEffect*		xEffect;
	ID3DXBuffer*		xEffErr;
	D3DXHANDLE			xTech;
	UINT				numPasses;
			D3DXMATRIX WIT;
				D3DXMATRIX transMat, rotMat, scaleMat, worldMat;

	//////////////////////////////////////////////////////////////////////////
	// Meshes
	//////////////////////////////////////////////////////////////////////////
	LPD3DXBUFFER			Abuff;

	ID3DXMesh*				xBox; // Cube
	ID3DXMesh*				xMesh;

	ID3DXBuffer*			xAbuff;
	ID3DXBuffer*			xMats;
	ID3DXBuffer*			xotherEff;
	
	DWORD					xNumMat;

	D3DXMATERIAL*			d3dmats;
	D3DMATERIAL9*			meshMats;


	LPDIRECT3DTEXTURE9*		meshTextures;

		struct Vertex
	{
		D3DXVECTOR3 position;
		D3DXVECTOR3 normal;
		D3DXVECTOR2 uv;
	};

	IDirect3DVertexDeclaration9*	xVdecl;
	IDirect3DVertexBuffer9*			xVbuff;
	IDirect3DIndexBuffer9*			xIbuff;


public:
		GameStates gamestate;// = GameStates::LOAD;
		GameStates tempstate;// = GameStates::LOAD;
	DXGame(void);
	~DXGame(void);

	void Init(HWND& hWnd, HINSTANCE& hInst, bool bWindowed);

	void InitFoundations(HWND& hWnd, HINSTANCE& hInst, bool bWindowed);
	
	void InitVertex(HWND& hWnd, HINSTANCE& hInst);
	
	void InitCamera(HWND& hWnd, HINSTANCE& hInst);
	
	void InitFont(HWND& hWnd, HINSTANCE& hInst);
	
	void InitSprite(HWND& hWnd, HINSTANCE& hInst);
	void InitPlayer();
	void InitOpponent();
	void InitMenu();
	void InitAster();
	void InitBrain();
	
	void InitLight(HWND& hWnd, HINSTANCE& hInst);

	void InitMaterial(HWND& hWnd, HINSTANCE& hInst);
	
	void InitShader(HWND& hWnd, HINSTANCE& hInst);
	
	void InitMesh(HWND& hWnd, HINSTANCE& hInst);
	
	void InitDirectInput(HWND& hWnd, HINSTANCE& hInst);
	
	void InitFMOD(HWND& hWnd, HINSTANCE& hInst);
	
	void InitDShow(HWND& hWnd, HINSTANCE& hInst);

	float distToPlay(float x1, float y1, float x2, float y2);

	Think decide(AIBot target);


	bool PlayIntro(float dt);
	
	void Update(float dt);
	Eavoid DecideBaye(AIBot bot,SpaceRock target);

	GameStates UpdateGUI(float dt);
	
	void UpdateVideo(float dt);
	
	GameStates UpdateMenuCollisions(float dt);
	
	void UpdateCollisions(float dt);
	
	void setUpCol(int i, int g);

	void enemyhit(float dt,int i, int g);

	void playerHit(float dt,int i, int g);

	void Wreck(float dt,int i, int g);

	void asterCols(float dt,int i, int g, int k);
		
	void explode(D3DCOLOR color, float dt, int size, int x, int y);

	void UpdateInput(float dt);
	
	void UpdateAction(float dt);
	
	void Updatebullets( float dt);

	void StopVid();
	
	void UpdateFPS(float dt);
	
	void UpdateFMOD(float dt);

	void Render(float dt);
	
	void RenderSprite(float dt);


	void RenderFont(float dt);

	void RenderMenu(float dt);

	void RenderEffect(float dt);

	void RenderMesh(float dt);

	void RenderPrimMesh(float dt);

	void ClearBB();

	void Shutdown();

	bool isDeviceLost();

	void onLostDevice();

	void onResetDevice();

	int getResolutionWidth() {return m_width;}
	int getResolutionHeight() {return m_height;}

	void setResolution(int w, int h) 
	{
		m_width = w; 
		m_height = h;
		if(xDevice)
		{
			x_3D.BackBufferWidth = m_width;
			onLostDevice(); 
			HR(xDevice->Reset(&x_3D));
			onResetDevice();
		}
	}

	void ERRCHECK(FMOD_RESULT result)
	{
		if (result != FMOD_OK)
        {
			printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
			MessageBox(0, L"FMOD error!", 0, 0);
			//MessageBox(0, FMOD_ErrorString(result), 0, 0);
		}
	}

BRect CRect( SprObj sprite, float xx1, float xx2, float yy1, float yy2 )
{
    sprite.BBox.x1 = xx1;
    sprite.BBox.x2 = xx2;
    sprite.BBox.y1 = yy1;
	sprite.BBox.y2 = yy2;
	return sprite.BBox;
}

bool mcollision(BRect rect, float x,float y)
{
	 if (rect.x1 < x &&
		 rect.x2 > x &&
		 rect.y1 < y &&
		 rect.y2 > y) 
		return true;
		return false;
}
bool collision(BRect rect1, BRect rect2)
{
	 if (rect1.x1 < rect2.x2 &&
			rect1.x2 > rect2.x1&&
			 rect1.y1 < rect2.y2 &&
					rect1.y2 > rect2.y1) 
						return true;
	/* else if ( (rect2.x1 < rect1.x2))
		{	if(  (rect2.x2 > rect1.x1) )
				if( ( rect2.y1 < rect1.y2))
					if( (rect2.y2 > rect1.y1)) 
						return true;}*/
	else
		return false;
}

bool Scol(const TSphere& tSph, const TSphere& tSph2)
{
    //Calculate a vector from the point on the plane to the center of the sphere
   // D3DXVECTOR3 vecTemp(tSph.vecCenter - tSph2.vecCenter);

    //Calculate the distance: dot product of the new vector with the plane's normal
    float fDist = distToPlay(tSph.vecCenter.x, tSph.vecCenter.y, tSph2.vecCenter.x, tSph2.vecCenter.y);

    if(fDist > tSph.radius &&
		fDist > tSph2.radius)
    {
        //The sphere is not touching the plane
        return false;
    }

    //Else, the sphere is colliding with the plane
    return true;
}
	//rect.x1 < x && rect.x2 > x
//Avoidence probability function
float AvoidProb( Eavoid avoid )
{
	int AvCount = 0;

	for ( int i = 0; i < ATTNUM; i++ )
	{
		if ( Braint[i].avoid == avoid )
		{
			AvCount++;//increment our chances
		}
	}


	if ( ATTNUM == 0 )
	{
		return 0.f;
	}
	//return the formulated result
	return( ( float ) AvCount / ( float ) ATTNUM );
}

//determine probabilty based on speed
float SpeedProb( Espeed speed, Eavoid avoid )
{
	int SpCount = 0;
	int AvCount = 0;

	for ( int i = 0; i < ATTNUM; i++ )
	{
		if ( Braint[i].avoid == avoid )
		{
			AvCount++;//increment our avoidence

			if ( Braint[i].speed == speed )
			{
				SpCount++;//increment our chances
			}
		}
	}


	if ( AvCount == 0 )
	{
		return 0.f;
	}
	//return forula
	return( ( float ) SpCount / ( float ) AvCount );
}
//probabilty by size
float SizeProb( Esize size, Eavoid avoid )
{
	int SiCount = 0;
	int AvCount = 0;
	
	for ( int i = 0; i < ATTNUM; i++ )
	{
		if ( Braint[i].avoid == avoid )
		{
			AvCount++;//increment our chances

			if ( Braint[i].size == size )
			{
				SiCount++;//increment our chances
			}
		}
	}


	if ( AvCount == 0 )
	{
		return 0.f;
	}
	
	return( ( float ) SiCount / ( float ) AvCount );
}

//prob by temerature
float TempProb( Etemp temp, Eavoid avoid )
{
	int TpCount = 0;
	int AvCount = 0;
	
	for ( int i = 0; i < ATTNUM; i++ )
	{
		if ( Braint[i].avoid == avoid )
		{
			AvCount++;//increment our chances

			if ( Braint[i].temp == temp )
			{
				TpCount++;//increment our chances
			}
		}
	}


	if ( AvCount == 0 )
	{
		return 0.f;
	}
	
	return( ( float ) TpCount / ( float ) AvCount );
}

};//end header
