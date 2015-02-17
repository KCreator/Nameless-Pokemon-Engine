#include "stdafx.h"
#include "item.h"
#include <typeinfo>

//Dirty hacky stuff here!
CBaseItem* GetFromID(int id)
{
	switch( id )
	{
		//Hardcoded items:
	case 1: return new MasterBall();
		//...
	case 50: return new RareCandy();

	default: return new ScriptableItem( id );
	}

	return NULL;
}