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

	//Get rid of old loaded surface
	SDL_FreeSurface( tempSurf );

	SDL_Color txtCol2 = { red-200, green-200, blue-200, 255 };
	tempSurf = TTF_RenderText_Solid( mFont, textString.c_str(), txtCol2 );
	texture_2 = SDL_CreateTextureFromSurface( gRenderer, tempSurf );

	//Get rid of old loaded surface
	SDL_FreeSurface( tempSurf );

	m_IFlags = flags;
}

CText::~CText( )
{
	SDL_DestroyTexture( texture );
	SDL_DestroyTexture( texture_2 );
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

extern bool pressingEnter;

//Multi choice:
int OWMultichoice( std::string input, std::string options[], int numOptions, SDL_Renderer *render, OverworldController *overworld, TTF_Font *font )
{
	const char* Text = input.c_str();

	int CharsToCopy = input.size();
	int CharsCopied = 0;
	
	std::string TextToConstuct = "";

	//Text box:
	while( true )
	{
		SDL_RenderClear( render );

		overworld->Render();
		overworld->RenderTxtBox( false );

		if( CharsCopied >= CharsToCopy )
		{
			break;
		}

		TextToConstuct += Text[CharsCopied];
		CharsCopied++;

		CText *txt = new CText( TextToConstuct.c_str(), render, font, 1, 0, 0, 0 );
		txt->Render( &GetRect( 50, 400, 20, 20 ) );

		delete txt;
		SDL_RenderPresent( render );

		SDL_Delay( 20 );
	}

	//Load menu cursor:
	SDL_Surface *loadedSurface = IMG_Load( "DATA/GFX/UI/MenuFrames.png" );
	SDL_Texture *m_texture = SDL_CreateTextureFromSurface( render, loadedSurface );

	//Clear RAM:
	SDL_FreeSurface( loadedSurface );

	//Selection vars:
	int m_iSelection = 1;
	bool Debounce = true;

	int size = numOptions+1;

	while( true )
	{
		SDL_PumpEvents();

		//Todo: Add controls and return values here:
		SDL_RenderClear( render );

		overworld->Render();
		overworld->RenderTxtBox( true, size );

		CText *txt = new CText( TextToConstuct.c_str(), render, font, 1, 0, 0, 0 );
		txt->Render( &GetRect( 50, 400, 20, 20 ) );
		delete txt;

		int ofs = 1;

		for( int i = 0; i < size-1; i++ )
		{
			txt = new CText( options[i].c_str(), render, font, 1, 0, 0, 0 );
			txt->Render( &GetRect( 500, 345 - size*30 + ofs*30, 20, 20 ) );
			delete txt;
			ofs++;
		}

		//Hook controls:
		const Uint8 *keystate = SDL_GetKeyboardState(NULL);

		if( Debounce == false )
		{
			if( keystate[SDL_GetScancodeFromKey(SDLK_s)] || keystate[SDL_GetScancodeFromKey(SDLK_DOWN)] )
			{
				Debounce = true;

				m_iSelection++;
				if( m_iSelection > size-1 )
					m_iSelection = 1;
			}
			else if( keystate[SDL_GetScancodeFromKey(SDLK_w)] || keystate[SDL_GetScancodeFromKey(SDLK_UP)] )
			{
				Debounce = true;

				m_iSelection--;
				if( m_iSelection < 1 )
					m_iSelection = size-1;
			}
			else if( keystate[SDL_GetScancodeFromKey(SDLK_RETURN)] || keystate[SDL_GetScancodeFromKey(SDLK_SPACE)] )
			{
				SDL_DestroyTexture( m_texture );
				return m_iSelection;
			}
		}
		else if( 
			!keystate[SDL_GetScancodeFromKey(SDLK_s)] && 
			!keystate[SDL_GetScancodeFromKey(SDLK_DOWN)] && 
			!keystate[SDL_GetScancodeFromKey(SDLK_w)] && 
			!keystate[SDL_GetScancodeFromKey(SDLK_UP)] &&
			!keystate[SDL_GetScancodeFromKey(SDLK_SPACE)] && 
			!keystate[SDL_GetScancodeFromKey(SDLK_RETURN)])
		{
			Debounce = false;
		}

		//Render that selection arrow:
		SDL_RenderCopy( render, m_texture, &GetRect(0, 24, 8, 9 ), &GetRect( 500-21, 345 - size*30 + ( 30 * m_iSelection ), 20, 30 ) );

		SDL_RenderPresent( render );

		SDL_Delay( 20 );
	}

	SDL_DestroyTexture( m_texture );

	return 0;
}

#include "DialogFrame.h"
int BattleMultichoice( std::string input, std::string options[], int numOptions, SDL_Renderer *render, BattleEngineGraphics *BattleUIGFX, TTF_Font *font )
{
	const char* Text = input.c_str();

	int CharsToCopy = input.size();
	int CharsCopied = 0;
	
	std::string TextToConstuct = "";

	//Text box:
	while( true )
	{
		SDL_RenderClear( render );

		BattleUIGFX->bg->Render();
		BattleUIGFX->menu->Render();

		/*if( renderHP )
		{*/
			BattleUIGFX->hpDisp->Render();
		//}

		BattleUIGFX->RenderPokes();

		if( CharsCopied >= CharsToCopy )
		{
			break;
		}

		TextToConstuct += Text[CharsCopied];
		CharsCopied++;

		CText *txt = new CText( TextToConstuct.c_str(), render, font, 1, 255, 255, 255 );
		txt->Render( &GetRect( 50, 400, 20, 20 ) );

		delete txt;
		SDL_RenderPresent( render );

		SDL_Delay( 20 );
	}

	//Load menu cursor:
	SDL_Surface *loadedSurface = IMG_Load( "DATA/GFX/UI/MenuFrames.png" );
	SDL_Texture *m_texture = SDL_CreateTextureFromSurface( render, loadedSurface );

	//Clear RAM:
	SDL_FreeSurface( loadedSurface );

	//Selection vars:
	int m_iSelection = 1;
	bool Debounce = true;

	int size = numOptions+1;

	while( true )
	{
		SDL_PumpEvents();
		SDL_RenderClear( render );

		BattleUIGFX->bg->Render();
		BattleUIGFX->menu->Render();

		/*if( renderHP )
		{*/
			BattleUIGFX->hpDisp->Render();
		//}

		BattleUIGFX->RenderPokes();

		DialogFrame *frame = new DialogFrame( );
		frame->Render(450, 350 - size*30, 130, 20 + size*30 );
		delete frame;

		CText *txt = new CText( TextToConstuct.c_str(), render, font, 1, 255, 255, 255 );
		txt->Render( &GetRect( 50, 400, 20, 20 ) );
		delete txt;

		int ofs = 1;

		for( int i = 0; i < size-1; i++ )
		{
			txt = new CText( options[i].c_str(), render, font, 1, 0, 0, 0 );
			txt->Render( &GetRect( 500, 345 - size*30 + ofs*30, 20, 20 ) );
			delete txt;
			ofs++;
		}

		//Hook controls:
		const Uint8 *keystate = SDL_GetKeyboardState(NULL);

		if( Debounce == false )
		{
			if( keystate[SDL_GetScancodeFromKey(SDLK_s)] || keystate[SDL_GetScancodeFromKey(SDLK_DOWN)] )
			{
				Debounce = true;

				m_iSelection++;
				if( m_iSelection > size-1 )
					m_iSelection = 1;
			}
			else if( keystate[SDL_GetScancodeFromKey(SDLK_w)] || keystate[SDL_GetScancodeFromKey(SDLK_UP)] )
			{
				Debounce = true;

				m_iSelection--;
				if( m_iSelection < 1 )
					m_iSelection = size-1;
			}
			else if( keystate[SDL_GetScancodeFromKey(SDLK_RETURN)] || keystate[SDL_GetScancodeFromKey(SDLK_SPACE)] )
			{
				SDL_DestroyTexture( m_texture );
				return m_iSelection;
			}
		}
		else if( 
			!keystate[SDL_GetScancodeFromKey(SDLK_s)] && 
			!keystate[SDL_GetScancodeFromKey(SDLK_DOWN)] && 
			!keystate[SDL_GetScancodeFromKey(SDLK_w)] && 
			!keystate[SDL_GetScancodeFromKey(SDLK_UP)] &&
			!keystate[SDL_GetScancodeFromKey(SDLK_SPACE)] && 
			!keystate[SDL_GetScancodeFromKey(SDLK_RETURN)])
		{
			Debounce = false;
		}

		//Render that selection arrow:
		SDL_RenderCopy( render, m_texture, &GetRect(0, 24, 8, 9 ), &GetRect( 500-21, 345 - size*30 + ( 30 * m_iSelection ), 20, 30 ) );

		SDL_RenderPresent( render );

		SDL_Delay( 20 );
	}

	SDL_DestroyTexture( m_texture );

	return 0;
}