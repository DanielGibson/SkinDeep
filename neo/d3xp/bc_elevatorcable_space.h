#include "Misc.h"
#include "Entity.h"
#include "Mover.h"

#include "Item.h"


class idElevatorcableSpace : public idBeam
{
public:
	CLASS_PROTOTYPE(idElevatorcableSpace);

	void					Save(idSaveGame *savefile) const;
	void					Restore(idRestoreGame *savefile);

	void					Spawn(void);

	virtual bool			DoFrob(int index = 0, idEntity * frobber = NULL);
	virtual void			Think(void);

private:

	virtual void			Event_PostSpawn(void);

	idEntityPtr<idEntity>	topEnt;
	idEntityPtr<idEntity>	bottomEnt;

	idFrobcube*				frobTrigger;

	int						GetZipDirection();
	enum					{ DIR_TO_START, DIR_TO_END, DIR_NONE };
	int						lastZipDirection;

};
//#pragma once