//Properties.cpp

#include "Properties.h"

namespace My {

prop_helper::~prop_helper()
{
	if(remove_func)
	remove_func();
}

} // namespace My
