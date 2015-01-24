#pragma once

#include "stdafx.h"
#include "math.h"
#include "utils.h"
#include "type.h"
#include <string>

class Move
{
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

	int GetPP( ) { return m_iPP; };

	int GetID( ) { return m_iID; };

	int GetBP( );
	std::string GetName();
	Type moveType;

	int GetType(){ return moveType.type; };

	void PlayAnimation( Pokemon *User, Pokemon *Target );

	int m_iPhySpeStat;

private:
	int m_iID;

	int m_iPP;
	int m_iMaxPP;

	int m_iBP;
	int m_iAcc;

	std::string m_sMoveAnimation;

	std::string m_sMoveName;
};