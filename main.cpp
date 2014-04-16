/*
=================
main.cpp
Main entry point for the Card application
=================
*/

#include "GameConstants.h"
#include "GameResources.h"
#include "cD3DManager.h"
#include "cD3DXSpriteMgr.h"
#include "cD3DXTexture.h"
#include "cBalloon.h"
#include "cSprite.h"
#include "cExplosion.h"
#include "cXAudio.h"
#include "cD3DXFont.h"


using namespace std;

HINSTANCE hInst; // global handle to hold the application instance
HWND wndHandle; // global variable to hold the window handle

// Get a reference to the DirectX Manager
static cD3DManager* d3dMgr = cD3DManager::getInstance();

// Get a reference to the DirectX Sprite renderer Manager 
static cD3DXSpriteMgr* d3dxSRMgr = cD3DXSpriteMgr::getInstance();

RECT clientBounds;																										//VARIABLES
D3DXVECTOR2 playerScale = D3DXVECTOR2 (0.2f,0.2f);
D3DXVECTOR2 enemyScale = D3DXVECTOR2 (0.2f,0.2f);

//FUNKY BUISNESS
bool leftArrow, rightArrow, upArrow, downArrow;
D3DXVECTOR2 enemyTrans = D3DXVECTOR2 (0.0f, 0.0f);
D3DXVECTOR2 playerTrans = D3DXVECTOR2 (0.0f,0.0f);

vector<cBalloon*> enemies;
vector<cBalloon*>::iterator iter;
vector<cBalloon*>::iterator iterA;
vector<cBalloon*>::iterator iterB;
vector<cBalloon*>::iterator iterC;
vector<cBalloon*>::iterator iterD;

vector<cSprite*> lasers;
vector<cSprite*>::iterator Biter;
vector<cSprite*>::iterator Liter;
float enemyY = 300;
short bulletsOnScreen = 0;
short bulletDelay = 0;
int ammo = 20;

cD3DXTexture* myTxtrs[4];
char* myTxts[] = {"Images\\alien2.png", "Images\\alien3.png", "Images\\alien4.png", "Images\\beams.png"};

//Scene management
short currentScene = 1;

int score = 0;
char storage[50];
char storage2[50];
char storage3[50];

cXAudio shot;
/*
==================================================================
* LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam,
* LPARAM lParam)
* The window procedure
==================================================================
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Check any available messages from the queue
	switch (message)
	{

		//Controles here	
		
		// Continious input handle suggested by forum people at	gamedev.net (Not in my own thread)
		case WM_KEYDOWN:
		{
			if(wParam == VK_SPACE)
			{
				//Shoot
				if(currentScene == 0)
				{
					if((bulletsOnScreen < 5 && bulletDelay == 0) && ammo > 0)
					{
						ammo--;
						bulletDelay++;
						cSprite* theLaser = new cSprite();
						theLaser->setTexture(myTxtrs[3]);
						theLaser->setSpritePos(D3DXVECTOR3(playerTrans.x, playerTrans.y, 0.0f));
						theLaser->setTranslation(D3DXVECTOR2 (2000.0f, 0));
						theLaser->setActive(true);

						shot.playSound(L"Sounds\\explosion.wav",false);
						lasers.push_back(theLaser);
						bulletsOnScreen++;
					}	
				}
				//Replay/quit
				if(currentScene == 3)
				{
					ammo = 20;
					score = 0;
					currentScene = 0;
				}
				if(currentScene == 1)
				{
					ammo = 20;
					score = 0;
					currentScene = 0;
				}
				//ALWAYS!
				return 0;
			}
			if(wParam == VK_LEFT)
			{
				leftArrow = true;
				return 0;
			}
			if(wParam == VK_RIGHT)
			{
				rightArrow = true;
				return 0;
			}
			if(wParam == VK_UP)
			{
				upArrow = true;
				return 0;
			}
			if(wParam == VK_DOWN)
			{
				downArrow = true;
				return 0;
			}
			if(wParam == VK_ESCAPE)
			{
				if(currentScene == 3)
				{
					PostQuitMessage(0);
					return 0;
				}
				if(currentScene == 1)
				{
					PostQuitMessage(0);
					return 0;
				}
			}
			return 0;
		}

		case WM_KEYUP:
		{
			if(wParam == VK_LEFT)
			{
				leftArrow = false;
				return 0;
			}
			if(wParam == VK_RIGHT)
			{
				rightArrow = false;
				return 0;
			}
			if(wParam == VK_DOWN)
			{
				downArrow = false;
				return 0;
			}
			if(wParam == VK_UP)
			{
				upArrow = false;
				return 0;
			}
			return 0;
		}
	
		case WM_CLOSE:	
		{
			// Exit the Game
			PostQuitMessage(0);
			return 0;
		}

		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
	}
	// Always return the message to the default window
	// procedure for further processing
	return DefWindowProc(hWnd, message, wParam, lParam);
}

/*
==================================================================
* bool initWindow( HINSTANCE hInstance )
* initWindow registers the window class for the application, creates the window
==================================================================
*/
bool initWindow( HINSTANCE hInstance )
{
	WNDCLASSEX wcex;
	// Fill in the WNDCLASSEX structure. This describes how the window
	// will look to the system
	wcex.cbSize = sizeof(WNDCLASSEX); // the size of the structure
	wcex.style = CS_HREDRAW | CS_VREDRAW; // the class style
	wcex.lpfnWndProc = (WNDPROC)WndProc; // the window procedure callback
	wcex.cbClsExtra = 0; // extra bytes to allocate for this class
	wcex.cbWndExtra = 0; // extra bytes to allocate for this instance
	wcex.hInstance = hInstance; // handle to the application instance
	wcex.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_MyWindowIcon)); // icon to associate with the application
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW); //Default Windows arrow
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1); // the background color
	wcex.lpszMenuName = NULL; // the resource name for the menu
	wcex.lpszClassName = "Balloons"; // the class name being created
	wcex.hIconSm = LoadIcon(hInstance,"Balloon.ico"); // the handle to the small icon

	RegisterClassEx(&wcex);
	// Create the window
	wndHandle = CreateWindow("Balloons",			// the window class to use
							 "Da game's nem",			// the title bar text
							WS_OVERLAPPEDWINDOW,	// the window style
							CW_USEDEFAULT, // the starting x coordinate
							CW_USEDEFAULT, // the starting y coordinate
							800, // the pixel width of the window
							600, // the pixel height of the window
							NULL, // the parent window; NULL for desktop
							NULL, // the menu for the application; NULL for none
							hInstance, // the handle to the application instance
							NULL); // no values passed to the window
	// Make sure that the window handle that is created is valid
	if (!wndHandle)
		return false;
	// Display the window on the screen
	ShowWindow(wndHandle, SW_SHOW);
	UpdateWindow(wndHandle);
	return true;
}

/*
==================================================================
// This is winmain, the main entry point for Windows applications
==================================================================
*/
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	// Initialize the window
	if ( !initWindow( hInstance ) )
		return false;
	// called after creating the window
	if ( !d3dMgr->initD3DManager(wndHandle) )
		return false;
	if ( !d3dxSRMgr->initD3DXSpriteMgr(d3dMgr->getTheD3DDevice()))
		return false;
	
	// Grab the frequency of the high def timer
	__int64 freq = 0;				// measured in counts per second;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	float sPC = 1.0f / (float)freq;			// number of seconds per count

	__int64 currentTime = 0;				// current time measured in counts per second;
	__int64 previousTime = 0;				// previous time measured in counts per second;

	float numFrames   = 0.0f;				// Used to hold the number of frames
	float elapsedTime = 0.0f;				// cumulative elapsed time

	GetClientRect(wndHandle,&clientBounds);

	float fpsRate = 1.0f/25.0f;


	/* initialize random seed: */	
	D3DXMATRIX enemyTransform;
	D3DXVECTOR3 enemyPos;

	//Loading up some textures
	for(int txtr = 0; txtr < 4; txtr++)
	{
		myTxtrs[txtr] = new cD3DXTexture(d3dMgr->getTheD3DDevice(), myTxts[txtr]);
	}
						
	
	int curEnemies = 0; // Number of enemies currently on the screen
	float delay = 0;   //Used for delay

	//Stuff stuff stuff																								// STUFF HERE!

	LPDIRECT3DSURFACE9 menuSurface;				// the Direct3D surface
	LPDIRECT3DSURFACE9 gameSurface;
	LPDIRECT3DSURFACE9 tutorialSurface;
	LPDIRECT3DSURFACE9 scoreSurface;
		
	LPDIRECT3DSURFACE9 theBackbuffer = NULL;  // This will hold the back buffer
	


	//Set starting position, instantiate the player
	cD3DXTexture* myTxtr = new cD3DXTexture ( d3dMgr->getTheD3DDevice(), "Images\\player.png");
	cSprite thePlayer;
	thePlayer.setTranslation(playerTrans);
	thePlayer.setTexture(myTxtr);
	
		

	//Transformation Matrix
	D3DXMATRIX playerTransform;
	D3DXMATRIX replayButtonMtrx;

	MSG msg;
	ZeroMemory( &msg, sizeof( msg ) );

	// Create the background surface
	gameSurface = d3dMgr->getD3DSurfaceFromFile("Images\\BG.jpg");
	menuSurface = d3dMgr->getD3DSurfaceFromFile("Images\\BGSTART.bmp");
	tutorialSurface = d3dMgr->getD3DSurfaceFromFile("Images\\snowyday.png");
	scoreSurface = d3dMgr->getD3DSurfaceFromFile("Images\\endScene.bmp");

	// load custom font
	cD3DXFont* balloonFont = new cD3DXFont(d3dMgr->getTheD3DDevice(),hInstance, "Coalition");
	srand((unsigned int)time(NULL));

	cXAudio background;
	background.playSound(L"Sounds\\background.wav", true);
	
	RECT textPos;
	SetRect(&textPos, 20, 10, 450, 100);
	RECT textPos2;
	SetRect(&textPos2, 20, 550, 450, 600);
	int i = 0;
	short onScreenEnemies = 0;

	//Menu scene variables
	cD3DXTexture* btnTxtrs[4];
	char* btnTxts[] = {"Images\\start.png", "Images\\controls.png", "Images\\menu.png"};

	QueryPerformanceCounter((LARGE_INTEGER*)&previousTime);
	while( msg.message!=WM_QUIT )
	{
		// Check the message queue
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{	
			QueryPerformanceCounter((LARGE_INTEGER*)&currentTime); 
			float dt = (currentTime - previousTime)*sPC; 
  
			// Accumulate how much time has passed. 
			elapsedTime += dt; 

			//Beginning of ending scene
			if(currentScene == 3)
			{
				d3dMgr->beginRender();
				theBackbuffer = d3dMgr->getTheBackBuffer();
				d3dMgr->updateTheSurface(scoreSurface, theBackbuffer);
				d3dMgr->releaseTheBackbuffer(theBackbuffer);
				d3dxSRMgr->beginDraw();
				//Draw stuff here!

				

				d3dxSRMgr->endDraw();
				d3dMgr->endRender();
				elapsedTime = 0.0f;
			}//end of ending scene


			//Beginning of tutorial scene -- TIMED OUT
			if(currentScene == 2)
			{
				d3dMgr->beginRender();
				theBackbuffer = d3dMgr->getTheBackBuffer();
				d3dMgr->updateTheSurface(tutorialSurface, theBackbuffer);
				d3dMgr->releaseTheBackbuffer(theBackbuffer);
				d3dxSRMgr->beginDraw();
				//Draw stuff here!
				

				d3dxSRMgr->endDraw();
				d3dMgr->endRender();
				elapsedTime = 0.0f;
			}//end of tutorial scene -- TIMED OUT

			//Beginning of menu scene
			if(currentScene == 1)
			{
				d3dMgr->beginRender();
				theBackbuffer = d3dMgr->getTheBackBuffer();
				d3dMgr->updateTheSurface(menuSurface, theBackbuffer);
				d3dMgr->releaseTheBackbuffer(theBackbuffer);
				d3dxSRMgr->beginDraw();
				//Draw stuff here!
					

				d3dxSRMgr->endDraw();
				d3dMgr->endRender();
				elapsedTime = 0.0f;
			}//end of menu scene

			//Beginning of game scene
			if(currentScene == 0)
			{
				if(ammo == 0)
				{
					currentScene = 3;
				}

				int randomizer = 0;
				randomizer += rand() % 3;

				//Difficulty increments
				if(score < 20)
				{
					//Spawn logic here, delay below
					if(curEnemies == 0 && onScreenEnemies < 8)
					{
						//Used for spawn delay ... scroll down for more on this one
						curEnemies++;
						onScreenEnemies++;
						//Randomise spawn position a bit ... not all that much though

					
						if(enemyPos.y < 500)
						{
							if(randomizer == 1)
								enemyY += 150;
							else if(randomizer == 0)
								enemyY += 220;
							else if(randomizer == 2 && enemyPos.y >= 100)
								enemyY -= 150;
							else if(randomizer == 2 && enemyPos.y < 100)
								enemyY += enemyY;	
							else if(randomizer == 3)
								enemyY -= 100;
						}
						//Ofcourse it makes no sence to keep changing position in crazy manners once enemies are no longer on screen
						else
							enemyY = 50;

						enemyPos = D3DXVECTOR3 (765.0f, enemyY, 0.0f);
						//Just asigning some variables for each enemy 
						enemies.push_back(new cBalloon());	
						enemies[i]->setSpritePos(enemyPos);
						enemies[i]->setTranslation(enemyTrans);
						enemies[i]->setTexture(myTxtrs[rand()%3]);
						//Sloppy for loop logic applied here
						i++;
					}
				}
				//Increment difficulty
				else if(score >= 20 && score < 40)
				{
					//Spawn logic here, delay below
					if(curEnemies == 0 && onScreenEnemies < 16)
					{

					//Used for spawn delay ... scroll down for more on this one
					curEnemies++;
					onScreenEnemies++;

						//Randomise spawn position a bit ... not all that much though
						if(enemyPos.y < 500)
						{
							if(randomizer == 1)
								enemyY += 150;
							else if(randomizer == 0)
								enemyY += 220;
							else if(randomizer == 2 && enemyPos.y >= 100)
								enemyY -= 150;
							else if(randomizer == 2 && enemyPos.y < 100)
								enemyY += enemyY;	
							else if(randomizer == 3)
								enemyY -= 100;
						}
						//Ofcourse it makes no sence to keep changing position in crazy manners once enemies are no longer on screen
						else
							enemyY = 50;

						enemyPos = D3DXVECTOR3 (765.0f, enemyY, 0.0f);
						//Just asigning some variables for each enemy 
						enemies.push_back(new cBalloon());	
						enemies[i]->setSpritePos(enemyPos);
						enemies[i]->setTranslation(enemyTrans);
						enemies[i]->setTexture(myTxtrs[rand()%3]);
						//Sloppy for loop logic applied here
						i++;
					}
				}

				else if(score >= 40 )
				{
					//Spawn logic here, delay below
					if(curEnemies == 0 && onScreenEnemies < 32)
					{

					//Used for spawn delay ... scroll down for more on this one
					curEnemies++;
					onScreenEnemies++;

						//Randomise spawn position a bit ... not all that much though
						if(enemyPos.y < 500)
						{
							if(randomizer == 1)
								enemyY += 150;
							else if(randomizer == 0)
								enemyY += 220;
							else if(randomizer == 2 && enemyPos.y >= 100)
								enemyY -= 150;
							else if(randomizer == 2 && enemyPos.y < 100)
								enemyY += enemyY;	
							else if(randomizer == 3)
								enemyY -= 100;
						}
						//Ofcourse it makes no sence to keep changing position in crazy manners once enemies are no longer on screen
						else
							enemyY = 50;

						enemyPos = D3DXVECTOR3 (765.0f, enemyY, 0.0f);
						//Just asigning some variables for each enemy 
						enemies.push_back(new cBalloon());	
						enemies[i]->setSpritePos(enemyPos);
						enemies[i]->setTranslation(enemyTrans);
						enemies[i]->setTexture(myTxtrs[rand()%3]);
						//Sloppy for loop logic applied here
						i++;
					}
				}

				//Transformation matrix
				D3DXMatrixTransformation2D(&playerTransform, NULL, 0.0f, &playerScale, &(thePlayer.getSpriteCentre()), NULL,  &playerTrans);

				//Continious messages handle happens here after the bools get passed from the message loop
				if(leftArrow)
					playerTrans.x -= 0.6f;
				if(rightArrow)
					playerTrans.x += 0.6f;
				if(downArrow)
					playerTrans.y += 0.6f;
				if(upArrow)	
					playerTrans.y -= 0.6f;
			
				// Keep ship in window, used specifics because I don't know how to check the window's size without storing it in some variable... unneccessery with set resolution
				if (playerTrans.x < 0) //0
					playerTrans.x = 0;
				if (playerTrans.x >= 730) //730
					playerTrans.x = 730;
				if (playerTrans.y < 50)
					playerTrans.y = 50;
				if (playerTrans.y >= 525) //565
					playerTrans.y = 525;

				//Enemy movement
				for(iterA = enemies.begin();iterA != enemies.end(); ++iterA)
				{
					if(score < 30)
					{
						(*iterA)->setSpritePos((*iterA)->getSpritePos() + D3DXVECTOR3(-0.2f,0,0));
					}
					if(score >= 30 && score < 60)
					{
						(*iterA)->setSpritePos((*iterA)->getSpritePos() + D3DXVECTOR3(-0.4f,0,0));
					}
					if(score >= 60)
					{
						(*iterA)->setSpritePos((*iterA)->getSpritePos() + D3DXVECTOR3(-0.6f,0,0));
					}

					//Keep enemies on screen
					if((*iterA)->getSpritePos().y > 500)
						(*iterA)->setSpritePos((*iterA)->getSpritePos() + D3DXVECTOR3(0,50.0f,0));
					if((*iterA)->getSpritePos().y < 50)
						(*iterA)->setSpritePos((*iterA)->getSpritePos() + D3DXVECTOR3(0,-50.0f,0))	;
				}
			

				if(score < 20)
				{
					//Spawn logic rev 2.0 (delay)
					for(iterC = enemies.begin(); iterC != enemies.end(); ++iterC)
					{
						if((*iterC)->getSpritePos().x == 600.0f)
						{
							curEnemies--;
						}
					}
				}

				if(score >= 20 && score < 40)
				{
					//Spawn logic rev 2.0 (delay)
					for(iterC = enemies.begin(); iterC != enemies.end(); ++iterC)
					{
						if((*iterC)->getSpritePos().x == 700.0f)
						{
							curEnemies--;
						}
					}
				}

				if(score >= 40)
				{
					//Spawn logic rev 2.0 (delay)
					for(iterC = enemies.begin(); iterC != enemies.end(); ++iterC)
					{
						if((*iterC)->getSpritePos().x == 799.5f)
						{
							curEnemies--;
						}
					}
				}
				Biter = lasers.begin();
				while(Biter != lasers.end())
				{
					if((*Biter)->getSpritePos2D().x > 800.0f)
					{
						Biter = lasers.erase(Biter);
						bulletsOnScreen--;
						}
					else
					{
						++Biter;
					}
				}

				//Delay logic
				for(Biter = lasers.begin();Biter != lasers.end(); ++Biter)
				{
					if((*Biter)->getSpritePos().x >= 250.0f)
					{
						bulletDelay = 0;
					}
				}
			
				//Collision detection
				Liter = lasers.begin();
				iterD = enemies.begin();
				while(Liter != lasers.end())
				{
					while(iterD != enemies.end())
					{
						if((*Liter)->collidedWith((*Liter)->getBoundingRect(), (*iterD)->getBoundingRect()))
						{
							//Mess
							curEnemies--;
							onScreenEnemies--;
							bulletsOnScreen--;
							i--;
							bulletDelay = 0;
							ammo++;
							score++;

							//Delete enemies and bullets that colide
							Liter = lasers.erase(Liter);
							iterD = enemies.erase(iterD);
						}
						else
						{
							++iterD;
						}
					}
					if(Liter != lasers.end())
					{
						++Liter;
					}
				}

				//Kill enemies once they reach a certain point in 2D space
				iterB = enemies.begin();
				while(iterB != enemies.end())
				{			
					if((*iterB)->getSpritePos().x < -20.0f)
					{
						onScreenEnemies--;
						i--;
						if(curEnemies >= 1)
						{
							curEnemies = 0;	
						}
						iterB = enemies.erase(iterB);
					}
					else
					{
						++iterB;
					}
				}

				//Texts
				sprintf_s(storage, 50, "Kills : %d", score);
				sprintf_s(storage2, 50, "Ammo Left : %d", ammo);

				//Begins drawing and sets front buffer
				d3dMgr->beginRender();
				theBackbuffer = d3dMgr->getTheBackBuffer();
				d3dMgr->updateTheSurface(gameSurface, theBackbuffer);
				d3dMgr->releaseTheBackbuffer(theBackbuffer);

				d3dxSRMgr->beginDraw();

				//Draw stuff here!
				d3dxSRMgr->setTheTransform(playerTransform);
				d3dxSRMgr->drawSprite(thePlayer.getTexture(), NULL, NULL, NULL, 0xFFFFFFFF);
				
				//Draw enemies
				for(iter = enemies.begin(); iter != enemies.end(); ++iter)
				{
					(*iter)->update(elapsedTime);
					d3dxSRMgr->setTheTransform((*iter)->getSpriteTransformMatrix());
					d3dxSRMgr->drawSprite((*iter)->getTexture(), NULL, NULL, NULL, 0xFFFFFFFF);
				}

				//Bullets logit - update and draw
				for(Biter = lasers.begin();Biter != lasers.end();++Biter)
				{
					D3DXVECTOR2 tempV2;
					D3DXVECTOR3 tempV3;
					tempV2 = (*Biter)->getSpritePos2D() + ((*Biter)->getTranslation())*elapsedTime;
					tempV3 = D3DXVECTOR3(tempV2.x,tempV2.y,0.0f);
					RECT tempRect;
					(*Biter)->setSpritePos(tempV3);
					//(*Biter)->setBoundingRect(&tempRect);
					(*Biter)->update();
					d3dxSRMgr->setTheTransform((*Biter)->getSpriteTransformMatrix());
					d3dxSRMgr->drawSprite((*Biter)->getTexture(), NULL, NULL, NULL, 0xFFFFFFFF);
				}
			
				d3dxSRMgr->endDraw();
				balloonFont->printText(storage, textPos);
				balloonFont->printText(storage2, textPos2);
				d3dMgr->endRender();
				elapsedTime = 0.0f;
			}
		} //end of game scene
		

		previousTime = currentTime;

	}

	d3dxSRMgr->cleanUp();
	d3dMgr->clean();
	return (int) msg.wParam;
}
