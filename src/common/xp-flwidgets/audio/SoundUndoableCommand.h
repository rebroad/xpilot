#ifndef	_SOUNDUNDOABLECOMMAND_H_
#define	_SOUNDUNDOABLECOMMAND_H_

#include "UndoableCommand.h"
#include "XpPointList.h"
//#include "SoundDocument.h"

class SoundWindow;

enum {
	CHANGE_MAIN_GAIN,
	CHANGE_BINDING,
};

#define	SoundUndoableCommandSUPERCLASS	UndoableCommand
class SoundUndoableCommand : public SoundUndoableCommandSUPERCLASS
{
public:
	SoundUndoableCommand();
	SoundUndoableCommand(PCSTR _menuTitle);
	SoundUndoableCommand(SoundWindow *window, int cType);
	SoundUndoableCommand(SoundWindow *window, int cType, int cSubType); //for working with entire Sound
/*	SoundUndoableCommand(SoundWindow *window, int cType,  int cSubType,
		XpPointList *lst, int mx, int my); //for adding items

	ShipUndoableCommand(ShipWindow *window, int cType, int cSubType,
		XpPointList *lst, XpPoint *pt); //For working with a single point in its current position
	ShipUndoableCommand(ShipWindow *window, int cType, int cSubType,
		XpPointList *lst, XpPoint *pt, int newx, int newy); //For moving a single point to a new position

	ShipUndoableCommand(ShipWindow *window, int cType, int cSubType,
		XpPointList *lst, XpPoint *pt1, XpPoint *pt2, int rev);
*/
	virtual void	Do();
	virtual void	Undo();

public: //Member vars
//	SoundDocument*	doc;

	int objType;
	int CommandType;
	int CommandSubType; //For the different type of area changes.

	SoundWindow*	mySoundWindow;

	int		newGain;
	int		oldGain;

	int		eventRow;
	int		oldSoundRow;
	int		newSoundRow;

private:
	void	DoChangeBinding();
	void	UndoChangeBinding();
};

#endif	// _SOUNDUNDOABLECOMMAND_H_
