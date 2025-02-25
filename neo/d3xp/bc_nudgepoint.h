#include "Misc.h"
#include "Entity.h"


class idNudgePoint : public idEntity
{
public:
	CLASS_PROTOTYPE(idNudgePoint);

							idNudgePoint(void);
	virtual					~idNudgePoint(void);

	void					Spawn(void);

	void					Save( idSaveGame *savefile ) const;
	void					Restore( idRestoreGame *savefile );


//private:

	

};
//#pragma once