#include "stdafx.h"
#include "graphics.h"
#include "text.h"
#include "ComuniPad.h"
#include "ComPadApps.h"
#include "Overworld.h"
#include "ComPadApps.h"

extern TTF_Font *gFont;
extern SDL_Renderer *gRenderer;
extern bool pressingEnter;

extern OverworldController *m_World;

#define MAX_BUTTONS 6

void ComuniPad::Initialise()
{
	//Load texture:
	SDL_Surface *loadedSurface = IMG_Load( "DATA/GFX/UI/ComuniPad.png" );
	m_tex = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );

	//Get rid of old loaded surface
	SDL_FreeSurface( loadedSurface );

	//Set variables:
	m_iSelection = -1;
	m_iAnimState = -1;
	m_iAnimProgress = 0;

	CursorX = CursorY = 200;

	//Set up defualt apps:
	int i = 0;
	apps[i++] = new CPadPhone();
	apps[i++] = new CBaseApp();
	apps[i++] = new CBaseApp();
	apps[i++] = new CBaseApp();
	apps[i++] = new CPadAppStore();
}

bool ComuniPad::Tick()
{
	//IsInApp.
	if( m_bIsInApp )
	{
		if( DoBtn( m_iSelection ) )
		{
			return true;
		}
		else
			m_bIsInApp = false;
	}

	//Hook controls:
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	if( keystate[SDL_GetScancodeFromKey(SDLK_ESCAPE)] && !pressingEnter && !( m_iAnimState < 0 ) )
	{
		m_iAnimState = -2;
		m_iAnimProgress = 0;
	}

	if( keystate[SDL_GetScancodeFromKey(SDLK_w)] || keystate[SDL_GetScancodeFromKey(SDLK_UP)] )
	{
		CursorY--;
	}
	if( keystate[SDL_GetScancodeFromKey(SDLK_s)] || keystate[SDL_GetScancodeFromKey(SDLK_DOWN)] )
	{
		CursorY++;
	}
	if( keystate[SDL_GetScancodeFromKey(SDLK_a)] || keystate[SDL_GetScancodeFromKey(SDLK_LEFT)] )
	{
		CursorX--;
	}
	if( keystate[SDL_GetScancodeFromKey(SDLK_d)] || keystate[SDL_GetScancodeFromKey(SDLK_RIGHT)] )
	{
		CursorX++;
	}

	SDL_RenderClear( gRenderer );

	m_World->Render();
	if( m_iAnimState == -1 )
	{
		m_iAnimProgress+=6;
	}
	if( m_iAnimState == -2 )
	{
		m_iAnimProgress+=6;

		if( m_iAnimProgress >= 460 )
		{
			m_iAnimProgress = 0;
			m_iAnimState = -1;
			return false;
		}
	}
	if( m_iAnimState == 0)
	{
		if( pressingEnter )
		{
			m_iAnimState = 1;
			pressingEnter = false;
		}
	}
	if( m_iAnimState == 1 )
	{
		m_iAnimProgress+=4;
	}
	if( m_iAnimState == 2 )
	{
		m_iAnimProgress+=4;
	}
	RenderBG();

	if( m_iAnimState == 3 )
	{
		m_iSelection = -1;
		int yOfs = 0;
		int xOfs = 0;
		for( int i = 0 ; i < MAX_BUTTONS; i++ )
		{
			if( CursorX > 80*(xOfs+1) && CursorX < 80*(xOfs+1)+80 )
			{
				if( CursorY > 80*(yOfs+1) && CursorY < 80*(yOfs+1)+80 )
				{
					m_iSelection = i;
				}
			}

			if( i > 5 )
				yOfs++;
			xOfs++;
		}
		//Check stuff:
		if( pressingEnter )
		{
			//Do functions
			if( DoBtn( m_iSelection ) )
			{
				m_bIsInApp = true;
				pressingEnter = false;
				return true;
			}
		}

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
	//BG stuff:
	if( m_iAnimState == -2 )
	{
		//Close the menu in an awesome way:
		SDL_RenderFillRect( gRenderer, &GetRect( 20, 20 + m_iAnimProgress, 560, 440 ) );
		SDL_RenderCopy( gRenderer, m_tex, &GetRect(0, 0, 340, 340 ), &GetRect( 10, 10 + m_iAnimProgress, 580, 460 ) );

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

		SDL_SetRenderDrawColor(gRenderer,0,0,240,255);
		SDL_RenderFillRect( gRenderer, &GetRect( 20, 20 + 420 - m_iAnimProgress%420, 560, 40 ) );
		SDL_SetRenderDrawColor(gRenderer,0,0,220,255);
		SDL_RenderFillRect( gRenderer, &GetRect( 20 + 540 - m_iAnimProgress%540, 20 , 40, 440 ) );

		m_iAnimProgress += 1;

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
	int buttonY = 1;

	int RowNum = 5;

	//Tempory:
	SDL_RenderCopy( gRenderer, m_tex, &GetRect(20*(buttonX-1), 340, 20, 20 ), &GetRect( 80*buttonX, 80*buttonY, 80, 80 ) );
	buttonX++;
	SDL_RenderCopy( gRenderer, m_tex, &GetRect(20*(buttonX-1), 340, 20, 20 ), &GetRect( 80*buttonX, 80*buttonY, 80, 80 ) );
	buttonX++;
	SDL_RenderCopy( gRenderer, m_tex, &GetRect(20*(buttonX-1), 340, 20, 20 ), &GetRect( 80*buttonX, 80*buttonY, 80, 80 ) );
	buttonX++;
	SDL_RenderCopy( gRenderer, m_tex, &GetRect(20*(buttonX-1), 340, 20, 20 ), &GetRect( 80*buttonX, 80*buttonY, 80, 80 ) );
	buttonX++;
	SDL_RenderCopy( gRenderer, m_tex, &GetRect(20*(buttonX-1), 340, 20, 20 ), &GetRect( 80*buttonX, 80*buttonY, 80, 80 ) );
	buttonY++;
	buttonX = 1;
	SDL_RenderCopy( gRenderer, m_tex, &GetRect(20*(RowNum+buttonX-1), 340, 20, 20 ), &GetRect( 80*buttonX, 80*buttonY, 80, 80 ) );

	//Render a cursor.
	//Clamp to screen bounds!
	if( CursorX > 550 )
		CursorX = 550;
	if( CursorX < 50 )
		CursorX = 50;

	if( CursorY > 440 )
		CursorY = 440;
	if( CursorY < 80 )
		CursorY = 80;

	//Draw it!
	SDL_SetRenderDrawColor(gRenderer,255,0,0,255);
	SDL_RenderFillRect( gRenderer, &GetRect( CursorX - 2, CursorY - 10, 4, 20 ) );
	SDL_RenderFillRect( gRenderer, &GetRect( CursorX - 10, CursorY - 2, 20, 4 ) );

	SDL_SetRenderDrawColor(gRenderer,0,0,0,0);
}

bool ComuniPad::DoBtn( int input )
{
	if( apps[input] != NULL )
	{
		apps[input]->cPad = this;
		return apps[input]->Think();
	}
	return false;
};