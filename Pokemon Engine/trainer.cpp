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

void Trainer::LoadFromFile( std::string input )
{
	//Load pokemon from a file:
	std::string filePath = "DATA/Trainers/" + input;

	FILE *fp;   
    fp = fopen( filePath.c_str(), "rb");

	for( int i = 0; i < MAX_PARTY; i++ )
	{
		if( i < MAX_PARTY )
		{
			//Read data:
			//Define vars:
			int species, level, evhp, evatk, evdef, evspatk, evspdef, evspeed, ivhp, ivatk, ivdef, ivspatk, ivspdef, ivspeed;
			//Species:
			fscanf( fp, "%d", &species );
			fscanf( fp, "%d", &level );

			fscanf( fp, "%d", &evhp );
			fscanf( fp, "%d", &evatk );
			fscanf( fp, "%d", &evdef );
			fscanf( fp, "%d", &evspatk );
			fscanf( fp, "%d", &evspdef );
			fscanf( fp, "%d", &evspeed );

			fscanf( fp, "%d", &ivhp );
			fscanf( fp, "%d", &ivatk );
			fscanf( fp, "%d", &ivdef );
			fscanf( fp, "%d", &ivspatk );
			fscanf( fp, "%d", &ivspdef );
			fscanf( fp, "%d", &ivspeed );

			//Generate a pokemon if its valid:
			if( species > 0 )
			{
				Pokemon *poke = new Pokemon();
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
				poke->side = 1;
				poke->Init( species, iv, ev, level );

				AddToParty( poke );
			}
		}
	}
}