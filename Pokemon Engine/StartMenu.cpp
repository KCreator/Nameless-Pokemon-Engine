#include "stdafx.h"
#include "StartMenu.h"
#include "Overworld.h"
#include "graphics.h"
#include "save.h"
#include "text.h"

extern PokemonPartyScene *m_Party;
extern TTF_Font *gFont;
extern SDL_Renderer *gRenderer;
extern int battleScene;
extern OverworldController *m_World;
extern bool pressingEnter;

#define MAX_SELECTIONS 7

void MainStartMenu::Initialise()
{
	//Set up the SDL_Texture!
	//Temp?
	SDL_Surface *loadedSurface = IMG_Load( "DATA/GFX/UI/MenuFrames.png" );
	m_texture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );

	//Clear RAM:
	SDL_FreeSurface( loadedSurface );

	m_iSelection = 0;
	m_iSubMenu = 0;
	Debounce = false;

	//ComuniPad:
	m_Phone = new ComuniPad();
	m_Phone->Initialise();
}

bool MainStartMenu::Tick()
{
	//Handle submenus?
	switch( m_iSubMenu )
	{
	case MENU_COMUNIPAD: 
		if( m_Phone->Tick() )
			return true;
		else
			m_iSubMenu = 0;
	}

	//Hook controls:
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);

	if( Debounce == false )
	{
		if( keystate[SDL_GetScancodeFromKey(SDLK_s)] || keystate[SDL_GetScancodeFromKey(SDLK_DOWN)] )
		{
			Debounce = true;

			m_iSelection++;
			if( m_iSelection > MAX_SELECTIONS )
				m_iSelection = 0;
		}
		else if( keystate[SDL_GetScancodeFromKey(SDLK_w)] || keystate[SDL_GetScancodeFromKey(SDLK_UP)] )
		{
			Debounce = true;

			m_iSelection--;
			if( m_iSelection < 0 )
				m_iSelection = MAX_SELECTIONS;
		}
	}
	else if( !keystate[SDL_GetScancodeFromKey(SDLK_s)] && !keystate[SDL_GetScancodeFromKey(SDLK_DOWN)] && !keystate[SDL_GetScancodeFromKey(SDLK_w)] && !keystate[SDL_GetScancodeFromKey(SDLK_UP)] )
	{
		Debounce = false;
	}

	if( pressingEnter )
	{
		if( m_iSelection == 1 )
		{
			//Open pokemon!
			battleScene = SCENE_PARTY;
			FadeToBlack();
			m_Party->IsBattle = false;
			pressingEnter = false;
			m_Party->FadeIn();
			return true;
		}
		if( m_iSelection == 3 )
		{
			m_iSubMenu = MENU_COMUNIPAD;
			pressingEnter = false;
			return true;
		}
		if( m_iSelection == 5 )
		{
			 std::string str[] = {"Yes", "No" };

			//Need to fix this:
			int multiChoice;
			multiChoice = OWMultichoice( "Would you like to save the game?",str , gRenderer, m_World, gFont );
			if( multiChoice == 1 )
			{
				//Save the game!
				SaveHandler handler;
				handler.Save();
			}
		}
		if( m_iSelection == MAX_SELECTIONS )
		{
			return false;
		}
	}

	//Render
	SDL_RenderClear( gRenderer );
	m_World->Render();
	Render();
	SDL_RenderPresent( gRenderer );

	return true;
}

void MainStartMenu::Render()
{
	//Render our main menu:
	SDL_RenderCopy( gRenderer, m_texture, &GetRect(0, 0, 8, 8 ), &GetRect( 445, 5, 8, 8 ) );
	SDL_RenderCopy( gRenderer, m_texture, &GetRect(8, 0, 8, 8 ), &GetRect( 445 + 8, 5, 150-8, 8 ) );
	SDL_RenderCopy( gRenderer, m_texture, &GetRect(16, 0, 8, 8 ), &GetRect( 445 + 150 - 8, 5, 8, 8 ) );

	SDL_RenderCopy( gRenderer, m_texture, &GetRect(0, 8, 8, 8 ), &GetRect( 445 + 0 - 0, 5+8, 8, 470 - 8 ) );
	SDL_RenderCopy( gRenderer, m_texture, &GetRect(8, 8, 8, 8 ), &GetRect( 445 + 8 - 0, 5+8, 150-8, 470 - 8 ) );
	SDL_RenderCopy( gRenderer, m_texture, &GetRect(16, 8, 8, 8 ), &GetRect( 445 + 150 - 8, 5+8, 8, 470 - 8 ) );

	SDL_RenderCopy( gRenderer, m_texture, &GetRect(0, 16, 8, 8 ), &GetRect( 445, 470, 8, 8 ) );
	SDL_RenderCopy( gRenderer, m_texture, &GetRect(8, 16, 8, 8 ), &GetRect( 445 + 8, 470, 150-8, 8 ) );
	SDL_RenderCopy( gRenderer, m_texture, &GetRect(16, 16, 8, 8 ), &GetRect( 445 + 150 - 8, 470, 8, 8 ) );

	//Render our options:
	int X, Y;
	X = 480; Y = 40;

	CText *txt = new CText( "Pokedex", gRenderer, gFont, 1 );
	txt->Render( &GetRect( X, Y, 0, 0 ) );
	delete txt;

	Y += 30;

	txt = new CText( "Pokemon", gRenderer, gFont, 1 );
	txt->Render( &GetRect( X, Y, 0, 0 ) );
	delete txt;

	Y += 30;

	txt = new CText( "Bag", gRenderer, gFont, 1 );
	txt->Render( &GetRect( X, Y, 0, 0 ) );
	delete txt;

	Y += 30;

	txt = new CText( "CommuniPad V1", gRenderer, gFont, 0 );
	txt->Render( &GetRect( X, Y, 100, 20 ) );
	delete txt;

	Y += 30;

	txt = new CText( "PLAYER_NAME", gRenderer, gFont, 0 );
	txt->Render( &GetRect( X, Y, 100, 20 ) );
	delete txt;

	Y += 30;

	txt = new CText( "Save", gRenderer, gFont, 1 );
	txt->Render( &GetRect( X, Y, 0, 0 ) );
	delete txt;

	Y += 30;

	txt = new CText( "Options", gRenderer, gFont, 1 );
	txt->Render( &GetRect( X, Y, 0, 0 ) );
	delete txt;

	Y += 30;

	//Exit is always last!
	Y = 40 + (30*MAX_SELECTIONS);
	txt = new CText( "Exit", gRenderer, gFont, 1 );
	txt->Render( &GetRect( X, Y, 0, 0 ) );
	delete txt;

	//Render that selection arrow:
	SDL_RenderCopy( gRenderer, m_texture, &GetRect(0, 24, 8, 9 ), &GetRect( X-21, 40 + ( 30 * m_iSelection ), 20, 30 ) );
}