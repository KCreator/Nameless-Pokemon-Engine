#pragma once
#include "stdafx.h"
#include "graphics.h"
#include "ComPadApps.h"

#define MAX_APPS 10

class ComuniPad
{
	friend class CBaseApp;
public:
	void Initialise();
	bool Tick();
	void RenderBG();
	void RenderButtons();

	SDL_Texture *GetTexture(){return m_tex;}; //Just in case!

private:
	int m_iAnimState;
	int m_iSelection;
	SDL_Texture *m_tex;

	int CursorX, CursorY;

	//Animation stuff:
	int m_iAnimProgress;
	bool m_bReverse;
	bool m_bIsInApp;

	CBaseApp *apps[MAX_APPS];

	bool DoBtn( int input );
};

