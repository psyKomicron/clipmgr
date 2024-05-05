#include "KeyboardListener.hpp"

#include <windows.h>

#include <iostream>

using namespace clipmgr;

clipmgr::KeyboardListener::KeyboardListener(const bool& once, const uint32_t& modifier, const wchar_t& key) :
    once{ once },
    modifier{ modifier },
    key{ key }
{
}

KeyboardListener::~KeyboardListener()
{
    stopListening();
}

void KeyboardListener::startListening(const callback_t& callback)
{
    this->callback = callback;
    keyboardListenerThread = std::thread(&KeyboardListener::listener, this);
    threadRunning.wait(false);
    threadRunning.clear();

    if (once)
    {
        threadRunning.wait(false);
        threadRunning.clear();
    }
}

void KeyboardListener::stopListening()
{
    if (hotKeyRegistered)
    {
        UnregisterHotKey(nullptr, id);
    }
    
    if (threadRunning.test())
    {
        PostThreadMessageW(id, WM_QUIT, 0, 0);
    }

    keyboardListenerThread.join();
}

void KeyboardListener::wait()
{
    threadRunning.wait(false);
    threadRunning.clear();
}


void KeyboardListener::listener()
{
    threadRunning.test_and_set();

    auto threadId = GetCurrentThreadId();
    id.store(threadId);
    
    if (RegisterHotKey(nullptr, id, modifier, key))
    {
        threadRunning.notify_all();
        hotKeyRegistered = true;

        MSG message{};
        while (GetMessageW(&message, reinterpret_cast<HWND>(-1), 0, 0))
        {
            if (message.message == WM_HOTKEY)
            {
                callback();

                if (once)
                {
                    break;
                }
            }
        }
    }
    else
    {
        std::cout << "[KeyboardListener]  Failed to register hotkey." << std::endl;
    }

    threadRunning.test_and_set();
    threadRunning.notify_all();
}
