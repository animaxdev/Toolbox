#ifndef TOOLBOX_EVENT_HPP
#define TOOLBOX_EVENT_HPP

/*
 * Toolbox/Event.hpp
 *
 * A generic event system.
 */


/*****************************************************************************
 * Example Program:

#include <Toolbox/Event.hpp>
#include <Toolbox/Memory.hpp>


void EventHandlerFunc( const Toolbox::Event::Data &data = Toolbox::Event::Data() )
{
	std::cout << "(EventHandlerFunc) - Handling \"Event1\"" << std::endl;

	for ( auto d = data.begin(), d_end = data.end(); d != d_end; ++d )
		std::cout << "    " << d->first << " : " << d->second << std::endl;
}


class EventHandlerObject : public Toolbox::Event::Listener
{
public:
	EventHandlerObject()
	{
		SetHandler( "Event2", std::bind(&EventHandlerObject::HandlerFunc, *this, std::placeholders::_1) );
	}

    void HandlerFunc( const Toolbox::Event::Data &data = Toolbox::Event::Data() )
	{
		std::cout << "(EventHandlerObject::HandlerFunc) - Handling \"Event2\"" << std::endl;

		for ( auto d = data.begin(), d_end = data.end(); d != d_end; ++d )
			std::cout << "    " << d->first << " : " << d->second << std::endl;
	}
};


int main( int argc, char *argv[] )
{
	int ReturnCode = 0;

    try
    {
		Toolbox::Event::Listener::Ptr Listener = std::make_shared< Toolbox::Event::Listener >();
		Listener->SetHandler( "Event1", EventHandlerFunc );

		EventHandlerObject::Ptr Object = std::make_shared< EventHandlerObject >();

		Toolbox::Event::Emitter Source;
		Source.AddListener( Listener );
		Source.AddListener( Object );

		// Toolbox::Event::Data is basically a std::map< std::string, Toolbox::Any >
		Toolbox::Event::Data Event1Data, Event2Data;
		Event1Data["Thing 1"] = 5;
		Event1Data["Thing 2"] = 5.9876;
		Event1Data["Thing 3"] = "info string";

		Event2Data["Thing 1"] = 1;
		Event2Data["Thing 2"] = 1.432;
		Event2Data["Thing 3"] = "info string";

		Source.Emit( "Event1", Event1Data );
		Source.Emit( "Event2", Event2Data );
    }
    catch ( std::exception &ex )
    {
		std::cerr << "Fatal error: " << ex.what() << std::endl;
		ReturnCode = 1;
    }

    return ReturnCode;
}


// Build with:
// g++ -std=c++14 -Wall -pedantic -o example
 ****************************************************************************/

/****************************************************************************/
/****************************************************************************/


#include <map>
#include <string>

#include <Toolbox/Any.hpp>
#include <Toolbox/Memory.hpp>


namespace Toolbox
{
	namespace Event
	{
		typedef std::string	Type;


		//
		// Toolbox::Event:Data is basically a std::map< std::string, Toolbox::Any >
		//
		class Data : public std::enable_shared_from_this< Data >
		{
		public:
			typedef std::string							KeyType;
			typedef std::map< KeyType, Any >			DataType;
			typedef DataType::iterator					iterator;
			typedef DataType::const_iterator			const_iterator;
			typedef DataType::reverse_iterator			reverse_iterator;
			typedef DataType::const_reverse_iterator	const_reverse_iterator;

		public:
			Data()
			{
			}

			virtual ~Data()
			{
			}

			DataType::iterator begin()
			{
				return _Data.begin();
			}

			const DataType::const_iterator begin() const
			{
				return _Data.begin();
			}

			DataType::reverse_iterator rbegin()
			{
				return _Data.rbegin();
			}

			const DataType::const_reverse_iterator rbegin() const
			{
				return _Data.rbegin();
			}

			DataType::iterator end()
			{
				return _Data.end();
			}

			const DataType::const_iterator end() const
			{
				return _Data.end();
			}

			DataType::reverse_iterator rend()
			{
				return _Data.rend();
			}

			const DataType::const_reverse_iterator rend() const
			{
				return _Data.rend();
			}

			Any &operator[]( const KeyType &key )
			{
				return _Data[ key ];
			}

			const Any operator[]( const KeyType &key ) const
			{
				return _Data.at( key );
			}

		protected:
			DataType	_Data;
		};


		class Listener
		{
		public:
			TOOLBOX_MEMORY_POINTERS_AND_LISTS( Listener )
			typedef std::function< void( const Data &data ) >	EventHandlerFunc;
			typedef std::map< Type, EventHandlerFunc >			HandlerFuncs;

		public:
			void SetHandler( const Type &type, EventHandlerFunc handler )
			{
				_Handlers[ type ] = handler;
			}

			void HandleEvent( const Type &type, const Data &data = Data() )
			{
				auto HandlerFunc = _Handlers.find( type );

				if ( HandlerFunc != _Handlers.end() )
					HandlerFunc->second( data );
			}

		protected:
			HandlerFuncs	_Handlers;
		};


		class Emitter
		{
		public:
			TOOLBOX_MEMORY_POINTERS_AND_LISTS( Emitter )

		public:
			void AddListener( Listener::Ptr listener )
			{
				// Check for duplicates first
				for ( auto l = Listeners.begin(), l_end = Listeners.end(); l != l_end; ++l )
				{
					Listener::Ptr CurListener = l->lock();

					// If something no longer exists, remove it from the list
					if ( !CurListener )
					{
						l = Listeners.erase( l );
						--l;						// Backup the pointer to accomodate our loop iterator
						continue;
					}

					if ( CurListener == listener )
						return;
				}

				Listeners.push_back( listener );
			}

			void RemoveListener( Listener::Ptr listener )
			{
				for ( auto l = Listeners.begin(), l_end = Listeners.end(); l != l_end; ++l )
				{
					Listener::Ptr CurListener = l->lock();

					// If something no longer exists, remove it from the list
					if ( !CurListener )
					{
						l = Listeners.erase( l );
						--l;						// Backup the pointer to accomodate our loop iterator
						continue;
					}

					if ( CurListener == listener )
					{
						Listeners.erase( l );
						return;
					}
				}
			}


			void Emit( const Type &type, const Data &data = Data() )
			{
				for ( auto l = Listeners.begin(), l_end = Listeners.end(); l != l_end; ++l )
				{
					Listener::Ptr CurListener = l->lock();

					// If something no longer exists, remove it from the list
					if ( !CurListener )
					{
						l = Listeners.erase( l );
						--l;						// Backup the pointer to accomodate our loop iterator
						continue;
					}

					CurListener->HandleEvent( type, data );
				}
			}

		protected:
			Listener::wList		Listeners;
		};
	}
}


#endif // TOOLBOX_EVENT_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper

