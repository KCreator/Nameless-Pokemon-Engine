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
