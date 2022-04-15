//-----------------------------------------------------------------------------
// File: CGameApp.cpp
//
// Desc: Game Application class, this is the central hub for all app processing
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CGameApp Specific Includes
//-----------------------------------------------------------------------------
#include<math.h>
#include "CGameApp.h"
#define TIMER_SEC 3
#define TIMER_SEC2 4

extern HINSTANCE g_hInst;

//-----------------------------------------------------------------------------
// CGameApp Member Functions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CGameApp () (Constructor)
// Desc : CGameApp Class Constructor
//-----------------------------------------------------------------------------
CGameApp::CGameApp()
{
	// Reset / Clear all required values
	m_hWnd			= NULL;
	m_hIcon			= NULL;
	m_hMenu			= NULL;
	m_pBBuffer		= NULL;
	m_pPlayer		= NULL;
	m_LastFrameRate = 0;
}

//-----------------------------------------------------------------------------
// Name : ~CGameApp () (Destructor)
// Desc : CGameApp Class Destructor
//-----------------------------------------------------------------------------
CGameApp::~CGameApp()
{
	// Shut the engine down
	ShutDown();
}

//-----------------------------------------------------------------------------
// Name : InitInstance ()
// Desc : Initialises the entire Engine here.
//-----------------------------------------------------------------------------
bool CGameApp::InitInstance( LPCTSTR lpCmdLine, int iCmdShow )
{
	// Create the primary display device
	if (!CreateDisplay()) { ShutDown(); return false; }

	// Build Objects
	if (!BuildObjects()) 
	{ 
		MessageBox( 0, _T("Failed to initialize properly. Reinstalling the application may solve this problem.\nIf the problem persists, please contact technical support."), _T("Fatal Error"), MB_OK | MB_ICONSTOP);
		ShutDown(); 
		return false; 
	}

	// Set up all required game states
	SetupGameState();

	// Success!
	return true;
}

//-----------------------------------------------------------------------------
// Name : CreateDisplay ()
// Desc : Create the display windows, devices etc, ready for rendering.
//-----------------------------------------------------------------------------
bool CGameApp::CreateDisplay()
{
	LPTSTR			WindowTitle		= _T("GameFramework");
	LPCSTR			WindowClass		= _T("GameFramework_Class");
	USHORT			Width			= 800;
	USHORT			Height			= 600;
	RECT			rc;
	WNDCLASSEX		wcex;


	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= CGameApp::StaticWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= g_hInst;
	wcex.hIcon			= LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= WindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON));

	if(RegisterClassEx(&wcex)==0)
		return false;

	// Retrieve the final client size of the window
	::GetClientRect( m_hWnd, &rc );
	m_nViewX		= rc.left;
	m_nViewY		= rc.top;
	m_nViewWidth	= rc.right - rc.left;
	m_nViewHeight	= rc.bottom - rc.top;

	m_hWnd = CreateWindow(WindowClass, WindowTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, Width, Height, NULL, NULL, g_hInst, this);

	if (!m_hWnd)
		return false;

	// Show the window
	ShowWindow(m_hWnd, SW_SHOW);

	// Success!!
	return true;
}

//-----------------------------------------------------------------------------
// Name : BeginGame ()
// Desc : Signals the beginning of the physical post-initialisation stage.
//		From here on, the game engine has control over processing.
//-----------------------------------------------------------------------------
int CGameApp::BeginGame()
{
	MSG		msg;

	// Start main loop
	while(true) 
	{
		// Did we recieve a message, or are we idling ?
		if ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) 
		{
			if (msg.message == WM_QUIT) break;
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		} 
		else 
		{
			// Advance Game Frame.
			FrameAdvance();

		} // End If messages waiting
	
	} // Until quit message is receieved

	return 0;
}

//-----------------------------------------------------------------------------
// Name : ShutDown ()
// Desc : Shuts down the game engine, and frees up all resources.
//-----------------------------------------------------------------------------
bool CGameApp::ShutDown()
{
	// Release any previously built objects
	ReleaseObjects ( );
	
	// Destroy menu, it may not be attached
	if ( m_hMenu ) DestroyMenu( m_hMenu );
	m_hMenu		 = NULL;

	// Destroy the render window
	SetMenu( m_hWnd, NULL );
	if ( m_hWnd ) DestroyWindow( m_hWnd );
	m_hWnd		  = NULL;
	
	// Shutdown Success
	return true;
}

//-----------------------------------------------------------------------------
// Name : StaticWndProc () (Static Callback)
// Desc : This is the main messge pump for ALL display devices, it captures
//		the appropriate messages, and routes them through to the application
//		class for which it was intended, therefore giving full class access.
// Note : It is VITALLY important that you should pass your 'this' pointer to
//		the lpParam parameter of the CreateWindow function if you wish to be
//		able to pass messages back to that app object.
//-----------------------------------------------------------------------------
LRESULT CALLBACK CGameApp::StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	// If this is a create message, trap the 'this' pointer passed in and store it within the window.
	if ( Message == WM_CREATE ) SetWindowLong( hWnd, GWL_USERDATA, (LONG)((CREATESTRUCT FAR *)lParam)->lpCreateParams);

	// Obtain the correct destination for this message
	CGameApp *Destination = (CGameApp*)GetWindowLong( hWnd, GWL_USERDATA );
	
	// If the hWnd has a related class, pass it through
	if (Destination) return Destination->DisplayWndProc( hWnd, Message, wParam, lParam );
	
	// No destination found, defer to system...
	return DefWindowProc( hWnd, Message, wParam, lParam );
}

//-----------------------------------------------------------------------------
// Name : DisplayWndProc ()
// Desc : The display devices internal WndProc function. All messages being
//		passed to this function are relative to the window it owns.
//-----------------------------------------------------------------------------
LRESULT CGameApp::DisplayWndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
	static UINT			fTimer;	

	// Determine message type
	switch (Message)
	{
		case WM_CREATE:
			SetTimer(hWnd, TIMER_SEC, 500, NULL);
		
			SetTimer(hWnd, TIMER_SEC2, 10000, NULL);
			break;
		
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		
		case WM_SIZE:
			if ( wParam == SIZE_MINIMIZED )
			{
				// App is inactive
				m_bActive = false;
			
			} // App has been minimized
			else
			{
				// App is active
				m_bActive = true;

				// Store new viewport sizes
				m_nViewWidth  = LOWORD( lParam );
				m_nViewHeight = HIWORD( lParam );
		
			
			} // End if !Minimized

			break;

		case WM_LBUTTONDOWN:
			// Capture the mouse
			SetCapture( m_hWnd );
			GetCursorPos( &m_OldCursorPos );
			break;
			

		case WM_LBUTTONUP:
			// Release the mouse
			ReleaseCapture( );
			break;

		case WM_KEYDOWN:
			switch(wParam)
			{
			case VK_ESCAPE:
				PostQuitMessage(0);
				break;
			case VK_RETURN:
				fTimer = SetTimer(m_hWnd, 1, 100, NULL);
				m_pPlayer->Explode();
				break;
		
			case 0x46:
				m_pRacheta->BulletExplosion();
				break;


			}
			break;
		case WM_KEYUP: {
			switch (wParam) {
			case VK_SPACE:
				m_pPlayer->BulletExplosion();
				break;
			}

			}
		case WM_CHAR:
		{
			switch (wParam) {
			case 'q':
				fTimer = SetTimer(m_hWnd, 2, 100, NULL);
				m_pRacheta->Explode();
				break;

			case 't':
				RotateIt++;
				m_pPlayer->RotateSprite(RotateIt%4);
				break;

			case 'r': 
				m_pPlayer->RotateSprite(RotateIt % 4);
			    RotateIt--;	
				if (RotateIt < 0) {
					RotateIt = 3;
				}
			case 'b':
				RotateIt++;
				m_pRacheta->RotateSprite(RotateIt2 % 4);
				break;
			case 'n':
				m_pRacheta->RotateSprite(RotateIt2 % 4);
				RotateIt2--;
				if (RotateIt2 < 0) {
					RotateIt2 = 3;
				}
				
				break;
			case 'h':
				SaveGame();
				break;
			case 'g':
				LoadGame();
				break;

			}
			
			
		

		}

		case WM_TIMER:
			switch(wParam)
			{
			case 1:
				if(!m_pPlayer->AdvanceExplosion())
			
					KillTimer(m_hWnd, 1);
				break;
			case 2:
				if (!m_pRacheta->AdvanceExplosion())

					KillTimer(m_hWnd, 2);
				break;
			case TIMER_SEC: 
				Crate->SpawnCrate();
				break;
			case TIMER_SEC2:
				Crate->spawncoins();
				break;

			}
			break;
		
		

		case WM_COMMAND:
			break;

		default:
			return DefWindowProc(hWnd, Message, wParam, lParam);

	} // End Message Switch
	
	return 0;
}

//-----------------------------------------------------------------------------
// Name : BuildObjects ()
// Desc : Build our demonstration meshes, and the objects that instance them
//-----------------------------------------------------------------------------
bool CGameApp::BuildObjects()
{
	m_pBBuffer = new BackBuffer(m_hWnd, m_nViewWidth, m_nViewHeight);
	m_pPlayer = new CPlayer(m_pBBuffer);
	m_pRacheta = new CPlayer2(m_pBBuffer);
	Crate = new Enemy(m_pBBuffer);
	if(!m_imgBackground.LoadBitmapFromFile("data/scrollingbg.bmp", GetDC(m_hWnd)))
		return false;

	// Success!
	return true;
}

//-----------------------------------------------------------------------------
// Name : SetupGameState ()
// Desc : Sets up all the initial states required by the game.
//-----------------------------------------------------------------------------
void CGameApp::SetupGameState()
{
	m_pPlayer->Position() = Vec2(100, 400);
	m_pRacheta->Position() = Vec2(400, 400);


}

//-----------------------------------------------------------------------------
// Name : ReleaseObjects ()
// Desc : Releases our objects and their associated memory so that we can
//		rebuild them, if required, during our applications life-time.
//-----------------------------------------------------------------------------
void CGameApp::ReleaseObjects( )
{
	if(m_pPlayer != NULL)
	{
		delete m_pPlayer;
		m_pPlayer = NULL;
	}
	if (m_pRacheta != NULL)
	{
		delete m_pRacheta;
		m_pRacheta = NULL;
	}
	if(m_pBBuffer != NULL)
	{
		delete m_pBBuffer;
		m_pBBuffer = NULL;
	}
	if (Crate != NULL)
	{
		delete Crate;
		Crate = NULL;
	}
}

//-----------------------------------------------------------------------------
// Name : FrameAdvance () (Private)
// Desc : Called to signal that we are now rendering the next frame.
//-----------------------------------------------------------------------------
void CGameApp::FrameAdvance()
{
	static TCHAR FrameRate[ 50 ];
	static TCHAR TitleBuffer[ 255 ];

	// Advance the timer
	m_Timer.Tick( );

	// Skip if app is inactive
	if ( !m_bActive ) return;
	
	// Get / Display the framerate
	if ( m_LastFrameRate != m_Timer.GetFrameRate() )
	{
		m_LastFrameRate = m_Timer.GetFrameRate( FrameRate, 50 );
		sprintf_s( TitleBuffer, _T("Game : %s  Score: %d Score2: %d  Lives: %d Lives2: %d")  , FrameRate,Score,Score2, Lives,Lives2 );
		SetWindowText( m_hWnd, TitleBuffer );

	} // End if Frame Rate Altered

	// Poll & Process input devices
	ProcessInput();

	// Animate the game objects
	AnimateObjects();

	// Drawing the game objects
	DrawObjects();

	//Collision();

	PlaneCrateCollision();
	BulletCrateCollision();
	FishCoinCollision();
	FishUpgrade();
	
	HeartCollision();



}

//-----------------------------------------------------------------------------
// Name : ProcessInput () (Private)
// Desc : Simply polls the input devices and performs basic input operations
//-----------------------------------------------------------------------------
void CGameApp::ProcessInput()
{
	static UCHAR pKeyBuffer[256];
	ULONG		Direction = 0;
	ULONG       Direction2 = 0;
	POINT		CursorPos;
	float		X = 0.0f, Y = 0.0f;

	// Retrieve keyboard state
	if (!GetKeyboardState(pKeyBuffer)) return;

	// Check the relevant keys
	if (pKeyBuffer[VK_UP] & 0xF0) Direction |= CPlayer::DIR_FORWARD;
	if (pKeyBuffer[VK_DOWN] & 0xF0) Direction |= CPlayer::DIR_BACKWARD;
	if (pKeyBuffer[VK_LEFT] & 0xF0) Direction |= CPlayer::DIR_LEFT;
	if (pKeyBuffer[VK_RIGHT] & 0xF0) Direction |= CPlayer::DIR_RIGHT;


	// Move the player
	m_pPlayer->Move(Direction);

	if (pKeyBuffer[0x57] & 0xF0) Direction2 |= CPlayer2::DIR_FORWARD;
	if (pKeyBuffer[0x53] & 0xF0) Direction2 |= CPlayer2::DIR_BACKWARD;
	if (pKeyBuffer[0x41] & 0xF0) Direction2 |= CPlayer2::DIR_LEFT;
	if (pKeyBuffer[0x44] & 0xF0) Direction2 |= CPlayer2::DIR_RIGHT;

	m_pRacheta->Move(Direction2);
	 

	// Now process the mouse (if the button is pressed)
	if ( GetCapture() == m_hWnd )
	{
		// Hide the mouse pointer
		SetCursor( NULL );

		// Retrieve the cursor position
		GetCursorPos( &CursorPos );

		// Reset our cursor position so we can keep going forever :)
		SetCursorPos( m_OldCursorPos.x, m_OldCursorPos.y );

	} // End if Captured
}

//-----------------------------------------------------------------------------
// Name : AnimateObjects () (Private)
// Desc : Animates the objects we currently have loaded.
//-----------------------------------------------------------------------------
void CGameApp::AnimateObjects()
{
	m_pPlayer->Update(m_Timer.GetTimeElapsed());
	m_pPlayer->UpdateHeart(m_Timer.GetTimeElapsed());
	m_pRacheta->Update(m_Timer.GetTimeElapsed());
	Crate->Update(m_Timer.GetTimeElapsed());
	Crate->updatecoins(m_Timer.GetTimeElapsed());
	
}

//-----------------------------------------------------------------------------
// Name : DrawObjects () (Private)
// Desc : Draws the game objects
//-----------------------------------------------------------------------------
void CGameApp::DrawObjects()
{
	m_pBBuffer->reset();

	Scrolling();

	m_pPlayer->Draw();
	m_pPlayer->DrawHeart();
	m_pRacheta->Draw();
	Crate->Draw();
	Crate->drawcoins();
	
	m_pBBuffer->present();
}
void CGameApp::SaveGame() {

	SaveData data;
	
	data.posPlayer1 = m_pPlayer->Position();
    data.posPlayer2 = m_pRacheta->Position();
	std::ofstream DataInput("data/Text.txt");
	DataInput.write((char*)&data, sizeof(data));


}
void CGameApp::LoadGame() {
	SaveData data;
	std::ifstream DataInput("data/Text.txt");
	DataInput.read((char*)&data, sizeof(data));
	m_pPlayer->Position() = data.posPlayer1;
	m_pRacheta->Position() = data.posPlayer2;
}
void CGameApp::Collision() {

	static UINT fTimer;
	double Distance = m_pPlayer->Position().Distance(m_pRacheta->Position());
	if (Distance <= (m_pPlayer->getPlayerWidth() + m_pRacheta->getPlayerWidth())/2 && !m_pPlayer->CheckExplosion()&& !m_pRacheta->CheckExplosion()){
		fTimer = SetTimer(m_hWnd, 1, 100, NULL);
		m_pPlayer->Velocity() = Vec2(0, 0);
		m_pPlayer->Explode();
		m_pPlayer->Position() = Vec2(100, 400);
		fTimer = SetTimer(m_hWnd, 2, 100, NULL);
		m_pRacheta->Velocity() = Vec2(0, 0);
		m_pRacheta->Explode();
		m_pRacheta->Position() = Vec2(400, 400);
	}




}
void CGameApp::BulletCrateCollision()
{
	std::vector <Sprite*> crates = Crate->getVectorCrate();
	std::vector <Sprite*> bulletsC = m_pPlayer->getVectorCplayer();
	std::vector <Sprite*> bulletsC2 = m_pRacheta->getVectorCplayer2();
	int p1 = 0;
	int p2 = 0;
	int p3 = 0;
	int p4 = 0;

	for (Sprite* bullet : bulletsC)
	{
		p1 = 0;
		for (Sprite* cratev : crates)
		{
			double distance = cratev->mPosition.Distance(bullet->mPosition);
			
			if (distance <= (cratev->width() + bullet->width()) / 2)
			{
				crates.erase(crates.begin() + p1);
				Crate->updatecrate(crates);
				bulletsC.erase(bulletsC.begin() + p2);
				m_pPlayer->updatebullets(bulletsC);
				Score=Score+100;

			}


			
			p1++;
		}
		p2++;
	}
	for (Sprite* bullet : bulletsC2)
	{
		p3 = 0;
		for (Sprite* cratev : crates)
		{
			double distance = cratev->mPosition.Distance(bullet->mPosition);

			if (distance <= (cratev->width() + bullet->width()) / 2)
			{
				crates.erase(crates.begin() + p3);
				Crate->updatecrate(crates);
				bulletsC2.erase(bulletsC2.begin() + p4);
				m_pRacheta->updatebullets2(bulletsC2);
				Score2 = Score2 + 100;

			}



			p3++;
		}
		p4++;
	}
}

void CGameApp::PlaneCrateCollision()
{
	std::vector <Sprite*> crates = Crate->getVectorCrate();
	int p1 = 0, p2=0;
	for (Sprite* cratev : crates)
	{
		double distance = cratev->mPosition.Distance(m_pPlayer->Position());
		if (distance <= (cratev->width() + m_pPlayer->getPlayerWidth()) / 2 && !m_pPlayer->CheckExplosion())
		{
			SetTimer(m_hWnd, 1, 50, NULL);
			m_pPlayer->Velocity() = Vec2(0, 0);
			m_pPlayer->Explode();
			m_pPlayer->Position() = Vec2(100, 400);
			
			crates.erase(crates.begin() + p1);
			Crate->updatecrate(crates);
			Lives--;
			LifeSpawn();
			if (Lives <= 0) {
				PostQuitMessage(0);
			}
			
		}
		p1++;

	}
	for (Sprite* cratev : crates)
	{
		double distance = cratev->mPosition.Distance(m_pRacheta->Position());
		if (distance <= (cratev->width() + m_pRacheta->getPlayerWidth()) / 2 && !m_pRacheta->CheckExplosion())
		{
			SetTimer(m_hWnd, 2, 50, NULL);
			m_pRacheta->Velocity() = Vec2(0, 0);
			m_pRacheta->Explode();
			m_pRacheta->Position() = Vec2(400, 400);

			crates.erase(crates.begin() + p2);
			Crate->updatecrate(crates);
			Lives2--;
			LifeSpawn();
			if (Lives2 <= 0) {
				PostQuitMessage(0);
			}

		}
		p2++;

	}
}
void CGameApp::FishCoinCollision()
{
	std::vector <Sprite*> crates = Crate->getVectorCoin();
	int p1 = 0, p2 = 0;
	for (Sprite* cratev : crates)
	{
		double distance = cratev->mPosition.Distance(m_pPlayer->Position());
		if (distance <= (cratev->width() + m_pPlayer->getPlayerWidth()) / 2 && !m_pPlayer->CheckExplosion())
		{
			

			crates.erase(crates.begin() + p1);
			Crate->updatevectorcoin(crates);
			
			Score = Score + 500;
		}
		p1++;

	}
	for (Sprite* cratev : crates)
	{
		double distance = cratev->mPosition.Distance(m_pRacheta->Position());
		if (distance <= (cratev->width() + m_pRacheta->getPlayerWidth()) / 2 && !m_pRacheta->CheckExplosion())
		{


			crates.erase(crates.begin() + p2);
			Crate->updatevectorcoin(crates);

			Score2 = Score2 + 500;
		}
		p2++;

	}
}

void CGameApp::FishUpgrade() {
	if (Score >= 1000) {
		m_pPlayer->lvl1();
		
	}
	if (Score >= 5000) {
		m_pPlayer->lvl2();
		
	}
	if (Score2 >= 1000) {
		
		m_pRacheta->lvl21();
	}
	if (Score2 >= 5000) {
		
		m_pRacheta->lvl22();
	}
}

void CGameApp::LifeSpawn()
{



	if (Lives <= 2)
	{
		m_pPlayer->SpawnHeart();
		
	}
	

}
void CGameApp::HeartCollision()
{
	int i = 0;
	std::vector<Sprite*> heartsSpawn = m_pPlayer->GetVectorHeart();
	for (Sprite* heart : heartsSpawn)
	{
		double distance = heart->mPosition.Distance(m_pPlayer->Position());
		if (distance <= (heart->width() + m_pPlayer->getPlayerWidth()) / 2)
		{
			Lives++;
			heartsSpawn.erase(heartsSpawn.begin() + i);
			m_pPlayer->UpdateVectorHeart(heartsSpawn);
		}
		i++;
	}
}
void CGameApp::Scrolling() {
	m_imgBackground.Paint(m_pBBuffer->getDC(), 0, y);
	y++;
	if (y == 0)
		y = -600;
}