
#include <windows.h>
#include <fstream>
#include <string>
#include <iostream>

#include <iomanip>
#include <ctime>
#include <conio.h>

#include <thread>

char keyToHook = 'Q';
char keyToSend = 'W';
int sleepTime = 10;





// The hook procedure
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {

    static bool ignore = false;

    if (nCode == HC_ACTION) {
        // Get the key information
        KBDLLHOOKSTRUCT* keyStruct = (KBDLLHOOKSTRUCT*)lParam;

        // Check for a specific key (e.g., 'A')
        if (keyStruct->vkCode == keyToHook) {

            if ((wParam == WM_KEYUP || wParam == WM_SYSKEYUP) && ((keyStruct->flags & LLKHF_UP) != 0)) {

                INPUT input = { 0 };
                input.type = INPUT_KEYBOARD;
                input.ki.wVk = keyToSend;
                SendInput(1, &input, sizeof(INPUT));

                clock_t delta = (CLOCKS_PER_SEC * sleepTime) / 1000;
                clock_t now = clock();
                clock_t end = now + delta;
                do {
                    Sleep(1); //1ms
                    now = clock();
                } while((now < end) && !kbhit());

                ZeroMemory(&input, sizeof(INPUT));
                input.type = INPUT_KEYBOARD;
                input.ki.wVk = keyToSend;
                input.ki.dwFlags = KEYEVENTF_KEYUP;
                SendInput(1, &input, sizeof(INPUT));

                ignore = false;

                return 1;

            }
            else{
                return 1;
            }
        }
    }

    // Call the next hook in the chain
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main() {


    // Read the key from the config file
    std::ifstream configFile("config.txt");
    std::string line;

    if (std::getline(configFile, line)) {
        size_t startPos = line.find('\'');
        size_t endPos = line.rfind('\'');
        if (startPos != std::string::npos && endPos != std::string::npos && startPos < endPos) {
            keyToHook = line[startPos + 1];
        }
    }

    std::cout << "Hacked Key: " << keyToHook << std::endl;

    if (std::getline(configFile, line)) {
        size_t startPos = line.find('\'');
        size_t endPos = line.rfind('\'');
        if (startPos != std::string::npos && endPos != std::string::npos && startPos < endPos) {
            keyToSend = line[startPos + 1];
        }
    }

    std::cout << "Key To Send: " << keyToSend << std::endl;

    if (std::getline(configFile, line)) {

        sleepTime = std::stoi(line.substr(line.find("=")+1, line.length()));
    }

    configFile.close();

    std::cout << "Press Length: " << sleepTime << std::endl;

    HHOOK keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);

    // Main message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Remove the hook
    UnhookWindowsHookEx(keyboardHook);

    return 0;
}