#include "stdafx.h"
#include "trainer.h"

void Trainer::AddToParty( Pokemon *poke )
{
	if( m_iNumPoke == 5 )
		return;

	if( poke->m_sOT == "" )
	{
		poke->m_sOT = m_strTrainerID;
	}

	m_pkmParty[m_iNumPoke] = poke;
	m_iNumPoke++;
}

void Trainer::SendOut( Pokemon *poke )
{

}

int Trainer::GetNumActivePkm()
{
	int num = 0;

	for( int i = 0; i <= m_iNumPoke; i++ )
	{
		if( m_pkmParty[i] != NULL && m_pkmParty[i]->GetActive() )
			num++;
	}
	return num;
}