#include "stdafx.h"
#include "partymenu.h"
#include "PokemonSummaryScreen.h"
#include "text.h"

extern SDL_Renderer *gRenderer;
extern TTF_Font *gFont;
extern int battleScene;
extern PokemonPartyScene *m_party;

extern bool pressingEnter;

void PokemonSummaryScene::Initialise( Player *player )
{
	m_Player = player;

	//Textures:
	SDL_Surface* loadedSurface = IMG_Load( "DATA/GFX/UI/GBASummary.png" );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", "DATA/GFX/UI/GBASummary.png", IMG_GetError() );
	}

	m_Texture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );

	loadedSurface = IMG_Load( "DATA/GFX/UI/PkmnTypes.png" );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", "DATA/GFX/UI/GBASummary.png", IMG_GetError() );
	}

	m_TypeTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );

	//Get rid of old loaded surface
	SDL_FreeSurface( loadedSurface );
}

bool PokemonSummaryScene::Tick()
{
	//Main handler:
	if (SDL_PollEvent(&events))
	{
		if (events.type == SDL_QUIT)
		{
			return false;
		}
		if (events.type == SDL_KEYDOWN)
		{
			//Esc key!
			if( events.key.keysym.sym == SDLK_ESCAPE && !pressingEnter )
			{
				m_iSelection = 0; //Reset for next call
				battleScene = SCENE_PARTY;
			}
		}
	}

	SDL_RenderClear( gRenderer );

	RenderSlide1();

	SDL_RenderPresent( gRenderer );

	return true;
}

void PokemonSummaryScene::RenderSlide1()
{
	SDL_RenderCopy( gRenderer, m_Texture, &GetRect( 592, 8, 240, 160 ), NULL );

	//Window name:
	CText *txt = new CText( "Pokemon Info", gRenderer, gFont, 1, 255, 255, 255 );
	txt->Render( &GetRect( 5, 10, 0, 0 ));
	delete txt;

	if( m_Player->m_pkmParty[ m_iSelection ] != NULL )
	{
		SDL_Texture *texToRender = m_Player->m_pkmParty[ m_iSelection ]->GetTexture();
		SDL_RenderCopy( gRenderer, texToRender, NULL, &GetRect( 20, 110, 160, 160 ) );

		std::string str = "No";
		if( m_Player->m_pkmParty[ m_iSelection ]->m_iSpecies < 100 )
			str+='0';
		if( m_Player->m_pkmParty[ m_iSelection ]->m_iSpecies < 10 )
			str+='0';
		str += std::to_string( (_ULonglong)m_Player->m_pkmParty[ m_iSelection ]->m_iSpecies );
		txt = new CText( str, gRenderer, gFont, 1, 255, 255, 255 );
		txt->Render( &GetRect( 20, 60, 0, 0 ) );
		delete txt;
		//OT:
		txt = new CText( "OT/", gRenderer, gFont, 1, 255, 255, 255 );
		txt->Render( &GetRect( 220, 110, 0, 0 ) );
		delete txt;
		txt = new CText( "PLAYER_NAME", gRenderer, gFont, 1, 0, 100, 255 ); //Todo: Add an original Trainer function!
		txt->Render( &GetRect( 260, 110, 0, 0 ) );
		delete txt;
		str = "IDNo";
		str += "13370";

		//Lots of text...
		txt = new CText( str, gRenderer, gFont, 1, 255, 255, 255 );
		txt->Render( &GetRect( 440, 110, 0, 0 ) );
		delete txt;
		//Type:
		txt = new CText( "TYPE/", gRenderer, gFont, 1 );
		txt->Render( &GetRect( 220, 150, 0, 0 ) );
		delete txt;

		//Render type icons:
		int iType = m_Player->m_pkmParty[ m_iSelection ]->GetType( 0 );
		iType--;
		SDL_RenderCopy( gRenderer, m_TypeTexture, &GetRect( 0, 14*iType, 32, 14 ), &GetRect( 290, 150, 64, 28 ) );
		iType = m_Player->m_pkmParty[ m_iSelection ]->GetType( 1 );
		if( iType != -1 )
		{
			iType--;
			SDL_RenderCopy( gRenderer, m_TypeTexture, &GetRect( 0, 14*iType, 32, 14 ), &GetRect( 290 + 64 + 10, 150, 64, 28 ) );
		}
	}
}

void PokemonSummaryScene::RenderSlide2()
{

}