#include "DxgiInfoManager.h"
#include "Window.h"
#include "Graphics.h"
#include "Errors/WindowErrors.h"
#include <dxgidebug.h>
#include <memory>


#pragma comment(lib, "dxguid.lib")

#define GFX_THROW_NOINFO(hrcall) if(HRESULT hr = hrcall; FAILED(hr)) throw Graphics::HrException(__LINE__, __FILE__, hr)

DxgiInfoManager::DxgiInfoManager()
{
    // Defines the function signature of DXGIGetDebugInterface
    typedef HRESULT (WINAPI* DXGIGetDebugInterface) (REFIID, void**);

    const auto hModDxgiDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hModDxgiDebug == nullptr)
    {
        throw RDWND_LAST_EXCEPT();
    }

    // Get adress of DXGIGetDebugInterface in the dll we just loaded
    const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
        reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface")));
    if (DxgiGetDebugInterface == nullptr)
    {
        throw RDWND_LAST_EXCEPT();
    }

    
    GFX_THROW_NOINFO(DxgiGetDebugInterface(UUID(IDXGIInfoQueue), (&pDxgiInfoQueue)));
}

void DxgiInfoManager::Set() noexcept
{
    // Set the index (next) so that the next call to GetMessages() will only get messages generated after this call
    next = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL); // All messages
}

std::vector<std::string> DxgiInfoManager::GetMessages() const
{
    std::vector<std::string> messages;
    const auto lastMsg = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);

    // Collect all messages since last time Set was called
    for (auto i  = next; i < lastMsg; i++)
    {
        SIZE_T messageLength;

        // Get the size of message i in bytes, pass nullptr to message such that we only fill out the length of it first
        GFX_THROW_NOINFO(pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength));

        // Allocate memory for the message then fill it out
        auto bytes = std::make_unique<byte[]>(messageLength);
        auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());

        // Get the message and push its description to the vector
        GFX_THROW_NOINFO(pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &messageLength));
        messages.emplace_back(pMessage->pDescription);
    }

    return messages;
}

