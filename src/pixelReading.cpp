// C++ Program to implement
// Read Screen Pixels 
#include <windows.h>
#include <iostream>
#include <vector>
#include <thread> // async
#include <functional> // for function callbacks

using namespace std;

bool pExit = false;

/*
void pressAutoKey(WORD vk) {
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vk;
    input.ki.dwFlags = 0; // key down
    SendInput(1, &input, sizeof(INPUT));
}

void releaseAutoKey(WORD vk) {
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vk;
    input.ki.dwFlags = KEYEVENTF_KEYUP; // key up
    SendInput(1, &input, sizeof(INPUT));
}
*/


struct keyBinding {
    int vk;
    function<void()> callback;
};

void mouseAutoKey(WORD vk, int posX, int posY) {
    INPUT Inputs[3] = { 0 };

    // For correct position
    int absX = (posX * 65535) / GetSystemMetrics(SM_CXSCREEN);
    int absY = (posY * 65535) / GetSystemMetrics(SM_CYSCREEN);

    Inputs[0].type = INPUT_MOUSE;
    Inputs[0].mi.dx = absX; // desired X coordinate
    Inputs[0].mi.dy = absY; // desired Y coordinate
    Inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

    Inputs[1].type = INPUT_MOUSE;
    Inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

    Inputs[2].type = INPUT_MOUSE;
    Inputs[2].mi.dwFlags = MOUSEEVENTF_LEFTUP;

    SendInput(3, Inputs, sizeof(INPUT));

    std::cout << "Left clicked!" << endl;

    Sleep(50);
}

bool colorMatch(COLORREF a, COLORREF b, int tolerance = 5)
{
    int dr = abs((int)GetRValue(a) - (int)GetRValue(b));
    int dg = abs((int)GetGValue(a) - (int)GetGValue(b));
    int db = abs((int)GetBValue(a) - (int)GetBValue(b));

    return dr <= tolerance && dg <= tolerance && db <= tolerance;
}

vector<keyBinding> keyBindings;
void inputCallback() {
    while (pExit != true) {     
        for (auto& bind : keyBindings) {
            if ((GetAsyncKeyState(bind.vk) & 0x0001) != 0) {
                bind.callback();
            }
        }
        
        Sleep(50); // avoid demolishing CPU
    }
}

// Some better methods for the input handling
bool isKeyPressed(int btn) {
    return (GetAsyncKeyState(btn) & 0x0001) != 0;
}

bool isKeyDown(int btn) {
    return (GetAsyncKeyState(btn) & 0x8000) != 0;
}

int main()
{
    // Assigning the device context to the current output device
    HDC dng = GetDC(NULL);

    int x = 0, y = 0;
    bool locationLock = false, isRunning = true;

    COLORREF c;
    //COLORREF targetColor = RGB(0, 0, 0);
    COLORREF targetColor = RGB(75, 219, 106); 
    COLORREF altColor = RGB(33, 33, 33);//rgb(33, 33, 33)

    POINT p;

    // Async inputs
    keyBindings.push_back({ 'E',[&]() 
        {
            if (GetCursorPos(&p) && isRunning) {
            // For correct position
            x = p.x;
            y = p.y;

            std::cout << "Targetting pixels: x:" << x << " - y:" << y << endl;
            locationLock = true;
            }
        } 
    });

    keyBindings.push_back({ VK_TAB,[&]()
        {
            isRunning = !isRunning;
            std::cout << "Run mode: " << isRunning << endl;
        }
    });

    keyBindings.push_back({ VK_ESCAPE,[&]()
        {
            exit(0);
        }
    });

    thread(inputCallback).detach();
    
    // Async inputs would be better
    while (locationLock != true) {
        std::cout << "";
    }

    std::cout << "Wait..." << endl;
    Sleep(1000);
    std::cout << "NOW" << endl;
    while (pExit != true) {
        
        c = GetPixel(dng, x, y);
        if(colorMatch(c, targetColor) && isRunning)
            mouseAutoKey(MOUSEEVENTF_LEFTDOWN, x, y);
        

        /*
        c = GetPixel(dng, x, y);

        if (colorMatch(c, targetColor) && isRunning) {
            pressAutoKey('D');
            Sleep(30); // avoid demolishing CPU
            releaseAutoKey('D');
            Sleep(150); // avoid demolishing CPU
            
        }
        else if (colorMatch(c, altColor) && isRunning) {
            pressAutoKey('A');
            Sleep(30); // avoid demolishing CPU
            releaseAutoKey('A');
            Sleep(150); // avoid demolishing CPU
        }
        */
    }

    // Releasing the Handle
    ReleaseDC(NULL, dng);
    return 0;
}