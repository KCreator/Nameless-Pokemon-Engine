#include "stdafx.h"
#include "pokemon.h"
#include "trainer.h"

class PokemonBattle
{
public:
	void Initialise( Pokemon *battler1, Pokemon *battler2, bool isWild, Trainer *m_Trainer );
	bool Tick();

	void SwapOut( Pokemon *NewBattler, int side );

	bool IsWild(){ return m_bWild; };

	void WildBattleStartAnim();

	void Clear(){delete m_pkmBattler1;};

	void Capture();

private:
	Pokemon *m_pkmBattler1;
	Pokemon *m_pkmBattler2;
	Trainer *m_trTrainer;
	SDL_Event events;

	bool m_bWild;
};