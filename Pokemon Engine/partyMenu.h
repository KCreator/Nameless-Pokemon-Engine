#pragma once
#include "stdafx.h"
#include "pokemon.h"
#include "player.h"

class PokemonPartyScene
{
public:
	void Initialise( Player *player );
	bool Tick();

	void RenderBG();
	void RenderMain();
	void RenderOthers();

	int m_iSelection;

	bool IsBattle;

private:
	void HandleSelection();

	bool m_bHasSelected;
	int m_iPkmnSelection;

	SDL_Event events;
	SDL_Texture *m_Texture;
	SDL_Texture *mSelectorTexture;

	Player *m_Player;
};