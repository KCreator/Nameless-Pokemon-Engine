#include "stdafx.h"
#include "pokemon.h"
#include "SDL_image.h"
#include <math.h>
#include "Windows.h"
#include "text.h"
#include "particle.h" // Temp
#include "PokemonBattle.h"

extern SDL_Renderer *gRenderer;
extern BattleEngineGraphics *BattleUIGFX;
extern TTF_Font *gFont;
extern PokemonBattle *m_Battle;
extern int battleScene;

void Pokemon::Init( int species, ivs iv, evs ev, int level )
{
	m_iLevel = level;
	m_iSpecies = species;

	GetBaseStats();

	pkmIVs = iv;
	pkmEVs = ev;

	m_iHealth = GetStat( "hp" );

	//Moveset needs to be defined, I guess;
	pAttacks[0] = new Move( 1 );
	pAttacks[1] = new Move( 33 );
	pAttacks[2] = new Move( 0 );
	pAttacks[3] = new Move( 0 );

	LoadSprite();

	m_sPkmName = GetName();

	m_bIsFainting = false;
	m_bIsFainted = false;

	//Assume fast for now.... :/
	m_iExp = (4 * pow( (float)level, 3 ) )/5;

	//Assume 100 for now :/
	m_iExpYeild = 100;
}

std::string Pokemon::GetName()
{
	std::string str = "DATA/pkm/";
	str += std::to_string( (_ULonglong)m_iSpecies );
	str += ".txt";
	FILE *pkmData = fopen( str.c_str() , "r" );

	if( pkmData == NULL )
	{
		return "MISSINGNO";
	}

	long lSize;
	// obtain file size:
	fseek (pkmData , 0 , SEEK_END);
	lSize = ftell(pkmData);
	rewind(pkmData);

	int buffpos = 0;
	int buffEOF;
	char * buffer;

	buffer = (char*) malloc (sizeof(char)*lSize);
    if (buffer == NULL)
	{
		return "MISSINGNO";
	}

	buffEOF = fread( buffer, 1, lSize, pkmData);

	std::string nameStr;

	while( buffer[buffpos] != NULL )
	{
		if( buffer[buffpos] == '\n' )
		{
			break;
		}
		else
		{
			nameStr += buffer[buffpos];
		}
		buffpos++;
	}

	return nameStr;
}

void Pokemon::Attack( Pokemon *target, int move, bool ai )
{
	if( m_bIsFainted == true )
		return;

	if( ai )
	{
		int numInval = 0;
		for( int i = 0; i < 3; i++ )
		{
			if( pAttacks[i]->GetID() == 0 )
				numInval++;
		}
		if( numInval == 4 )
		{
			return;
		}

		while( pAttacks[move]->GetID() == 0 )
		{
			move = rand()%4;
		}
	}

	//Todo: status moves, STAB and other damage bonuses.

	//Calculate damage:
	//Raw damage:
	float damage, typeEffectiveness;

	typeEffectiveness = pAttacks[move]->moveType.TestFor( target->types[0] );
	typeEffectiveness *= pAttacks[move]->moveType.TestFor( target->types[1] );

	std::string AttackStat, DefenceStat;

	switch( pAttacks[move]->m_iPhySpeStat )
	{
	case 1: AttackStat = "attack"; DefenceStat = "defence"; break;
	case 2: AttackStat = "special attack"; DefenceStat = "special defence"; break;
	}

	damage = (((( 2 * m_iLevel / 5 + 2) * (GetStat(AttackStat)) * pAttacks[move]->GetBP() / target->GetStat(DefenceStat) ) / 50 ) + 2 );

	damage *= typeEffectiveness;

	floor( damage );

	//Animations:
	BattleUIGFX->menu->subMenu = -1;

 	std::string TextDisplay = m_sPkmName + " used " + pAttacks[move]->GetName() +"!";

	BattleText( TextDisplay, gRenderer, BattleUIGFX, gFont );

	pAttacks[move]->PlayAnimation( this, target );

	target->IncrementHealth( -damage, true, this, BattleUIGFX->hpDisp );

	if( typeEffectiveness > 1 )
	{
		BattleText( "Its super effective!", gRenderer, BattleUIGFX, gFont );
	}
	if( typeEffectiveness < 1 )
	{
		BattleText( "Its not very effective!", gRenderer, BattleUIGFX, gFont );
	}

	if( target->CheckFainted() )
	{
		//Gain some EXP!
		if( m_iLevel < 100 ) //Todo: Make level cap arbitary!
		{
			int expGain = 0;
			if( side == 0 )
			{
				//Todo: finish forumula
				expGain = 1 * 1 * target->m_iExpYeild * target->m_iLevel;
				expGain = expGain / 7;

				std::string str = m_sPkmName + " gained " + std::to_string( (_ULonglong)expGain ) + " EXP!";
				BattleText( str, gRenderer, BattleUIGFX, gFont );

				//Do the EXP BAR animation:
				int TargetEXP = m_iExp + expGain;
				while( m_iExp < TargetEXP )
				{
					m_iExp = (int)lerp( m_iExp, TargetEXP, 1 );
					BattleUIGFX->hpDisp->UpdateHP( target->GetHealth(), m_iHealth, target->GetStat( "hp" ), GetStat( "hp" ) );
					BattleUIGFX->hpDisp->Render();
					SDL_RenderPresent( gRenderer );


					if( m_iExp >= ((4 * pow( (float)(m_iLevel + 1), 3) ))/5 )
					{
						m_iLevel++;
						str = m_sPkmName + " grew to level " + std::to_string( (_ULonglong)m_iLevel );
						BattleText( str, gRenderer, BattleUIGFX, gFont );
					}
					Sleep( 20 );
				}
			}

			if( m_Battle->IsWild() )
			{
				//Since its a wild battle, just end it here.
				FadeToBlack( );
				BattleUIGFX->menu->cursorPos = 1;
				BattleUIGFX->menu->subMenu = 0;
				battleScene = SCENE_OVERWORLD;

				//Todo: Evolution?
			}
		}
	}
}

void Pokemon::GetBaseStats()
{
	//Temp: Assume NULL base stats
	baseStats.hp = 0;
	baseStats.atk = 0;
	baseStats.def = 0;
	baseStats.spatk = 0;
	baseStats.spdef = 0;
	baseStats.speed = 0;

	std::string str = "DATA/pkm/";
	str += std::to_string( (_ULonglong)m_iSpecies );
	str += ".txt";
	FILE *pkmData = fopen( str.c_str() , "r" );

	if( pkmData == NULL )
	{
		return;
	}

	long lSize;
	// obtain file size:
	fseek (pkmData , 0 , SEEK_END);
	lSize = ftell(pkmData);
	rewind(pkmData);

	int buffpos = 0;
	int buffEOF;
	char * buffer;

	buffer = (char*) malloc (sizeof(char)*lSize);
    if (buffer == NULL)
	{
		return;
	}

	buffEOF = fread( buffer, 1, lSize, pkmData);

	std::string nameStr;
	int line = 0; int mode = 0;

	while( buffer[buffpos] != NULL )
	{
		if( buffer[buffpos] == '\n' )
		{
			line++;
			mode = 0;
		}

		if( line == 1 )
		{
			if( buffer[buffpos] == '/' )
			{
				switch( mode )
				{
				case 0: baseStats.hp = atoi( nameStr.c_str() ); break;
				case 1: baseStats.atk = atoi( nameStr.c_str() ); break;
				case 2: baseStats.def = atoi( nameStr.c_str() ); break;
				case 3: baseStats.spatk = atoi( nameStr.c_str() ); break;
				case 4: baseStats.spdef = atoi( nameStr.c_str() ); break;
				case 5: baseStats.speed = atoi( nameStr.c_str() ); break;
				}
				nameStr = "";
				mode++;
				buffpos++;
				continue;
			}
			nameStr += buffer[buffpos];
		}

		if( line == 2 )
		{
			if( buffer[buffpos] == '/' )
			{
				switch( mode )
				{
				case 0: types[0].type = atoi( nameStr.c_str() );
				case 1: types[1].type = atoi( nameStr.c_str() );
				}
				nameStr = "";
				mode++;
				buffpos++;
				continue;
			}
			nameStr += buffer[buffpos];
		}
	
		buffpos++;
	}

	return;
}

int Pokemon::GetStat( std::string statType )
{
	int baseStat, ivStat, evStat;

	ivStat = 0;
	evStat = 0;

	if( statType == "hp" )
	{
		baseStat = baseStats.hp;
		ivStat = pkmIVs.hp;
		evStat = pkmEVs.hp;

		//HP is wierd, do this here:
		int ret;
		ret = ((ivStat + (2*baseStat) + (evStat/4) + 100)*m_iLevel)/100;
		ret += 10;

		return ret;
	}
	if( statType == "attack" )
	{
		baseStat = baseStats.atk;
		ivStat = pkmIVs.atk;
		evStat = pkmEVs.atk;
	}
	if( statType == "special attack" )
	{
		baseStat = baseStats.spatk;
		ivStat = pkmIVs.spatk;
		evStat = pkmEVs.spatk;
	}
	if( statType == "defence" )
	{
		baseStat = baseStats.def;
		ivStat = pkmIVs.def;
		evStat = pkmEVs.def;
	}
	if( statType == "special defence" )
	{
		baseStat = baseStats.spdef;
		ivStat = pkmIVs.spdef;
		evStat = pkmEVs.spdef;
	}
	if( statType == "speed" )
	{
		baseStat = baseStats.speed;
		ivStat = pkmIVs.speed;
		evStat = pkmEVs.speed;
	}

	//Calculate:
	int ret;
	ret = ((ivStat + (2*baseStat) + (evStat/4))*m_iLevel) / 50 ;
	ret += 5;

	return ret;
}

void Pokemon::LoadSprite()
{
	tex = NULL;
	tex_front = NULL;
	std::string str;

	if( side == 1 )
	{
		str = "DATA/GFX/Front/";
	}
	else
	{
		str = "DATA/GFX/Back/";
	}

	str += std::to_string( (_ULonglong)m_iSpecies );
	str += ".png";

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( str.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", str.c_str(), IMG_GetError() );
	}
	else
	{
		//SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        tex = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( tex == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", str.c_str(), SDL_GetError() );
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//For the summary screen!
	str = "DATA/GFX/Front/";
	str += std::to_string( (_ULonglong)m_iSpecies );
	str += ".png";

	//Load image at specified path
	loadedSurface = IMG_Load( str.c_str() );
	tex_front = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
	SDL_FreeSurface( loadedSurface );
}

void Pokemon::Render( SDL_Renderer *Renderer )
{
	if( m_bIsFainted )
		return;

	if( !m_bShouldRender )
		return;

	if( side == 1 )
	{
		m_iPositionX = 360;
		m_iPositionY = 110;

		if( !m_bIsFainting )
			SDL_RenderCopy( Renderer, tex, NULL, &GetRect( 360, 110, 160, 160 ) );
		else
		{
			int X, Y, Xw, Yw;
			X = 360, Y = 110;
			Xw = 160, Yw = 160;

			X+=m_iFaintAnimTime/2;
			Y+=m_iFaintAnimTime/2;

			Xw-=m_iFaintAnimTime;
			Yw-=m_iFaintAnimTime;

			SDL_RenderCopy( Renderer, tex, NULL, &GetRect( X, Y, Xw, Yw ) );
		}
	}
	else
	{
		m_iPositionX = 90;
		m_iPositionY = 190;

		if( !m_bIsFainting )
			SDL_RenderCopy( Renderer, tex, NULL, &GetRect( 90, 190, 160, 160 ) );
		else
		{
			int X, Y, Xw, Yw;
			X = 90, Y = 190;
			Xw = 160, Yw = 160;

			X+=m_iFaintAnimTime/2;
			Y+=m_iFaintAnimTime;

			Xw-=m_iFaintAnimTime;
			Yw-=m_iFaintAnimTime;

			SDL_RenderCopy( Renderer, tex, NULL, &GetRect( X, Y, Xw, Yw ) );
		}
	}
}

bool Pokemon::CheckFainted()
{
	if( m_iHealth <= 0 )
	{
		m_iHealth = 0;
		Faint();
		return true;
	}
	return false;
}

void Pokemon::IncrementHealth( int amount, bool animated, Pokemon *pokeOther, HPDisplays *disp )
{
	if( animated )
	{
		//Do a HP BAR animation:
		int TargetHealth = m_iHealth + amount;
		while( true )
		{
			m_iHealth = (int)lerp( m_iHealth, TargetHealth, 0.1 );
			if( side == 1 )
			{
				disp->UpdateHP( pokeOther->GetHealth(), m_iHealth, pokeOther->GetStat( "hp" ), GetStat( "hp" ) );
			}
			else
			{
				disp->UpdateHP( m_iHealth, pokeOther->GetHealth(), GetStat( "hp" ), pokeOther->GetStat( "hp" ) );
			}

			disp->Render();
			SDL_RenderPresent( gRenderer );

			if( m_iHealth <= TargetHealth )
				break;

			if( m_iHealth <= 0 )
			{
				m_iHealth = 0;
				break;
			}

			Sleep( 10 );
		}
	}
	else
	{
		m_iHealth += amount;
	}
};

void Pokemon::Faint()
{
	//Set up text
	std::string tempStr = m_sPkmName + " fainted!";
	BattleText( tempStr, gRenderer, BattleUIGFX, gFont );

	//Handle animation
	m_bIsFainting = true;
	m_iFaintAnimTime = 1;

	while( true )
	{
		SDL_RenderClear( gRenderer );

		BattleUIGFX->bg->Render();
		BattleUIGFX->menu->Render();
		BattleUIGFX->hpDisp->Render();
		BattleUIGFX->RenderPokes();

		SDL_RenderPresent( gRenderer );

		m_iFaintAnimTime+=4;

		if( m_iFaintAnimTime >= 160 )
			break;

		Sleep( 1 );
	}
	m_bIsFainting = false;
	m_bIsFainted = true;

	//Check stuff:
	//If its a wild battle, just end it already!

	//Moved!
}