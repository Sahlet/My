//Properties.cpp

#include "Properties.h"

prop_helper::~prop_helper()
{
	if(remove_func)
	remove_func();
}
