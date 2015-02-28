// Pokemon Engine.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "Windows.h"
#include <iostream>
#include "time.h"
#include "graphics.h"
#include "pokemon.h"
#include "PokemonBattle.h"
#include "partyMenu.h"
#include "PokemonSummaryScreen.h"
#include "player.h"
#include "save.h"
#include "Overworld.h"
#include "bag.h"
#include "particle.h"

//Temp:
#include "TileMap.h"

extern SDL_Renderer *gRenderer;

BattleEngineGraphics *BattleUIGFX = NULL;
PokemonBattle* m_Battle = NULL;
PokemonPartyScene *m_Party = NULL;
PokemonSummaryScene *m_summary = NULL;
BagScene *m_Bag = NULL;

OverworldController *m_World = NULL;

Player *gPlayer = NULL;

int battleScene = 0;
bool pressingEnter = false;
bool pressingEsc = false;

int main( int argc, char* args[] )
{
	//Seed RNG:
	srand( time(NULL) );

	//Init SDL:
	initSDL();

	//Init BattleUIGFX
	BattleUIGFX = new BattleEngineGraphics;

	BattleUIGFX->gRender = gRenderer;

	BattleUIGFX->bg = new BattleBackGround;
	BattleUIGFX->menu = new BattleMenu;
	BattleUIGFX->hpDisp = new HPDisplays;

	BattleUIGFX->bg->LoadFromFile( "DATA/GFX/BackGround/PlatinumBattleBack.png" );
	BattleUIGFX->menu->LoadFromFile( "DATA/GFX/UI/GBAMenuBars.png" , 1 );
	BattleUIGFX->menu->LoadFromFile( "DATA/GFX/UI/PkmnTypes.png" , 2 );
	BattleUIGFX->menu->LoadFromFile( "DATA/GFX/UI/GBAMenuBars.png" , 3 );
	BattleUIGFX->hpDisp->LoadFromFile( "DATA/GFX/UI/GBAMenuBars.png" );
	BattleUIGFX->menu->cursorPos = 1;
	BattleUIGFX->menu->subMenu = 0;

	//Init underlying variables
	bool debounce = false;
	
	//Init player:
	gPlayer = new Player();

	m_Battle = new PokemonBattle();

	m_Party = new PokemonPartyScene();
	m_Party->Initialise( gPlayer );

	m_summary = new PokemonSummaryScene();
	m_summary->Initialise( gPlayer );

	m_World = new OverworldController;
	m_World->thePlayer = gPlayer;
	m_World->Initialise();

	m_Bag = new BagScene();
	m_Bag->Init();

	battleScene = SCENE_OVERWORLD;

	//Load data?
	if( FileExists( "DATA/Save/save.txt" ) )
	{
		SaveHandler handler;
		handler.Load();
	}
	m_World->RunMapScripts();

	while( true )
	{
		//Battle
		if( battleScene == SCENE_BATTLE )
		{
			if( !m_Battle->Tick() )
			{
				break;
			}
		}
		//Party
		else if( battleScene == SCENE_PARTY )
		{
			if( !m_Party->Tick() )
			{
				break;
			}
		}
		//Summary
		else if( battleScene == SCENE_SUMMARY )
		{
			if( !m_summary->Tick() )
			{
				break;
			}
		}
		//Overworld
		else if( battleScene == SCENE_OVERWORLD )
		{
			if( !m_World->Tick() )
			{
				break;
			}
		}
		//Bag
		else if( battleScene == SCENE_BAG )
		{
			if( !m_Bag->Tick() )
			{
				break;
			}
		}

		//Hacky, but it does help...
		const Uint8 *keystate = SDL_GetKeyboardState( NULL );
		SDL_PumpEvents();
		if ( ( keystate[SDL_GetScancodeFromKey(SDLK_RETURN)] || keystate[SDL_GetScancodeFromKey(SDLK_SPACE)] ) )
		{
			if( !debounce )
			{
				pressingEnter = true;
			}
			else
			{
				pressingEnter = false;
			}
			debounce = true;
		}
		else if ( keystate[SDL_GetScancodeFromKey(SDLK_ESCAPE)] )
		{
			if( !debounce )
			{
				pressingEsc = true;
			}
			else
			{
				pressingEsc = false;
			}
			debounce = true;
		}
		else
		{
			debounce = false;
		}

		Sleep( 10 );
	}
	//Deactivate SDL
	SDL_Quit();
	IMG_Quit();
	exit(0);

	return 0;
}
