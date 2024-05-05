#include "ClipboardListener.hpp"

#include <intrin.h>

#include <iostream>

using namespace clipmgr;

ClipboardListener::~ClipboardListener()
{
    if (messageOnlyWindowHandle != nullptr)
    {
        PostMessageW(messageOnlyWindowHandle, WM_QUIT, 0, 0);
        messagesThread.join();
        DestroyWindow(messageOnlyWindowHandle);
    }

    if (windowClassAtom != 0)
    {
        UnregisterClassW(windowClassName, nullptr);
    }
}

ClipboardListener& ClipboardListener::getInstance()
{
    static ClipboardListener listener{};
    return listener;
}

void ClipboardListener::enable()
{
    if (createPrivateWindow())
    {
        if (registered)
        {
            notifyWindowReady();
            processMessages();
        }
    }
}


bool ClipboardListener::createPrivateWindow()
{
    bool success = false;

    WNDCLASSEXW windowClass{ sizeof(WNDCLASSEXW) };
    windowClass.lpfnWndProc = &ClipboardListener::MessageOnlyWindowProc;
    windowClass.hInstance = nullptr/*GetModuleHandleW(nullptr)*/;
    windowClass.lpszClassName = windowClassName;

    windowClassAtom = RegisterClassExW(&windowClass);
    if (windowClassAtom != 0)
    {
        messageOnlyWindowHandle = CreateWindowExW(0, windowClassName, L"Clipboard manager", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, nullptr, nullptr);
        if (messageOnlyWindowHandle != nullptr)
        {
            success = true;
        }
        else
        {
            std::cout << "[ClipboardListener]  Failed create window." << std::endl;
        }
    }
    else
    {
        std::cout << "[ClipboardListener]  Failed to register window class." << std::endl;
    }

    return success;
}

void ClipboardListener::notifyWindowReady()
{
    processWindowMessages.test_and_set();
    processWindowMessages.notify_all();
}

bool ClipboardListener::setListenerRegistered(const bool& success)
{
    registered = success;
    return success;
}

void ClipboardListener::processMessages()
{
    std::cout << "[ClipboardListener]  Waiting to process messages..." << std::endl;

    processWindowMessages.wait(false);
    processWindowMessages.clear();

    std::cout << "[ClipboardListener]  Window ready, processing messages." << std::endl;

    bool run = true;
    while (run)
    {
        MSG message{};
        PeekMessageW(&message, messageOnlyWindowHandle, 0, 0, PM_REMOVE);

        switch (message.message)
        {
            case WM_CLIPBOARDUPDATE:
            {
                auto lastEntry = clipboardManager.getLastEntry();
                std::wcout << L"[ClipboardListener]  Clipboard content's changed:\n" << (lastEntry.empty() ? L"(clipboard is empty)" : lastEntry) << std::endl;
                break;
            }
            case WM_QUIT:
            {
                run = false;
                break;
            }
        }
    }

    std::cout << "[ClipboardListener]  Message processing thread exiting." << std::endl;
}

LRESULT ClipboardListener::MessageOnlyWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto&& instance = getInstance();

    switch (msg)
    {
        case WM_CREATE:
        {
            win32::repeat_func<HWND, BOOL> addClipboardFormatListener{ AddClipboardFormatListener, 5 };
            if (instance.setListenerRegistered(addClipboardFormatListener(hwnd)))
            {
                std::cout << "[ClipboardListener]  Failed to add window to clipboard format listener list." << std::endl;
            }

            break;
        }
        
        case WM_DESTROY:
        {
            win32::repeat_func<HWND, BOOL> removeClipboardFormatListener{ RemoveClipboardFormatListener, 5 };
            if (!removeClipboardFormatListener(hwnd))
            {
                std::cout << "[ClipboardListener]  Failed to remove window to clipboard format listener list." << std::endl;
            }

            break;
        }

        default:
        {
            std::cout << "[ClipboardListener]  Unrecognized window message." << std::endl;
        }
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}
