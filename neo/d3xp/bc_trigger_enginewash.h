#include "Trigger.h"

class idTrigger_enginewash : public idTrigger_Multi
{
public:
	CLASS_PROTOTYPE(idTrigger_enginewash);

						idTrigger_enginewash();
	virtual				~idTrigger_enginewash(void);
	void				Spawn();
	virtual void		Think(void);

	float				baseAngle;

private:
	void				Event_Touch(idEntity* other, trace_t* trace);
	
	int					timer;
	int					state;
	enum				{ EW_NONE, EW_PUSHING };
	
};
