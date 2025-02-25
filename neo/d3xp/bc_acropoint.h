#include "Misc.h"
#include "Entity.h"



class idAcroPoint : public idAnimated
{
public:
	CLASS_PROTOTYPE(idAcroPoint);

	void					Spawn(void);

	void					Save( idSaveGame *savefile ) const;
	void					Restore( idRestoreGame *savefile );

	//virtual void			Think( void );
	//virtual void			Present(void);

	int						state;

	float					baseAngle;

	virtual bool			DoFrob(int index = 0, idEntity * frobber = NULL);

//private:

	

};
//#pragma once