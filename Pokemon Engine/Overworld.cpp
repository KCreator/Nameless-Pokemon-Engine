#include "stdafx.h"
#include "graphics.h"
#include "Overworld.h"
#include "utils.h"
#include "pokemon.h"
#include "PokemonBattle.h"
#include "ScriptableObject.h"
#include "text.h"
#include "StartMenu.h"

extern SDL_Renderer *gRenderer;
extern TTF_Font *gFont;
extern PokemonBattle *m_Battle;
extern bool pressingEnter;

extern ObjectFlags obj;

extern int battleScene;

#define COOLDOWN m_bIsRunning ? 10 : 20

bool EditorEnabled;

int editorMode;

std::vector<ScriptableObject*> MapObjects;

bool debounceEditorCombo;
bool useOldMovementSystem = true;

MainStartMenu *m_MainMenu = NULL;

void OverworldController::Initialise()
{
	//Load Textures:
	SDL_Surface* loadedSurface = IMG_Load( "DATA/GFX/Overworlds/Player/Male.png" );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", "DATA/GFX/Overworlds/Player/Male.png", IMG_GetError() );
	}

	Player_Texture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );

	Player_X = 7;
	Player_Y = 7;

	cameraProgressY = (Player_Y-5)*40;
	cameraProgressX = (Player_X-7)*40;

	Player_Facing = 1;

	//Temp?
	loadedSurface = IMG_Load( "DATA/GFX/UI/Party.png" );
	OWTextBox = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );

	editorMode = 0;

	//Init object flags:
	for( int i = 0; i<MAX_OBJECT_TYPES; i++ )
	{
		obj.Objflags[i] = false;
	}

	debounceEditorCombo = false;

	//Start up the menu:
	m_MainMenu = new MainStartMenu();
	m_MainMenu->Initialise();

	m_bMainMenuOpen = false;

	//Init the running shoes system:
	m_bIsRunning = false;

	//Get rid of old loaded surface
	SDL_FreeSurface( loadedSurface );

	//Load tile map:
	tm = new TileMap();
	tm->LoadMap( "DATA/Maps/start.txt" );
	tm->LoadTileImage( "DATA/GFX/Tilesets/EmeraldTiles.png", "DATA/GFX/Tilesets/EmeraldPriorityTiles.png" );
	tm->SetCamera( 0, 0 );
	tm->debug = false;

	mapPrefix = "";

	mapX = 0;
	mapY = 0;

	//Load adjacent maps, if any.
	LoadAdjMaps();
}

bool OverworldController::Tick()
{
	//Set "OOB" colour to black:
	SDL_SetRenderDrawColor( gRenderer, 0, 0, 0, 255);

	//Handle quit event:
	SDL_Event events;
	if (SDL_PollEvent(&events))
	{
		if (events.type == SDL_QUIT)
		{
			return false;
		}
	}

	//Main menu:
	if( m_bMainMenuOpen )
	{
		if( m_MainMenu->Tick() )
		{
			return true;
		}
		else
		{
			m_bMainMenuOpen = false;
		}
	}

	//Check enter:
	if( pressingEnter )
	{
		TryInteract();
	}

	//Check keys:
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	if( MoveCoolDown <=0 )
	{
		//"old" movement system:
		if (useOldMovementSystem)
		{
			//Can sprint and "move key" at the same time, so handle this first:
			if( keystate[SDL_GetScancodeFromKey(SDLK_LSHIFT)] || keystate[SDL_GetScancodeFromKey(SDLK_RSHIFT)] )
				m_bIsRunning = true;
			else
				m_bIsRunning = false;

			if( keystate[SDL_GetScancodeFromKey(SDLK_s)] || keystate[SDL_GetScancodeFromKey(SDLK_DOWN)] )
			{
				if( Player_Facing == 1 )
				{
					if( CheckCollision() )
					{
						Player_Y++;
	
						IsAnimating = true;
					}
				}
	
				MoveCoolDown = COOLDOWN;
				Player_Facing = 1;
			}
			else if( keystate[SDL_GetScancodeFromKey(SDLK_w)] || keystate[SDL_GetScancodeFromKey(SDLK_UP)] )
			{
				if( Player_Facing == 2 )
				{
					if( CheckCollision() )
					{
						Player_Y--;
	
						IsAnimating = true;
					}
				}
	
				MoveCoolDown = COOLDOWN;
				Player_Facing = 2;
			}
			else if( keystate[SDL_GetScancodeFromKey(SDLK_a)] || keystate[SDL_GetScancodeFromKey(SDLK_LEFT)] )
			{
				if( Player_Facing == 3 )
				{
					if( CheckCollision() )
					{
						Player_X--;
	
						IsAnimating = true;
					}
				}
	
				MoveCoolDown = COOLDOWN;
				Player_Facing = 3;
			}
			else if( keystate[SDL_GetScancodeFromKey(SDLK_d)] || keystate[SDL_GetScancodeFromKey(SDLK_RIGHT)])
			{
				if( Player_Facing == 4 )
				{
					if( CheckCollision() )
					{
						Player_X++;
	
						IsAnimating = true;
					}
				}
	
				MoveCoolDown = COOLDOWN;
				Player_Facing = 4;
			}
		}
		else
		{
			//Uses facing to as yFirst. --not sure if that will work.
			int x = Player_X;
			int y = Player_Y;
			
			//Y axis
			if( keystate[SDL_GetScancodeFromKey(SDLK_a)] || keystate[SDL_GetScancodeFromKey(SDLK_LEFT)])		y -= 1;
			else if( keystate[SDL_GetScancodeFromKey(SDLK_a)] || keystate[SDL_GetScancodeFromKey(SDLK_LEFT)])	y += 1;
			
			//X axis
			if( keystate[SDL_GetScancodeFromKey(SDLK_a)] || keystate[SDL_GetScancodeFromKey(SDLK_LEFT)])		x -= 1;
			else if( keystate[SDL_GetScancodeFromKey(SDLK_d)] || keystate[SDL_GetScancodeFromKey(SDLK_RIGHT)])	x += 1;
			
			//lets not move him for nothing
			if (x != 0 || y != 0)
				MovePlayer(x, y);
		}
	}

	if( ((Player_Y-5)*40 - cameraProgressY) != 0 )
	{
		int amount = m_bIsRunning ? 4 : 2;
		bool negativeY =  (((Player_Y-5)*40 - cameraProgressY) < 0 );
		cameraProgressY = negativeY ? cameraProgressY - amount : cameraProgressY + amount;
	}
	if( ((Player_X-7)*40 - cameraProgressX) != 0 )
	{
		int amount = m_bIsRunning ? 4 : 2;
		bool negativeX = (((Player_X-7)*40 - cameraProgressX) < 0 );
		cameraProgressX = negativeX ? cameraProgressX - amount : cameraProgressX + amount;
	}

	if( ((Player_Y-5)*40 - cameraProgressY) == 0 && ((Player_X-7)*40 - cameraProgressX) == 0  && IsAnimating )
	{
		IsAnimating = false;
		ChecKTrigger(); //Trigger has priority!
		if( CheckEncounter() )
			return true;
	}

	tm->SetCamera( cameraProgressX, cameraProgressY );

	if( adjacentMapPosY != NULL )
		adjacentMapPosY->SetCamera( cameraProgressX, adjacentMapPosY->MemoryY*40 + cameraProgressY );
	if( adjacentMapNegY != NULL )
		adjacentMapNegY->SetCamera( cameraProgressX, -tm->MemoryY*40 + cameraProgressY );

	MoveCoolDown--;
	if( MoveCoolDown < 0 )
		MoveCoolDown = 0;

	SDL_RenderClear( gRenderer );
	Render();
	SDL_RenderPresent( gRenderer );

	if( EditorEnabled )
		EditorThink();

	if( keystate[SDL_GetScancodeFromKey(SDLK_LCTRL)] && keystate[SDL_GetScancodeFromKey(SDLK_b)] && EditorEnabled == false && debounceEditorCombo == false )
	{
		EditorInit();

		EditorEnabled = true;
		tm->debug = true;

		debounceEditorCombo = true;
	}
	else if( keystate[SDL_GetScancodeFromKey(SDLK_LCTRL)] && keystate[SDL_GetScancodeFromKey(SDLK_b)] && debounceEditorCombo == false )
	{
		SDL_DestroyRenderer( editorRenderer );
		SDL_DestroyTexture( editorTexture );
		SDL_DestroyWindow( editor );
		EditorEnabled = false;
		tm->debug = false;
	}
	else
	{
		debounceEditorCombo = false;
	}

	if( keystate[SDL_GetScancodeFromKey(SDLK_1)] && EditorEnabled == true )
	{
		editorMode = 0;
		SetEditorTexture();
	}
	else if( keystate[SDL_GetScancodeFromKey(SDLK_2)] && EditorEnabled == true )
	{
		editorMode = 1;
		SetEditorTexture();
	}
	else if( keystate[SDL_GetScancodeFromKey(SDLK_3)] && EditorEnabled == true )
	{
		editorMode = 2;
	}

	//Main menu:
	if( keystate[SDL_GetScancodeFromKey(SDLK_ESCAPE)] && m_bMainMenuOpen == false )
	{
		m_bMainMenuOpen = true;
	}

	return true;
}

void OverworldController::Render()
{
	//Render tile map:
	tm->RenderBG();
	tm->RenderMap();

	//Render adjacent maps:
	if( adjacentMapPosY != NULL )
	{
		adjacentMapPosY->RenderMap();
		adjacentMapPosY->RenderPriorityTiles();
	}
	if( adjacentMapNegY != NULL )
	{
		adjacentMapNegY->RenderMap();
		adjacentMapNegY->RenderPriorityTiles();
	}

	//Test mathmatics!
	int SpriteSheetPosX = 0;
	int SpriteSheetPosY = 0;

	AnimStep ? SpriteSheetPosY = 1 : SpriteSheetPosY = 2;

	if( MoveCoolDown <= 6 )
		SpriteSheetPosY = 0;

	SpriteSheetPosX = Player_Facing - 1;
	if( SpriteSheetPosX > 2 )
		SpriteSheetPosX = 2;

	bool flip = (Player_Facing == 4);

	//Render misc ow sprites:
	for( int i = 0; i < MapObjects.size(); i++ )
	{
		if( MapObjects.at( i )->GetY() <= Player_Y )
		{
			MapObjects.at( i )->Render(cameraProgressX, cameraProgressY);
		}
	}

	SDL_RenderCopyEx( gRenderer, Player_Texture, &GetRect( 15*SpriteSheetPosX, 22*SpriteSheetPosY, 14, 21 ), &GetRect( 40 * 7, 40 * 5 - 15, 40, 55 ), 0, NULL, flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE );

	if( MoveCoolDown == ( COOLDOWN ) - 1 )
	AnimStep = !AnimStep;

	//Render misc ow sprites:
	for( int i = 0; i < MapObjects.size(); i++ )
	{
		if( MapObjects.at( i )->GetY() > Player_Y )
		{
			MapObjects.at( i )->Render(cameraProgressX, cameraProgressY);
		}
	}

	//Render "layer1" tiles:
	tm->RenderPriorityTiles();
}

void OverworldController::RenderTxtBox( bool isMultichoice, int numOptions )
{
	SDL_RenderCopy( gRenderer, OWTextBox, &GetRect(162, 232, 180, 28), &GetRect( 20, 380, 560, 80 ) );

	if( isMultichoice )
	{
		SDL_RenderCopy( gRenderer, OWTextBox, &GetRect(162, 232, 180, 28), &GetRect( 480, 380 - numOptions*30, 100, 20 + numOptions*30 ) );
	}
}

void OverworldController::EditorInit()
{
	editor = SDL_CreateWindow( "Tile Selector", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600, 500, SDL_WINDOW_SHOWN );
	SDL_HideWindow( editor );
	editorRenderer = SDL_CreateRenderer( editor, -1, SDL_RENDERER_ACCELERATED );
	SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	const char *path = "DATA/GFX/Tilesets/EmeraldTiles.png";

	//Textures:
	SDL_Surface* loadedSurface = IMG_Load( path );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError() );
	}

	editorTexture = SDL_CreateTextureFromSurface( editorRenderer, loadedSurface );

	int wide, tall;
	SDL_QueryTexture( editorTexture, NULL, NULL, &wide, &tall );
	//SDL_SetWindowSize( editor, wide, tall );
	SDL_RenderCopy( editorRenderer, editorTexture, NULL , &GetRect( 0, 0, wide, tall ) );
	SDL_RenderPresent( editorRenderer );

	editorCameraX = 0;
	editorCameraY = 0;

	editorCollisionSelection = 0;

	//Get rid of old loaded surface
	SDL_FreeSurface( loadedSurface );
}

void OverworldController::SetEditorTexture()
{
	//Free up RAM:
	SDL_DestroyTexture( editorTexture );

	std::string path;

	if( editorMode == 0 )
	{
		path = "DATA/GFX/Tilesets/EmeraldTiles.png";
	}
	if( editorMode == 1 )
	{
		path = "DATA/GFX/Tilesets/EmeraldPriorityTiles.png";
	}

	//Textures:
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError() );
	}

	editorTexture = SDL_CreateTextureFromSurface( editorRenderer, loadedSurface );

	//Clear surface for RAM:
	SDL_FreeSurface( loadedSurface );
}

void OverworldController::EditorThink()
{
	SDL_ShowWindow( editor );
	SDL_RenderClear( editorRenderer );
	int wide, tall;
	SDL_QueryTexture( editorTexture, NULL, NULL, &wide, &tall );

	if( editorMode != 2 )
	{
		SDL_RenderCopy( editorRenderer, editorTexture, NULL, &GetRect( editorCameraX, editorCameraY, wide, tall ) );
	}
	else
	{
		int objs = 1;
		//Oh god...
		while( true )
		{
			std::string str = "DATA/Overworld/Events/";
			str += std::to_string( (_ULonglong)objs );
			str += ".txt";

			FILE *file = fopen( str.c_str(), "r" );
			if( !file )
				break;
			fclose( file );

			CText *txt = new CText( std::to_string( (_ULonglong)objs ), editorRenderer, gFont );
			txt->Render( &GetRect( objs*16, 0, 16, 16 ) );
			delete txt;

			objs++;
		}
	}

	SDL_Window *mWindow = SDL_GetMouseFocus();

	if( mWindow == GetScreen() )
	{
		int mx,my;
		Uint32 mState = SDL_GetMouseState( &mx, &my );

		mx += cameraProgressX;
		my += cameraProgressY;

		for( int y = 0; y < tm->MemoryY; y++ )
		{
			for( int x = 0; x < tm->MemoryX; x++ )
			{
				if( (mx > x*40 && mx < x*40+40) && (my > y*40 && my < y*40+40) )
				{
					//Create highlighted square:
					SDL_RenderDrawRect( gRenderer, &GetRect( x*40 - cameraProgressX, y*40 -cameraProgressY , 40, 40 ) );

					if( mState == SDL_BUTTON_LEFT )
					{
						if( editorMode == 0 )
							tm->SetTileType( x, y, editorSelection );
						if( editorMode == 1 )
							tm->SetLayerTileType( x, y, editorSelection );

						if( editorMode == 2 )
						{
							if( editorSelection == 0 )
							{
								for( int i = 0; i < MapObjects.size(); i++ )
								{
									if( MapObjects.at(i)->GetX() == x && MapObjects.at(i)->GetY() == y )
									{
										MapObjects.erase(MapObjects.begin() + i);
									}
								}
							}
							else
							{
								for( int i = 0; i < MapObjects.size(); i++ )
								{
									if( MapObjects.at(i)->GetX() == x && MapObjects.at(i)->GetY() == y )
									{
										MapObjects.erase(MapObjects.begin() + i);
									}
								}

								ScriptableObject *obj = new ScriptableObject();
								obj->SetUp( editorSelection, x, y );
								MapObjects.push_back( obj );
							}
						}
					}
					if( mState == SDL_BUTTON(3) )
					{
						tm->SetCollision( x, y, editorCollisionSelection );
					}
				}
			}
		}

		SDL_RenderPresent( gRenderer );
	}

	if( mWindow == editor )
	{
		int mx,my;
		Uint32 mState = SDL_GetMouseState( &mx, &my );

		if( mState == SDL_BUTTON(3) )
		{
			if( mx > 600 - 20 )
			{
				editorCameraX -= 16;
			}
			if( my > 480 - 20 )
			{
				editorCameraY -= 16;
			}

			if( mx < 20 && editorCameraX != 0 )
			{
				editorCameraX += 16;
			}
			if( my < 20 && editorCameraY != 0 )
			{
				editorCameraY += 16;
			}
		}

		mx -= editorCameraX;
		my -= editorCameraY;

		for( int y = 0; y < (tall/16); y++ )
		{
			for( int x = 0; x < (wide/16); x++ )
			{
				if( (mx > x*16 && mx < x*16+16) && (my > y*16 && my < y*16+16) )
				{
					//Create highlighted square:
					SDL_SetRenderDrawColor( editorRenderer, 255, 0, 0, 255);
					SDL_RenderDrawRect( editorRenderer, &GetRect( x*16 + editorCameraX, y*16 + editorCameraY, 16, 16 ) );

					if( mState == SDL_BUTTON_LEFT )
					{
						int tilex = x, tiley = (y) * (wide/16);
						editorSelection = tilex + tiley;
					}
				}
			}
		}

		//Hacky!
		mx += editorCameraX;
		my += editorCameraY;

		//Collsion modes:
		SDL_SetRenderDrawColor( editorRenderer, 0, 0, 0, 255);
		SDL_RenderFillRect( editorRenderer, &GetRect( 0, 480, 20, 20 ) );

		SDL_SetRenderDrawColor( editorRenderer, 255, 0, 0, 255);
		SDL_RenderFillRect( editorRenderer, &GetRect( 20, 480, 20, 20 ) );

		SDL_SetRenderDrawColor( editorRenderer, 0, 255, 0, 255);
		SDL_RenderFillRect( editorRenderer, &GetRect( 40, 480, 20, 20 ) );

		SDL_SetRenderDrawColor( editorRenderer, 255, 100, 100, 255);
		SDL_RenderFillRect( editorRenderer, &GetRect( 60, 480, 20, 10 ) );

		if( mState == SDL_BUTTON(1) )
		{
			if( (mx > 0 && mx < 20) && (my > 480 && my < 500) )
			{
				editorCollisionSelection = 0;
			}
			if( (mx > 20 && mx < 40) && (my > 480 && my < 500) )
			{
				editorCollisionSelection = 1;
			}
			if( (mx > 40 && mx < 60) && (my > 480 && my < 500) )
			{
				editorCollisionSelection = 2;
			}
			if( (mx > 60 && mx < 80) && (my > 480 && my < 500) )
			{
				editorCollisionSelection = TILE_SOLID_CLIFF_UP;
			}
		}

		//Hacky!
		mx -= editorCameraX;
		my -= editorCameraY;

		SDL_SetRenderDrawColor( editorRenderer, 255, 255, 255, 255);
	}

	int tilex = 0, tiley = 0;
	tilex = editorSelection;
	while( tilex >= wide/16 )
	{
		tilex -= wide/16;
		tiley++;
	}

	SDL_SetRenderDrawColor( editorRenderer, 255, 0, 0, 255);
	SDL_RenderDrawRect( editorRenderer, &GetRect( tilex*16 + editorCameraX, tiley*16 + editorCameraY, 16, 16 ) );
	SDL_SetRenderDrawColor( editorRenderer, 255, 255, 255, 255);

	SDL_RenderPresent( editorRenderer );

	//Save system:
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	if( keystate[SDL_GetScancodeFromKey(SDLK_LCTRL)] && keystate[SDL_GetScancodeFromKey(SDLK_s)] )
	{
		/*std::string str = "DATA/Maps/map_";

		str += std::to_string( (_ULonglong)mapX );
		str += "_";
		str += std::to_string( (_ULonglong)mapY );
		str += ".txt";
		tm->TilePath = str.c_str();*/
		tm->SaveMap();
	}
}

bool OverworldController::CheckEncounter()
{
	int chance = rand()%20;
	if( tm->GetCollision( Player_X, Player_Y ) == 2 )
	{
		if( chance == 1 )
		{
			//Initialise battle!
			Pokemon *WildPoke;
			WildPoke = new Pokemon();

			evs ev;
			ev.hp = 0;
			ev.atk = 0;
			ev.def = 0;
			ev.spatk = 0;
			ev.spdef = 0;
			ev.speed = 0;

			ivs iv;
			iv.hp = rand()%32;
			iv.atk = rand()%32;
			iv.def = rand()%32;
			iv.spatk = rand()%32;
			iv.spdef = rand()%32;
			iv.speed = rand()%32;

			WildPoke->side = 1;
			int encounterType = rand()%tm->encounters.size;
			WildPoke->Init( tm->encounters.encounters[encounterType], iv, ev, randomMinMax(  tm->encounters.minLevel[encounterType], tm->encounters.maxLevel[encounterType] ));
			WildPoke->m_bShouldRender = true;
			m_Battle->Initialise( WildPoke, thePlayer->m_pkmParty[0], true, NULL );

			battleScene = SCENE_BATTLE;

			FadeToBlack();

			m_Battle->WildBattleStartAnim();
			return true;
		}
	}
	return false;
}

void OverworldController::TryInteract()
{
	for( int i = 0; i < MapObjects.size(); i++ )
	{
		if( MapObjects.at(i)->Type == 2 )
					continue;

		if( Player_Facing == 1 )
		{
			if( MapObjects.at(i)->GetX() == Player_X && MapObjects.at(i)->GetY() == Player_Y + 1 )
			{
				MapObjects.at(i)->Interact();
			}
		}

		if( Player_Facing == 2 )
		{
			if( MapObjects.at(i)->GetX() == Player_X && MapObjects.at(i)->GetY() == Player_Y - 1 )
			{
				MapObjects.at(i)->Interact();
			}
		}

		if( Player_Facing == 3 )
		{
			if( MapObjects.at(i)->GetX() + 1 == Player_X && MapObjects.at(i)->GetY() == Player_Y )
			{
				MapObjects.at(i)->Interact();
			}
		}

		if( Player_Facing == 4 )
		{
			if( MapObjects.at(i)->GetX() - 1 == Player_X && MapObjects.at(i)->GetY() == Player_Y)
			{
				MapObjects.at(i)->Interact();
			}
		}
	}
}

bool OverworldController::CheckCollision()
{
	//Hacky...
	for( int i = 0; i < MapObjects.size(); i++ )
	{
		if( MapObjects.at(i)->Type != 1 )
					continue;

		if( Player_Facing == 1 )
		{
			if( MapObjects.at(i)->GetX() == Player_X && MapObjects.at(i)->GetY() == Player_Y + 1 )
			{
				return false;
			}
		}

		if( Player_Facing == 2 )
		{
			if( MapObjects.at(i)->GetX() == Player_X && MapObjects.at(i)->GetY() == Player_Y - 1 )
			{
				return false;
			}
		}

		if( Player_Facing == 3 )
		{
			if( MapObjects.at(i)->GetX() + 1 == Player_X && MapObjects.at(i)->GetY() == Player_Y )
			{
				return false;
			}
		}

		if( Player_Facing == 4 )
		{
			if( MapObjects.at(i)->GetX() - 1 == Player_X && MapObjects.at(i)->GetY() == Player_Y)
			{
				return false;
			}
		}
	}

	if( Player_Facing == 1 )
	{
		//If I am about to enter a new "map":
		if( Player_Y == tm->MemoryY - 1 )
		{
			if( adjacentMapNegY != NULL )
			{
				mapY--;
				//String:
				std::string str = "DATA/Maps/" + mapPrefix  + "_";

				str += std::to_string( (_ULonglong)mapX );
				str += "_";
				str += std::to_string( (_ULonglong)mapY );
				str += ".txt";

				//Clear current map
				delete tm;
				tm = NULL;
				MapObjects.clear();

				//Kill adjacent maps:
				delete adjacentMapPosY;
				adjacentMapPosY = NULL;
				delete adjacentMapPosX;
				adjacentMapPosX = NULL;
				delete adjacentMapNegX;
				adjacentMapNegX = NULL;
				delete adjacentMapNegY;
				adjacentMapNegY = NULL;

				tm = new TileMap();
				tm->LoadMap( str.c_str() );
				tm->LoadTileImage( "DATA/GFX/Tilesets/EmeraldTiles.png", "DATA/GFX/Tilesets/EmeraldPriorityTiles.png" );
				tm->SetCamera( 0, 0 );
				tm->debug = false;

				LoadAdjMaps();

				Player_Y = -1;
				cameraProgressY = (Player_Y-5)*40;

				RunMapScripts();
			}
		}
		if( tm->GetCollision( Player_X, Player_Y + 1 ) != 1 &&
			tm->GetCollision( Player_X, Player_Y + 1 ) != TILE_SOLID_CLIFF_UP )
		{
			return true;
		}
	}
	else if( Player_Facing == 2 )
	{
		//If I am about to enter a new "map":
		if( Player_Y == 0 )
		{
			if( adjacentMapPosY != NULL )
			{
				mapY++;
				//String:
				std::string str = "DATA/Maps/" + mapPrefix  + "_";

				str += std::to_string( (_ULonglong)mapX );
				str += "_";
				str += std::to_string( (_ULonglong)mapY );
				str += ".txt";

				bool shouldbedebug = tm->debug;

				//Clear current map
				delete tm;
				tm = NULL;
				MapObjects.clear();

				//Kill adjacent maps:
				delete adjacentMapPosY;
				adjacentMapPosY = NULL;
				delete adjacentMapPosX;
				adjacentMapPosX = NULL;
				delete adjacentMapNegX;
				adjacentMapNegX = NULL;
				delete adjacentMapNegY;
				adjacentMapNegY = NULL;

				tm = new TileMap();
				MapObjects.clear();
				tm->LoadMap( str.c_str() );
				tm->LoadTileImage( "DATA/GFX/Tilesets/EmeraldTiles.png", "DATA/GFX/Tilesets/EmeraldPriorityTiles.png" );
				tm->SetCamera( 0, 0 );
				tm->debug = shouldbedebug;

				LoadAdjMaps();

				Player_Y = tm->MemoryY;
				cameraProgressY = (Player_Y-5)*40;

				RunMapScripts();
			}
		}

		//Check "solid tiles" that the player can stand on, but cant move further on: EI, cliffs/ect
		if( tm->GetCollision( Player_X, Player_Y ) == TILE_SOLID_CLIFF_UP )
		{
			return false;
		}

		if( tm->GetCollision( Player_X, Player_Y - 1 ) != 1 )
		{
			return true;
		}
	}
	else if( Player_Facing == 3 )
	{
		if( tm->GetCollision( Player_X - 1, Player_Y) != 1 )
		{
			return true;
		}
	}
	else if( Player_Facing == 4 )
	{
		if( tm->GetCollision( Player_X + 1, Player_Y ) != 1 )
		{
			return true;
		}
	}

	return false;
}

void OverworldController::LoadAdjMaps()
{
	//Load map x y+1:
	std::string str = "DATA/Maps/" + mapPrefix  + "_";

	str += std::to_string( (_ULonglong)mapX );
	str += "_";
	str += std::to_string( (_ULonglong)(mapY + 1) );
	str += ".txt";

	if( FileExists( str.c_str() ) )
	{
		adjacentMapPosY = new TileMap();
		adjacentMapPosY->LoadMapAdjacent( str.c_str() );
		adjacentMapPosY->LoadTileImage( "DATA/GFX/Tilesets/EmeraldTiles.png", "DATA/GFX/Tilesets/EmeraldPriorityTiles.png" );
		adjacentMapPosY->SetCamera( 0, tm->MemoryY );
		adjacentMapPosY->debug = false;
	}
	else
		adjacentMapPosY = NULL;
	 

	//Load map x y-1:
	str = "DATA/Maps/" + mapPrefix  + "_";

	str += std::to_string( (_ULonglong)mapX );
	str += "_";
	str += std::to_string( (_ULonglong)(mapY - 1) );
	str += ".txt";

	if( FileExists( str.c_str() ) )
	{
		adjacentMapNegY = new TileMap();
		adjacentMapNegY->LoadMapAdjacent( str.c_str() );
		adjacentMapNegY->LoadTileImage( "DATA/GFX/Tilesets/EmeraldTiles.png", "DATA/GFX/Tilesets/EmeraldPriorityTiles.png" );
		adjacentMapNegY->SetCamera( 0, tm->MemoryY );
		adjacentMapNegY->debug = false;
	}
	else
		adjacentMapNegY = NULL;

	//Load map x-1 y:
	str = "DATA/Maps/" + mapPrefix  + "_";

	str += std::to_string( (_ULonglong)mapX - 1 );
	str += "_";
	str += std::to_string( (_ULonglong)(mapY) );
	str += ".txt";

	if( FileExists( str.c_str() ) )
	{
		adjacentMapNegX = new TileMap();
		adjacentMapNegX->LoadMapAdjacent( str.c_str() );
		adjacentMapNegX->LoadTileImage( "DATA/GFX/Tilesets/EmeraldTiles.png", "DATA/GFX/Tilesets/EmeraldPriorityTiles.png" );
		adjacentMapNegX->SetCamera( tm->MemoryX, 0 );
		adjacentMapNegX->debug = false;
	}
	else
		adjacentMapNegX = NULL;

	//Load map x+1 y:
	str = "DATA/Maps/" + mapPrefix  + "_";

	str += std::to_string( (_ULonglong)mapX + 1 );
	str += "_";
	str += std::to_string( (_ULonglong)(mapY) );
	str += ".txt";

	if( FileExists( str.c_str() ) )
	{
		adjacentMapPosX = new TileMap();
		adjacentMapPosX->LoadMapAdjacent( str.c_str() );
		adjacentMapPosX->LoadTileImage( "DATA/GFX/Tilesets/EmeraldTiles.png", "DATA/GFX/Tilesets/EmeraldPriorityTiles.png" );
		adjacentMapPosX->SetCamera( tm->MemoryX, 0 );
		adjacentMapPosX->debug = false;
	}
	else
		adjacentMapPosX = NULL;
}

void OverworldController::ChecKTrigger()
{
	for( int i = 0; i < MapObjects.size(); i++ )
	{
		if( MapObjects.at(i)->GetX() == Player_X && MapObjects.at(i)->GetY() == Player_Y )
		{
			if( MapObjects.at(i)->Type == 2 )
			{
				MapObjects.at(i)->Interact();
			}
		}
	}
}

void OverworldController::MovePlayer( int x, int y, bool yFirst )
{
	if( !yFirst )
	{
		Player_X = x;

		while( ((Player_X-7)*40 - cameraProgressX) != 0 )
		{
			MoveCoolDown--;
			if( MoveCoolDown < 0 )
				MoveCoolDown = COOLDOWN;

			bool negativeX = (((Player_X-7)*40 - cameraProgressX) < 0 );
			cameraProgressX = negativeX ? cameraProgressX - 2 : cameraProgressX + 2;

			if( negativeX )
				Player_Facing = 3;
			else
				Player_Facing = 4;

			tm->SetCamera( cameraProgressX, cameraProgressY );
			if( adjacentMapPosY != NULL )
				adjacentMapPosY->SetCamera( cameraProgressX, adjacentMapPosY->MemoryY*40 + cameraProgressY );
			if( adjacentMapNegY != NULL )
				adjacentMapNegY->SetCamera( cameraProgressX, -tm->MemoryY*40 + cameraProgressY );

			SDL_RenderClear( gRenderer );
			Render();
			SDL_RenderPresent( gRenderer );

			SDL_Delay( 10 );
		}

		Player_Y = y;

		while( ((Player_Y-5)*40 - cameraProgressY) != 0 )
		{
			MoveCoolDown--;
			if( MoveCoolDown < 0 )
				MoveCoolDown = COOLDOWN;

			bool negativeY =  (((Player_Y-5)*40 - cameraProgressY) < 0 );
			cameraProgressY = negativeY ? cameraProgressY - 2 : cameraProgressY + 2;

			if( negativeY )
				Player_Facing = 2;
			else
				Player_Facing = 1;

			tm->SetCamera( cameraProgressX, cameraProgressY );
			if( adjacentMapPosY != NULL )
				adjacentMapPosY->SetCamera( cameraProgressX, adjacentMapPosY->MemoryY*40 + cameraProgressY );
			if( adjacentMapNegY != NULL )
				adjacentMapNegY->SetCamera( cameraProgressX, -tm->MemoryY*40 + cameraProgressY );

			SDL_RenderClear( gRenderer );
			Render();
			SDL_RenderPresent( gRenderer );

			SDL_Delay( 10 );
		}

		MoveCoolDown = 0;
		SDL_RenderClear( gRenderer );
		Render();
		SDL_RenderPresent( gRenderer );

		SDL_Delay( 10 );
	}
}

void OverworldController::SetMapPos( std::string path, int x, int y, int mapx, int mapy )
{
	mapPrefix = "";
	mapPrefix += path;

	std::string str = "DATA/Maps/" + mapPrefix  + "_";

	mapX = mapx;
	mapY = mapy;

	str += std::to_string( (_ULonglong)( mapX ) );
	str += "_";
	str += std::to_string( (_ULonglong)( mapY ) );
	str += ".txt";

	//Clear current map
	delete tm;
	tm = NULL;
	MapObjects.clear();

	//Kill adjacent maps:
	delete adjacentMapPosY;
	adjacentMapPosY = NULL;
	delete adjacentMapPosX;
	adjacentMapPosX = NULL;
	delete adjacentMapNegX;
	adjacentMapNegX = NULL;
	delete adjacentMapNegY;
	adjacentMapNegY = NULL;

	//Load tile map:
	tm = new TileMap();
	tm->LoadMap( str.c_str() );
	tm->LoadTileImage( "DATA/GFX/Tilesets/EmeraldTiles.png", "DATA/GFX/Tilesets/EmeraldPriorityTiles.png" );
	tm->SetCamera( 0, 0 );
	tm->debug = false;

	//Set pos:
	Player_X = x;
	Player_Y = y;

	cameraProgressY = (Player_Y-5)*40;
	cameraProgressX = (Player_X-7)*40;

	//Load ajacent maps:
	LoadAdjMaps();

	tm->SetCamera( cameraProgressX, cameraProgressY );
	if( adjacentMapPosY != NULL )
		adjacentMapPosY->SetCamera( cameraProgressX, adjacentMapPosY->MemoryY*40 + cameraProgressY );
	if( adjacentMapNegY != NULL )
		adjacentMapNegY->SetCamera( cameraProgressX, -tm->MemoryY*40 + cameraProgressY );
	if( adjacentMapPosX != NULL )
		adjacentMapPosX->SetCamera( adjacentMapPosX->MemoryX*40 + cameraProgressX, cameraProgressY );
	if( adjacentMapNegX != NULL )
		adjacentMapNegX->SetCamera( -tm->MemoryX*40 + cameraProgressX, cameraProgressY );

	RunMapScripts(); //Recursion *could* be an issue here... but thats the map-makers fault, I guess!
}

void OverworldController::FadeIn()
{
	int progress = 255;
	while( true )
	{
		SDL_RenderClear( gRenderer );

		Render();

		//Set up tempory surfaces
		SDL_Surface *surf = SDL_CreateRGBSurface( SDL_SWSURFACE, 600, 480, 1, 0,0,0, progress );
		SDL_Texture *texture = SDL_CreateTextureFromSurface( gRenderer, surf );

		//Get rid of old loaded surface
		SDL_FreeSurface( surf );

		SDL_SetTextureBlendMode( texture, SDL_BLENDMODE_BLEND );
		SDL_SetTextureAlphaMod( texture, progress );
		SDL_SetTextureColorMod( texture, 0, 0, 0 );

		SDL_RenderCopy( gRenderer, texture, NULL, NULL );

		SDL_RenderPresent( gRenderer );

		progress -= 10;

		if( progress <= 0 )
		{
			//Destroy texture to save RAM:
			SDL_DestroyTexture( texture );
			break;
		}
		//Free up texture memory:
		SDL_DestroyTexture( texture );
		SDL_Delay( 1 );
	}
}

void OverworldController::RunMapScripts()
{
	for( int i = 0; i < MapObjects.size(); i++ )
	{
		if( MapObjects.at(i)->Type == 3 )
			 MapObjects.at(i)->Interact();
	}
}