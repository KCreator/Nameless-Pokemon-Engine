#pragma once

#include "stdafx.h"
#include "math.h"
#include "utils.h"
#include "type.h"
#include <string>
#include <map> //Datamaps
#include <vector>

class Move
{
	friend class Pokemon;
public:
	Move::Move( int ID )
	{
		m_iID = ID;
		LoadMove();
	};

	void LoadMove();

	void IncrementPP( int amount )
	{
		m_iPP += amount;
		Clamp( m_iPP, 0, m_iMaxPP );
	};

	int GetPP( bool max = false ) { return max ? m_iMaxPP : m_iPP; };

	int GetID( ) { return m_iID; };

	int GetBP( );
	std::string GetName();
	Type moveType;

	int GetType(){ return moveType.type; };

	void DispatchParticle( Pokemon *User, Pokemon *Target, std::string m_sMoveAnimation, int EmitterType );
	void DispatchParticleSystem( Pokemon *User, Pokemon *Target, std::vector<std::string> m_sMoveAnimation );

	int m_iPhySpeStat;
	int m_iEffect;

	void DoAttack( Pokemon *user, Pokemon* target, float Damage );

private:
	int m_iID;

	int m_iPP;
	int m_iMaxPP;

	int m_iBP;
	int m_iAcc;

	std::string m_sMoveScript; //Will use this instead of the 4 particle anim strings!

	std::string m_sMoveName;

	std::map< std::string, int > Variables; //Idk if this is *too* needed here!
};