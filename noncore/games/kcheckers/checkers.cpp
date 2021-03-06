//
// KCheckers Engine

//
// Board = 54 Fields:
//
// |  06  07  08  09| MAN2
// |11  12  13  14  |
// |  17  18  19  20|
// |22  23  24  25  |
// |  28  29  30  31|
// |33  34  35  36  |
// |  39  40  41  42|
// |44  45  46  47  | MAN1

#include <time.h>
#include <stdlib.h>

#include "checkers.h"


Checkers::Checkers(int skill)
{
  for(int i=0;i<54;i++) board[i]=NONE;
  board[6] =MAN2; board[7] =MAN2; board[8] =MAN2; board[9] =MAN2;
  board[11]=MAN2; board[12]=MAN2; board[13]=MAN2; board[14]=MAN2;
  board[17]=MAN2; board[18]=MAN2; board[19]=MAN2; board[20]=MAN2;
  board[22]=FREE; board[23]=FREE; board[24]=FREE; board[25]=FREE;
  board[28]=FREE; board[29]=FREE; board[30]=FREE; board[31]=FREE;
  board[33]=MAN1; board[34]=MAN1; board[35]=MAN1; board[36]=MAN1;
  board[39]=MAN1; board[40]=MAN1; board[41]=MAN1; board[42]=MAN1;
  board[44]=MAN1; board[45]=MAN1; board[46]=MAN1; board[47]=MAN1;

  levelmax=skill;

  srand(time(0)); // Seed the random number generator
}


///////////////////////////////////////////////////
//
//  User Functions
//
///////////////////////////////////////////////////


bool Checkers::checkMove1()
{
  for(int i=6;i<48;i++)
  {
    switch(board[i])
    {
      case MAN1:
        if(board[i-6]==FREE) return true;
        if(board[i-5]==FREE) return true;
        break;
      case KING1:
        if(board[i-6]==FREE) return true;
        if(board[i-5]==FREE) return true;
        if(board[i+5]==FREE) return true;
        if(board[i+6]==FREE) return true;
    }
  }
  return false;
}


////////////////////////////////////////////////////
//
// Computer Functions
//
////////////////////////////////////////////////////


void Checkers::go2()
{
  level=0;
  for(int i=6;i<48;i++) bestboard[i]=board[i];
  turn();
  for(int i=6;i<48;i++) board[i]=bestboard[i];
  ;
}


void Checkers::turn(int &resMax,bool capture)
{
  if(level<levelmax)
  {
    bool f12,f13,f14,f17,f18,f19,f23,f24,f25,f28,f29,f30,f34,f35,f36,f39,f40,f41;
    if(capture)
    {
      if(board[12]==NONE) {f12=true;board[12]=FREE;} else f12=false;
      if(board[13]==NONE) {f13=true;board[13]=FREE;} else f13=false;
      if(board[14]==NONE) {f14=true;board[14]=FREE;} else f14=false;
      if(board[17]==NONE) {f17=true;board[17]=FREE;} else f17=false;
      if(board[18]==NONE) {f18=true;board[18]=FREE;} else f18=false;
      if(board[19]==NONE) {f19=true;board[19]=FREE;} else f19=false;
      if(board[23]==NONE) {f23=true;board[23]=FREE;} else f23=false;
      if(board[24]==NONE) {f24=true;board[24]=FREE;} else f24=false;
      if(board[25]==NONE) {f25=true;board[25]=FREE;} else f25=false;
      if(board[28]==NONE) {f28=true;board[28]=FREE;} else f28=false;
      if(board[29]==NONE) {f29=true;board[29]=FREE;} else f29=false;
      if(board[30]==NONE) {f30=true;board[30]=FREE;} else f30=false;
      if(board[34]==NONE) {f34=true;board[34]=FREE;} else f34=false;
      if(board[35]==NONE) {f35=true;board[35]=FREE;} else f35=false;
      if(board[36]==NONE) {f36=true;board[36]=FREE;} else f36=false;
      if(board[39]==NONE) {f39=true;board[39]=FREE;} else f39=false;
      if(board[40]==NONE) {f40=true;board[40]=FREE;} else f40=false;
      if(board[41]==NONE) {f41=true;board[41]=FREE;} else f41=false;
    }

    int b6=board[6];
    int b7=board[7];
    int b8=board[8];
    int b9=board[9];
    int b11=board[11];
    int b12=board[12];
    int b13=board[13];
    int b14=board[14];
    int b17=board[17];
    int b18=board[18];
    int b19=board[19];
    int b20=board[20];
    int b22=board[22];
    int b23=board[23];
    int b24=board[24];
    int b25=board[25];
    int b28=board[28];
    int b29=board[29];
    int b30=board[30];
    int b31=board[31];
    int b33=board[33];
    int b34=board[34];
    int b35=board[35];
    int b36=board[36];
    int b39=board[39];
    int b40=board[40];
    int b41=board[41];
    int b42=board[42];
    int b44=board[44];
    int b45=board[45];
    int b46=board[46];
    int b47=board[47];

    board[6]=FULL-b47;
    board[7]=FULL-b46;
    board[8]=FULL-b45;
    board[9]=FULL-b44;
    board[11]=FULL-b42;
    board[12]=FULL-b41;
    board[13]=FULL-b40;
    board[14]=FULL-b39;
    board[17]=FULL-b36;
    board[18]=FULL-b35;
    board[19]=FULL-b34;
    board[20]=FULL-b33;
    board[22]=FULL-b31;
    board[23]=FULL-b30;
    board[24]=FULL-b29;
    board[25]=FULL-b28;
    board[28]=FULL-b25;
    board[29]=FULL-b24;
    board[30]=FULL-b23;
    board[31]=FULL-b22;
    board[33]=FULL-b20;
    board[34]=FULL-b19;
    board[35]=FULL-b18;
    board[36]=FULL-b17;
    board[39]=FULL-b14;
    board[40]=FULL-b13;
    board[41]=FULL-b12;
    board[42]=FULL-b11;
    board[44]=FULL-b9;
    board[45]=FULL-b8;
    board[46]=FULL-b7;
    board[47]=FULL-b6;

    int res=-turn();

    board[6]=b6;
    board[7]=b7;
    board[8]=b8;
    board[9]=b9;
    board[11]=b11;
    board[12]=b12;
    board[13]=b13;
    board[14]=b14;
    board[17]=b17;
    board[18]=b18;
    board[19]=b19;
    board[20]=b20;
    board[22]=b22;
    board[23]=b23;
    board[24]=b24;
    board[25]=b25;
    board[28]=b28;
    board[29]=b29;
    board[30]=b30;
    board[31]=b31;
    board[33]=b33;
    board[34]=b34;
    board[35]=b35;
    board[36]=b36;
    board[39]=b39;
    board[40]=b40;
    board[41]=b41;
    board[42]=b42;
    board[44]=b44;
    board[45]=b45;
    board[46]=b46;
    board[47]=b47;

    if(res>resMax)
    {
      resMax=res;
      if(level==1)
      {
        for(int i=6;i<48;i++) bestboard[i]=board[i];
        bestcounter=1;
      }
    }
    else if(res==resMax && level==1)
    {
      bestcounter++;
      if((rand()%bestcounter)==0)
      {
        for(int i=6;i<48;i++) bestboard[i]=board[i];
      }
    }

    if(capture)
    {
      if(f12) board[12]=NONE;
      if(f13) board[13]=NONE;
      if(f14) board[14]=NONE;
      if(f17) board[17]=NONE;
      if(f18) board[18]=NONE;
      if(f19) board[19]=NONE;
      if(f23) board[23]=NONE;
      if(f24) board[24]=NONE;
      if(f25) board[25]=NONE;
      if(f28) board[28]=NONE;
      if(f29) board[29]=NONE;
      if(f30) board[30]=NONE;
      if(f34) board[34]=NONE;
      if(f35) board[35]=NONE;
      if(f36) board[36]=NONE;
      if(f39) board[39]=NONE;
      if(f40) board[40]=NONE;
      if(f41) board[41]=NONE;
    }
  }
  else if(resMax<0) resMax=0;
}


bool Checkers::checkMove2()
{
  for(int i=6;i<48;i++)
  {
    switch(board[i])
    {
      case MAN2:
        if(board[i+5]==FREE) return true;
        if(board[i+6]==FREE) return true;
        break;
      case KING2:
        if(board[i-6]==FREE) return true;
        if(board[i-5]==FREE) return true;
        if(board[i+5]==FREE) return true;
        if(board[i+6]==FREE) return true;
    }
  }
  return false;
}


int Checkers::turn()
{
  int resMax=(level-levelmax)*10;
  level++;
  if(checkCapture2())
  {
    for(int i=6;i<48;i++)
    {
      switch(board[i])
      {
        case MAN2:
          manCapture2(i,resMax);
          break;
        case KING2:
          kingCapture2(i,UL,resMax);
          kingCapture2(i,UR,resMax);
          kingCapture2(i,DL,resMax);
          kingCapture2(i,DR,resMax);
      }
    }
  }
  else if(checkMove2())
  {
    for(int i=6;i<48;i++)
    {
      switch(board[i])
      {
        case MAN2:
          if(board[i+5]==FREE)
          {
            board[i]=FREE;
            if(i>38) board[i+5]=KING2;
            else board[i+5]=MAN2;
            turn(resMax);
            board[i+5]=FREE;
            board[i]=MAN2;
          }
          if(board[i+6]==FREE)
          {
            board[i]=FREE;
            if(i>38) board[i+6]=KING2;
            else board[i+6]=MAN2;
            turn(resMax);
            board[i+6]=FREE;
            board[i]=MAN2;
          }
          break;
        case KING2:
          kingMove2(i,resMax);
          break;
      }
    }
  }
  else ;

  level--;
  return resMax;
}


