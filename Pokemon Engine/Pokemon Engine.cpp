// Pokemon Engine.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "Windows.h"
#include <iostream>
#include "graphics.h"
#include "pokemon.h"
#include "PokemonBattle.h"
#include "partyMenu.h"
#include "PokemonSummaryScreen.h"
#include "player.h"

#include "Overworld.h"

//Temp:
#include "TileMap.h"

extern SDL_Renderer *gRenderer;

BattleEngineGraphics *BattleUIGFX = NULL;
PokemonBattle* m_Battle = NULL;
PokemonPartyScene *m_Party = NULL;
PokemonSummaryScene *m_summary = NULL;

OverworldController *m_World = NULL;

int battleScene = 0;
bool pressingEnter = false;

int main( int argc, char* args[] )
{
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

	//Tempory setup!
	ivs iv;
	iv.hp = 0;
	iv.atk = 0;
	iv.def = 0;
	iv.spatk = 0;
	iv.spdef = 0;
	iv.speed = 0;

	evs ev;
	ev.hp = 0;
	ev.atk = 0;
	ev.def = 0;
	ev.spatk = 0;
	ev.spdef = 0;
	ev.speed = 0;
	
	//Init player:
	Player *player = new Player();

	Pokemon *defender = new Pokemon();
	defender->side = 0;
	defender->Init(2, iv, ev, 16 );
	defender->pAttacks[2] = new Move( 52 ); //LOL

	Pokemon *defender2 = new Pokemon();
	defender2->Init(3, iv, ev, 100 ); //XD

	Pokemon *defender3 = new Pokemon();
	defender3->Init(4, iv, ev, 1 );

	player->AddToParty( defender );
	player->AddToParty( defender2 );
	player->AddToParty( defender3 );

	m_Battle = new PokemonBattle();

	m_Party = new PokemonPartyScene();
	m_Party->Initialise( player );

	m_summary = new PokemonSummaryScene();
	m_summary->Initialise( player );

	m_World = new OverworldController;
	m_World->Initialise();

	m_World->thePlayer = player;

	battleScene = SCENE_OVERWORLD;

	while( true )
	{

		if( battleScene == SCENE_BATTLE )
		{
			if( !m_Battle->Tick() )
			{
				break;
			}
		}
		else if( battleScene == SCENE_PARTY )
		{
			if( !m_Party->Tick() )
			{
				break;
			}
		}
		else if( battleScene == SCENE_SUMMARY )
		{
			if( !m_summary->Tick() )
			{
				break;
			}
		}
		else if( battleScene == SCENE_OVERWORLD )
		{
			if( !m_World->Tick() )
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
		else
		{
			debounce = false;
		}

		Sleep( 10 );
	}

	SDL_Quit();
	IMG_Quit();
	exit(0);

	return 0;
}
