#pragma once
#include "stdafx.h"
#include "pokemon.h"
#include "player.h"

class PokemonPartyScene
{
public:
	void Initialise( Player *player );
	bool Tick();
	int SelectPokemon();

	void RenderBG();
	void RenderMain();
	void RenderOthers();

	void FadeIn();

	int m_iSelection;

	bool IsBattle;

	int m_iLastScene;

private:
	void HandleSelection();

	void BattleSceneSwapout();

	bool m_bHasSelected;
	int m_iPkmnSelection;

	SDL_Event events;
	SDL_Texture *m_Texture;
	SDL_Texture *mSelectorTexture;

	int numHM;

	Player *m_Player;
};