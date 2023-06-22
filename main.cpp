#include <cctype>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <array>

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

const WORD get_key(const char* argv)
{
    static bool use_literal_numbers {false};

    bool is_codepoint = true;

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
        return static_cast<WORD>(std::strtol(argv, nullptr, 10));
    }

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
        const WORD key {get_key(argv[i+1])};
        if (key)
        {
            cout << "Sending input: " << key << endl;
            ::ZeroMemory(&(inputs[i]), sizeof(INPUT));
            inputs[i].ki.wScan = key;
            inputs[i].ki.dwFlags = KEYEVENTF_UNICODE;
            inputs[i].type = INPUT_KEYBOARD;
        }
    }

    ::SendInput(keysCount, inputs.data(), sizeof(INPUT));

    return 0;
}
