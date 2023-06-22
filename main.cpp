#include <cctype>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <array>

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

HWND get_console_hwnd()
{
    static constexpr auto SIZE {1024u};
    char org_title[SIZE];
    HWND hwnd{0};
    std::string title;

    ::GetConsoleTitle(org_title, SIZE);

    // Format a "unique" window title
    std::stringstream sout(title);
    sout << GetTickCount() << '/' << GetCurrentProcessId();

    ::SetConsoleTitle(title.data());
    ::Sleep(40);

    hwnd = ::FindWindow(NULL, title.data());
    ::SetConsoleTitle(org_title);

    std::cout << "Found hWND: " << hwnd << " (" << ::GetConsoleWindow() << ")" << std::endl;
    return(hwnd);
}

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
        //::SendMessage(get_console_hwnd(), WM_SYSCOMMAND, SC_PREVWINDOW, 0);
        //::ShowWindow(get_console_hwnd(), SW_MINIMIZE);

        array<char, 1000> text;
        auto hwnd = ::GetConsoleWindow();
        //auto hwnd = get_console_hwnd();
        //auto hwnd = ::GetWindow(get_console_hwnd(), GW_HWNDPREV);
        //hwnd = ::GetWindow(hwnd, GW_HWNDPREV);
        ::GetWindowTextA(hwnd, text.data(), (int)text.size());
        cout << "Activate window: " << text.data() << endl;
        ::ShowWindow(hwnd, SW_MAXIMIZE);
        //::SendMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
        //::PostMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
        //::SetActiveWindow(hwnd);

        //array<INPUT, 2> input;
        //::ZeroMemory(input.data(), 2*sizeof(INPUT));
        //input[0].type = INPUT_KEYBOARD;
        //input[0].ki = {VK_TAB, 0};
        //input[0].type = INPUT_KEYBOARD;
        //input[0].ki = {VK_TAB, 0, KEYEVENTF_KEYUP};
        //::SendInput(2, input.data(), sizeof(INPUT));

        ::Sleep(200);

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
