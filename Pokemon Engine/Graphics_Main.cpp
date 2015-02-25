#include "stdafx.h"
#include "utils.h"
#include "text.h"
#include "pokemon.h"
#include "graphics.h"
#include "include\SDL_mixer.h"

SDL_Window *screen = NULL;
SDL_Renderer *gRenderer = NULL;

extern BattleEngineGraphics *BattleUIGFX;

TTF_Font *gFont = NULL;

BattleBackGround::BattleBackGround()
{
	mTexture = NULL;
}
BattleBackGround::~BattleBackGround()
{
	free();
}

void BattleBackGround::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
	}
}

bool BattleBackGround::LoadFromFile( std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

void BattleBackGround::Render()
{
	//Render to screen
	SDL_RenderCopy( gRenderer, mTexture, &GetRect( 260, 148, 255, 143 ), &GetRect( 0, 0, 600, 350 ) );
}

void initSDL()
{
	//Initialize SDL
	if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		screen = SDL_CreateWindow( "Pokemon Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600, 480, SDL_WINDOW_SHOWN );
		if( screen == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer( screen, -1, SDL_RENDERER_ACCELERATED );
			SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "nearest"); 

			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0, 0, 0, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
				}
				//Initialize SDL_ttf
                if( TTF_Init() == -1 )
                {
                    printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
                }

				gFont = TTF_OpenFont( "DATA/font/Pkmn.ttf", 28 );
			}

			// init SDL_mixer
			Mix_Init( MIX_INIT_OGG );
			if( Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) )
			{
				fprintf(stderr, "SDL_Mixer Error: %s\n", Mix_GetError());
			}

		}
	}
}

SDL_Window* GetScreen()
{
	return screen;
}


BattleMenu::BattleMenu()
{
	mTexture_1 = NULL;
	mTexture_2 = NULL;
	mTexture_3 = NULL;
}
BattleMenu::~BattleMenu()
{
	free();
}

void BattleMenu::free()
{
	//Free texture if it exists
	if( mTexture_1 != NULL )
	{
		SDL_DestroyTexture( mTexture_1 );
		mTexture_1 = NULL;
	}
	if( mTexture_2 != NULL )
	{
		SDL_DestroyTexture( mTexture_2 );
		mTexture_2 = NULL;
	}
	if( mTexture_3 != NULL )
	{
		SDL_DestroyTexture( mTexture_3 );
		mTexture_3 = NULL;
	}
}

bool BattleMenu::LoadFromFile( std::string path, int texNum )
{
	//Get rid of preexisting texture
	//free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	switch( texNum )
	{
		case 1: mTexture_1 = newTexture; break;
		case 2: mTexture_2 = newTexture; break;
		case 3: mTexture_3 = newTexture; break;
	}
	return true;
}

void BattleMenu::Render()
{
	//Text box
	if( subMenu == -1 )
	{
		//Greenish bar /w text
		SDL_RenderCopy( gRenderer, mTexture_1, &GetRect( 297, 56, 240, 48 ), &GetRect( 0, 350, 600, 480-350 ) );

		//Do text stuff here!
	}

	//"FIGHT BAG POKEMON RUN" options
	if( subMenu == 0 )
	{
		//Greenish bar /w text
		SDL_RenderCopy( gRenderer, mTexture_1, &GetRect( 297, 56, 240, 48 ), &GetRect( 0, 350, 600, 480-350 ) );

		//"FIGHT BAG POKEMON RUN" options
		SDL_RenderCopy( gRenderer, mTexture_1, &GetRect( 146, 4, 120, 48 ), &GetRect( 300, 350, 300, 480-350 ) );

		std::string WhatWillxDo = "What will " + Poke->m_sPkmName + " do?";
		CText *txt = new CText( WhatWillxDo.c_str(), gRenderer, gFont, 1, 255, 255, 255 );
		txt->Render( &GetRect( 40, 400, 20, 20 ) );
		delete txt;
	
		//Cursor requires some hax, I guess...
		switch( cursorPos )
		{
		case 1:SDL_RenderCopy( gRenderer, mTexture_3, &GetRect( 269, 4, 6, 10 ), &GetRect( 320, 385, 10, 20 ) ); break;
		case 2:SDL_RenderCopy( gRenderer, mTexture_3, &GetRect( 269, 4, 6, 10 ), &GetRect( 465, 385, 10, 20 ) ); break;
		case 3:SDL_RenderCopy( gRenderer, mTexture_3, &GetRect( 269, 4, 6, 10 ), &GetRect( 320, 430, 10, 20 ) ); break;
		case 4:SDL_RenderCopy( gRenderer, mTexture_3, &GetRect( 269, 4, 6, 10 ), &GetRect( 465, 430, 10, 20 ) ); break;
		}
	}
	if( subMenu == 1 ) //Fight options:
	{
		SDL_RenderCopy( gRenderer, mTexture_1, &GetRect( 297, 4, 240, 48 ), &GetRect( 0, 350, 600, 480-350 ) );

		//Render move text!
		SDL_Rect *rects[4] = { &GetRect( 40, 385, 0, 0 ), &GetRect( 220, 385, 0, 0 ), &GetRect( 40, 430, 0, 0 ), &GetRect( 220, 430, 0, 0 ) };
		for( int i = 0; i < 4; i++ )
		{
			CText *txt = new CText( Poke->pAttacks[i]->GetName(), gRenderer, gFont , 1 );
			txt->Render( rects[i] );
			delete txt;
		}
		//Dont even try for NULL moves...
		if( Poke->pAttacks[cursorPos-1]->GetID() != 0 )
		{
			int moveType = Poke->pAttacks[cursorPos-1]->GetType();
			int PP, maxPP;
			maxPP = Poke->pAttacks[cursorPos-1]->GetPP( true );
			PP = Poke->pAttacks[cursorPos-1]->GetPP();

			//Render PP/MAXPP
			std::string str;
			str = std::to_string( (_ULonglong)maxPP );

			CText *txt = new CText( str, gRenderer, gFont , 1 );
			txt->Render( &GetRect( 555, 383, 0, 0 ) );
			delete txt;

			str = std::to_string( (_ULonglong)PP );

			txt = new CText( str, gRenderer, gFont , 1 );
			txt->Render( &GetRect( 520 - (str.length()*5), 383, 0, 0 ) );
			delete txt;

			//Render move type!
			moveType--;
			SDL_RenderCopy( gRenderer, mTexture_2, &GetRect( 0, 14*moveType, 32, 14 ), &GetRect( 488, 427, 64, 28 ) );
		}

		switch( cursorPos )
		{
		case 1:SDL_RenderCopy( gRenderer, mTexture_3, &GetRect( 269, 4, 6, 10 ), &GetRect( 20, 385, 10, 20 ) ); break;
		case 2:SDL_RenderCopy( gRenderer, mTexture_3, &GetRect( 269, 4, 6, 10 ), &GetRect( 200, 385, 10, 20 ) ); break;
		case 3:SDL_RenderCopy( gRenderer, mTexture_3, &GetRect( 269, 4, 6, 10 ), &GetRect( 20, 430, 10, 20 ) ); break;
		case 4:SDL_RenderCopy( gRenderer, mTexture_3, &GetRect( 269, 4, 6, 10 ), &GetRect( 200, 430, 10, 20 ) ); break;
		}
	}
}

HPDisplays::HPDisplays()
{
	mTexture = NULL;
	mFont = NULL;
	mFont = TTF_OpenFont( "DATA/Font/Pkmn.ttf", 28 );
}
HPDisplays::~HPDisplays()
{
	free();
}

void HPDisplays::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
	}
}

bool HPDisplays::LoadFromFile( std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0xFF, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

void HPDisplays::Render( )
{
	//Render to screen

	SDL_Color txtCol = { 0, 0, 0, 255 };
	SDL_Surface* textSurface = NULL;

	SDL_Texture* tex = NULL;
	int textXSize = 10;

	if( pmkn1->GetActive() )
	{
		//GUI background
		SDL_RenderCopy( gRenderer, mTexture, &GetRect( 2, 3, 101, 29 ), &GetRect( 20, 20, 280, 100 ) );
		
		//Pokemon Names:

		//Name 1:
		CText *txt = new CText( pmkn1->m_sPkmName.c_str(), gRenderer, mFont, 1 );
		txt->Render( &GetRect( 50, 40, 100, 20 ) );
		delete txt;

		//Load up pkmn levels!
		//Init Level1
		textXSize = 10;
		if( pmkn1->m_iLevel > 9 )
			textXSize = 20;
		if( pmkn1->m_iLevel > 99 )
			textXSize = 30;

		textSurface = TTF_RenderText_Solid( mFont, std::to_string( (_ULonglong)pmkn1->m_iLevel).c_str(), txtCol );
		tex = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		//Render Level1
		SDL_RenderCopy( gRenderer, tex, NULL, &GetRect( 230, 42, textXSize, 20 ) );

	}
	if( pmkn2->GetActive() )
	{
		//GUI background
		SDL_RenderCopy( gRenderer, mTexture, &GetRect( 3, 44, 104, 37 ), &GetRect( 320, 250, 280, 100 ) );

		//Pokemon Names:

		//Name 2:
		CText *txt = new CText( pmkn2->m_sPkmName.c_str(), gRenderer, mFont, 1 );
		txt->Render( &GetRect( 365, 265, 100, 20 ) );
		delete txt;

		//Init Level2
		textXSize = 10;
		if( pmkn2->m_iLevel > 9 )
			textXSize = 20;
		if( pmkn2->m_iLevel > 99 )
			textXSize = 30;

		textSurface = TTF_RenderText_Solid( mFont, std::to_string( (_ULonglong)pmkn2->m_iLevel).c_str(), txtCol );
		tex = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		//Render Level2
		SDL_RenderCopy( gRenderer, tex, NULL, &GetRect( 540, 265, textXSize, 20 ) );
	}


	//Free memory
	SDL_FreeSurface( textSurface );

	//Create the HP bars
	//Enemy
	int TargetHPBarWidth;
	float Hp, MaxHp;
	Hp = m_iBattler2_Health;
	MaxHp = m_iBattler2_MaxHealth;

	int colourBar1Red, colourBar1Green, colourBar1Blue;
	int colourBar2Red, colourBar2Green, colourBar2Blue;

	if( Hp >= (MaxHp/3)*2 )
	{
		colourBar1Red = 122; colourBar1Green = 248, colourBar1Blue = 168;
		colourBar2Red = 88; colourBar2Green = 208, colourBar2Blue = 128;
	}
	if( Hp <= (MaxHp/3)*2 )
	{
		colourBar1Red = 248; colourBar1Green = 224, colourBar1Blue = 56;
		colourBar2Red = 200; colourBar2Green = 168, colourBar2Blue = 8;
	}
	if( Hp <= (MaxHp/3) )
	{
		colourBar1Red = 248; colourBar1Green = 88, colourBar1Blue = 56;
		colourBar2Red = 168; colourBar2Green = 64, colourBar2Blue = 72;
	}

	TargetHPBarWidth = (Hp/MaxHp) * 133;

	if( TargetHPBarWidth > 0 )
	{
		SDL_SetRenderDrawColor( gRenderer, colourBar1Red, colourBar1Green, colourBar1Blue, 255);
		SDL_RenderFillRect( gRenderer, &GetRect( 131, 82, TargetHPBarWidth, 7 ) );
		SDL_SetRenderDrawColor( gRenderer, colourBar2Red, colourBar2Green, colourBar2Blue, 255);
		SDL_RenderFillRect( gRenderer, &GetRect( 131, 78, TargetHPBarWidth, 4 ) );
	}

	//Player
	Hp = m_iBattler1_Health;
	MaxHp = m_iBattler1_MaxHealth;

	TargetHPBarWidth = (Hp/MaxHp) * 130;

	if( Hp > (MaxHp/3)*2 )
	{
		colourBar1Red = 122; colourBar1Green = 248, colourBar1Blue = 168;
		colourBar2Red = 88; colourBar2Green = 208, colourBar2Blue = 128;
	}
	if( Hp < (MaxHp/3)*2 )
	{
		colourBar1Red = 248; colourBar1Green = 224, colourBar1Blue = 56;
		colourBar2Red = 200; colourBar2Green = 168, colourBar2Blue = 8;
	}
	if( Hp < (MaxHp/3) )
	{
		colourBar1Red = 248; colourBar1Green = 88, colourBar1Blue = 56;
		colourBar2Red = 168; colourBar2Green = 64, colourBar2Blue = 72;
	}

	SDL_SetRenderDrawColor( gRenderer, colourBar1Red, colourBar1Green, colourBar1Blue, 255);
	SDL_RenderFillRect( gRenderer, &GetRect( 449, 297, TargetHPBarWidth, 7 ) );
	SDL_SetRenderDrawColor( gRenderer, colourBar2Red, colourBar2Green, colourBar2Blue, 255);
	SDL_RenderFillRect( gRenderer, &GetRect( 449, 295, TargetHPBarWidth, 4 ) );


	//Create the EXP bar:
	float ExpToNextLevel = pmkn2->GetNextLevelEXP();
	float Exp = pmkn2->GetEXP();
	float DeltaEXP = ( ExpToNextLevel - Exp );
	float DeltaExp2 = ExpToNextLevel - ((4 * pow( (float)(pmkn2->m_iLevel), 3) ))/5;

	TargetHPBarWidth = DeltaEXP/DeltaExp2 * 174;

	SDL_SetRenderDrawColor( gRenderer, 64, 200, 248, 255);
	SDL_RenderFillRect( gRenderer, &GetRect( 405, 339, 174 - TargetHPBarWidth, 6 ) );
}

//Main battle renderer:
void BattleEngineGraphics::RenderPokes()
{
		BattlePkmn[0]->Render(gRender);
		BattlePkmn[1]->Render(gRender);
}