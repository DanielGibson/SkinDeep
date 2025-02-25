#include "Misc.h"
#include "Entity.h"
//#include "Mover.h"
//#include "Item.h"


class idTrashExit : public idStaticEntity
{
public:
	CLASS_PROTOTYPE(idTrashExit);

							idTrashExit(void);
	virtual					~idTrashExit(void);

	void					Save(idSaveGame *savefile) const;
	void					Restore(idRestoreGame *savefile);

	void					Spawn(void);

	virtual bool			DoFrob(int index = 0, idEntity * frobber = NULL);
	virtual bool			DoFrobHold(int index = 0, idEntity * frobber = NULL);

	void					SetupChute(idEntity * ent, bool isOpen);

	void					Event_ChuteEnable(int value);
	
	bool					IsFuseboxTrashgateShut();

private:

	void					Event_SpectatorTouch(idEntity *other, trace_t *trace);
	idVec3					FindClearExitPosition();
	void					DoPlayerTeleport(idVec3 position);

	idEntityPtr<idEntity>	myChute;

	class idVentpeek		*peekEnt;

	idEntity				*gateModel;

};
//#pragma once