#include "App.h"
#include "Window.h"
#include "Log.h"


/// <summary>
/// For a "Windows" application, the default entry point is WinMain, NOT Main
/// 'CALLBACK'/'WINAPI' is stdcall, which is an API declspec type thing for functions the window API will call (like this one)
/// </summary>
int WINAPI WinMain(
	_In_		HINSTANCE	hInstance,		/* Handle to application instance used to identify the EXE*/
	_In_opt_	HINSTANCE	hPrevInstance,	/* Docs say has no meaning, was used in 16-bit, but now is always zero */
	_In_		LPSTR		lpCmdLine,		/* Contains command line arguments as a unicode string */
	_In_		int			nShowCmd)		/* Flag that indicates whether the main app window is minimized, maximized, or shown normally (See docs for specific flags) */
{
	try
	{
		return App{}.Go();
	}
	catch (const RomanceException& e)
	{
		// Setting first param to null (window Handle) means message box window has no parent window
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, "No details available", "Unkown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}