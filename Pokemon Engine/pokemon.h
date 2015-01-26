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
	void Init( int species, ivs iv, evs ev, int level );
	void Attack( Pokemon* target, int move, bool ai = false );
	std::string GetName();

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
	void CheckFainted();

	int side; //For now...

	std::string m_sPkmName;

	int m_iLevel;
	int m_iSpecies;
	Move * pAttacks[4];

	int m_iPositionX;
	int m_iPositionY;

	SDL_Texture *GetTexture() { return tex_front; };

	int GetType( int num ){ return types[num].type; };
	void LoadSprite();

	bool m_bShouldRender;

private:
	int m_iExp;
	int m_iHealth;

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

	Type types[2];
};
