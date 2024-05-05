#include "src/KeyboardListener.hpp"
#include "src/ClipboardListener.hpp"

#include <Windows.h>

#include <string>
#include <vector>
#include <iostream>

int main(int argc, char* argv[])
{
    //std::vector<std::string> args{ &argv[0], &argv[0 + argc]};
    try
    {
        clipmgr::ClipboardListener& clipboardListener = clipmgr::ClipboardListener::getInstance();
        clipboardListener.enable();

        clipmgr::KeyboardListener keyboardListener{ true, MOD_ALT, L'C' };
        keyboardListener.startListening([]()
        {
            std::cout << "[main]  Hot key pressed." << std::endl;
        });
    }
    catch (std::exception ex)
    {
        std::cout << "[main]  Exception occured: " << ex.what() << std::endl;
    }
}