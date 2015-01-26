#include "stdafx.h"
#include "graphics.h"

//class BaseCPApp
//{
//public:
//	void RenderBtn();
//	void RenderApp();
//
//	void Think();
//}

class ComuniPad
{
public:
	void Initialise();
	bool Tick();
	void RenderBG();
	void RenderButtons();

private:
	int m_iAnimState;
	int m_iSelection;
	SDL_Texture *m_tex;

	//Animation stuff:
	int m_iAnimProgress;
	bool m_bReverse;
};

