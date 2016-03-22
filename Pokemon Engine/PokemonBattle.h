#include "stdafx.h"
#include "pokemon.h"
#include "trainer.h"

class PokemonBattle
{
public:
	void Initialise( Pokemon *battler1, Pokemon *battler2, bool isWild, Trainer *m_Trainer );
	bool Tick();

	void SwapOut( Pokemon *NewBattler, int side, bool isFaintedSwapout = false );

	//Pokeball related:
	//void Throwball( int BallID )
	//void ShakeBall( int shakes );
	//void RenderBall();

	bool IsWild(){ return m_bWild; };
	bool IsMP(){ return m_bIsMultiplayer; };

	void WildBattleStartAnim();

	void Clear(){delete m_pkmBattler1;};

	void Capture();

	Pokemon* GetPoke( int i )
	{
		if(i==1)
			return m_pkmBattler1;
		if(i==2)
			return m_pkmBattler2;
		else
			return NULL;
	}; 

	void SetMultiplayer( bool in ) { m_bIsMultiplayer = in; };

private:
	Pokemon *m_pkmBattler1;
	Pokemon *m_pkmBattler2;
	Trainer *m_trTrainer;
	SDL_Event events;

	bool m_bPokeballThrown;
	bool m_bWild;
	bool m_bIsMultiplayer;
};