#pragma once
#include <Windows.h>

#include <string>
#include <cstdint>
#include <vector>

namespace clipmgr
{
    class ClipboardManager
    {
    public:
        bool hasNewEntry();

        std::wstring getLastEntry();

    private:
        uint32_t lastClipboardSequenceNumber = 0;
    };

    class auto_managed_clipboard
    {
    public:
        ~auto_managed_clipboard()
        {
            if (opened)
            {
                close();
            }
        }

        bool open()
        {
            opened = OpenClipboard(nullptr);
            return opened;
        }

        bool close()
        {
            return CloseClipboard();
        }

    private:
        bool opened = false;
    };
}