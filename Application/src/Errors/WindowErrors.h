#pragma once


// NOTE: Don't throw exceptions in WinAPI/Callback functions, will just crash the program and will not allow us to retrieve error info as its windows that calls it not our program
// To do the above, we catch the exceptions in those WINAPI functions, store them, and them throw them later in our own functions
/* Error exception helper macros for windows */
#define RDWND_EXCEPT(hr) Window::HrException(__LINE__, __FILE__, hr) // useful macros for filling out line number and file location
#define RDWND_LAST_EXCEPT() Window::HrException(__LINE__, __FILE__, GetLastError())
#define RDWND_NOGFX_EXCEPT() Window::NoGFXException(__LINE__, __FILE__)