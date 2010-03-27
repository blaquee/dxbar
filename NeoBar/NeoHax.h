#include <strsafe.h>
#pragma warning( disable : 4995 ) 
#pragma warning( default : 4996 )
#include <string>
#include <vector>
#include <algorithm>
#include <Windows.h>
#include "AntTweakBar.h"

//structure for different Hacks
template <typename T >
struct MemHacks
{
	std::string &nameofHack; //identify the hack
	unsigned int address; //its address
	T& value;	//its value
};



class NeoHax
{

};