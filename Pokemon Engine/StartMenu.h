#include "stdafx.h"
#include "utils.h"
#include "partyMenu.h"
#include "pokemon.h"
#include "player.h"

class MainStartMenu
{
public:
	void Initialise();
	void Tick();
	void Render();

private:
	int m_iSelection;
	SDL_Texture *m_texture;
};
