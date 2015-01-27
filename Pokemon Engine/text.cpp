#include "stdafx.h"
#include "text.h"
#include "utils.h"
#include "windows.h"

CText::CText( std::string text, SDL_Renderer *renderer, TTF_Font *mFont, int flags, int red, int green, int blue )
{
	texture = NULL;

	textString = text;

	SDL_Color txtCol = { red, green, blue, 255 };
	SDL_Surface *tempSurf = NULL;
	tempSurf = TTF_RenderText_Solid( mFont, textString.c_str(), txtCol );

	gRenderer = renderer;

	texture = SDL_CreateTextureFromSurface( gRenderer, tempSurf );

	SDL_Color txtCol2 = { red-200, green-200, blue-200, 255 };
	tempSurf = TTF_RenderText_Solid( mFont, textString.c_str(), txtCol2 );
	texture_2 = SDL_CreateTextureFromSurface( gRenderer, tempSurf );

	//Get rid of old loaded surface
	SDL_FreeSurface( tempSurf );

	m_IFlags = flags;
}

CText::~CText( )
{

}

void CText::Render( SDL_Rect *rect )
{
	//Draw the texture to the rect!
	if( m_IFlags & TEXTFLAG_AUTOSCALE )
	{
		int texW = 0;
		int texH = 0;
		SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);

		if( m_IFlags & TEXTFLAG_SHADOWED )
		{
			SDL_RenderCopy( gRenderer, texture_2, NULL, &GetRect( rect->x+2, rect->y+2, texW, texH) );
		}

		SDL_RenderCopy( gRenderer, texture, NULL, &GetRect( rect->x, rect->y, texW, texH) );
	}
	else
	{
		SDL_RenderCopy( gRenderer, texture, NULL, rect );
	}
}

void BattleText( std::string input, SDL_Renderer *render, BattleEngineGraphics *BattleUIGFX, TTF_Font *font, bool renderHP )
{
	const char* Text = input.c_str();

	int CharsToCopy = input.size();
	int CharsCopied = 0;
	
	std::string TextToConstuct = "";

	while( true )
	{
		SDL_RenderClear( render );

		BattleUIGFX->bg->Render();
		BattleUIGFX->menu->Render();

		if( renderHP )
		{
			BattleUIGFX->hpDisp->Render();
		}

		BattleUIGFX->RenderPokes();

		if( CharsCopied >= CharsToCopy )
		{
			if( AwaitUserInput() )
				break;

			Sleep( 10 );
			continue;
		}

		TextToConstuct += Text[CharsCopied];
		CharsCopied++;

		CText *txt = new CText( TextToConstuct.c_str(), render, font, 1, 255, 255, 255 );
		txt->Render( &GetRect( 50, 400, 20, 20 ) );
		delete txt;

		SDL_RenderPresent( render );

		Sleep( 20 );
	}
}

void OWText( std::string input, SDL_Renderer *render, OverworldController *overworld, TTF_Font *font )
{
	const char* Text = input.c_str();

	int CharsToCopy = input.size();
	int CharsCopied = 0;
	
	std::string TextToConstuct = "";

	while( true )
	{
		SDL_RenderClear( render );

		overworld->Render();
		overworld->RenderTxtBox();

		if( CharsCopied >= CharsToCopy )
		{
			if( AwaitUserInput() )
				break;

			Sleep( 10 );
			continue;
		}

		TextToConstuct += Text[CharsCopied];
		CharsCopied++;

		CText *txt = new CText( TextToConstuct.c_str(), render, font, 1, 0, 0, 0 );
		txt->Render( &GetRect( 50, 400, 20, 20 ) );

		delete txt;
		SDL_RenderPresent( render );

		Sleep( 20 );
	}
}