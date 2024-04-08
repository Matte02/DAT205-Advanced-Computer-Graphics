#include "procedural_world.h"

#ifdef _WIN32
extern "C" _declspec(dllexport) unsigned int NvOptimusEnablement = 0x00000001;
#endif

int main(int argc, char* argv[])
{
	auto proceduralWorld = ProceduralWorld();
	proceduralWorld.Run();
	
	return 0;
}
