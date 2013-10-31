// Ppbox.cpp

#include <windows.h>

#include "Ppbox.h"

using namespace PpboxRT;
using namespace Platform;

Error Ppbox::start(
    String ^ gid, 
    String ^ pid, 
    String ^ auth)
{
	char gidc[MAX_PATH];
	char pidc[MAX_PATH];
	char authc[MAX_PATH];

	WideCharToMultiByte(CP_ACP, 0, gid->Data(), -1, gidc, MAX_PATH, 0, 0);
	WideCharToMultiByte(CP_ACP, 0, pid->Data(), -1, pidc, MAX_PATH, 0, 0);
	WideCharToMultiByte(CP_ACP, 0, auth->Data(), -1, authc, MAX_PATH, 0, 0);

    Error ec = (Error)PPBOX_StartEngine(gidc, pidc, authc);
    return ec;
}

Error Ppbox::stop()
{
    Error ec = (Error)PPBOX_StopEngine();
	return ec;
}
