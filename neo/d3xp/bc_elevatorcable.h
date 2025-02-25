#include "Misc.h"
#include "Entity.h"
#include "Mover.h"

#include "Item.h"


class idElevatorcable : public idAnimated
{
public:
	CLASS_PROTOTYPE(idElevatorcable);

	void					Save(idSaveGame *savefile) const;
	void					Restore(idRestoreGame *savefile);

	void					Spawn(void);

	virtual bool			DoFrob(int index = 0, idEntity * frobber = NULL);
	virtual void			Think(void);


	

private:

	bool					hasInitialized;

	void					Initialize();

	idEntity *				topEnt;
	idEntity *				bottomEnt;

	int						animTimer;
	int						animState;

	enum
	{
		ELEVCABLESTATE_IDLE,
		ELEVCABLESTATE_WAITING,
		ELEVCABLESTATE_WIGGLING
	};

};
//#pragma once