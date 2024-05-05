#pragma once
#include "ClipboardManager.hpp"

#include <Windows.h>

#include <string>
#include <functional>
#include <atomic>
#include <thread>

namespace clipmgr
{
    class ClipboardListener
    {
    public:
        ClipboardListener(const ClipboardListener&) = delete;
        void operator=(const ClipboardListener&) = delete;
        
        ~ClipboardListener();

        static ClipboardListener& getInstance();

        void enable();

    private:
        const WCHAR* windowClassName = L"71F1D127-CF0B-406E-A021-0B5C9E048237";
        HWND messageOnlyWindowHandle = nullptr;
        ATOM windowClassAtom = 0;
        std::thread messagesThread{};
        std::atomic_flag processWindowMessages{};
        std::atomic_bool registered = false;
        ClipboardManager clipboardManager{};

        ClipboardListener() = default;

        bool createPrivateWindow();
        void notifyWindowReady();
        bool setListenerRegistered(const bool& success);

        void processMessages();

        static LRESULT MessageOnlyWindowProc(HWND unnamedParam1, UINT unnamedParam2, WPARAM unnamedParam3, LPARAM unnamedParam4);
    };
}

namespace clipmgr::win32
{
    template<typename T, typename _BoolConvertible>
    class repeat_func
    {
    public:
        repeat_func(const std::function<_BoolConvertible(T)>& function, const uint32_t& max) :
            max{ max },
            function{ function }
        {
            static_assert(std::convertible_to<_BoolConvertible, bool>);
        }

        bool operator()(const T& parameter)
        {
            uint32_t count = 0;
            bool success = false;
            while ((count++) < max)
            {
                if (function(parameter))
                {
                    success = true;
                    break;
                }
            }

            return success;
        }

    private:
        std::function<_BoolConvertible(T)> function{};
        uint32_t max{};
    };
}