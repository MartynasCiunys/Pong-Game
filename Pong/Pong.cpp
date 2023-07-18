#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include <string>
#include <fstream>
#include <cmath>


using namespace std;

const int x = 81;
const int y = 21;
const int PadSize = 5;

enum BallD { STOP = 0, LEFT = 1, UPLEFT = 2, DOWNLEFT = 3, RIGHT = 4, UPRIGHT = 5, DOWNRIGHT = 6 };

struct paddle
{
    int  Opad1Y = y / 2 - PadSize / 2, Opad2Y = y / 2 - PadSize / 2, //Default paddle location
        pad1Y = Opad1Y, pad2Y = Opad2Y; //Paddle movement location
    char pad1up = 'w', pad1down = 's', //Player 1 controls
        pad2up = 'i', pad2down = 'k'; //Player 2 controls
};

struct Ball
{
    int OballY = y / 2, OballX = x / 2, //Default ball location
        ballY = OballY, ballX = OballX; //Ball movement location
    BallD d = RIGHT; //Ball direction
};

void ShowConsoleCursor(bool showFlag);
void print(char map[][x], int score1, int score2);
void remap(char map[][x], char D[][x], paddle& pad, Ball& b);
void padmove(paddle& pad, bool& quit);
void BallMove(paddle& pad, Ball& b, int& score1, int& score2, int& error);
void Bot(Ball b, Ball& t, paddle& pad, int score1, int score2, int error, int& s);
void Logic(paddle& pad, Ball& b, int& score1, int& score2, int& error);
void gotoxy(int column, int line);

int main()
{
    int error = 0, s = 0, speed = 50;
    int score1 = 0, score2 = 0;
    char map[y][x], D[y][x];
    bool quit = false;
    Ball b, t, v;
    paddle pad;


    ShowConsoleCursor(false);

    for (int i = 0; i < y; i++)
    {
        for (int j = 0; j < x; j++)
        {
            if (i == 0 || i == y - 1)
            {
                D[i][j] = 196;
            }

            else
                if (j < x / 2 || j > x / 2)
                {
                    D[i][j] = ' ';
                }
                else { D[i][j] = '.'; }
        }
    }
    ofstream ofs("data.txt");
    clock_t now = clock();

    while (!quit)
    {
        if (speed > 0)
            speed = 50 - ((clock() - now) / 1000);
        Logic(pad, b, score1, score2, error);
        remap(map, D, pad, b);
        print(map, score1, score2);
        padmove(pad, quit);
        BallMove(pad, b, score1, score2, error);
        Bot(b, t, pad, score1, score2, error, s);
        gotoxy(0, 0);
        Sleep(20);

        ofs << b.ballX << " " << b.ballY << " | " << t.ballX << " " << t.ballY << " | " << pad.pad2Y << endl;

        if (quit)
            return 0;
        if (score1 == 5 || score2 == 5)
            break;
    }
    if (score1 == 5)
    {
        system("CLS");
        cout << "WINNER: PLAYER 1";
        Sleep(1000);
    }
    if (score2 == 5)
    {
        system("CLS");
        cout << "WINNER: PLAYER 2";
        Sleep(1000);
    }


    cout << endl << endl << "If you want to play again press Y, if not press N";
    if (_getch() == 'y')
    {
        system("CLS");
        main();
    }

    else if (_getch() == 'n')
        return 0;
}

void print(char map[][x], int score1, int score2)
{
    string score1T = "Player 1 score: ", score2T = "Player 2 score: ";
    char line = 196;
    int s = 0;

    //Score
    score1T += to_string(score1);
    score2T += to_string(score2);

    for (int i = 0; i < x; i++)
        cout << line;
    cout << endl;

    for (int i = 0; i < x; i++)
    {
        if (i == 0)
        {
            cout << score1T;
            s += score1T.length();
        }
        else if (i == x - score2T.length() - 1)
        {
            cout << score2T;
            s += score2T.length();
        }
        else if (i == x / 2 - 1)
            cout << '|';
        else if (i < x / 2 && s<x / 2 || i>x / 2 && s < x)
        {
            cout << " ";
            s++;
        }
    }
    cout << endl;

    //Map
    for (int i = 0; i < y; i++)
    {
        for (int j = 0; j < x; j++)
        {
            cout << map[i][j];
        }
        cout << endl;
    }
}

void remap(char map[][x], char D[][x], paddle& pad, Ball& b)
{
    for (int i = 0; i < y; i++)
    {
        for (int j = 0; j < x; j++)
        {
            map[i][j] = D[i][j];
            for (int s = 0; s < PadSize; s++)
            {
                if (i == pad.pad1Y + s && j == 0)
                    map[i][j] = 987;
                if (i == pad.pad2Y + s && j == x - 1)
                    map[i][j] = 987;
            }

            if (i == b.ballY && j == b.ballX)
                map[i][j] = 2;
        }
    }
}

void padmove(paddle& pad, bool& quit)
{
    if (_kbhit())
    {
        char current = _getch();
        //Paddle 1

        if (current == pad.pad1up)
            if (pad.pad1Y > 1)
                pad.pad1Y--;
        if (current == pad.pad1down)
            if (pad.pad1Y < y - (PadSize + 1))
                pad.pad1Y++;

        //Paddle 2
        if (current == pad.pad2up)
            if (pad.pad2Y > 1)
                pad.pad2Y--;
        if (current == pad.pad2down)
            if (pad.pad2Y < y - (PadSize + 1))
                pad.pad2Y++;

        if (current == 'q')
            quit = true;
    }

}

void BallMove(paddle& pad, Ball& b, int& score1, int& score2, int& error)
{
    switch (b.d)
    {
    case STOP:
        break;
    case LEFT:
        b.ballX--;
        Logic(pad, b, score1, score2, error);
        if (b.d == LEFT)
            b.ballX--;
        break;
    case UPLEFT:
        b.ballX--;
        b.ballY--;
        break;
    case DOWNLEFT:
        b.ballX--;
        b.ballY++;
        break;
    case RIGHT:
        b.ballX++;
        Logic(pad, b, score1, score2, error);
        if (b.d == RIGHT)
            b.ballX++;
        break;
    case UPRIGHT:
        b.ballX++;
        b.ballY--;
        break;
    case DOWNRIGHT:
        b.ballX++;
        b.ballY++;
        break;
    default:
        break;
    }
}

void Logic(paddle& pad, Ball& b, int& score1, int& score2, int& error)
{
    //Up Wall
    if (b.ballY == 1)
        if (b.d == UPRIGHT)
            b.d = DOWNRIGHT;
        else if (b.d == UPLEFT)
            b.d = DOWNLEFT;
    //Down Wall
    if (b.ballY == y - 2)
        if (b.d == DOWNRIGHT)
            b.d = UPRIGHT;
        else if (b.d == DOWNLEFT)
            b.d = UPLEFT;

    //Left Wall
    if (b.ballX == 0)
    {
        score2++;
        pad.pad1Y = pad.Opad1Y;
        pad.pad2Y = pad.Opad2Y;
        b.ballX = b.OballX;
        b.ballY = b.OballY;
        b.d = LEFT;
        error = 0;
    }

    //Right Wall
    if (b.ballX == x - 1)
    {
        score1++;
        pad.pad1Y = pad.Opad1Y;
        pad.pad2Y = pad.Opad2Y;
        b.ballX = b.OballX;
        b.ballY = b.OballY;
        b.d = RIGHT;
        error = 0;
    }

    //Paddle
    srand(time(NULL));
    for (int i = 0; i < PadSize; i++)
    {
        if (b.ballY == pad.pad1Y + i && b.ballX == 1)
        {
            b.d = (BallD)((rand() % 3) + 4);
            error = ((rand() % PadSize + 2) - 1);

        }
        if (b.ballY == pad.pad2Y + i && b.ballX == x - 2)
        {
            b.d = (BallD)((rand() % 3) + 1);
        }

    }
}
void Bot(Ball b, Ball& t, paddle& pad, int score1, int score2, int error, int& s)
{
    int temp = pad.pad2Y;
    if (b.ballX > x / 2 + x / 4)
    {
        if (b.d == RIGHT || b.d == DOWNRIGHT || b.d == UPRIGHT)
        {
            if (t.ballX != x - 2)
            {
                t = b;
                while (t.ballX != x - 2)
                {
                    Logic(pad, t, score1, score2, error);
                    BallMove(pad, t, score1, score2, error);
                }
            }
            else {
                while (t.ballY - error < 1)
                    error--;
                while (t.ballY + (PadSize - error) > y - 2)
                    error++;

                if (t.ballY != pad.pad2Y + error)
                {
                    if (s == 1)
                    {
                        if (pad.pad2Y + error > t.ballY)
                            pad.pad2Y--;
                        else if (pad.pad2Y + error < t.ballY)
                            pad.pad2Y++;
                        s = 0;
                    }
                    else s++;

                }
            }
        }
    }
    else {
        if (pad.Opad2Y != pad.pad2Y)
        {
            if (s == 2)
            {
                if (pad.pad2Y > pad.Opad2Y)
                    pad.pad2Y--;
                else if (pad.pad2Y < pad.Opad2Y)
                    pad.pad2Y++;
                s = 0;
            }
            else s++;

        }
    }
    if (t.ballY == pad.pad2Y + error && t.ballX == x - 2)
    {
        t.ballX = 0;
        t.ballY = 0;
        t.d = STOP;
    }

}


void ShowConsoleCursor(bool showFlag)
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO     cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
}

void gotoxy(int column, int line) {
    COORD coord;
    coord.X = column;
    coord.Y = line;
    SetConsoleCursorPosition(
        GetStdHandle(STD_OUTPUT_HANDLE),
        coord
    );
}