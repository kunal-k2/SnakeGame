// Snake.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>
#include <list>
#include <thread>
using namespace std;

struct snakeSegment
{
    int x;
    int y;
};

int screenHeight = 30;
int screenWidth = 120;

void GetDesktopResolution(int& horizontal, int& vertical)
{
    RECT desktop;
    // Get a handle to the desktop window
    const HWND hDesktop = GetDesktopWindow();
    // Get the size of screen to the variable desktop
    GetWindowRect(hDesktop, &desktop);
    // The top left corner will have coordinates (0,0)
    // and the bottom right corner will have coordinates
    // (horizontal, vertical)
    horizontal = desktop.right / 4;
    vertical = desktop.bottom / 4;
}
int main()
{
    // GetDesktopResolution(screenHeight, screenWidth);
    //create screen buffer
    wchar_t* screen = new wchar_t[screenHeight * screenWidth];
    int area = screenHeight * screenWidth;
    for (int i = 0;i < area;i++) screen[i] = L' ';

    HANDLE handle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(handle);
    DWORD dwBytesWritten = 0;

    while (1)
    {
        //Initialize Snake's body Segment
        std::list<snakeSegment> snake = { {40,20},{41,20},{42,20}, {43,20},{44,20}, {45,20},{46,20}, {47,20},{48,20} };
        
        //Initialize  food location, score, Snake move direction, life, keyEvent
        int nFoodx = 30;
        int nFoody = 10;
        int nScore = 0;
        int bDead = false;
        int nSnakeDirection = 3;
        bool bKeyLeft = false, bKeyRight = false, bKeyLeftOld = false, bKeyRightOld = false;
        bool bKeyUp = false, bKeyDown = false, bKeyUpOld = false, bKeyDownOld = false;
        int f1 = 0, f2 = 0;

        while (!bDead)
        {
            f1++;
            // Timing & Input
            // this_thread::sleep_for(200ms);
            // Frame Timing
            auto t1 = chrono::system_clock::now();
            // compensate for aspect ratio of command line on Y-axis 
            while ((chrono::system_clock::now() - t1) < ((nSnakeDirection % 2 == 1) ? 120ms : 200ms))
            {
                //this_thread::sleep_for(200ms); // it will slow your performance 
                f2++;
                //Get the Input
                //bKeyRight = (0x8000 & GetAsyncKeyState((unsigned char)('\x25'))) != 0;//25
                //bKeyLeft = (0x8000 & GetAsyncKeyState((unsigned char)('\x27'))) != 0;//27

                bKeyRight = (GetAsyncKeyState(VK_RIGHT));
                bKeyLeft  = (GetAsyncKeyState(VK_LEFT));
                bKeyUp    = (GetAsyncKeyState(VK_UP));
                bKeyDown  = (GetAsyncKeyState(VK_DOWN));
                // To acknowledge the input on change  of the input
                // to find when the key is pressed, comapre with it's prev state.
                // if we don't maintain the prev state, snake will coiling up and hitting itself 
                // as soon as the player presses an arrow key 
                // the current direction should  not be opposite of the pressed key direction
                if ((bKeyRight && !bKeyRightOld) && nSnakeDirection != 3)
                {
                    nSnakeDirection = 1;
                }
                if ((bKeyLeft && !bKeyLeftOld) && nSnakeDirection != 1)
                {
                    nSnakeDirection = 3;
                }
                if ((bKeyUp && !bKeyUpOld) && nSnakeDirection != 2)
                {
                    nSnakeDirection = 0;
                }
                if ((bKeyDown && !bKeyDownOld) && nSnakeDirection != 0)
                {
                    nSnakeDirection = 2;
                }
                bKeyRightOld = bKeyRight;
                bKeyLeftOld = bKeyLeft;
                bKeyUpOld = bKeyUp;
                bKeyDownOld = bKeyDown;
            }
            //update snake direction 

            switch (nSnakeDirection)
            {
            case 0: // UP
                snake.push_front({ snake.front().x,snake.front().y - 1 });
                break;
            case 1:// Right
                snake.push_front({ snake.front().x + 1, snake.front().y });
                break;
            case 2:// Down
                snake.push_front({ snake.front().x,snake.front().y + 1 });
                break;
            case 3:// Left
                snake.push_front({ snake.front().x - 1,snake.front().y });
                break;
            }
            //collison Ditection 

            //collison Ditection Snake vs World
            if (snake.front().x < 0 || snake.front().x >= screenWidth)
                bDead = true;
            // Top three line contain Title and Messages
            if (snake.front().y < 3 || snake.front().y >= screenHeight)
                bDead = true;
            //collison Ditection Snake vs Food 
            if (snake.front().x == nFoodx && snake.front().y == nFoody)
            {
                nScore = nScore + 100;
                //search for the empty space to feed Food
                while (screen[nFoody * screenWidth + nFoodx] != L' ')
                {
                    nFoodx = rand() % screenWidth;
                    nFoody = (rand() % (screenHeight - 3)) + 3;
                }
                //The snake's body length increases after a meal (hear duplicating 5 body segment)
                for (int i = 0;i < 5;i++)
                {
                    snake.push_back({ snake.back().x, snake.back().y });
                }
            }
            //collison Ditection snake vs Snake 
            // Compare head to other body segment
            for (list<snakeSegment>::iterator i = snake.begin();i != snake.end();i++)
            {
                if (i != snake.begin() && i->x == snake.front().x && i->y == snake.front().y)
                    bDead = true;
           }


            //chop-off snake tail 
            snake.pop_back();

            //display Segment 
            //clearing screen buffer
            for (int i = 0;i < area; i++) screen[i] = L' ';
            // Title and Message Body
            for (int i = 0; i < screenWidth; i++)
            {
                screen[i] = L'=';
                screen[2 * screenWidth + i] = L'=';
            }
            wsprintf(&screen[screenWidth + 10], L"   ------------- - S N A K E ! !----- f1: %d f2: %d   ------- %d  SCORE: %d", screenHeight, screenWidth, nSnakeDirection, nScore);

            // Draw Snake Body
            for (auto s : snake)
                screen[s.y * screenWidth + s.x] = bDead ? L'+' : L'O';

            // Draw Snake Head
            screen[snake.front().y * screenWidth + snake.front().x] = bDead ? L'X' : L'@';

            // Draw Food
            screen[nFoody * screenWidth + nFoodx] = L'%';

            //Interface to indicate dead condition 
            if (bDead)
                wsprintf(&screen[15 * screenWidth + 40], L"   Press 'SPACE' tab to play again !!! ");

            //Display Frame
            WriteConsoleOutputCharacter(handle, screen, screenWidth * screenHeight, { 0,0 }, &dwBytesWritten);
        }

        //Wait for Space 
        while ((0x8000 & GetAsyncKeyState((unsigned char)('\x20'))) == 0);
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
