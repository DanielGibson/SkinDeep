#include "Misc.h"
#include "Entity.h"
//#include "Mover.h"
//#include "Item.h"


class idCryospawn : public idAnimated
{
public:
	CLASS_PROTOTYPE(idCryospawn);

							idCryospawn(void);
	virtual					~idCryospawn(void);

	void					Save(idSaveGame *savefile) const;
	void					Restore(idRestoreGame *savefile);

	void					Spawn(void);

	virtual void			Think(void);

private:

	int						stateTimer;
	int						state;
	enum					{ OPEN, CLOSEDELAY, SMOKEDELAY, CLOSED  };

	
};
//#pragma once