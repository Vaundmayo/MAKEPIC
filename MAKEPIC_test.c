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
    // Linux에서는 컬러를 ANSI escape code로 처리할 수 있음. 여기서는 무시.
    switch(color) {
        case 9:
        printf("\033[31m"); // color = 9일때 빨강색으로 변경
        break;

        case 10:
        printf("\033[32m"); // color = 10일때 초록색으로 변경
        break;

        case 11:
        printf("\033[33m"); // color = 11일때 노랑색으로 변경
        break;

        case 15:
        printf("\033[34m"); // color = 15일때 파랑색으로 변경
        break;

        default:
        printf("\033[37m"); // 기본색은 흰색
    }
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
    while(1){
        prxy(45,10,"How many count for x(1~20):");
	    scanf("%d",&longx);
        prxy(45,12,"                                 "); // 오류 메시지 지우기
        prxy(45,11,"How many count for y(1~20):");
	    scanf("%d",&longy);
        if(longx >= 1 && longx <= 20 && longy >= 1 && longy <= 20){ // 범위 초과시 오류 메시지
            break;
        }
        else {
            prxy(45,12,"Invalid number. Please try again.");
            prxy(45,10,"How many count for x(1~20):     "); // 입력란 초기화
            prxy(45,11,"How many count for y(1~20):     ");
        }
    }
	
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
        case 'r'   : {int tempx, tempy; // 'r' (그림 초기화)
                
                     // picture 배열의 모두 공백으로 변경
                     for(tempy=0; tempy<longy; tempy++) {
                     for(tempx=0; tempx<longx; tempx++) {
                             picture[tempy][tempx] = ' ';
                         }
                     }
                
                     cls(); // 화면 지우기
                     mon(); // 메뉴, 격자 다시 그리기
                    
                     // 배열, 커서 위치 초기화
                     nowx=whereX-1;
                     nowy=whereY;
                     picx=0;
                     picy=0;
                     gotoxy(nowx,nowy); // 커서 시작 위치로 이동
                     prxy(45, 20, "Picture reset complete."); // 초기화 완료 메시지
                     }
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
            fprintf(fp, "'%c'", picture[tempy][tempx]); // fprintf로 '저장
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
        /*
          화면 x좌표를 picture의 열 인덱스로 변경
          - 그림판 시작점(whereX)으로부터의 거리 계산 : x - whereX
          - 한 셀 폭이 2칸이므로 /2 -> c = 0,1,2,3,...로 열 인덱스로 변경
        */
        int c = (x - whereX) / 2;

        /*
          열의 모든 행을 위에서 아래로 출력
          - picture[y][c]이 공백이면 화면에 ','로 표시
        */ 
        for(y = 0; y < longy; y++)
        {
            char m = picture[y][c]; //picture 2차원 배열

            // 빈칸을 화면상 ','로 표시
            if(m == ' ')
            {
                m = ',';
            }

            /*
              커서 위치 지정 
              - 행 : whereY(3)부터 아래로 한칸씩 내려감
              - 열 : x는 2칸 간격 -> 초기 정렬 맞춤을 위해 x-1로 열 간격 유지
            */
            gotoxy(x-1, whereY + y);
            // 해당 위치에 문자 출력(빈칸이면 ',' 출력)
            putch(m);
        }
    }

    // 메뉴화면
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
 	prxy(48,14," s . save");
 	prxy(48,15," q . save & exit");
 	prxy(48,16," x . exit");
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
