#pragma once
#include "stdafx.h"
#include "pokemon.h"

#define MAX_PARTY 6 //Only 6 party slots for now!
#define MAX_POKEMON MAX_PARTY //Only 6 storage slots for now!

class Trainer
{
public:
	Trainer::Trainer()
	{
		m_iNumPoke = 0; 
		m_pkmParty[0] = NULL; m_pkmParty[1] = NULL; m_pkmParty[2] = NULL; m_pkmParty[3] = NULL; m_pkmParty[4] = NULL; m_pkmParty[5] = NULL;

		//Generate trainer ID:
		m_strTrainerID = "";
		m_strTrainerID += std::to_string( (_ULonglong)rand()%9 );
		m_strTrainerID += std::to_string( (_ULonglong)rand()%9 );
		m_strTrainerID += std::to_string( (_ULonglong)rand()%9 );
		m_strTrainerID += std::to_string( (_ULonglong)rand()%9 );
		m_strTrainerID += std::to_string( (_ULonglong)rand()%9 );
	};

	//Free up memory!
	Trainer::~Trainer()
	{
		for( int i = 0; i <= m_iNumPoke; i++ )
		{
			if( m_pkmParty[i] != NULL )
				delete m_pkmParty[i];
		}
	}

	void AddToParty( Pokemon *poke );
	void SendOut( Pokemon *poke );
	int GetNumActivePkm();

	void LoadFromFile( std::string input );

	Pokemon *m_pkmParty[6];
	int m_iNumPoke;

	std::string m_strTrainerID;
	std::string m_strName;
};