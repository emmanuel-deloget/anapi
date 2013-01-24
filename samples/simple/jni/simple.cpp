#include "anapi/anapi_app.h"

// step 1: inherit a class from anapi::app
class simple_app : public anapi::app
{
public:
	simple_app()
	: anapi::app()
	{ }

	virtual ~simple_app()
	{ }
};

// step 2: instantiate that class as a global
simple_app my_app;
