#ifndef	_SHIPUNDOABLECOMMAND_H_
#define	_SHIPUNDOABLECOMMAND_H_

#include "UndoableCommand.h"
#include "XpPointList.h"
#include "ShipDocument.h"

//Im not sure how this works, but Dik did it this way too ;)
class ShipWindow;

#define	ShipUndoableCommandSUPERCLASS	UndoableCommand
class ShipUndoableCommand : public ShipUndoableCommandSUPERCLASS
{
public:
	ShipUndoableCommand();
	ShipUndoableCommand(PCSTR _menuTitle);
	ShipUndoableCommand(ShipWindow *window, int cType, int cSubType); //for working with entire ship
	ShipUndoableCommand(ShipWindow *window, int cType,  int cSubType,
		XpPointList *lst, int mx, int my); //for adding items
	ShipUndoableCommand(ShipWindow *window, int cType, int cSubType,
		XpPointList *lst, XpPoint *pt); //For working with a single point in its current position
	ShipUndoableCommand(ShipWindow *window, int cType, int cSubType,
		XpPointList *lst, XpPoint *pt, int newx, int newy); //For moving a single point to a new position

	ShipUndoableCommand(ShipWindow *window, int cType, int cSubType,
		XpPointList *lst, XpPoint *pt1, XpPoint *pt2, int rev);
	virtual void	Do();
	virtual void	Undo();

public: //Member vars
	int newx; //The new point x coord
	int newy; //The new point y coord
	XpPoint point; //A "temporary" storage area for coord data
	XpPoint *spoint; //A storage area for the point.
	XpPoint *frstPt; //Points to the first point selected, used for splitting.
	XpPoint *nxtPt; //Points to the next point in the list, so we know where to return to.
	XpPointList *ptlst; //The pointlist to be working with.
	ShipDocument *shp;

	int objType;
	int CommandType;
	int CommandSubType; //For the different type of area changes.

	ShipWindow*	myShipWindow;

private:
};

#endif	// _SHIPUNDOABLECOMMAND_H_
