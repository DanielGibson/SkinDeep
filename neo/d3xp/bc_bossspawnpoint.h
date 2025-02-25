#include "Misc.h"
#include "Entity.h"


class idBossSpawnpoint : public idEntity
{
public:
	CLASS_PROTOTYPE(idBossSpawnpoint);

	void					Spawn(void);

	void					Save( idSaveGame *savefile ) const;
	void					Restore( idRestoreGame *savefile );

	

//private:
//	int						state;

	

};
//#pragma once