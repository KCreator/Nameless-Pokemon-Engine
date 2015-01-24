#pragma once
#include "stdafx.h"
#include "trainer.h"

class Player : public Trainer
{
public:
	void SendOutPokemon( int i );
	void SendOut( Pokemon *poke );
};