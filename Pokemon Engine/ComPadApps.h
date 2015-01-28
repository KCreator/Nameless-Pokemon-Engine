#pragma once
#define MAX_APPS 10

#include "ComuniPad.h"
#include "graphics.h"
#include <vector>

class CBaseApp
{
	friend class ComuniPad;
public:
	virtual void RenderBtn(int x, int y){};
	virtual bool Think(){ return false; };

	ComuniPad *cPad;

	virtual void RenderAppStoreText() {};
};


//App store?
class CPadAppStore : public CBaseApp
{
public:
	CPadAppStore::CPadAppStore();

	void RenderApp();
	void RenderBtn(int x, int y);
	bool Think();

private:
	std::vector<CBaseApp *> Apps;

	int Selector;
	bool debouncer;
};

//Phone
class CPadPhone : public CBaseApp
{
public:
	void RenderApp();
	void RenderBtn(int x, int y);
	bool Think();

};

//Calculator
class CPadCalc : public CBaseApp
{
public:
	void RenderApp();
	void RenderBtn(int x, int y);
	bool Think();

	void RenderAppStoreText();
};

//Clock
class CPadClock : public CBaseApp
{
public:
	void RenderApp();
	void RenderBtn(int x, int y);
	bool Think();

	void RenderAppStoreText();
};