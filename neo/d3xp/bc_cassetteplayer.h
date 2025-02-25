#include "Misc.h"
#include "Entity.h"


class idCassettePlayer : public idStaticEntity
{
public:
	CLASS_PROTOTYPE(idCassettePlayer);

							idCassettePlayer(void);
	virtual					~idCassettePlayer(void);

	void					Save(idSaveGame *savefile) const;
	void					Restore(idRestoreGame *savefile);

	void					Spawn(void);
	virtual void			Event_PostSpawn(void);

	virtual void			Think(void);

	//virtual bool			DoFrob(int index = 0, idEntity * frobber = NULL);

	virtual void			DoGenericImpulse(int index);

private:

	idListGUI*				tracklist; //the gui music track list.


	idList<idStr>           soundshaderList; //list of sound shaders

};
//#pragma once