#include "Misc.h"
#include "Entity.h"
#include "Mover.h"
//#include "Item.h"
#include "Light.h"


class idVendingmachine : public idStaticEntity
{
public:
	CLASS_PROTOTYPE(idVendingmachine);

							idVendingmachine(void);
	virtual					~idVendingmachine(void);

	void					Save(idSaveGame *savefile) const;
	void					Restore(idRestoreGame *savefile);

	void					Spawn(void);

	virtual void			Think(void);

	//virtual void			Damage(idEntity *inflictor, idEntity *attacker, const idVec3 &dir, const char *damageDefName, const float damageScale, const int location, const int materialType = SURFTYPE_NONE);	

	virtual bool			DoFrob(int index = 0, idEntity * frobber = NULL);

	virtual void			DoRepairTick(int amount);

	virtual void			Killed(idEntity *inflictor, idEntity *attacker, int damage, const idVec3 &dir, int location);

private:

	bool					SpawnTheItem();

	enum					{ VENDSTATE_IDLE, VENDSTATE_DELAY, VENDSTATE_DISABLED };
	int						vendState;
	int						stateTimer;

	idEntity*				frobbutton1;


	const idDeclEntityDef	*itemDef;
	
	int						itemDeathSpewTimer;
	int						itemDeathSpewCounter;


	int						totalItemCounter;

};
//#pragma once