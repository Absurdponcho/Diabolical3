#include "Check.h"
#include "Types/DString.h"
#include "Logging/Logging.h"
//#include "DiabolicalEngine.h"

void CheckCrash(const char* Func, const char* Reason, int Line)
{
	DString ErrorMessage = DString::Format("CHECK FAILED! %s (%i): %s\n", Func, Line, Reason);
	Logging::LogPlain(ErrorMessage, 12);
	//DEngine::AtExit();
	abort();

	(*(uint8_t*)0) = 0; // trigger an exception to get a call stack
}

