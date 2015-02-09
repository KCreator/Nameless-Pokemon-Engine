#pragma once
#include "stdafx.h"
#include "pokemon.h"
#include "player.h"


class PokemonSummaryScene
{
public:
	void Initialise( Player *player );
	bool Tick();

	void RenderSlide1();
	void RenderSlide2();
	void RenderSlide3();

	int m_iSlide;
	int m_iSelection;

private:
	int slide;
	int animState;
	bool Debounce;

	SDL_Event events;
	SDL_Texture *m_Texture;
	SDL_Texture *m_TypeTexture;

	Player *m_Player;

	void SharedPokeRender();
};