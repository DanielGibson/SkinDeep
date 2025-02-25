#include "Misc.h"
#include "Entity.h"

#include "Item.h"


class idWrench : public idMoveableItem
{
public:
	CLASS_PROTOTYPE(idWrench);

	void					Save(idSaveGame *savefile) const;
	void					Restore(idRestoreGame *savefile);

	void					Spawn(void);

	virtual void			Think(void);
	//virtual bool			DoFrob(int index = 0, idEntity * frobber = NULL);
	virtual bool			Collide(const trace_t &collision, const idVec3 &velocity);

	virtual bool			JustBashed(trace_t tr);

private:

	bool					DoRepairOnEnt(idEntity *ent);
	idEntity *				FindBashTarget();
	idVec3					lastBashPos;

	int						damageCooldownTimer;
	



	

};
//#pragma once