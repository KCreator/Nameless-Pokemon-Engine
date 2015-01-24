#include "stdafx.h"
#include "StartMenu.h"
#include "Overworld.h"
#include "graphics.h"
#include "text.h"

extern PokemonPartyScene *m_Party;
extern TTF_Font *gFont;
extern SDL_Renderer *gRenderer;
extern int battleScene;
extern OverworldController *m_World;

void MainStartMenu::Initialise()
{
	//Set up the SDL_Texture!
	//Temp?
	SDL_Surface *loadedSurface = IMG_Load( "DATA/GFX/UI/MenuFrames.png" );
	m_texture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );

	m_iSelection = 0;
}

void MainStartMenu::Tick()
{
	//Hook controls:

	//Render
	SDL_RenderClear( gRenderer );
	m_World->Render();
	Render();
	SDL_RenderPresent( gRenderer );
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

	txt = new CText( "Exit", gRenderer, gFont, 1 );
	txt->Render( &GetRect( X, Y, 0, 0 ) );
	delete txt;

	//Render that selection arrow:
	SDL_RenderCopy( gRenderer, m_texture, &GetRect(0, 24, 8, 9 ), &GetRect( X-21, 40 + ( 30 * m_iSelection ), 20, 30 ) );}