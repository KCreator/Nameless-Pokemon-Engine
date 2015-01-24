#include "stdafx.h"
#include "player.h"

void Player::SendOutPokemon( int i )
{
	if( m_pkmParty[i] != NULL )
	{
		SendOut( m_pkmParty[i] );
	}
}

void Player::SendOut( Pokemon *poke )
{
	//Hook into the main battle scene:

}
