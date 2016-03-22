#pragma once

#include "stdafx.h"
#include "move.h"
#include <string>
#include "graphics.h"
#include "type.h"

struct ivs
{
	int hp;
	int atk;
	int def;
	int spatk;
	int spdef;
	int speed;
};

struct evs
{
	int hp;
	int atk;
	int def;
	int spatk;
	int spdef;
	int speed;
};

struct basestats
{
	int hp;
	int atk;
	int def;
	int spatk;
	int spdef;
	int speed;
};

class Pokemon
{
public:

	Pokemon::Pokemon()
	{
		baseStats.atk = 0;
		baseStats.def = 0;
		baseStats.spatk = 0;
		baseStats.spdef= 0;
		baseStats.speed = 0;
		baseStats.hp = 0;

		m_iEvolveLvl = 0; m_iEvolveSpecies = -1; m_iEvolveType = -1;

		m_iCatchRate = 255; //Assume 255, for ease of access?
	};

	void Init( int species, ivs iv, evs ev, int level );
	void Attack( Pokemon* target, int move, bool ai = false );
	std::string GetName();

	evs GetEvs(){return pkmEVs;};
	ivs GetIvs(){return pkmIVs;};

	Move *GetAttack( int num )
	{
		return pAttacks[num];
	}

	int GetHealth()
	{
		 return m_iHealth;
	};

	int GetStat( std::string stattype );

	void IncrementHealth( int amount, bool animated = false, Pokemon *poke = NULL, HPDisplays *disp = NULL );

	void Render( SDL_Renderer *Renderer );

	bool GetActive( ) { return !m_bIsFainted; };
	bool CheckFainted();

	int side; //For now...

	std::string m_sPkmName;

	std::string m_sOT;

	int m_iLevel;
	int m_iSpecies;
	int m_iCatchRate;
	Move * pAttacks[4];
	int curAttacks;

	int m_iPositionX;
	int m_iPositionY;

	SDL_Texture *GetTexture() { return tex_front; };

	int GetType( int num ){ return types[num].type; };
	void LoadSprite();

	bool m_bShouldRender;

	void Heal(){ m_iHealth = GetStat( "hp" ); };

	int GetEXP(){ return m_iExp; };
	int GetNextLevelEXP( )
	{ 
		return ((4 * pow( (float)(m_iLevel + 1), 3) ))/5;  //Tempory!
	};

	void ForceLevelUp()
	{
		m_iExp = GetNextLevelEXP( );
		m_iLevel++;
	}

	void CheckEvolution();

	void SetExp( int i ){ m_iExp = i; }; //Quick and dirty hack!

	Type types[2]; //Aparently, this needs to be here!

	void CheckLearnSet();
	void TryLearnNewMove();

	std::string SerialisePokemon()
	{
		std::string pokeString = "";
		//Species, Level and EXP:
		pokeString += std::to_string( (_ULonglong)(m_iSpecies)) + " ";
		pokeString += std::to_string( (_ULonglong)GetEXP()) + " ";
		pokeString += std::to_string( (_ULonglong)m_iLevel) + " ";

		//EVs and IVs

		pokeString += std::to_string( (_ULonglong)GetEvs().hp) + " ";
		pokeString += std::to_string( (_ULonglong)GetEvs().atk) + " ";
		pokeString += std::to_string( (_ULonglong)GetEvs().def) + " ";
		pokeString += std::to_string( (_ULonglong)GetEvs().spatk) + " ";
		pokeString += std::to_string( (_ULonglong)GetEvs().spdef) + " ";
		pokeString += std::to_string( (_ULonglong)GetEvs().speed) + " ";

		pokeString += std::to_string( (_ULonglong)GetIvs().hp) + " ";
		pokeString += std::to_string( (_ULonglong)GetIvs().atk) + " ";
		pokeString += std::to_string( (_ULonglong)GetIvs().def) + " ";
		pokeString += std::to_string( (_ULonglong)GetIvs().spatk) + " ";
		pokeString += std::to_string( (_ULonglong)GetIvs().spdef) + " ";
		pokeString += std::to_string( (_ULonglong)GetIvs().speed) + " ";

		//HP
		pokeString += std::to_string( (_ULonglong)GetHealth()) + " ";

		//Moves:
		for( int i = 0 ; i < 4; i++ )
		{
			pokeString += std::to_string( (_ULonglong)pAttacks[i]->GetID()) + " ";
		}

		return pokeString;
	}

	void DeSerialisePokemon( std::string pokeString )
	{
		//tokenise and parse:
		char seps[] = " ";
		char *token;

		token = strtok( &pokeString[0], seps );

		int species = atoi(token);
		token = strtok( NULL, seps );
		int exp = atoi(token);
		token = strtok( NULL, seps );
		int level = atoi(token);
		token = strtok( NULL, seps );

		int evhp = atoi(token);
		token = strtok( NULL, seps );
		int evatk = atoi(token);
		token = strtok( NULL, seps );
		int evdef = atoi(token);
		token = strtok( NULL, seps );
		int evspatk = atoi(token);
		token = strtok( NULL, seps );
		int evspdef = atoi(token);
		token = strtok( NULL, seps );
		int evspeed = atoi(token);
		token = strtok( NULL, seps );

		int ivhp = atoi(token);
		token = strtok( NULL, seps );
		int ivatk = atoi(token);
		token = strtok( NULL, seps );
		int ivdef = atoi(token);
		token = strtok( NULL, seps );
		int ivspatk = atoi(token);
		token = strtok( NULL, seps );
		int ivspdef = atoi(token);
		token = strtok( NULL, seps );
		int ivspeed = atoi(token);
		token = strtok( NULL, seps );

		int curhp = atoi(token);
		token = strtok( NULL, seps );

		ivs iv;
		iv.hp = ivhp;
		iv.atk = ivatk;
		iv.def = ivdef;
		iv.spatk = ivspatk;
		iv.spdef = ivspdef;
		iv.speed = ivspeed;

		evs ev;
		ev.hp = evhp;
		ev.atk = evatk;
		ev.def = evdef;
		ev.spatk = evspatk;
		ev.spdef = evspdef;
		ev.speed = evspeed;

		Init( species, iv, ev, level );
		SetExp( exp );
		IncrementHealth( curhp - GetHealth() );

		//Set moves:
		for( int i = 0 ; i < 4; i++ )
		{
			if( pAttacks[i] != NULL )
			{
				delete pAttacks[i];
			}
			pAttacks[i] = new Move( atoi(token) );
			token = strtok( NULL, seps );
		}
	}

private:
	int m_iExp;
	int m_iHealth;

	int m_iExpYeild;

	int m_iEvolveLvl, m_iEvolveSpecies, m_iEvolveType;

	basestats baseStats;

	ivs pkmIVs;
	evs pkmEVs;

	SDL_Texture *tex;

	SDL_Texture *tex_front;

	void GetBaseStats();
	void Faint();

	bool m_bIsFainting;
	int m_iFaintAnimTime;
	bool m_bIsFainted;
};
