#include "Misc.h"
#include "Entity.h"
#include "Fx.h"

class idBananaPeel : public idMoveableItem
{
public:
	CLASS_PROTOTYPE(idBananaPeel);

							idBananaPeel(void);
	virtual					~idBananaPeel(void);

	void					Save(idSaveGame *savefile) const;
	void					Restore(idRestoreGame *savefile);

	void					Spawn(void);

	virtual void			Think(void);

	//virtual bool			DoFrob(int index = 0, idEntity * frobber = NULL);

	virtual void			Killed(idEntity *inflictor, idEntity *attacker, int damage, const idVec3 &dir, int location);

	virtual void			JustPickedUp();

	virtual bool			Collide(const trace_t& collision, const idVec3& velocity);

private:

	int						thinkTimer;
	bool					flipuprightCheckDone;

	idFuncEmitter			*particleEmitter;

	int						actorBounceTimer;

};
//#pragma once