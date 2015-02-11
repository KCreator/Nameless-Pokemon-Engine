#include "graphics.h"
#include "item.h"
#include <vector>

class BagScene
{
public:
	//Initialiser
	void Init();

	//Think function, used by the engine controller
	bool Tick();

	void Render();
	void AddItem( CBaseItem *item, int count );
	bool HasItem( int ID );
	int GetItemCount( int ID );

	void DeleteUsedItems();

	bool IsBattle;
	int PreviousScene;

	std::vector< CBaseItem * > items; //Item array.

private:
	SDL_Texture *BagUI;

	int m_iCurPocket;
	bool isMovingItem;
	int numItems;

	int selection;

	bool SelectedItem;
	int m_iSubSelection;

	bool debouncer;
};