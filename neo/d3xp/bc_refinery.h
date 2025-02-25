#include "Misc.h"
#include "Entity.h"
#include "Mover.h"
//#include "Item.h"


class idRefinery : public idMover
{
public:
	CLASS_PROTOTYPE(idRefinery);

							idRefinery(void);
	virtual					~idRefinery(void);

	void					Save(idSaveGame *savefile) const;
	void					Restore(idRestoreGame *savefile);

	void					Spawn(void);

	void					Event_Activate(idEntity* activator);
	
	virtual void			Think(void);
	virtual void			Damage(idEntity *inflictor, idEntity *attacker, const idVec3 &dir, const char *damageDefName, const float damageScale, const int location, const int materialType = SURFTYPE_NONE);

private:

	int						refineryState;
	enum					{ REFINERY_IDLE, REFINERY_MOVING, REFINERY_CHARGING,  REFINERY_SPRAYING, REFINERY_POSTSPRAY, REFINERY_STATIC };

	int						stateTimer;

	bool					initialized;
	idEntityPtr<idEntity>	pathEnt;
	int						rotationCounter;

	idFuncEmitter			*smokeEmitters[4];
	const idDeclSkin		*skins[2];


	idFuncEmitter			*algaeEmitter;
	idFuncEmitter			*splashEmitter;

	idLight *				sirenLight;

	int						splashTimer;

};
//#pragma once