#pragma once
#include "stdafx.h"
#include "pokemon.h"

class Trainer
{
public:
	void AddToParty( Pokemon *poke );
	void SendOut( Pokemon *poke );
	int GetNumActivePkm();

	Pokemon *m_pkmParty[6];
	int m_iNumPoke;
};