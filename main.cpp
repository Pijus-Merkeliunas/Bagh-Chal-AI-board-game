#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif
#include <iomanip>
//#include <iostream>
//#include <stdio.h>
//#include <ctype.h>
#include <tchar.h>
//#include <windows.h>
#include "BaghChal.h"
#include <windowsx.h>
//#include <algorithm>
//#include <cstring>
//#include <sstream>
#include <fstream>
//#include <time.h>
//#include <algorithm> // for std::find
//#include <iterator> // for std::begin, std::end

//#include <objidl.h>
#include <gdiplus.h>

#include <chrono>
using namespace std::chrono;
using namespace Gdiplus;
//typedef int (__cdecl *MYPROC)(LPWSTR);
using namespace std;

const int CELL_SIZE=100;
const int GBSIZE=6;
HICON aIcon;
HBRUSH blue,red;
int gameBoard[GBSIZE+1][GBSIZE+1];
int player=1;
int winner=0;
int dPiece[2];
 int isheepTot=21;
int sheepTot=isheepTot;
 int isuSheep=2;
int suSheep=isuSheep;
int sheep = 0;
int unSheep = sheepTot-suSheep;
int ax=-1;
int ay=-1;
int bx=-1;
int by=-1;
bool sPiece=false;
int ran[(GBSIZE*GBSIZE-5*5)+1];
int history[16][5];
int hisMove=0;
int sWin=0; //sheep wins
int wWin=0; //wolf wins
unsigned int Nodes=0;
unsigned int Prune=0;
int pic=1;

int pvp=1; //0 - PVP, 1 - PVA, 2 - AVA, 3-FAVA
bool extra=false;
int wait=2; //*100ms

int moves = 4;

const int antiRep=150;
int ARHis[antiRep][5]={-1};
int ARHisMove=0;

int gflag=0;
int tmpflag=0;

struct TempBoard
{
    int gameBoard[GBSIZE+1][GBSIZE+1]={0};
    int dPiece[2];
    int suSheep=isuSheep;
    int sheep = 0;
    int unSheep = isheepTot-isuSheep;
    int player=1;

    int last0 = -1;
    int last1 = -1;
    int last2 = -1;
    int last3 = -1;
    int last4 = -1;
};

void ranGen(int lower, int upper, int amount, int num[(GBSIZE*GBSIZE-5*5)+1])
{
    for (int i = 0; i < amount; i++)
    {
        num[i] = (rand() %(upper - lower + 1)) + lower;
        if(num[i]==0||num[i]==5||num[i]==30||num[i]==35)
            i--;
        for(int x=0; x<i;x++)
        {
            if(num[x]==num[i])
            {i--;break;}
        }
    }
}

void drawPic(HDC hdc, RECT * pRect, int type) //Changes the profile
{
    if(pRect != NULL)
    {
        int right = pRect->right + ((pRect->left - pRect->right))+1;
        int bottom = pRect->bottom + ((pRect->top - pRect->bottom))+1;
        //DrawIcon(hdc,left,bottom,hIcon);

    wchar_t buffer[20];
    Graphics graphics(hdc);
    swprintf(buffer,20,L"pic/Pic%d%d.png",type, pic); //Changes the picture from it's currently set
    Image *image1 = new Image(buffer);
    graphics.DrawImage(image1,right,bottom);    //Draws the Chosen picture
    }
}
/*
void DrawIconCentered(HDC hdc, RECT * pRect, HICON hIcon) //Draws the icon in the square
{
    if(pRect != NULL)
    {
        int left = pRect->left + ((pRect->right - pRect->left)-32)/2;
        int bottom = pRect->bottom + ((pRect->top - pRect->bottom)-32)/2;
        printf("%d %d\n",left,bottom);
        DrawIcon(hdc,left,bottom,hIcon);
    }
}
*/
bool pExist(int x, int y,int gameBoard[GBSIZE+1][GBSIZE+1])
{

    if(gameBoard[x][y]!=0&&gameBoard[x][y]!=5)
    {
        return true;
    } else
    return false;
}

bool wCheck(int x1, int y1,int x2, int y2,int gameBoard[GBSIZE+1][GBSIZE+1])
{
    if(gameBoard[x2][y2]==0)
    {
        if(x1-1==x2&&y1==y2)
            {
            return true;
            }
        else if(x1+1==x2&&y1==y2)
            {
            return true;
            }
        else if(y1-1==y2&&x1==x2)
            {
            return true;
            }
        else if(y1+1==y2&&x1==x2)
            {
            return true;
            }
        else if(x1-1==x2&&y1-1==y2)
            {
            return true;
            }
        else if(x1+1==x2&&y1-1==y2)
            {
            return true;
            }
        else if(x1-1==x2&&y1+1==y2)
            {
            return true;
            }
        else if(x1+1==x2&&y1+1==y2)
            {
            return true;
            }
    }
    return false;

}

bool bCheck(int x1, int y1,int x2, int y2,int gameBoard[GBSIZE+1][GBSIZE+1], int dPiece[2])
{
    if(gameBoard[x2][y2]==0)
    {
        if(wCheck(x1,y1,x2,y2,gameBoard))
        {
        return true;
        }else if(x1-2==x2&&y1==y2&&gameBoard[x1-1][y1]==1)
            {
            dPiece[0]=x1-1;
            dPiece[1]=y1;
            return true;
            }
        else if(x1+2==x2&&y1==y2&&gameBoard[x1+1][y1]==1)
            {
            dPiece[0]=x1+1;
            dPiece[1]=y1;
            return true;
            }
        else if(y1-2==y2&&x1==x2&&gameBoard[x1][y1-1]==1)
            {
            dPiece[0]=x1;
            dPiece[1]=y1-1;
            return true;
            }
        else if(y1+2==y2&&x1==x2&&gameBoard[x1][y1+1]==1)
            {
            dPiece[0]=x1;
            dPiece[1]=y1+1;
            return true;
            }
        else if(x1-2==x2&&y1-2==y2&&gameBoard[x1-1][y1-1]==1)
            {
            dPiece[0]=x1-1;
            dPiece[1]=y1-1;
            return true;
            }
        else if(x1+2==x2&&y1-2==y2&&gameBoard[x1+1][y1-1]==1)
            {
            dPiece[0]=x1+1;
            dPiece[1]=y1-1;
            return true;
            }
        else if(x1-2==x2&&y1+2==y2&&gameBoard[x1-1][y1+1]==1)
            {
            dPiece[0]=x1-1;
            dPiece[1]=y1+1;
            return true;
            }
        else if(x1+2==x2&&y1+2==y2&&gameBoard[x1+1][y1+1]==1)
            {
            dPiece[0]=x1+1;
            dPiece[1]=y1+1;
            return true;
            }
    }
    return false;
}

bool placeSheep(int x, int y,int gameBoard[GBSIZE+1][GBSIZE+1], int type)
{
    if((y>=0&&y<GBSIZE)&&(x>=0&&x<GBSIZE))
    {
        if(!pExist(x,y,gameBoard))
        {
            gameBoard[x][y]=type;
            return true;
        }
    }
    return false;
}

void changeTurn()
{
    if(player==1)
        player=2;
    else if(player==2)
        player=1;
}

bool movePiece(int x1, int y1, int x2, int y2,int gameBoard[GBSIZE+1][GBSIZE+1],int player)
{
    dPiece[0]=-1;
    if((y1>=0&&y1<GBSIZE)&&(y2>=0&&y2<GBSIZE)&&(x1>=0&&x1<GBSIZE)&&(x2>=0&&x2<GBSIZE))
    {
        if(pExist(x1,y1,gameBoard))
        {
            if(player==1&&gameBoard[x1][y1]==1)
            {
                if(wCheck(x1,y1,x2,y2,gameBoard))
                {
                    gameBoard[x1][y1]=0;
                    gameBoard[x2][y2]=1;
                    return true;
                }
            } else if(player==1&&gameBoard[x1][y1]==3)
            {
                if(wCheck(x1,y1,x2,y2,gameBoard))
                {
                    gameBoard[x1][y1]=0;
                    gameBoard[x2][y2]=3;
                    return true;
                }
            }
            else if(player==2&&gameBoard[x1][y1]==2)
            {

                if(bCheck(x1,y1,x2,y2,gameBoard,dPiece))
                {
                    if(dPiece[0]!=-1)
                    gameBoard[dPiece[0]][dPiece[1]]=0;
                gameBoard[x1][y1]=0;
                gameBoard[x2][y2]=2;
                return true;
                }
            } else if(player==2&&gameBoard[x1][y1]==4)
            {
                if(bCheck(x1,y1,x2,y2,gameBoard,dPiece))
                {
                    if(dPiece[0]!=-1)
                    {
                    gameBoard[dPiece[0]][dPiece[1]]=0;
                    //changeTurn();
                    }
                gameBoard[x1][y1]=0;
                gameBoard[x2][y2]=4;
                return true;
                }
            }
        }
    }
    return false;
}

int whiteWin(int gameBoard[GBSIZE+1][GBSIZE+1])
{
    int blocked=0;
    bool flag = true;
    for(int x=0;x<GBSIZE;x++)
    {
       for(int y=0;y<GBSIZE;y++)
       {
           if(gameBoard[x][y]==2||gameBoard[x][y]==4)
           {
               for(int a=-2;a<3;a++)
               {
                   for(int b=-2;b<3;b++)
                   {
                       if((a!=0||b!=0)&&x+a>-1&&x+a<GBSIZE&&y+b>-1&&y+b<GBSIZE)
                       {
                           if(bCheck(x,y,x+a,y+b,gameBoard,dPiece))
                           {
                               flag=false;
                               //return false;
                           }
                       }
                   }
               }
               if(flag)
               {
                   blocked++;
                   flag=true;
               }
           }
       }
    }
    return blocked;
}

int blackWin(int sheep, int unSheep, int suSheep)
{
    return sheepTot-(sheep+unSheep+suSheep);
}

BOOL GetGameBoardRect(HWND hwnd, RECT * pRect)
{
    RECT rc;
    if(GetClientRect(hwnd, &rc)){
      int width = rc.right - rc.left;
      int height = rc.bottom - rc.top;

      pRect->left = (width - CELL_SIZE * GBSIZE) / 2;
      pRect->top = (height - CELL_SIZE * GBSIZE) / 4;

      pRect->right = pRect->left + CELL_SIZE * GBSIZE;
      pRect->bottom = pRect->top + CELL_SIZE * GBSIZE;
      return TRUE;
    }

    SetRectEmpty(pRect) ;
    return FALSE;
}

void DrawLine(HDC hdc, int x1, int y1, int x2, int y2)
{
    MoveToEx(hdc,x1,y1,NULL);
    LineTo(hdc,x2,y2);
}

int GetCellNumberFromPoint(HWND hwnd, int x, int y, int gameBoard[GBSIZE+1][GBSIZE+1], int &a, int &b)
{
    POINT pt= {x,y};
    RECT rc;

    if(GetGameBoardRect(hwnd, &rc)){

        if(PtInRect(&rc, pt)){

            x = pt.x - rc.left;
            y = pt.y - rc.top;

            int column = x / CELL_SIZE;
            int row = y / CELL_SIZE;

            a=column;
            b=row;
            return gameBoard[row][column];
        }
    }
    return -1; //outside the board

}

BOOL GetCellRect(HWND hwnd, int x, int y, RECT * pRect)
{
    RECT rcBoard;

    SetRectEmpty(pRect);
    if(!(x>-1&&x<GBSIZE && y>-1&&y<GBSIZE)){
        return FALSE;
    }

    if(GetGameBoardRect(hwnd,&rcBoard)){

        pRect->left = rcBoard.left + x * CELL_SIZE+1; //+1 ir -1 skirti kad nenubrauktu liniju kai paspaudi langelius
        pRect->top = rcBoard.top + y * CELL_SIZE+1;
        pRect->right = pRect->left + CELL_SIZE-1;
        pRect->bottom = pRect->top + CELL_SIZE-1;
        return TRUE;
    }
return FALSE;
}

void RedrawBoard(HWND hwnd,int gameBoard[GBSIZE+1][GBSIZE+1])
{
    RECT rcBoard;
    RECT rcCell;
    HDC hdc=GetDC(hwnd);

    GetGameBoardRect(hwnd,&rcBoard);

    for(int x=0;x<GBSIZE;x++)
    {
        for(int y=0;y<GBSIZE;y++)
        {
            rcCell.left = rcBoard.left + x * CELL_SIZE+1;
            rcCell.top = rcBoard.top + y * CELL_SIZE+1;
            rcCell.right = rcCell.left + CELL_SIZE-1;
            rcCell.bottom = rcCell.top + CELL_SIZE-1;

            if(gameBoard[x][y]==1)
            {
            drawPic(hdc,&rcCell,1);
            } else if(gameBoard[x][y]==2) {
            drawPic(hdc,&rcCell,2);
            } else if(gameBoard[x][y]==0) {
            FillRect(hdc,&rcCell,CreateSolidBrush(RGB(255,255,255)));
            } else if(gameBoard[x][y]==5) {
            FillRect(hdc,&rcCell,CreateSolidBrush(RGB(0,255,0)));
            } else if(gameBoard[x][y]==3) {
            drawPic(hdc,&rcCell,3);
            } else if(gameBoard[x][y]==4) {
            drawPic(hdc,&rcCell,4);
            }
        }
    }
}

void ShowTurn(HWND hwnd, HDC hdc)
{
    RECT rc;
    static const char turn1[] = "Turn: Player 1";
    static const char turn2[] = "Turn: Player 2";
    HFONT hFont;
    const char * turnText = NULL;

    switch(winner)
    {
    case 0:
        turnText = player==1 ? turn1 : turn2;
        break;
    case 1:
        turnText = "Player 1 is the winner!";
        break;
    case 2:
        turnText = "Player 2 is the winner!";
        break;
    }

    /*if(GetClientRect(hwnd,&rcClient)){
    SetTextColor(hdc,RGB(255,0,0));
    TextOut(hdc, rcClient.right - 800, rcClient.top+800, tmp,tmp.length());
    }*/
    if(turnText != NULL && GetClientRect(hwnd,&rc)){

        rc.top = rc.bottom - 75;
        FillRect(hdc,&rc, (HBRUSH)GetStockObject(GRAY_BRUSH));
        SetTextColor(hdc,RGB(255,255,255));
        SetBkMode(hdc,TRANSPARENT);
        hFont = CreateFont(64,0,0,0,FW_DONTCARE,FALSE,TRUE,FALSE,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,
                CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY, VARIABLE_PITCH,TEXT("Impact"));
        SelectObject(hdc, hFont);
        DrawText(hdc, turnText, lstrlen(turnText), &rc, DT_CENTER);
        DeleteObject(hFont);
    }

    //SHOW UNPLACED SHEEP

    if(GetClientRect(hwnd,&rc))
    {
        std::stringstream buffer;
        buffer << "Unplaced sheep - "<<unSheep<<"; super sheep - "<<suSheep;
        rc.bottom-=75;
        rc.top = rc.bottom - 75;
        FillRect(hdc,&rc, (HBRUSH)GetStockObject(GRAY_BRUSH));
        SetTextColor(hdc,RGB(0,0,255));
        SetBkMode(hdc,TRANSPARENT);
        hFont = CreateFont(64,0,0,0,FW_DONTCARE,FALSE,TRUE,FALSE,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,
                CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY, VARIABLE_PITCH,TEXT("Impact"));
        SelectObject(hdc, hFont);
        DrawText(hdc, buffer.str().c_str(), -1, &rc, DT_CENTER);
        DeleteObject(hFont);
    }
}

void ShowSide(HWND hwnd, HDC hdc)
{
    RECT rc;
    HFONT hFont;
    if(GetClientRect(hwnd,&rc))
    {
        std::stringstream buffer;
        buffer << "A           B           C           D           E           F";
        rc.bottom-=200;
        rc.top = rc.bottom - 75;
        rc.right-=497;
        rc.left=rc.right-600;
        FillRect(hdc,&rc, (HBRUSH) COLOR_BACKGROUND);
        SetTextColor(hdc,RGB(69,162,158));
        SetBkMode(hdc,TRANSPARENT);
        hFont = CreateFont(52,0,0,0,FW_REGULAR,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY, DEFAULT_PITCH,TEXT("Impact"));
        SelectObject(hdc, hFont);
        DrawText(hdc, buffer.str().c_str(), -1, &rc, DT_CENTER);

        std::stringstream buffer1;
        buffer1 << "1\n\n2\n\n3\n\n4\n\n5\n\n6";
        rc.bottom-=75;
        rc.top = rc.bottom - 600;
        rc.right-=600;
        rc.left=rc.right-60;
        FillRect(hdc,&rc, (HBRUSH) COLOR_BACKGROUND);
        SetTextColor(hdc,RGB(69,162,158));
        SetBkMode(hdc,TRANSPARENT);
        hFont = CreateFont(52,0,0,0,FW_REGULAR,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY, DEFAULT_PITCH,TEXT("Impact"));
        SelectObject(hdc, hFont);
        DrawText(hdc, buffer1.str().c_str(), -1, &rc, DT_CENTER);
        DeleteObject(hFont);
    }
}

void recHist(int ax, int ay, int bx, int by, int type)
{
   if(hisMove<16)
   {
         history[hisMove][0]=ax;
         history[hisMove][1]=ay;
         history[hisMove][2]=bx;
         history[hisMove][3]=by;
         history[hisMove][4]=type;
         hisMove++;
   } else
   {
   for(int i =0; i<16-1; i++)
   {
    history[i][0]=history[i+1][0];
    history[i][1]=history[i+1][1];
    history[i][2]=history[i+1][2];
    history[i][3]=history[i+1][3];
    history[i][4]=history[i+1][4];
   }
    history[15][0]=ax;
    history[15][1]=ay;
    history[15][2]=bx;
    history[15][3]=by;
    history[15][4]=type;
   }


}
void recARHist(int ax, int ay, int bx, int by, int type)
{
   if(ARHisMove<antiRep)
   {
         ARHis[ARHisMove][0]=type;
         ARHis[ARHisMove][1]=ax;
         ARHis[ARHisMove][2]=ay;
         ARHis[ARHisMove][3]=bx;
         ARHis[ARHisMove][4]=by;
         ARHisMove++;
   } else
   {
   for(int i =0; i<antiRep-1; i++)
   {
    ARHis[i][0]=ARHis[i+1][0];
    ARHis[i][1]=ARHis[i+1][1];
    ARHis[i][2]=ARHis[i+1][2];
    ARHis[i][3]=ARHis[i+1][3];
    ARHis[i][4]=ARHis[i+1][4];
   }
    ARHis[antiRep-1][0]=type;
    ARHis[antiRep-1][1]=ax;
    ARHis[antiRep-1][2]=ay;
    ARHis[antiRep-1][3]=bx;
    ARHis[antiRep-1][4]=by;
   }


}

double antiRepeat(struct TempBoard board)
{
    int repeats=0;
    for(int i = 0; i<antiRep;i++)
    {
        if(ARHis[i][0]==board.last0&&
           ARHis[i][1]==board.last1&&
           ARHis[i][2]==board.last2&&
           ARHis[i][3]==board.last3&&
           ARHis[i][4]==board.last4)
        {
            repeats++;
        }
    }
    return repeats*0.25;
}

double Eva(struct TempBoard board)
{
    int bWolf=whiteWin(board.gameBoard);
    int tSheep=blackWin(board.sheep,board.unSheep,board.suSheep);

    if(bWolf>=4)
        return 2000+bWolf*2-tSheep*1.4+board.suSheep*0.2;
    else if (tSheep>=5)
        return -2000+bWolf*2-tSheep*1.4+board.suSheep*0.2;
    else
        return bWolf*2-tSheep*1.4+board.suSheep*0.2;
}

#define dminimax_alphabeta();                       \
if(turn==1)                                         \
{                                                   \
    if(amax<score)                                  \
    {                                               \
        amax=score;                                 \
        alpha = (alpha > score)? alpha : amax;      \
        if (beta < alpha) {                         \
            Prune++;                                \
            goto skip;                              \
        }                                           \
        bestBoard[0][0]=tempBoard.last0;            \
        bestBoard[0][1]=tempBoard.last1;            \
        bestBoard[0][2]=tempBoard.last2;            \
        bestBoard[0][3]=tempBoard.last3;            \
        bestBoard[0][4]=tempBoard.last4;            \
    }                                               \
}else                                               \
{                                                   \
    if(amin>score)                                  \
    {                                               \
        amin=score;                                 \
        beta = (beta < score)? beta : amin;         \
        if (beta < alpha) {                         \
            Prune++;                                \
            goto skip;                              \
        }                                           \
        bestBoard[0][0]=tempBoard.last0;            \
        bestBoard[0][1]=tempBoard.last1;            \
        bestBoard[0][2]=tempBoard.last2;            \
        bestBoard[0][3]=tempBoard.last3;            \
        bestBoard[0][4]=tempBoard.last4;            \
    }                                               \
}                                                   \

double AlphaBeta(int moves, int tmoves, struct TempBoard board,double alpha, double beta, int *bestMove)
{
    double score;
    double amax=-9999;
    double amin=9999;
    int turn=board.player;
    struct TempBoard tempBoard(board);
    unsigned int ranArr=0;
    int bestBoard[10][5];
    bool ab=true;
    int tmp2=0;//rand() %(ranArr+1);
    double curscr=Eva(board);

    if(moves==0||curscr>999||curscr<-999)
    {
        return curscr;
    }

    for (int y = 0; y<GBSIZE;y++)
    {
        for (int x = 0; x<GBSIZE;x++)
        {
            tempBoard=board;
            if(turn==1&&tempBoard.suSheep>0&&tempBoard.gameBoard[x][y]==0)
            {
                tempBoard.gameBoard[x][y]=3;
                tempBoard.sheep++;
                tempBoard.suSheep--;
                tempBoard.player=board.player==2?1:2;

                tempBoard.last0=3;
                tempBoard.last1=x;
                tempBoard.last2=y;
                tempBoard.last3=-1;
                tempBoard.last4=-1;

                score=AlphaBeta(moves-1,tmoves,tempBoard,alpha,beta,bestMove);
                if(score!=-9999)
                    score=score-antiRepeat(tempBoard);

                if(score>amax&&ab==false)
                {
                    amax=score;
                    bestBoard[0][0]=tempBoard.last0;
                    bestBoard[0][1]=tempBoard.last1;
                    bestBoard[0][2]=tempBoard.last2;
                    bestBoard[0][3]=tempBoard.last3;
                    bestBoard[0][4]=tempBoard.last4;
                }
                dminimax_alphabeta();

                tempBoard=board;

            }

            if(turn==1&&tempBoard.unSheep>0&&tempBoard.gameBoard[x][y]==0)
            {
                tempBoard.gameBoard[x][y]=1;
                tempBoard.sheep++;
                tempBoard.unSheep--;
                tempBoard.player=board.player==2?1:2;

                tempBoard.last0=1;
                tempBoard.last1=x;
                tempBoard.last2=y;
                tempBoard.last3=-1;
                tempBoard.last4=-1;

                score=AlphaBeta(moves-1,tmoves,tempBoard,alpha,beta,bestMove);
                if(score!=-9999)
                    score=score-antiRepeat(tempBoard);

                if(score>amax&&ab==false)
                {
                    amax=score;
                    bestBoard[0][0]=tempBoard.last0;
                    bestBoard[0][1]=tempBoard.last1;
                    bestBoard[0][2]=tempBoard.last2;
                    bestBoard[0][3]=tempBoard.last3;
                    bestBoard[0][4]=tempBoard.last4;
                }
                dminimax_alphabeta();

                tempBoard=board;
            }
            else if(turn==1&&tempBoard.suSheep==0&&tempBoard.unSheep==0&&(tempBoard.gameBoard[x][y]==1||tempBoard.gameBoard[x][y]==3))
            {
                for(int a=-1;a<2;a++)
                   {
                       for(int b=-1;b<2;b++)
                       {
                           if(a!=0||b!=0)
                           {
                               if(movePiece(x,y,x+a,y+b,tempBoard.gameBoard,tempBoard.player))
                               {
                                   tempBoard.player=board.player==2?1:2;
                                    tempBoard.last0=tempBoard.gameBoard[x+a][y+b];
                                    tempBoard.last1=x;
                                    tempBoard.last2=y;
                                    tempBoard.last3=x+a;
                                    tempBoard.last4=y+b;
                                    score=AlphaBeta(moves-1,tmoves,tempBoard,alpha,beta,bestMove);
                                    if(score!=-9999)
                                        score=score-antiRepeat(tempBoard);

                                    if(score>amax&&ab==false)
                                    {
                                        amax=score;
                                        bestBoard[0][0]=tempBoard.last0;
                                        bestBoard[0][1]=tempBoard.last1;
                                        bestBoard[0][2]=tempBoard.last2;
                                        bestBoard[0][3]=tempBoard.last3;
                                        bestBoard[0][4]=tempBoard.last4;
                                    }
                                    dminimax_alphabeta();

                                    tempBoard=board;
                               }
                           }
                       }
                   }
            }
            else if(turn==2&&(tempBoard.gameBoard[x][y]==2||tempBoard.gameBoard[x][y]==4))
            {
                for(int a=-2;a<3;a++)
                   {
                       for(int b=-2;b<3;b++)
                       {
                           if(a!=0||b!=0)
                           {
                               if(movePiece(x,y,x+a,y+b,tempBoard.gameBoard,tempBoard.player))
                               {
                                   if(dPiece[0]!=-1)
                                    tempBoard.sheep--;

                                   if(!(tempBoard.gameBoard[x+a][y+b]==4&&dPiece[0]!=-1))
                                   tempBoard.player=board.player==2?1:2;

                                   dPiece[0]=-1;

                                    tempBoard.last0=tempBoard.gameBoard[x+a][y+b];
                                    tempBoard.last1=x;
                                    tempBoard.last2=y;
                                    tempBoard.last3=x+a;
                                    tempBoard.last4=y+b;

                                    score=AlphaBeta(moves-1,tmoves,tempBoard,alpha,beta,bestMove);
                                    if(score!=9999)
                                        score=score+antiRepeat(tempBoard);

                                    if(score<amin&&ab==false)
                                    {
                                        amin=score;
                                        bestBoard[0][0]=tempBoard.last0;
                                        bestBoard[0][1]=tempBoard.last1;
                                        bestBoard[0][2]=tempBoard.last2;
                                        bestBoard[0][3]=tempBoard.last3;
                                        bestBoard[0][4]=tempBoard.last4;
                                    }
                                    dminimax_alphabeta();

                                    tempBoard=board;
                               }
                           }
                       }
                   }
            }
        }
    }
    skip:

    bestMove[0] = bestBoard[tmp2][0];
    bestMove[1] = bestBoard[tmp2][1];
    bestMove[2] = bestBoard[tmp2][2];
    bestMove[3] = bestBoard[tmp2][3];
    bestMove[4] = bestBoard[tmp2][4];

    if(turn==1)                    //depending on whose turn it is returns the amax or amin need for the alpha beta pruning
    {
        return amax;
    }else
    {
        return amin;
    }
}

 double AIPrep(int *nextTurn)
 {
     struct TempBoard tempBoard;
     for (int y = 0; y<GBSIZE;y++)
    {
        for (int x = 0; x<GBSIZE;x++)
        {
            tempBoard.gameBoard[x][y]=gameBoard[x][y];
        }
    }
    tempBoard.player=player;
    tempBoard.sheep=sheep;
    tempBoard.suSheep=suSheep;
    tempBoard.unSheep=unSheep;
    double alpha=-9999;
    double beta=9999;

    int tmoves=moves;

    double score=AlphaBeta(moves,tmoves,tempBoard,alpha,beta, nextTurn);

    return score;
 }

string translet(int x)
{
    switch(x)
    {
        case(-1):
        {
          return "";
        }
        break;
        case(0):
        {
          return "A";
        }
        break;
        case(1):
        {
          return "B";
        }
        break;
        case(2):
        {
          return "C";
        }
        break;
        case(3):
        {
          return "D";
        }
        break;
        case(4):
        {
          return "E";
        }
        break;
        case(5):
        {
          return "F";
        }
        break;
        default:
            return "";
    }
}

string transtyp(int x)
{
    switch(x)
    {
        case(1):
        {
          return "Sheep";
        }
        break;
        case(2):
        {
          return "Wolf";
        }
        break;
        case(3):
        {
          return "Super sheep";
        }
        break;
        case(4):
        {
          return "Super wolf";
        }
        break;
        case(5):
        {
          return "wall";
        }
        break;
        default:
            return "";
    }
}

void ShowHist(HWND hwnd, HDC hdc)
{
    RECT rc;
    HFONT hFont;
    if(GetClientRect(hwnd,&rc))
    {
        std::stringstream buffer;
        buffer << "History";
        rc.bottom-=280;
        rc.top = rc.bottom - 600;
        rc.right-=1200;
        rc.left=rc.right-300;
        FillRect(hdc,&rc, (HBRUSH)GetStockObject(GRAY_BRUSH));
        SetTextColor(hdc,RGB(102,252,241));
        SetBkMode(hdc,TRANSPARENT);
        hFont = CreateFont(48,0,0,0,FW_REGULAR,FALSE,true,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY, DEFAULT_PITCH,TEXT("Impact"));
        SelectObject(hdc, hFont);
        DrawText(hdc, buffer.str().c_str(), -1, &rc, DT_CENTER);
        //SetTextColor(hdc,RGB(69,162,158));
        buffer.str("");
        buffer<<"\n\n";
        for(int i = 0; i<hisMove;i++)
        {
            if(history[i][2]!=-1&&history[i][3]!=-1)
            buffer<<translet(history[i][0])<<history[i][1]+1<< " to " <<translet(history[i][2])<<history[i][3]+1<<"; type - "<<transtyp(history[i][4])<<"\n";
            else
            buffer<<translet(history[i][0])<<history[i][1]+1<<"; type - "<<transtyp(history[i][4])<<"\n";
        }
        hFont = CreateFont(32,0,0,0,FW_REGULAR,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY, DEFAULT_PITCH,TEXT("Impact"));
        SelectObject(hdc, hFont);
        DrawText(hdc, buffer.str().c_str(), -1, &rc, DT_CENTER);
    }

}

void ShowRez(HWND hwnd, HDC hdc)
{
    RECT rc;
    HFONT hFont;
    if(GetClientRect(hwnd,&rc))
    {
        std::stringstream buffer;
        buffer << "Result";
        rc.bottom-=580;
        rc.top = rc.bottom - 300;
        rc.right-=100;
        rc.left=rc.right-300;
        FillRect(hdc,&rc, (HBRUSH)GetStockObject(GRAY_BRUSH));
        SetTextColor(hdc,RGB(102,252,241));
        SetBkMode(hdc,TRANSPARENT);
        hFont = CreateFont(48,0,0,0,FW_REGULAR,FALSE,true,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY, DEFAULT_PITCH,TEXT("Impact"));
        SelectObject(hdc, hFont);
        DrawText(hdc, buffer.str().c_str(), -1, &rc, DT_CENTER);
        buffer.str("");
        buffer<<"\n\n";
        SetTextColor(hdc,RGB(102,252,241));
        buffer<<"Sheep wins - "<<sWin;
        hFont = CreateFont(48,0,0,0,FW_REGULAR,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY, DEFAULT_PITCH,TEXT("Impact"));
        SelectObject(hdc, hFont);
        DrawText(hdc, buffer.str().c_str(), -1, &rc, DT_CENTER);
        SetTextColor(hdc,RGB(102,252,241));
        buffer.str("");
        buffer<<"\n\n\n\nWolf wins - "<<wWin;
        DrawText(hdc, buffer.str().c_str(), -1, &rc, DT_CENTER);

    }

}

void ShowTaken(HWND hwnd, HDC hdc)
{
    RECT rc;
    HFONT hFont;
    int Taken = sheepTot-sheep-unSheep-suSheep;
    if(GetClientRect(hwnd,&rc))
    {
        std::stringstream buffer;
        buffer << "Taken sheep: "<<Taken;
        rc.bottom-=370;
        rc.top = rc.bottom - 70;
        rc.right-=100;
        rc.left=rc.right-300;
        FillRect(hdc,&rc, (HBRUSH)GetStockObject(GRAY_BRUSH));
        SetTextColor(hdc,RGB(102,252,241));
        SetBkMode(hdc,TRANSPARENT);
        hFont = CreateFont(48,0,0,0,FW_REGULAR,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY, DEFAULT_PITCH,TEXT("Impact"));
        SelectObject(hdc, hFont);
        DrawText(hdc, buffer.str().c_str(), -1, &rc, DT_CENTER);

    }

}

void PlayAIMove(HWND hwnd)
{
if(winner==0)
{
int nextTurn[5]={-1};
AIPrep(nextTurn);
//printf("Nodes - %d, Pruned - %d\n",Nodes,Prune);
dPiece[0]=-1;
int tmp = 0;
if(nextTurn[0]==-1)
    printf("error next turn -1\n");
else
{
    if(nextTurn[0]==1&&unSheep>0)
    {
        placeSheep(nextTurn[1],nextTurn[2],gameBoard,nextTurn[0]);
        unSheep--;sheep++;
    } else if(nextTurn[0]==3&&suSheep>0)
    {
        placeSheep(nextTurn[1],nextTurn[2],gameBoard,nextTurn[0]);
        suSheep--;sheep++;
    } else
    {
        movePiece(nextTurn[1],nextTurn[2],nextTurn[3],nextTurn[4],gameBoard,player);
        if(dPiece[0]!=-1)
           {sheep--;
           }

    }
    recHist(nextTurn[1],nextTurn[2],nextTurn[3],nextTurn[4],nextTurn[0]);
    recARHist(nextTurn[1],nextTurn[2],nextTurn[3],nextTurn[4],nextTurn[0]);
}
tmp =dPiece[0];
HDC hdc = GetDC(hwnd);

RedrawBoard(hwnd,gameBoard);
ShowHist(hwnd,hdc);
ShowRez(hwnd, hdc);
ShowTaken(hwnd, hdc);
if(blackWin(sheep,unSheep,suSheep)>=5)
{
    winner=2;
    wWin++;
    printf("\n Black player is the winner!\n");
    ShowTurn(hwnd,hdc);
    return;
}
else if(whiteWin(gameBoard)>=4)
{
    winner=1;
    sWin++;
    printf("\n White player is the winner!\n");
    ShowTurn(hwnd,hdc);
    return;
}
if(!(nextTurn[0]==4&&tmp!=-1))
{changeTurn();
} else{
dPiece[0]=-1;
Sleep(wait*100);
if(winner==0)
PlayAIMove(hwnd);
}
dPiece[0]=-1;

ShowTurn(hwnd,hdc);
}
}

void resetGameBoard(HWND hwnd)
{
    for(int x=0;x<GBSIZE;x++)
    {
       for(int y=0;y<GBSIZE;y++)
       {
           gameBoard[x][y]=0;
       }
    }
    player=1;
    winner=0;
    dPiece[0]=-1;
    sheep = 0;

    ax=-1;
    ay=-1;
    bx=-1;
    by=-1;
    hisMove=0;

    srand(time(0));
    int tmp=0;
    ranGen(0, (GBSIZE*GBSIZE)-1, GBSIZE,ran);
    //sort(ran, ran + sizeof(ran) / sizeof(ran[0]));
    while(tmp<GBSIZE) //inverted
    {
        for(int x=0;x<GBSIZE;x++)
        {
            for(int y=0;y<GBSIZE;y++)
            {
            if(x+GBSIZE*y==ran[tmp])
                {
                gameBoard[x][y]=5;
                tmp++;
                }

            }
        }
    }

    gameBoard[0][GBSIZE-1]=4;
    gameBoard[GBSIZE-1][0]=4;
    gameBoard[GBSIZE-1][GBSIZE-1]=2;
    gameBoard[0][0]=2;
    suSheep=isuSheep;
    sheepTot = isheepTot;
    unSheep = isheepTot-suSheep;

    InvalidateRect(hwnd,NULL,true);
    UpdateWindow(hwnd);
    RedrawWindow(hwnd, NULL, NULL, RDW_UPDATENOW);
}

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    ULONG_PTR token;
    GdiplusStartupInput input = {0};
    input.GdiplusVersion = 1;
    GdiplusStartup(&token, &input, NULL);

    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = (HICON)LoadImage(GetModuleHandle(NULL),MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON, //I set the programs icons here
            128, 128,LR_DEFAULTCOLOR);
    wincl.hIconSm = (HICON)LoadImage(GetModuleHandle(NULL),MAKEINTRESOURCE(IDI_ICON2),IMAGE_ICON,
            128, 128,LR_DEFAULTCOLOR);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;
    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("BaghChal"),       /* Title Text */
           (WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU), /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           1600,                 /* The programs width */
           1020,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           LoadMenu(hThisInstance, MAKEINTRESOURCE(IDR_MYMENU)),                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (GetConsoleWindow(), SW_HIDE);  //Hides CMD WINDOW
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_LBUTTONDOWN:
        {

            if (winner!=0)
                break;
            if(pvp==2)
            {
                int nextTurn[5]={-1};
                AIPrep(nextTurn);
                dPiece[0]=-1;
                int tmp = 0;
                if(nextTurn[0]==-1)
                    printf("error next turn -1\n");
                else
                {
                    if(nextTurn[0]==1&&unSheep>0)
                    {
                        placeSheep(nextTurn[1],nextTurn[2],gameBoard,nextTurn[0]);
                        unSheep--;sheep++;
                    } else if(nextTurn[0]==3&&suSheep>0)
                    {
                        placeSheep(nextTurn[1],nextTurn[2],gameBoard,nextTurn[0]);
                        suSheep--;sheep++;
                    } else
                    {
                        movePiece(nextTurn[1],nextTurn[2],nextTurn[3],nextTurn[4],gameBoard,player);
                        if(dPiece[0]!=-1)
                           {sheep--;
                           }
                    }
                    recHist(nextTurn[1],nextTurn[2],nextTurn[3],nextTurn[4],nextTurn[0]);
                    recARHist(nextTurn[1],nextTurn[2],nextTurn[3],nextTurn[4],nextTurn[0]);
                }
                tmp=dPiece[0];
                HDC hdc = GetDC(hwnd);


                if(blackWin(sheep,unSheep,suSheep)>=5)
                {
                    winner=2;
                    wWin++;
                    printf("\n Black player is the winner!\n");
                }
                else if(whiteWin(gameBoard)>=4)
                {
                    winner=1;
                    sWin++;
                    printf("\n White player is the winner!\n");
                }
                RedrawBoard(hwnd,gameBoard);

                ShowHist(hwnd,hdc);
                ShowRez(hwnd, hdc);
                ShowTaken(hwnd, hdc);
                if(!(nextTurn[0]==4&&tmp!=-1)&&winner==0)
                changeTurn();
                dPiece[0]=-1;
                ShowTurn(hwnd,hdc);
            }
            else if(pvp==3)
            {
                int tmpturn=0;
                int times =100;
                int bwin=0;
                int wwin=0;
                int draw=0;

                /*remove("test.csv");
                ofstream outData;
                outData.open("test.csv",ios::app);
                outData<<"moves ts tss wwin bwin draw"<<endl;*/
                long long unsigned sum=0;

                //auto started = std::chrono::high_resolution_clock::now();
                while(winner==0&&tmpturn<20)
                    {
                tmpturn++;
                int nextTurn[5]={-1};
                AIPrep(nextTurn);
                //printf("Move Score - %f\n",AIPrep(nextTurn));
                //printf("Nodes - %d, Pruned - %d\n",Nodes,Prune);
                dPiece[0]=-1;
                int tmp = 0;

                if(nextTurn[0]==-1)
                    printf("error next turn -1\n");
                else
                {
                    if(nextTurn[0]==1&&unSheep>0)
                    {
                        placeSheep(nextTurn[1],nextTurn[2],gameBoard,nextTurn[0]);
                        unSheep--;sheep++;
                    } else if(nextTurn[0]==3&&suSheep>0)
                    {
                        placeSheep(nextTurn[1],nextTurn[2],gameBoard,nextTurn[0]);
                        suSheep--;sheep++;
                    } else
                    {
                        movePiece(nextTurn[1],nextTurn[2],nextTurn[3],nextTurn[4],gameBoard,player);
                        if(dPiece[0]!=-1)
                           {sheep--;
                           }
                    }
                    recHist(nextTurn[1],nextTurn[2],nextTurn[3],nextTurn[4],nextTurn[0]);
                    recARHist(nextTurn[1],nextTurn[2],nextTurn[3],nextTurn[4],nextTurn[0]);
                }
                HDC hdc = GetDC(hwnd);
                tmp = dPiece[0];


                if(blackWin(sheep,unSheep,suSheep)>=5)
                {
                    winner=2;
                    wWin++;
                    //printf("\n Black player is the winner!\n");
                }
                else if(whiteWin(gameBoard)>=4)
                {
                    winner=1;
                    sWin++;
                    //printf("\n White player is the winner!\n");
                }
                RedrawBoard(hwnd,gameBoard);

                ShowHist(hwnd,hdc);
                ShowRez(hwnd, hdc);
                ShowTaken(hwnd, hdc);
                if(!(nextTurn[0]==4&&tmp!=-1)&&winner==0)
                changeTurn();
                dPiece[0]=-1;
                ShowTurn(hwnd,hdc);
                /*if(winner!=0||tmpturn==400)
                    {
                        if(winner==1)
                            wwin++;
                        else if (winner==2)
                            bwin++;
                        else
                            draw++;

                        winner=0;resetGameBoard(hwnd);
                        tmpturn=0;
                        times--;
                    }*/
                }
            }else{
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);
            HDC hdc = GetDC(hwnd);
            if(hdc!=NULL){

            if(!sPiece)
            {
            GetCellNumberFromPoint(hwnd,xPos,yPos,gameBoard,ax,ay);

                if(unSheep>0 && player == 1)
                {
                    if(gameBoard[ax][ay]==0)
                    if(placeSheep(ax,ay,gameBoard,1))
                        {
                            unSheep--;
                            sheep++;
                            recHist(ax,ay,bx,by,1);
                            ax=-1;ay=-1;bx=-1;by=-1;
                            changeTurn();
                            if(pvp==1)
                                PlayAIMove(hwnd);
                        }

                } else if(ax!=-1&&ay!=-1&&(gameBoard[ax][ay]==2||gameBoard[ax][ay]==4)||(player==1&&suSheep==0&&unSheep==0&&(gameBoard[ax][ay]==1||gameBoard[ax][ay]==3)))
                {
                    sPiece=true;
                }
            } else
            {
            GetCellNumberFromPoint(hwnd,xPos,yPos,gameBoard,bx,by);

                if(ax==bx&&ay==by)
                {
                    ax=-1;ay=-1;bx=-1;by=-1;
                    sPiece = false;
                } else if(gameBoard[bx][by]==player||(gameBoard[bx][by]==player+2&&player+2<5))
                {
                    ax=bx;
                    ay=by;

                } else
                {
                    if(movePiece(ax,ay,bx,by,gameBoard,player)==true)
                        {
                            if(dPiece[0]!=-1)
                                sheep--;
                        recHist(ax,ay,bx,by,gameBoard[bx][by]);
                        if(!(gameBoard[bx][by]==4&&dPiece[0]!=-1))
                        {changeTurn();
                        dPiece[0]=-1;
                        ax=-1;ay=-1;bx=-1;by=-1;
                        if(pvp==1)
                        PlayAIMove(hwnd);}

                        dPiece[0]=-1;
                        ax=-1;ay=-1;bx=-1;by=-1;
                        sPiece=false;


                        }
                        else
                        {
                        bx=-1;by=-1;
                        }
                }
            }
            }

            RedrawBoard(hwnd,gameBoard);
            ShowTurn(hwnd,hdc);
            ShowHist(hwnd,hdc);
            ShowRez(hwnd, hdc);
            ShowTaken(hwnd, hdc);

            }
        }
        break;
        case WM_RBUTTONDOWN:
        {
            if (winner!=0)
                break;
            if(pvp==0||(player==1&&pvp==1))
            {
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);
            HDC hdc = GetDC(hwnd);
            RECT rcCell;

            if(hdc!=NULL){

                GetCellNumberFromPoint(hwnd,xPos,yPos,gameBoard,ax,ay);
                //printf("%d %d %d %d, turn - %d\n", ax,ay,bx,by,player);
                if(suSheep>0 && player == 1)
                {
                    if(placeSheep(ax,ay,gameBoard,3))
                        {
                            GetCellRect(hwnd,ax,ay,&rcCell);
                            //DrawIconCentered(hdc,&rcCell,aIcon);
                            suSheep--;
                            sheep++;
                            recHist(ax,ay,bx,by,3);
                            ax=-1;ay=-1;bx=-1;by=-1;
                            changeTurn();
                            if(pvp==1)
                            PlayAIMove(hwnd);
                        }

                }

            }
            if(whiteWin(gameBoard)>=4)
                {
                    winner=1;
                    sWin++;
                    printf("\n White player is the winner!\n");
                }
            InvalidateRect(hwnd,NULL,true);
            UpdateWindow(hwnd);
            RedrawWindow(hwnd, NULL, NULL, RDW_UPDATENOW);
            }
            else{
            InvalidateRect(hwnd,NULL,true);
            UpdateWindow(hwnd);
            RedrawWindow(hwnd, NULL, NULL, RDW_UPDATENOW);
            }
        }
        break;
        case WM_CREATE:
        {
            srand(time(0));
            int tmp=0;
            ranGen(0, (GBSIZE*GBSIZE)-1, GBSIZE,ran);
            //sort(ran, ran + sizeof(ran) / sizeof(ran[0]));
            while(tmp<GBSIZE) //inverted
            {
                for(int x=0;x<GBSIZE;x++)
                {
                    for(int y=0;y<GBSIZE;y++)
                    {
                    if(x+GBSIZE*y==ran[tmp])
                        {
                        gameBoard[x][y]=5;
                        tmp++;
                        }

                    }
                }
            }
            gameBoard[0][GBSIZE-1]=4;
            gameBoard[GBSIZE-1][0]=4;
            gameBoard[GBSIZE-1][GBSIZE-1]=2;
            gameBoard[0][0]=2;

            /* interesting case NOT A BUG
            gameBoard[2][3]=4;
            gameBoard[3][3]=1;
            gameBoard[3][2]=1;
            gameBoard[4][3]=0;
            gameBoard[2][1]=0;
            */

            // white win
            /*
            gameBoard[1][1]=3;
            gameBoard[0][2]=3;
            gameBoard[1][2]=3;
            gameBoard[1][1]=3;
            gameBoard[1][0]=3;
            gameBoard[4][4]=3;
            gameBoard[0][4]=3;
            gameBoard[4][0]=3;
            gameBoard[4][1]=3;
            gameBoard[1][4]=3;
            gameBoard[5][1]=3;
            gameBoard[1][5]=3;
            gameBoard[5][4]=3;
            gameBoard[4][5]=3;
            */

            //SetTimer (hwnd, ID_TIMER, 1000, NULL);
        }
        break;
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            Graphics graphics(hdc);
            RECT rc;

            if(GetGameBoardRect(hwnd, &rc)){

                RECT rcClient;

                SetBkMode(hdc,TRANSPARENT);
                if(GetClientRect(hwnd,&rcClient)){
                    ShowTurn(hwnd,hdc);
                }

                Rectangle(hdc,rc.left,rc.top,rc.right,rc.bottom);

                for(int tmp=0; tmp<=GBSIZE;tmp++){
                DrawLine(hdc,rc.left+CELL_SIZE*tmp,rc.top,rc.left+CELL_SIZE*tmp,rc.bottom);
                if(tmp<=GBSIZE)
                DrawLine(hdc,rc.left,rc.top+CELL_SIZE*tmp,rc.right,rc.top+CELL_SIZE*tmp);
                }
                RedrawBoard(hwnd,gameBoard);
                ShowSide(hwnd,hdc);
                ShowHist(hwnd,hdc);
                ShowRez(hwnd, hdc);
                ShowTaken(hwnd, hdc);

                /*if(counts > 0)
                {
                    RECT rcsa;
                    rcsa.bottom-=0;
                    rcsa.top = rcsa.bottom + 1000;
                    rcsa.right-=0;
                    rcsa.left=rcsa.right-1600;
                    FillRect(hdc,&rcsa, CreateSolidBrush(RGB(255,0,0)));
                    SetBkMode(hdc,TRANSPARENT);
                    //GetClientRect (hwnd, &rcClient);
                    //wsprintf (szBuffer, "Seconds: %d", sec);
                    //DrawText (hdc, szBuffer, -1, &rc, DT_LEFT);
                }
                else
                {
                    SendMessage (hwnd, WM_CLOSE, 0, 0L);
                }*/

            }

            EndPaint(hwnd, &ps);

        }
        break;

        /*case WM_TIMER:
        {
            counts--;
            InvalidateRect (hwnd, NULL, TRUE);
        }
        break;*/

        case WM_COMMAND:
            switch(LOWORD(wParam)){
                case ID_EXIT:
                    {
                    PostQuitMessage(0);
                    return 0;
                    }

                break;
                case ID_RESTART:
                    {
                    resetGameBoard(hwnd);
                    /*if(pvp==1)
                        if(rand() % 2 + 1==1)
                            PlayAIMove(hwnd);*/
                    }
                break;
                case PVP:
                    {
                        pvp=0;
                    resetGameBoard(hwnd);
                    }
                break;
                case PVAS:
                    {
                        pvp=1;
                    resetGameBoard(hwnd);
                    }
                break;
                case PVAW:
                    {
                        pvp=1;
                    resetGameBoard(hwnd);
                    PlayAIMove(hwnd);
                    }
                break;
                case AVA:
                    {
                        pvp=2;
                    resetGameBoard(hwnd);
                    }
                break;
                case FAVA:
                    {
                        pvp=3;
                    resetGameBoard(hwnd);
                    }
                break;
                case A1:
                    {
                        moves=1;
                        if(pvp==0)
                        {
                            pvp=1;
                        }
                    resetGameBoard(hwnd);
                    }
                break;
                case A2:
                    {
                        moves=2;
                        if(pvp==0)
                        {
                            pvp=1;
                        }
                    resetGameBoard(hwnd);
                    }
                break;
                case A3:
                    {
                        moves=3;
                        if(pvp==0)
                        {
                            pvp=1;
                        }
                    resetGameBoard(hwnd);
                    }
                break;
                case A4:
                    {
                        moves=4;
                        if(pvp==0)
                        {
                            pvp=1;
                        }
                    resetGameBoard(hwnd);
                    }
                break;
                case A5:
                    {
                        moves=5;
                        if(pvp==0)
                        {
                            pvp=1;
                        }
                    resetGameBoard(hwnd);
                    }
                break;
                case A6:
                    {
                        moves=6;
                        if(pvp==0)
                        {
                            pvp=1;
                        }
                    resetGameBoard(hwnd);
                    }
                break;
                case P1:
                    {
                        pic=1;
                        RedrawBoard(hwnd,gameBoard);
                    }
                break;
                case P2:
                    {
                        pic=2;
                        RedrawBoard(hwnd,gameBoard);
                    }
                break;
                case P3:
                    {
                        pic=3;
                        RedrawBoard(hwnd,gameBoard);
                    }
                break;
            }
            break;
        case WM_DESTROY:
            KillTimer (hwnd, ID_TIMER);
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
