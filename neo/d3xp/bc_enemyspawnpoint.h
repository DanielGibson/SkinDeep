#include "Misc.h"
#include "Entity.h"


class idEnemySpawnpoint : public idEntity
{
public:
	CLASS_PROTOTYPE(idEnemySpawnpoint);

	void					Spawn(void);

	void					Save( idSaveGame *savefile ) const;
	void					Restore( idRestoreGame *savefile );

	

//private:
//	int						state;

	

};
//#pragma once