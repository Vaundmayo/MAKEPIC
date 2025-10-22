#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <stdarg.h>

void clrscr() {
    printf("\033[2J\033[1;1H");
    fflush(stdout);
}

void gotoxy(int x, int y) {
    printf("\033[%d;%dH", y, x);
    fflush(stdout);
}

void textcolor(int color) {
    printf("\033[%dm", color + 82);
    fflush(stdout);
}

int getch(void) {
    struct termios oldattr, newattr;
    int ch;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    return ch;
}

void putch(char c) {
    putchar(c);
    fflush(stdout);
}

void cprintf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    fflush(stdout);
}

// 화살표 키 대신 일반 키로 변경
#define whereX 3
#define whereY 3
#define LEFT 'a'    // 왼쪽 화살표 대신 'a' 사용
#define RIGHT 'd'   // 오른쪽 화살표 대신 'd' 사용
#define UP 'w'      // 위쪽 화살표 대신 'w' 사용
#define DOWN 'z'    // 아래쪽 화살표 대신 'z' 사용


void mon(void);
void make(void);
void filesave(int nowx,int nowy);
void filewrite1(void);
void filewrite2(void);
void prxy(int x,int y,char *msg);
void cls(void);

char picture[25][25]=
   {{' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '}};

int longx=10,longy=10;
int main()
{
    FILE *fp;
    int allxy[101],lastxy;
    char readline,ch,filename,sel;
    cls();
	textcolor(10);
	prxy(10,7,"You can make pictures with \"*,0,o\"");
	textcolor(9);
	prxy(25,3," P I C T U R E   M A K E E R ");
	textcolor(11);
	prxy(45,10,"How many count for x(1~20):");
	scanf("%d",&longx);
    prxy(45,11,"How many count for y(1~20):");
	scanf("%d",&longy);
    /*lastxy=longx*longy;*/
    textcolor(15);
    cls();
    mon();
    gotoxy(whereX-1,whereY);
    make();
    return 0;
}
void make() 
{
    int nowx=whereX-1,nowy=whereY,picx=0,picy=0;
    char go,yn;
    while(1/*(go=getch())!='!'*/){
    go=getch();
    switch(go){
        case LEFT  : if(nowx==whereX-1){prxy(45,20,"You can't go there(LEFT)        ");gotoxy(nowx,nowy);}
                     else{
                        nowx=nowx-2;
                        gotoxy(nowx,nowy);
                        picx--;
                        }
                     break;

        case RIGHT : if(nowx==(whereX-3+(longx*2))){prxy(45,20,"You can't go there(RIGHT)       ");gotoxy(nowx,nowy);}
                     else{
                        nowx=nowx+2;
                        gotoxy(nowx,nowy);
                        picx++;
                        }
                     break;

        case UP    : if(nowy==whereY){prxy(45,20,"You can't go there(UP)        ");gotoxy(nowx,nowy);}
                     else {
                         nowy=nowy-1;
                         gotoxy(nowx,nowy);
                         picy--;
                        }
                     break;

        case DOWN  : if(nowy==(whereY+longy-1)){prxy(45,20,"You can't go there(DOWN)      ");gotoxy(nowx,nowy);}
                     else {
                     nowy=nowy+1;
                     gotoxy(nowx,nowy);
                     picy++;
                    }
                     break;
                    // '*' 그리기
        case '1'   : picture[picy][picx]='*';
                     putch('*');
                     gotoxy(nowx,nowy);
                     break;
                    // '0' 그리기
        case '2'   : picture[picy][picx]='0';
                     putch('0');
                     gotoxy(nowx,nowy);
                     break;
                    // 'o' 그리기
        case '3'   : picture[picy][picx]='o';
                     putch('o');
                     gotoxy(nowx,nowy);
                     break;
                    // 공백 그리기
        case '4'   : picture[picy][picx]=' ';
                     putch(' ');
                     gotoxy(nowx,nowy);
                     break;

        case 'q'   : filesave(nowx,nowy); // 저장 후 종료
                     exit(0);
                     break;

        case 'x'   : exit(0);

		case 's'   : filesave(nowx,nowy); // 저장
                     break;


        default    : gotoxy(nowx,nowy);
                     /*putch(go);*/
                     break;
            }
        }

}

void filesave(int nowx,int nowy)
{
    int yn;
	prxy(45,22,"Save with \" \' \" or now (y/n/q): ");
	yn=getch();
	prxy(45,22,"                                        ");
	if(yn=='y'){
	filewrite1();gotoxy(nowx,nowy);}
    else if(yn=='n'){
	filewrite2();gotoxy(nowx,nowy);}
	else {gotoxy(nowx,nowy);}
}

void filewrite1()
{
     FILE *fp;
     char filename[10],buff[20],buff2[100];
     int tempx,tempy;
     prxy(45,22,"File Name:");
     scanf("%s",filename);
     sprintf(buff,"%s",filename);
     if((fp=fopen(buff,"w+t"))==NULL){prxy(45,20,"File open error");exit(0);}
     putc('{',fp);
     putc('\n',fp);
     for(tempy=0;tempy<longy;tempy++)
     {
        for(tempx=0;tempx<longx;tempx++)
        {
            fprintf(fp, "'%c',", picture[tempy][tempx]); // fprintf로 ,까지 저장
        }
        putc('\n',fp);
     }
     putc('}',fp);putc(';',fp);
     fclose(fp);
	 prxy(45,22,"                         ");
}

void filewrite2()
{
     FILE *fp;
     char filename[10],buff[20],buff2[100];
     int tempx,tempy;
     prxy(45,22,"File Name:");
     scanf("%s",filename);
     sprintf(buff,"%s",filename);
     if((fp=fopen(buff,"w+t"))==NULL){prxy(45,20,"File open error");exit(0);}
     putc('{',fp);
     putc('\n',fp);
     for(tempy=0;tempy<longy;tempy++)
     {
        for(tempx=0;tempx<longx;tempx++)
        {
            putc(picture[tempy][tempx], fp); // putc로 그림만 저장
        }
        putc('\n',fp);
     }
     putc('}',fp);putc(';',fp);
	 fclose(fp);
	 prxy(45,22,"                         ");
}

void mon() /* 메뉴 화면 출력 */
{
    int x,y,tempy,myx,myy;
    myx=(2*longx)+whereX;
    myy=longy+whereX;
    tempy=myy;
    for(x=whereX;x<myx;x=x+2)
    {
        /* 복구 */ 
    }
         textcolor(11);
 	prxy(48,5,"--- M E N U ---");
     textcolor(10);
 	prxy(48,6," 1 . put \"*\"");
 	prxy(48,7," 2 . put \"0\"");
 	prxy(48,8," 3 . put \"o\"");
 	prxy(48,9," 4 . put \" \"");
 	prxy(48,10," w . move up");
 	prxy(48,11," z . move down");
 	prxy(48,12," a . move left");
 	prxy(48,13," d . move right");
    prxy(48,14," r . reset screen");  // 그림 초기화
 	prxy(48,15," s . save");
 	prxy(48,16," q . save & exit");
 	prxy(48,17," x . exit");
     textcolor(15);
}
void cls(void)
{
    clrscr();
}
void prxy(int x,int y,char *msg)
{
    gotoxy(x,y);
	cprintf("%s",msg);
}
