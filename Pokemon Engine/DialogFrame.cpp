#include "stdafx.h"
#include "utils.h"
#include "DialogFrame.h"

extern SDL_Renderer *gRenderer;

DialogFrame::DialogFrame()
{
	//Initilise texture!

	//Set up the SDL_Texture!
	SDL_Surface *loadedSurface = IMG_Load( "DATA/GFX/UI/MenuFrames.png" );
	m_texture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );

	//Clear RAM:
	SDL_FreeSurface( loadedSurface );
}

DialogFrame::~DialogFrame()
{
	//Clear texture, save RAM!
	SDL_DestroyTexture( m_texture );
}

void DialogFrame::Render( int StartX, int StartY, int EndX, int EndY )
{
	//Top
	SDL_RenderCopy( gRenderer, m_texture, &GetRect(0, 0, 8, 8 ), &GetRect( StartX, StartY, 8, 8 ) );
	SDL_RenderCopy( gRenderer, m_texture, &GetRect(8, 0, 8, 8 ), &GetRect( StartX + 8, StartY, EndX-8, 8 ) );
	SDL_RenderCopy( gRenderer, m_texture, &GetRect(16, 0, 8, 8 ), &GetRect( StartX + EndX - 8, StartY, 8, 8 ) );

	SDL_RenderCopy( gRenderer, m_texture, &GetRect(0, 8, 8, 8 ), &GetRect( StartX + 0 - 0, StartY+8, 8, EndY - 8 ) );
	SDL_RenderCopy( gRenderer, m_texture, &GetRect(8, 8, 8, 8 ), &GetRect( StartX + 8 - 0, StartY+8, EndX-8, EndY - 8 ) );
	SDL_RenderCopy( gRenderer, m_texture, &GetRect(16, 8, 8, 8 ), &GetRect( StartX + EndX - 8, StartY+8, 8, EndY - 8 ) );

	//Bottom
	SDL_RenderCopy( gRenderer, m_texture, &GetRect(0, 16, 8, 8 ), &GetRect( StartX, StartY+EndY-8, 8, 8 ) );
	SDL_RenderCopy( gRenderer, m_texture, &GetRect(8, 16, 8, 8 ), &GetRect( StartX + 8, StartY+EndY-8, EndX-8, 8 ) );
	SDL_RenderCopy( gRenderer, m_texture, &GetRect(16, 16, 8, 8 ), &GetRect( StartX + EndX - 8, StartY+EndY-8, 8, 8 ) );
}