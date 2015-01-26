#include "stdafx.h"
#include "utils.h"
#include "partyMenu.h"
#include "pokemon.h"
#include "player.h"
#include "ComuniPad.h"

#define MENU_COMUNIPAD 1

class MainStartMenu
{
public:
	void Initialise();
	bool Tick();
	void Render();

private:
	int m_iSelection;
	SDL_Texture *m_texture;
	bool Debounce;

	int m_iSubMenu;
	ComuniPad *m_Phone;
};
