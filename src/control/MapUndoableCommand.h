#ifndef	_MAPUNDOABLECOMMAND_H_
#define	_MAPUNDOABLECOMMAND_H_

#include "UndoableCommand.h"

//Im not sure how this works, but Dik did it this way too ;)
class MapWindow;

#define	MapUndoableCommandSUPERCLASS	UndoableCommand
class MapUndoableCommand : public MapUndoableCommandSUPERCLASS
{
public:
	MapUndoableCommand();
	MapUndoableCommand(PCSTR _menuTitle);
	MapUndoableCommand(MapWindow *window, int cType, bool swap, ObjList *chngLst);
	virtual void	Do();
	virtual void	Undo();

public: //Member vars
	ObjList *changeList2;

	//We can undo many different types of things.
	int CommandType;

	bool swapSize; //Should we swap the map size?

	MapWindow*	myMapWindow; //Pointer to the current Map Window

private:
};

#endif	// _MAPUNDOABLECOMMAND_H_
