#include "Misc.h"
#include "Entity.h"


class idCassetteTape : public idStaticEntity
{
public:
	CLASS_PROTOTYPE(idCassetteTape);

							idCassetteTape(void);
	virtual					~idCassetteTape(void);

	void					Save(idSaveGame *savefile) const;
	void					Restore(idRestoreGame *savefile);

	void					Spawn(void);

	virtual void			Think(void);

	virtual bool			DoFrob(int index = 0, idEntity * frobber = NULL);

private:

	int						tapeIndex;


};
//#pragma once