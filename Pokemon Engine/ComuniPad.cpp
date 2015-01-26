#include "stdafx.h"
#include "graphics.h"
#include "text.h"
#include "ComuniPad.h"
#include "Overworld.h"

extern TTF_Font *gFont;
extern SDL_Renderer *gRenderer;
extern bool pressingEnter;

extern OverworldController *m_World;

void ComuniPad::Initialise()
{
	//Load texture:
	SDL_Surface *loadedSurface = IMG_Load( "DATA/GFX/UI/ComuniPad.png" );
	m_tex = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );

	//Set variables:
	m_iSelection = 0;
	m_iAnimState = -1;
	m_iAnimProgress = 0;
}

bool ComuniPad::Tick()
{
	//Hook controls:
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	if( keystate[SDL_GetScancodeFromKey(SDLK_ESCAPE)] && !pressingEnter )
	{
		m_iAnimState = -1;
		m_iAnimProgress = 0;
		return false;
	}

	SDL_RenderClear( gRenderer );

	m_World->Render();
	if( m_iAnimState == -1 )
	{
		m_iAnimProgress+=6;
	}
	if( m_iAnimState == 0)
	{
		if( pressingEnter )
		{
			m_iAnimState = 1;
		}
	}
	if( m_iAnimState == 1 )
	{
		m_iAnimProgress+=2;
	}
	if( m_iAnimState == 2 )
	{
		m_iAnimProgress+=4;
	}
	RenderBG();

	if( m_iAnimState == 3 )
	{
		RenderButtons();
	}

	SDL_RenderPresent( gRenderer );

	return true;
}

void ComuniPad::RenderBG()
{
	//BG stuff:
	if( m_iAnimState == -1 )
	{
		//Bring up the menu in an awesome way:
		SDL_RenderFillRect( gRenderer, &GetRect( 20, 20 + 460 - m_iAnimProgress, 560, 440 ) );
		SDL_RenderCopy( gRenderer, m_tex, &GetRect(0, 0, 340, 340 ), &GetRect( 10, 10 + 460 - m_iAnimProgress, 580, 460 ) );

		if( m_iAnimProgress >= 460 )
		{
			m_iAnimProgress = 0;
			m_iAnimState = 0;
		}

		return;
	}
	if( m_iAnimState == 0 )
	{
		SDL_RenderFillRect( gRenderer, &GetRect( 20, 20, 560, 440 ) );
	}
	if( m_iAnimState == 1 )
	{
		SDL_RenderFillRect( gRenderer, &GetRect( 20, 20, 560, 440 ) );

		SDL_SetRenderDrawColor(gRenderer,255,255,255,255);
		SDL_RenderFillRect( gRenderer, &GetRect( (20 + 560/2) - m_iAnimProgress, 20, m_iAnimProgress*2, 440 ) );

		if( m_iAnimProgress*2 >= 560 )
		{
			m_iAnimProgress = 560/2;
			m_iAnimState = 2;
			m_iAnimProgress = 0;
		}
	}
	if( m_iAnimState == 2 )
	{
		if( m_iAnimProgress >= 255 )
		{
			m_iAnimState = 3;
		}
		SDL_SetRenderDrawColor(gRenderer, 255 - m_iAnimProgress, 255 - m_iAnimProgress, 255, 255);
		SDL_RenderFillRect( gRenderer, &GetRect( 20, 20, 560, 440 ) );
	}
	if( m_iAnimState == 3 )
	{
		SDL_SetRenderDrawColor(gRenderer,0,0,255,255);
		SDL_RenderFillRect( gRenderer, &GetRect( 20, 20, 560, 440 ) );

		SDL_SetRenderDrawColor(gRenderer,0,0,200,255);
		SDL_RenderFillRect( gRenderer, &GetRect( 20, 20 + 420 - m_iAnimProgress%420, 560, 40 ) );
		SDL_SetRenderDrawColor(gRenderer,0,0,180,255);
		SDL_RenderFillRect( gRenderer, &GetRect( 20 + 540 - m_iAnimProgress%540, 20 , 40, 440 ) );

		m_iAnimProgress += 3;

		//Tempory:
		CText *txt = new CText( "Communication Pad VERSION 1.01", gRenderer, gFont, 1, 255 );
		txt->Render(  &GetRect( 40, 40, 0, 0 ) );
		delete txt;
	}

	SDL_SetRenderDrawColor(gRenderer,0,0,0,0);

	//Main texture:
	SDL_RenderCopy( gRenderer, m_tex, &GetRect(0, 0, 340, 340 ), &GetRect( 10, 10, 580, 460 ) );
}

void ComuniPad::RenderButtons()
{
	int buttonX = 1;

	//Tempory:
	SDL_RenderCopy( gRenderer, m_tex, &GetRect(20*(buttonX-1), 340, 20, 20 ), &GetRect( 80*buttonX, 80, 80, 80 ) );
	buttonX++;
	SDL_RenderCopy( gRenderer, m_tex, &GetRect(20*(buttonX-1), 340, 20, 20 ), &GetRect( 80*buttonX, 80, 80, 80 ) );
	buttonX++;
	SDL_RenderCopy( gRenderer, m_tex, &GetRect(20*(buttonX-1), 340, 20, 20 ), &GetRect( 80*buttonX, 80, 80, 80 ) );
	buttonX++;
	SDL_RenderCopy( gRenderer, m_tex, &GetRect(20*(buttonX-1), 340, 20, 20 ), &GetRect( 80*buttonX, 80, 80, 80 ) );
}