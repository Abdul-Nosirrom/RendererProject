#pragma once
#include "RomanceWin.h"
#include <string>

namespace HRErrors
{
    inline std::string TranslateErrorCode(HRESULT hr)
    {
        char* pMsgBuf = nullptr;
        // Format Message takes hResult and returns a readable error message, filling out our char*. Returns length of error string
        DWORD nMsgLen = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |                 /*  Allocates a buffer for our message, lets pMsgBuf point to it */
            FORMAT_MESSAGE_FROM_SYSTEM |                            /* Searches message table for a requested message. Allows us to use GetLastError() */
            FORMAT_MESSAGE_IGNORE_INSERTS,                          /* Ignores insert formatting (e.g %1), so output message can be formatted later */
            nullptr,
            hr,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<LPSTR>(&pMsgBuf),
            0, nullptr);                             /* nSize zero because we're using Allocate_Buffer, otherwise this would set the size of the output buffer. If Alloc flag is set, this is the min size */

        if (nMsgLen == 0)
        {
            return "Unidentified Error Code";
        }

        std::string errorString = pMsgBuf;
        LocalFree(pMsgBuf);
        return errorString;
    }
}