#include "stdafx.h"
#include "item.h"
#include <typeinfo>

//Dirty hacky stuff here!
CBaseItem* GetFromID(int id)
{
	switch( id )
	{
	case 1: return new MasterBall();
		//...
	case 50: return new RareCandy();
	}

	return NULL;
}