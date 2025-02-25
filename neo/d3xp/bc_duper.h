#include "Misc.h"
#include "Entity.h"

#include "Item.h"


class idDuper : public idMoveableItem
{
public:
	CLASS_PROTOTYPE(idDuper);

	void					Save(idSaveGame *savefile) const;
	void					Restore(idRestoreGame *savefile);

	void					Spawn(void);

	virtual void			Think(void);
	virtual bool			DoFrob(int index = 0, idEntity * frobber = NULL);
	virtual bool			Collide(const trace_t &collision, const idVec3 &velocity);
	//virtual void			Killed(idEntity *inflictor, idEntity *attacker, int damage, const idVec3 &dir, int location);

private:
	
	idEntity *				FindAimedEntity();
	idEntity *				FindMisfireEntity();
	void					DoMisfire();
	void					DoDupe();
	idVec3					FindClearSpawnSpot();

	int						chargesRemaining;
	int						misfireCooldownTimer;
	bool					lastAimState;
	idEntityPtr<idEntity>	aimedEnt;
	int						spawnTime;


	

};
//#pragma once