/*
 ============================================================================
 Name        : 2048.c
 Author      : Maurits van der Schee
 Description : Console version of the game "2048" for GNU/Linux
 ============================================================================
 */

#include "gsdk.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define SIZE 8

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

void Game2048(void);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

static uint32_t m_u32Score = 0;
static uint8_t  m_u8Scheme = 0;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static void GetColor(uint8_t value, char* color, size_t length)
{
    const uint8_t original[]   = {8, 255, 1, 255, 2, 255, 3, 255, 4, 255, 5, 255, 6, 255, 7, 255, 9, 0, 10, 0, 11, 0, 12, 0, 13, 0, 14, 0, 255, 0, 255, 0};
    const uint8_t blackwhite[] = {232, 255, 234, 255, 236, 255, 238, 255, 240, 255, 242, 255, 244, 255, 246, 0, 248, 0, 249, 0, 250, 0, 251, 0, 252, 0, 253, 0, 254, 0, 255, 0};
    const uint8_t bluered[]    = {235, 255, 63, 255, 57, 255, 93, 255, 129, 255, 165, 255, 201, 255, 200, 255, 199, 255, 198, 255, 197, 255, 196, 255, 196, 255, 196, 255, 196, 255, 196, 255};

    uint8_t* schemes[]  = {original, blackwhite, bluered};
    uint8_t* background = schemes[m_u8Scheme] + 0;
    uint8_t* foreground = schemes[m_u8Scheme] + 1;

    if (value > 0)
    {
        while (value--)
        {
            if (background + 2 < schemes[m_u8Scheme] + sizeof(original))
            {
                background += 2;
                foreground += 2;
            }
        }
    }

    snprintf(color, length, "\033[38;5;%d;48;5;%dm", *foreground, *background);
}

static void DrawBoard(uint8_t board[SIZE][SIZE])
{
    uint8_t x, y;
    char    color[40], reset[] = "\033[m";
    GSDK_PRINTF("\033[H");

    GSDK_PRINTF("2048.c %17d pts\n\n", m_u32Score);

    for (y = 0; y < SIZE; y++)
    {
        for (x = 0; x < SIZE; x++)
        {
            GetColor(board[x][y], color, 40);
            GSDK_PRINTF("%s", color);
            GSDK_PRINTF("       ");
            GSDK_PRINTF("%s", reset);
        }
        GSDK_PRINTF("\n");
        for (x = 0; x < SIZE; x++)
        {
            GetColor(board[x][y], color, 40);
            GSDK_PRINTF("%s", color);
            if (board[x][y] != 0)
            {
                char s[8];
                snprintf(s, 8, "%u", (uint32_t)1 << board[x][y]);
                uint8_t t = 7 - strlen(s);
                GSDK_PRINTF("%*s%s%*s", t - t / 2, "", s, t / 2, "");
            }
            else
            {
                GSDK_PRINTF("   ·   ");
            }
            GSDK_PRINTF("%s", reset);
        }
        GSDK_PRINTF("\n");
        for (x = 0; x < SIZE; x++)
        {
            GetColor(board[x][y], color, 40);
            GSDK_PRINTF("%s", color);
            GSDK_PRINTF("       ");
            GSDK_PRINTF("%s", reset);
        }
        GSDK_PRINTF("\n");
    }
    GSDK_PRINTF("\n");
    GSDK_PRINTF("        ←,↑,→,↓ or q        \n");
    GSDK_PRINTF("\033[A");  // one line up
}

static uint8_t FindTarget(uint8_t array[SIZE], uint8_t x, uint8_t stop)
{
    uint8_t t;
    // if the position is already on the first, don't evaluate
    if (x == 0)
    {
        return x;
    }
    for (t = x - 1;; t--)
    {
        if (array[t] != 0)
        {
            if (array[t] != array[x])
            {
                // merge is not possible, take next position
                return t + 1;
            }
            return t;
        }
        else
        {
            // we should not slide further, return this one
            if (t == stop)
            {
                return t;
            }
        }
    }
    // we did not find a
    return x;
}

static bool SlideArray(uint8_t array[SIZE])
{
    bool    bSuccess = false;
    uint8_t x, t, stop = 0;

    for (x = 0; x < SIZE; x++)
    {
        if (array[x] != 0)
        {
            t = FindTarget(array, x, stop);
            // if target is not original position, then move or merge
            if (t != x)
            {
                // if target is zero, this is a move
                if (array[t] == 0)
                {
                    array[t] = array[x];
                }
                else if (array[t] == array[x])
                {
                    // merge (increase power of two)
                    array[t]++;
                    // increase score
                    m_u32Score += (uint32_t)1 << array[t];
                    // set stop to avoid double merge
                    stop = t + 1;
                }
                array[x] = 0;
                bSuccess = true;
            }
        }
    }
    return bSuccess;
}

static void RotateBoard(uint8_t board[SIZE][SIZE])
{
    uint8_t i, j, n = SIZE;
    uint8_t tmp;
    for (i = 0; i < n / 2; i++)
    {
        for (j = i; j < n - i - 1; j++)
        {
            tmp                         = board[i][j];
            board[i][j]                 = board[j][n - i - 1];
            board[j][n - i - 1]         = board[n - i - 1][n - j - 1];
            board[n - i - 1][n - j - 1] = board[n - j - 1][i];
            board[n - j - 1][i]         = tmp;
        }
    }
}

static bool MoveUp(uint8_t board[SIZE][SIZE])
{
    bool    bSuccess = false;
    uint8_t x;

    for (x = 0; x < SIZE; x++)
    {
        bSuccess |= SlideArray(board[x]);
    }

    return bSuccess;
}

static bool MoveLeft(uint8_t board[SIZE][SIZE])
{
    bool bSuccess;
    RotateBoard(board);
    bSuccess = MoveUp(board);
    RotateBoard(board);
    RotateBoard(board);
    RotateBoard(board);
    return bSuccess;
}

static bool MoveDown(uint8_t board[SIZE][SIZE])
{
    bool bSuccess;
    RotateBoard(board);
    RotateBoard(board);
    bSuccess = MoveUp(board);
    RotateBoard(board);
    RotateBoard(board);
    return bSuccess;
}

static bool MoveRight(uint8_t board[SIZE][SIZE])
{
    bool bSuccess;
    RotateBoard(board);
    RotateBoard(board);
    RotateBoard(board);
    bSuccess = MoveUp(board);
    RotateBoard(board);
    return bSuccess;
}

static bool FindPairDown(uint8_t board[SIZE][SIZE])
{
    bool    bSuccess = false;
    uint8_t x, y;
    for (x = 0; x < SIZE; x++)
    {
        for (y = 0; y < SIZE - 1; y++)
        {
            if (board[x][y] == board[x][y + 1])
            {
                return true;
            }
        }
    }
    return bSuccess;
}

static uint8_t CountEmpty(uint8_t board[SIZE][SIZE])
{
    uint8_t x, y;
    uint8_t count = 0;
    for (x = 0; x < SIZE; x++)
    {
        for (y = 0; y < SIZE; y++)
        {
            if (board[x][y] == 0)
            {
                count++;
            }
        }
    }
    return count;
}

static bool GameEnded(uint8_t board[SIZE][SIZE])
{
    bool ended = true;
    if (CountEmpty(board) > 0)
    {
        return false;
    }
    if (FindPairDown(board))
    {
        return false;
    }
    RotateBoard(board);
    if (FindPairDown(board))
    {
        ended = false;
    }
    RotateBoard(board);
    RotateBoard(board);
    RotateBoard(board);
    return ended;
}

static void AddRandom(uint8_t board[SIZE][SIZE])
{
    static bool initialized = false;
    uint8_t     x, y;
    uint8_t     r, len = 0;
    uint8_t     n, list[SIZE * SIZE][2];

    if (!initialized)
    {
        srand(GetTickUs());
        initialized = true;
    }

    for (x = 0; x < SIZE; x++)
    {
        for (y = 0; y < SIZE; y++)
        {
            if (board[x][y] == 0)
            {
                list[len][0] = x;
                list[len][1] = y;
                len++;
            }
        }
    }

    if (len > 0)
    {
        r           = rand() % len;
        x           = list[r][0];
        y           = list[r][1];
        n           = (rand() % 10) / 9 + 1;
        board[x][y] = n;
    }
}

static void InitBoard(uint8_t board[SIZE][SIZE])
{
    uint8_t x, y;
    for (x = 0; x < SIZE; x++)
    {
        for (y = 0; y < SIZE; y++)
        {
            board[x][y] = 0;
        }
    }
    AddRandom(board);
    AddRandom(board);
    DrawBoard(board);
    m_u32Score = 0;
}

void Game2048(void)
{
    uint8_t board[SIZE][SIZE];

    char c;
    bool bSuccess;

    m_u8Scheme = 0;  // original
                     // m_u8Scheme = 1;  // blackwhite
                     // m_u8Scheme = 2;  // bluered

    GSDK_PRINTF("\033[?25l\033[2J");

    InitBoard(board);

    while (true)
    {
        c = getchar();

        if (c == -1)
        {
            GSDK_PRINTF("\nError! Cannot read keyboard input!");
            break;
        }

        switch (c)
        {
            case 'a':  // 'a' key
            case 'h':  // 'h' key
            case 68:   // left arrow
                bSuccess = MoveLeft(board);
                break;
            case 'd':  // 'd' key
            case 'l':  // 'l' key
            case 67:   // right arrow
                bSuccess = MoveRight(board);
                break;
            case 'w':  // 'w' key
            case 'k':  // 'k' key
            case 65:   // up arrow
                bSuccess = MoveUp(board);
                break;
            case 's':  // 's' key
            case 'j':  // 'j' key
            case 66:   // down arrow
                bSuccess = MoveDown(board);
                break;
            default: bSuccess = false;
        }

        if (bSuccess)
        {
            DrawBoard(board);
            DelayBlockMs(150);
            AddRandom(board);
            DrawBoard(board);

            if (GameEnded(board))
            {
                GSDK_PRINTF("         GAME OVER          \n");
                break;
            }
        }

        switch (c)
        {
            case 'q':
            {
                GSDK_PRINTF("        QUIT? (y/n)         \n");
                c = getchar();
                if (c == 'y')
                {
                    break;
                }
                DrawBoard(board);
                break;
            }

            case 'r':
            {
                GSDK_PRINTF("       RESTART? (y/n)       \n");
                c = getchar();
                if (c == 'y')
                {
                    InitBoard(board);
                }
                DrawBoard(board);
                break;
            }

            default:
            {
                break;
            }
        }
    }

    GSDK_PRINTF("\033[?25h\033[m");
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------