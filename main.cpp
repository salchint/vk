#include <cctype>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <array>

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

namespace key_flag
{
    enum Value : uint8_t
    {
        None,
        Unicode,
        Extended,
        MAX
    };
}

const WORD get_key(const char* argv, key_flag::Value& flag)
{
    static bool use_literal_numbers {false};

    bool is_codepoint = true;
    flag = key_flag::None;

    // Check if the input is a literal character or a code point
    const char* pchar = argv;
    for(; *pchar != '\0'; ++pchar)
    {
        //std::cout << "Check if digit: " << *pchar << std::endl;
        if (!std::isdigit(*pchar))
        {
            is_codepoint = false;
        }
    }

    if (std::string("--") == argv)
    {
        use_literal_numbers = true;
        return 0;
    }

    if (is_codepoint && !use_literal_numbers)
    {
        const auto key = static_cast<WORD>(std::strtol(argv, nullptr, 10));
        flag = (key >= 0x100) ? key_flag::Extended : key_flag::None;
        return key;
    }

    flag = key_flag::Unicode;
    return argv[0];
}

void usage()
{
    using namespace std;
    cout << "Usage: vk [--|CODEPOINT|LITERAL_CHARACTER]" << endl;
    cout << endl;
    cout << "  Simulate keyboard input. All arguments shall be separated by spaces and are" << endl;
    cout << "  treated as virtual keys in Unicode format. Except if they are decimal numbers," << endl;
    cout << "  in which case they represent code-points, unless they are preceeded by a" << endl;
    cout << "  double-hyphen." << endl;
    cout << endl;
}

void move_window_to_background()
{
    //auto hwnd = get_console_hwnd();
    auto hwnd = ::GetConsoleWindow();
    if (hwnd)
    {
        ::ShowWindow(hwnd, SW_MINIMIZE);
    }
}

int main(int argc, char* argv[])
{
    using namespace std;

    const auto keysCount {argc-1};

    if (keysCount == 0)
    {
        usage();
        return 0;
    }

    vector<INPUT> inputs(keysCount);

    for (auto i{0}; i<keysCount; ++i)
    {
        key_flag::Value flag {key_flag::None};
        const WORD key {get_key(argv[i+1], flag)};
        if (key)
        {
            ::ZeroMemory(&(inputs[i]), sizeof(INPUT));
            inputs[i].type = INPUT_KEYBOARD;
            switch (flag)
            {
            case key_flag::Unicode:
                cout << "Sending unicode input: " << key << endl;
                inputs[i].ki.wScan = key;
                inputs[i].ki.dwFlags = KEYEVENTF_UNICODE;
                break;
            case key_flag::Extended:
                cout << "Sending extended input: " << key << endl;
                inputs[i].ki.wScan = 0xE000 + key;
                inputs[i].ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
                break;
            default:
                cout << "Sending input: " << key << endl;
                inputs[i].ki.wVk = key;
                break;
            }
        }
    }

    move_window_to_background();
    ::Sleep(1000);
    ::SendInput(keysCount, inputs.data(), sizeof(INPUT));

    return 0;
}
