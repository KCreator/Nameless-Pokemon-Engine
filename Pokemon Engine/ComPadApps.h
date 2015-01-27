#pragma once
#include "ComuniPad.h"
#include "graphics.h"

class CBaseApp
{
	friend class ComuniPad;
public:
	virtual void RenderBtn(){};
	virtual bool Think(){ return false; };

	ComuniPad *cPad;
};


//App store?
class CPadAppStore : public CBaseApp
{
public:
	void RenderApp();
	void RenderBtn();
	bool Think();
};

//Phone

class CPadPhone : public CBaseApp
{
public:
	void RenderApp();
	void RenderBtn();
	bool Think();
};