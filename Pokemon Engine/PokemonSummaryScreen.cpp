#include "stdafx.h"
#include "partymenu.h"
#include "PokemonSummaryScreen.h"
#include "text.h"

extern SDL_Renderer *gRenderer;
extern TTF_Font *gFont;
extern int battleScene;
extern PokemonPartyScene *m_party;

extern bool pressingEnter;

#define MAX_SUMMARY_SLIDES 3 //Temp!

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

	slide = 1;
	animState = 0;
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

	//Hook controls:
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);

	//Check key states:
	if( Debounce == false )
	{
		if( keystate[SDL_GetScancodeFromKey(SDLK_a)] || keystate[SDL_GetScancodeFromKey(SDLK_LEFT)] )
		{
			slide--;
			if( slide < 1 )
				slide = 1;
			Debounce = true;
		}
		else if( keystate[SDL_GetScancodeFromKey(SDLK_d)] || keystate[SDL_GetScancodeFromKey(SDLK_RIGHT)] )
		{
			slide++;
			if( slide > MAX_SUMMARY_SLIDES ) //Temp!
				slide = MAX_SUMMARY_SLIDES;
			Debounce = true;
		}
		if( keystate[SDL_GetScancodeFromKey(SDLK_w)] || keystate[SDL_GetScancodeFromKey(SDLK_UP)] )
		{
			m_iSelection--;

			if( m_Player->m_pkmParty[m_iSelection] == NULL )
			{
				for( int i = 0; i < 6; i++ )
				{
					if( m_Player->m_pkmParty[i] != NULL )
					{
						m_iSelection = i;
					}
				}
			}

			if( m_iSelection < 0 )
			{
				for( int i = 0; i < 6; i++ )
				{
					if( m_Player->m_pkmParty[i] != NULL )
					{
						m_iSelection = i;
					}
				}
			}

			Debounce = true;
		}
		if( keystate[SDL_GetScancodeFromKey(SDLK_s)] || keystate[SDL_GetScancodeFromKey(SDLK_DOWN)] )
		{
			m_iSelection++;
			if( m_Player->m_pkmParty[m_iSelection] == NULL )
			{
				m_iSelection = 0;
			}
			if( m_iSelection > 5 )
				m_iSelection = 0;

			Debounce = true;
		}
	}
	else if( !keystate[SDL_GetScancodeFromKey(SDLK_a)] && !keystate[SDL_GetScancodeFromKey(SDLK_LEFT)] && !keystate[SDL_GetScancodeFromKey(SDLK_d)] && !keystate[SDL_GetScancodeFromKey(SDLK_RIGHT)] &&
		 !keystate[SDL_GetScancodeFromKey(SDLK_w)] && !keystate[SDL_GetScancodeFromKey(SDLK_UP)] && !keystate[SDL_GetScancodeFromKey(SDLK_s)] && !keystate[SDL_GetScancodeFromKey(SDLK_DOWN)])
	{
		Debounce = false;
	}

	SDL_RenderClear( gRenderer );

	switch( slide )
	{
	case 1: RenderSlide1(); break;
	case 2: RenderSlide2(); break;
	case 3: RenderSlide3(); break;
	}

	SDL_RenderPresent( gRenderer );

	return true;
}

void PokemonSummaryScene::RenderSlide1()
{
	SDL_RenderCopy( gRenderer, m_Texture, &GetRect( 0, 0, 240, 160 ), NULL );

	//Window name:
	CText *txt = new CText( "Pokemon Info", gRenderer, gFont, 3, 255, 255, 255 );
	txt->Render( &GetRect( 5, 10, 0, 0 ));
	delete txt;

	if( m_Player->m_pkmParty[ m_iSelection ] != NULL )
	{
		SharedPokeRender();

		std::string str = "";

		//OT:
		txt = new CText( "OT/", gRenderer, gFont, 3, 255, 255, 255 );
		txt->Render( &GetRect( 220, 110, 0, 0 ) );
		delete txt;
		txt = new CText( "PLAYER_NAME", gRenderer, gFont, 1, 0, 100, 255 ); //Todo: Add an original Trainer function!
		txt->Render( &GetRect( 260, 110, 0, 0 ) );
		delete txt;
		str = "IDNo";
		str += m_Player->m_pkmParty[ m_iSelection ]->m_sOT; //Todo: Add trainer ids assigned on startup!

		//Lots of text...
		txt = new CText( str, gRenderer, gFont, 3, 255, 255, 255 );
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
	SDL_RenderCopy( gRenderer, m_Texture, &GetRect( 0, 160, 240, 160 ), NULL );

	//Window name:
	CText *txt = new CText( "Pokemon Skills", gRenderer, gFont, 3, 255, 255, 255 );
	txt->Render( &GetRect( 5, 10, 0, 0 ));
	delete txt;

	if( m_Player->m_pkmParty[ m_iSelection ] != NULL )
	{
		SharedPokeRender();

		std::string str = "";

		//Item:
		//Todo: Implement items

		//Ribbons:
		//Todo: Implement Ribbons

		//Stats:
		//HP:
		txt = new CText( "HP", gRenderer, gFont, 3, 255, 255, 255 );
		txt->Render( &GetRect( 255, 180, 0, 0 ));
		delete txt;

		str += std::to_string( (_ULonglong)m_Player->m_pkmParty[ m_iSelection ]->GetHealth());
		str += "/";
		str += std::to_string( (_ULonglong)m_Player->m_pkmParty[ m_iSelection ]->GetStat( "hp" ));

		txt = new CText( str, gRenderer, gFont, 1, 0, 0, 0 );
		txt->Render( &GetRect( 345, 180, 0, 0 ));
		delete txt;

		str = "";

		//Attack:
		txt = new CText( "ATTACK", gRenderer, gFont, 3, 255, 255, 255 );
		txt->Render( &GetRect( 235, 220, 0, 0 ));
		delete txt;
		str += std::to_string( (_ULonglong)m_Player->m_pkmParty[ m_iSelection ]->GetStat( "attack" ));
		txt = new CText( str, gRenderer, gFont, 1, 0, 0, 0 );
		txt->Render( &GetRect( 350, 220, 0, 0 ));
		delete txt;
		str = "";

		//Defense:
		txt = new CText( "DEFENSE", gRenderer, gFont, 3, 255, 255, 255 );
		txt->Render( &GetRect( 230, 260, 0, 0 ));
		delete txt;
		str += std::to_string( (_ULonglong)m_Player->m_pkmParty[ m_iSelection ]->GetStat( "defence" ));
		txt = new CText( str, gRenderer, gFont, 1, 0, 0, 0 );
		txt->Render( &GetRect( 350, 260, 0, 0 ));
		delete txt;
		str = "";

		//SP Attack:
		txt = new CText( "SP.ATK", gRenderer, gFont, 3, 255, 255, 255 );
		txt->Render( &GetRect( 450, 180, 0, 0 ));
		delete txt;
		str += std::to_string( (_ULonglong)m_Player->m_pkmParty[ m_iSelection ]->GetStat( "special attack" ));
		txt = new CText( str, gRenderer, gFont, 1, 0, 0, 0 );
		txt->Render( &GetRect( 550, 180, 0, 0 ));
		delete txt;
		str = "";

		//SP Defense:
		txt = new CText( "SP.DEF", gRenderer, gFont, 3, 255, 255, 255 );
		txt->Render( &GetRect( 450, 220, 0, 0 ));
		delete txt;
		str += std::to_string( (_ULonglong)m_Player->m_pkmParty[ m_iSelection ]->GetStat( "special defence" ));
		txt = new CText( str, gRenderer, gFont, 1, 0, 0, 0 );
		txt->Render( &GetRect( 550, 220, 0, 0 ));
		delete txt;
		str = "";

		//Speed:
		txt = new CText( "SPEED", gRenderer, gFont, 3, 255, 255, 255 );
		txt->Render( &GetRect( 455, 260, 0, 0 ));
		delete txt;
		str += std::to_string( (_ULonglong)m_Player->m_pkmParty[ m_iSelection ]->GetStat( "speed" ));
		txt = new CText( str, gRenderer, gFont, 1, 0, 0, 0 );
		txt->Render( &GetRect( 550, 260, 0, 0 ));
		delete txt;
		str = "";

		//EXP info:

		int EXP = m_Player->m_pkmParty[ m_iSelection ]->GetEXP();
		int EXP_NextLvl = m_Player->m_pkmParty[ m_iSelection ]->GetNextLevelEXP();
		//EXP. Points:
		txt = new CText( "EXP. Points", gRenderer, gFont, 3, 255, 255, 255 );
		txt->Render( &GetRect( 230, 360, 0, 0 ));
		delete txt;

		str += std::to_string( (_ULonglong)m_Player->m_pkmParty[ m_iSelection ]->GetEXP() );
		txt = new CText( str, gRenderer, gFont, 1, 0, 0, 0 );
		txt->Render( &GetRect( 450, 360, 0, 0 ));
		delete txt;
		str = "";

		//Next level:
		txt = new CText( "Next LV.", gRenderer, gFont, 3, 255, 255, 255 );
		txt->Render( &GetRect( 230, 390, 0, 0 ));
		delete txt;
		int ExpToNextLvl = EXP_NextLvl - EXP;
		str += std::to_string( (_ULonglong)( ExpToNextLvl ) );
		txt = new CText( str, gRenderer, gFont, 1, 0, 0, 0 );
		txt->Render( &GetRect( 450, 390, 0, 0 ));
		delete txt;
		str = "";
	}
}

void PokemonSummaryScene::RenderSlide3()
{
	SDL_RenderCopy( gRenderer, m_Texture, &GetRect( 0, 160*2, 240, 160 ), NULL ); //Lazy...

	//Window name:
	CText *txt = new CText( "Battle Moves", gRenderer, gFont, 3, 255, 255, 255 );
	txt->Render( &GetRect( 5, 10, 0, 0 ));
	delete txt;

	if( m_Player->m_pkmParty[ m_iSelection ] != NULL )
	{
		SharedPokeRender();

		std::string str = "";

		CText *txt;

		//Render a list of moves:
		for( int i = 0; i < 4; i++ )
		{
			//Render type icon:
			int moveType = m_Player->m_pkmParty[ m_iSelection ]->GetAttack( i )->GetType();
			moveType--;
			SDL_RenderCopy( gRenderer, m_TypeTexture, &GetRect( 0, 14*moveType, 32, 14 ), &GetRect( 212, 115 + (i * 40), 80, 30 ) );

			str = m_Player->m_pkmParty[ m_iSelection ]->GetAttack( i )->GetName();
			txt = new CText( str, gRenderer, gFont, 3, 255, 255, 255 );

			txt->Render( &GetRect( 300, 120 + (i * 40) , 0, 0 ) );
			delete txt;
		}
	}
}


void PokemonSummaryScene::SharedPokeRender()
{
	CText *txt = NULL;

	SDL_Texture *texToRender = m_Player->m_pkmParty[ m_iSelection ]->GetTexture();
	SDL_RenderCopy( gRenderer, texToRender, NULL, &GetRect( 20, 110, 160, 160 ) );

	//Number
	std::string str = "No";
	if( m_Player->m_pkmParty[ m_iSelection ]->m_iSpecies < 100 )
		str+='0';
	if( m_Player->m_pkmParty[ m_iSelection ]->m_iSpecies < 10 )
		str+='0';
	str += std::to_string( (_ULonglong)m_Player->m_pkmParty[ m_iSelection ]->m_iSpecies );
	txt = new CText( str, gRenderer, gFont, 3, 255, 255, 255 );
	txt->Render( &GetRect( 20, 60, 0, 0 ) );
	delete txt;

	//Name:
	str = "";
	str += m_Player->m_pkmParty[ m_iSelection ]->m_sPkmName;
	txt = new CText( str, gRenderer, gFont, 3, 255, 255, 255 );
	txt->Render( &GetRect( 20, 300, 0, 0 ) );
	delete txt;

	str = "/";
	str += m_Player->m_pkmParty[ m_iSelection ]->m_sPkmName; //Fixme: This will break with nicknames!
	txt = new CText( str, gRenderer, gFont, 3, 255, 255, 255 );
	txt->Render( &GetRect( 20, 300 + 30, 0, 0 ) );
	delete txt;
}