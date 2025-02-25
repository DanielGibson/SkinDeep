#include "Misc.h"
#include "Entity.h"
#include "Mover.h"
//#include "Item.h"
#include "Light.h"


class idCryointerior : public idStaticEntity
{
public:
	CLASS_PROTOTYPE(idCryointerior);

							idCryointerior(void);
	virtual					~idCryointerior(void);

	void					Save(idSaveGame *savefile) const;
	void					Restore(idRestoreGame *savefile);

	void					Spawn(void);

	virtual void			Think(void);

	virtual bool			DoFrob(int index = 0, idEntity * frobber = NULL);

	void					SetExitPoint(idEntity *ent);
	

private:

	enum					{ IDLE, OPENING, MOVINGHEAD, DONE };
	int						state;
	int						stateTimer;

	idLight *				ceilingLight;
	idAnimated*				doorProp;

	idEntity*				frobBar;

	idEntityPtr<idEntity>	cryospawn;
	
	idEntity				*ventpeekEnt;

	int						iceMeltTimer;
	enum					{ ICEMELT_INITIALDELAY, ICEMELT_MELTING, ICEMELT_LIGHTFADE, ICEMELT_DONE };
	int						iceMeltState;

	idFuncEmitter			*dripEmitter;

	void					SetCryoFrobbable(int value);


};
//#pragma once