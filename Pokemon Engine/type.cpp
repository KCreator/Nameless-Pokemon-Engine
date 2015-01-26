#include "stdafx.h"
#include "type.h"

float Type::TestFor( Type target )
{
	//Type Table:
	//Attacker = NORMAL
	if( type == TYPE_NORMAL )
	{
		if( target.type == TYPE_ROCK || target.type == TYPE_STEEL )
		{
			return 0.5;
		}
		if( target.type == TYPE_GHOST )
			return 0;
	}
	//Attacker = FIGHTING
	if( type == TYPE_FIGHT )
	{
		if( target.type == TYPE_NORMAL || target.type == TYPE_ROCK || target.type == TYPE_STEEL || target.type == TYPE_ICE || target.type == TYPE_DARK )
		{
			return 2;
		}
		if( target.type == TYPE_FLYING || target.type == TYPE_POISON || target.type == TYPE_BUG || target.type == TYPE_PHYCHIC || target.type == TYPE_FAIRY )
		{
			return 0.5;
		}
		if( target.type == TYPE_GHOST )
			return 0;
	}
	//Attacker = FLYING
	if( type == TYPE_FLYING )
	{
		if( target.type == TYPE_FIGHT || target.type == TYPE_BUG || target.type == TYPE_GRASS )
		{
			return 2;
		}
		if( target.type == TYPE_ROCK || target.type == TYPE_STEEL || target.type == TYPE_ELECTRIC )
		{
			return 0.5;
		}
	}


	//Blahblahblah
	//Attacker = FIRE
	if( type == TYPE_FIRE )
	{
		if( target.type == TYPE_GRASS )
		{
			return 2;
		}
		if( target.type == TYPE_WATER )
		{
			return 0.5;
		}
	}

	return 1;
}