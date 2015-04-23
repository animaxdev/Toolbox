#ifndef PLUGINS_SIMPLE1_H
#define PLUGINS_SIMPLE1_H

/*
 * SimpleTask plugin implementation called SimpleImple1
 */

#include <Toolbox/Plugin.hpp>

#include "../SimpleTask_interface.h"


DEFINE_TOOLBOX_PLUGIN( SimpleTask, SimpleImple1 )

	virtual const std::string &Operation()
	{
		static std::string Op( "* 5" );
		return Op;
	}

	virtual int Task( int input )
	{
		return input * 5;
	}

END_TOOLBOX_PLUGIN_DEF


extern "C"
{
	DEFINE_TOOLBOX_PLUGIN_FACTORY( SimpleTask, SimpleImple1 )

	// Optional plugin event functions
	// void onLoad();
	// void onUnload();
}


#endif // PLUGINS_SIMPLE1_H


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper

