#pragma once
#include "stdafx.h"
#include "graphics.h"
#include "ComPadApps.h"

class ComuniPad
{
	friend class CBaseApp;
public:
	void Initialise();
	bool Tick();
	void RenderBG();
	void RenderButtons();

	void InstallApp(CBaseApp *app)
	{
		apps[numApps++] = app;
	};

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
	int numApps;
	CBaseApp *apps[MAX_APPS];

	bool DoBtn( int input );
};

