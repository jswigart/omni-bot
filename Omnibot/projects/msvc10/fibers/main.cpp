#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <assert.h>
#include <iostream>

//#include "common.pb.h"
#include <google/protobuf/text_format.h>

//////////////////////////////////////////////////////////////////////////

class Fiber
{
public:	
	friend class FiberManager;

	static void __stdcall FiberWork( void * data )
	{
		Fiber * thisFiber = static_cast<Fiber*>( data );
		// run the fiber

		// free the fiber
		DeleteFiber( thisFiber->fiber );
		thisFiber->fiber = NULL;
	}

	bool IsRunning() const { return fiber != NULL; }

	void Execute() {
		SwitchToFiber( fiber );
	}

	Fiber() : nextFiber( NULL ) {
		fiber = CreateFiber( 0, FiberWork, this );
	}
protected:
	Fiber *			nextFiber;
private:
	void *			fiber;	
};

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

class FiberManager
{
public:
	void Init()
	{
		primaryFiber = ConvertThreadToFiber( NULL );
	}
	void Shutdown()
	{
		ConvertFiberToThread();
	}
	void UpdateFibers()
	{
		Fiber * lastFiber = NULL;
		Fiber * currentFiber = firstFiber;
		while ( currentFiber ) {
			currentFiber->Execute();
			// delete it if it's done
			if ( !currentFiber->IsRunning() ) {
				if ( currentFiber == firstFiber ) {
					firstFiber = currentFiber->nextFiber;
				} else {
					assert( lastFiber );
					lastFiber->nextFiber = currentFiber->nextFiber;
				}
				delete currentFiber;
			}
			currentFiber = currentFiber->nextFiber;
		}
	}
	bool HasActiveFibers() const {
		return firstFiber != NULL;
	}
	/*void wait()
	{
		SwitchToFiber( primaryFiber );
	}
	void println(const char* _msg, ...)
	{
		static char buffer[8192] = {0};
		va_list list;
		va_start(list, _msg);
#ifdef WIN32
		_vsnprintf(buffer, 8192, _msg, list);	
#else
		vsnprintf(buffer, 8192, _msg, list);
#endif
		va_end(list);
		std::cout << buffer << std::endl;
	}*/

	FiberManager() : primaryFiber( NULL ) {
	}
private:
	void * primaryFiber;

	Fiber * firstFiber;
};

//////////////////////////////////////////////////////////////////////////

//class MoveTo : public Fiber
//{
//public:
//	virtual void operator()( FiberManager * sys )
//	{
//		for ( int i = 0; i < 10; ++i ) {
//			sys->println( "moveto: %d %d %d", params[0],params[1]+i,params[2]+i );
//			sys->wait();
//		}
//	}	
//
//	MoveTo( int a = 0, int b = 0, int c = 0 )
//	{
//		params[0] = a;
//		params[1] = b;
//		params[2] = c;
//	}
//private:
//	int params[3];
//};

//////////////////////////////////////////////////////////////////////////

int main(int argc,const char **argv)
{
	//Remote::Game game;
	////game.set_name( "Test Game" );
	////google::protobuf::C
	//for ( int i = 0; i < 1024; ++i ) {
	//	Remote::Waypoint * wp = game.add_waypoints();
	//	wp->set_uid( i );
	//	wp->mutable_position()->set_x( 10 );
	//	wp->mutable_position()->set_y( 20 );
	//	wp->mutable_position()->set_z( 30 );
	//	
	//	wp->mutable_facing()->set_x( 10 );
	//	wp->mutable_facing()->set_y( 20 );
	//	wp->mutable_facing()->set_z( 30 );

	//	//wp->set_name( "Test Wp" );
	//	wp->set_radius( 90 );
	//}
	//for ( int i = 0; i < 1024; ++i ) {
	//	Remote::Entity * ent = game.add_entities();
	//	ent->set_uid( i );
	//	ent->mutable_position()->set_x( 10 );
	//	ent->mutable_position()->set_y( 20 );
	//	ent->mutable_position()->set_z( 30 );

	//	ent->mutable_facing()->set_x( 10 );
	//	ent->mutable_facing()->set_y( 20 );
	//	ent->mutable_facing()->set_z( 30 );

	//	ent->set_classid( 3034 );
	//	ent->set_teamid( 200 );

	//	ent->set_health( 100 );
	//	ent->set_healthmax( 100 );

	//	ent->set_armor( 100 );
	//	ent->set_armormax( 100 );

	//	ent->mutable_size()->set_x( 64 );
	//	ent->mutable_size()->set_y( 64 );
	//	ent->mutable_size()->set_z( 64 );

	//	ent->set_fov( 90 );


	//	//ent->set_name( "Entity Name" );
	//}

	//const int memSize = game.ByteSize();
	//
	//std::string stringOutput;
	//google::protobuf::TextFormat::PrintToString( game, &stringOutput );

	FiberManager fiberMan;

	fiberMan.Init();

	/*enum { NumFibers = 8 };
	for ( int i = 0; i < NumFibers; ++i ) {
		fiberMan.AllocFiber( new MoveTo( i, i * 10, i * 1000 ) );
	}*/
	
	while( fiberMan.HasActiveFibers() ) {
		fiberMan.UpdateFibers();
	}

	fiberMan.Shutdown();

	return 0;
}
