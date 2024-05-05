#include "ClipboardManager.hpp"

#include <stdexcept>
#include <iostream>
#include <functional>

using namespace clipmgr;

std::wstring ClipboardManager::getLastEntry()
{
    auto_managed_clipboard clipboard{};

    if (!clipboard.open())
    {
        throw std::runtime_error("ClipboardManager failed to open clipboard.");
    }

    std::wstring text{};

    const uint32_t format = CF_UNICODETEXT;
    if (IsClipboardFormatAvailable(format))
    {
        HANDLE handle = GetClipboardData(format);
        if (handle != nullptr)
        {
            //TODO: Better convertion between unicode text to wstring. Use multibyte encoding.
            auto chars = static_cast<wchar_t*>(GlobalLock(handle));
            if (chars != nullptr)
            {
                std::unique_ptr<void, std::function<BOOL(HGLOBAL)>> raiiLockedHandle{ handle, GlobalUnlock };
                text = std::wstring(chars);
            }
        }
    }

    return text;
}
