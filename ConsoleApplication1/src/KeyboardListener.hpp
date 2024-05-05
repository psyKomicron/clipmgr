#pragma once
#include <cstdint>
#include <thread>
#include <optional>
#include <functional>

namespace clipmgr
{
    class KeyboardListener
    {
    public:
        using callback_t = std::function<void()>;

        KeyboardListener(const bool& once, const uint32_t& modifier, const wchar_t& key);
        ~KeyboardListener();

        void startListening(const callback_t& callback);
        void stopListening();

        void wait();

    private:
        const bool once;
        const wchar_t key;
        const uint32_t modifier;
        std::thread keyboardListenerThread{};
        std::atomic_flag threadRunning{};
        std::atomic_bool hotKeyRegistered{};
        std::atomic<uint32_t> id{};
        callback_t callback{};

        void listener();
    };
}
