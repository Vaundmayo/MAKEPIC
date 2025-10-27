#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <stdarg.h>
#include <ctype.h>

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
        printf("\033[37m"); // color = 15일때 흰색으로 변경
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

int valid_int(const char *str) {
    if (str[0] == '\0') return 0; // 빈 문자열은 오류
    
    for (int i = 0; str[i] != '\0'; i++) { // 문자열의 모든 문자가 숫자인지 확인
        if (!isdigit(str[i])) {
            return 0; // 숫자가 아닌 문자가 포함되어 있으면 False
        }
    }
    return 1; // 모두 숫자이면 True
}

// 화살표 키 대신 일반 키로 변경
#define whereX 3
#define whereY 3
#define LEFT 'a'    // 왼쪽 화살표 대신 'a' 사용
#define RIGHT 'd'   // 오른쪽 화살표 대신 'd' 사용
#define UP 'w'      // 위쪽 화살표 대신 'w' 사용
#define DOWN 'z'    // 아래쪽 화살표 대신 'z' 사용
#define MAX_HISTORY 1000 //replay 기능

void mon(void);
void make(void);
void filesave(int nowx,int nowy);
void filewrite1(void);
void filewrite2(void);
void prxy(int x,int y,char *msg);
void cls(void);
int fileload(char* filename, int format);
int valid_int(const char *str);

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

typedef struct {
	int x, y;
	char ch;
	useconds_t delay; //시간 간격(ms)
} Drawing;


/* 
    [추가 기능] replay(리플레이)

    구현 배경
    - 만드는 과정을 시각적으로 시연함
    
    설계
    - drawing 구조체에 (x, y, ch, delay) 정보를 저장하여 사용자가 1/2/3/4 키를 누를때마다
      history[]에 기록
    - 'p' 키 입력 시, history[]에 저장된 정보를 순서대로 출력(putch)
    - 각 출력 사이에 일정 시간 간격 유지
    - 재생 완료 후 격자 복원 및 커서 원위치

    사용법
    - 1/2/3/4 키로 그림을 그린 후, 'p' 키를 누르면 입력 순서대로 자동으로 재생
*/
Drawing history[MAX_HISTORY];
int drawing_count = 0;


int longx=10,longy=10;
int main()
{
    FILE *fp;
    int allxy[101],lastxy;
    char readline,ch,filename,sel;
    char last_filename[100];
    int last_format = 0;
    int loaded = 0;
    char buffer[100];
    cls();

    fp = fopen("saveinfo", "r");
    if (fp != NULL) {
        // saveinfo로부터 이전에 저장한 그림의 파일명, 크기(longx, longy), 저장 형식(filewrite 1,2)을 읽어옴
        if (fscanf(fp, "%s\n%d %d\n%d", last_filename, &longx, &longy, &last_format) == 4) {
            fclose(fp);

            // 파일 크기 유효성 검사
            if (longx < 1 || longx > 20 || longy < 1 || longy > 20) {
                // 크기 범위가 잘못되었으면 로드 시도 X
                longx = 10; longy = 10;
            } else {
                prxy(10, 5, "Last saved file found: "); // 파일 찾았을때 메시지
                cprintf("%s (%dx%d)", last_filename, longx, longy); // 파일에서 파일명,크기 읽어 옴
                prxy(10, 6, "Load this file? (y/n): "); // 불러올지 선택
                
                sel = getch(); // 선택지 받아오기
                
                if (sel == 'y' || sel == 'Y') { // y 입력시 fileload 함수 실행
                    if (fileload(last_filename, last_format)) {
                        prxy(10, 8, "File loaded successfully!"); // 성공 메시지
                        sleep(1);
                        loaded = 1; // 로드 성공 했으므로 초기 입력 메뉴 생략
                    } else {
                        prxy(10, 8, "Failed to load file. Starting new drawing."); // 실패 메시지
                        longx = 10; longy = 10; // 크기 기본값으로 리셋
                        sleep(1);
                    }
                }
            }
        } else {
            fclose(fp); // Info 파일이 비어있거나 형식이 잘못됨
        }
    }

	if (!loaded) { // 파일을 불러오지 못했을 때, n으로 대답했을 때
        prxy(10,5,"                                     ");
        prxy(10,6,"                         ");
        textcolor(10);
        prxy(10,7,"You can make pictures with \"*,0,o\"");
        textcolor(9);
        prxy(25,3," P I C T U R E   M A K E E R ");
        textcolor(11);
        while(1){
            prxy(45,10,"How many count for x(1~20):");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = '\0'; // 개행 문자 제거
            if(!valid_int(buffer)) { // 유효한 정수가 아니면
                prxy(45,12,"Invalid number. Please try again."); // 오류 메시지 출력
                prxy(45,10,"How many count for x(1~20):             "); // 입력란 초기화
                continue; // 다시 입력 받기
            }
            longx = atoi(buffer); // 정수로 변환하여 저장
            prxy(45,11,"How many count for y(1~20):");
            buffer[0] = '\0'; // 버퍼 초기화
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = '\0';
            if(!valid_int(buffer)) {
                prxy(45,12,"Invalid number. Please try again.");
                prxy(45,10,"How many count for x(1~20):             ");
                prxy(45,11,"How many count for y(1~20):             ");
                continue;
            }
            longy = atoi(buffer);
            if(longx > 0 && longx < 21 && longy > 0 && longy < 21){ // longx, longy 값이 존재하고 범위 안이면 탈출
                break;
            }
            else{
                prxy(45,12,"Invalid number. Please try again.");
                prxy(45,10,"How many count for x(1~20):             ");
                prxy(45,11,"How many count for y(1~20):             ");
            }
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
                         nowy=nowy-1; // y는 1칸씩
                         gotoxy(nowx,nowy);
                         picy--; // 배열의 행 인덱스 감소
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
        case '1'   : picture[picy][picx]='*'; // 배열에 * 입력
                     putch('*'); // 화면에 * 출력
                     gotoxy(nowx,nowy); // 커서 원위치
		     if(drawing_count<MAX_HISTORY)
			     history[drawing_count++] = (Drawing){nowx, nowy, '*', 100000}; //replay
                     break;
                    // '0' 그리기
        case '2'   : picture[picy][picx]='0';
                     putch('0');
                     gotoxy(nowx,nowy);
		     if(drawing_count<MAX_HISTORY)
                             history[drawing_count++] = (Drawing){nowx, nowy, '0', 100000}; //replay
                     break;
                    // 'o' 그리기
        case '3'   : picture[picy][picx]='o';
                     putch('o');
                     gotoxy(nowx,nowy);
		     if(drawing_count<MAX_HISTORY)
                             history[drawing_count++] = (Drawing){nowx, nowy, 'o', 100000}; //replay
                     break;
                    // 공백 그리기
        case '4'   : picture[picy][picx]=' ';
                     putch(' ');
                     gotoxy(nowx,nowy);
		     if(drawing_count<MAX_HISTORY)
                             history[drawing_count++] = (Drawing){nowx, nowy, ' ', 100000}; //replay
                     break;

        case 'p' :  
                     if(drawing_count != 0){ // 입력이 있었을 때
                        cls(); // 화면 지우기
                        prxy(45, 20, "replay start");
                        usleep(500000); //0.5초 대기
                        for(int i = 0;i<drawing_count;i++){
                             gotoxy(history[i].x, history[i].y); //좌표 이동
                             putch(history[i].ch); //기록된 문자 출력
                             usleep(history[i].delay); //딜레이
                        }
                        prxy(45, 20, "                    ");
                        prxy(45, 20, "replay complete! press any key");
                        getch(); //키 입력
                        cls();
                        mon();
                        gotoxy(nowx, nowy); //화면 복원
                        break;
                     }
                     else{
                         prxy(45, 20, "No drawing history! press any key");
                         getch();
                         cls();
                         mon();
                         gotoxy(nowx, nowy);
                         break;
                     }



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
		     drawing_count = 0; //replay 기록 초기화
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


/* 
    [추가 기능] 저장 및 불러오기 

    구현배경
    - 프로그램 종료 후에도 동일 상태로 작업을 이어서 할 수 있게 함

    설계
    - 작업 내용을 파일에 저장 후, 파일 정보는 saveinfo에 기록
    - 프로그램을 다시 실행할 때 saveinfo를 읽어 마지막으로 저장한 파일 불러오기(y/n)
    - 사용자가 불러오기를 선택하면(y) fileload()를 통해 저장된 picture 내용 복구

    사용법
    - s: 저장 / q: 저장 후 종료 / 프로그램 실행 시 저장된 파일 불러오기(y/n)
*/
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
            if(tempx != longx - 1){ // , 찍기
                putc(',', fp);
            }
        }
        putc('\n',fp);
     }
     putc('}',fp);putc(';',fp);
     fclose(fp);
     FILE *info_fp = fopen("saveinfo", "w"); // 불러오기 위한 정보 저장
     if (info_fp != NULL) {
         fprintf(info_fp, "%s\n", filename); // 파일명
         fprintf(info_fp, "%d %d\n", longx, longy); // 크기
         fprintf(info_fp, "1"); // 저장 형식 (1)
         fclose(info_fp);
     }
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
            if(tempx != longx - 1){
                putc(',', fp);
            }
        }
        putc('\n',fp);
     }
     putc('}',fp);putc(';',fp);
	 fclose(fp);
     FILE *info_fp = fopen("saveinfo", "w"); // 불러오기 위한 정보 저장
     if (info_fp != NULL) {
         fprintf(info_fp, "%s\n", filename); // 파일명
         fprintf(info_fp, "%d %d\n", longx, longy); // 크기
         fprintf(info_fp, "2"); // 저장 형식 (2)
         fclose(info_fp);
     }
	 prxy(45,22,"                         ");
}

int fileload(char* filename, int format) {
    FILE *fp = fopen(filename, "r"); // saveinfo에서 읽어온 파일명
    if (fp == NULL) { // 파일 열기 실패
        prxy(10, 9, "Error: Cannot open file.");
        sleep(1);
        return 0;
    }

    int c;
    int tempx, tempy;

    // '{' 찾기
    while((c = fgetc(fp)) != EOF && (c != '{')); // 처음 받은 c가 EOF가 아니고 '{'를 만날때까지 계속 읽음
    if (c == EOF) { fclose(fp); return 0; } // '{'를 못만남, 함수 종료

    // '{' 다음 줄바꿈 건너뛰기
    while((c = fgetc(fp)) != EOF && (c == '\n' || c == ' '));
    ungetc(c, fp); // 첫 번째 문자를 다시 스트림에 넣음

    // longx, longy 크기에 맞춰 데이터 불러오기
    for(tempy=0; tempy<longy; tempy++) {
        for(tempx=0; tempx<longx; tempx++) {
            
            if (format == 1) { // filewrite1 형식 ,와 ''
                // 여는 따옴표(') 찾기
                while((c = fgetc(fp)) != EOF && (c != '\'')); // ' 만날때까지
                if (c == EOF) { fclose(fp); return 0; }
                
                // 그린 문자 읽기
                c = fgetc(fp);
                if (c == EOF) { fclose(fp); return 0; }
                picture[tempy][tempx] = (char)c;
                
                // 닫는 따옴표(') 찾기
                while((c = fgetc(fp)) != EOF && (c != '\'')); // ' 만날떄까지
                if (c == EOF) { fclose(fp); return 0; }

            } else { // filewrite2 형식 ,만 존재
                c = fgetc(fp);
                if (c == EOF) { fclose(fp); return 0; }
                picture[tempy][tempx] = (char)c;
            }

            // , 건너뛰기
            if(tempx != longx - 1) {
                while((c = fgetc(fp)) != EOF && (c != ','));
                if (c == EOF) { fclose(fp); return 0; }
            }
        }
        // \n 건너뛰기
        while((c = fgetc(fp)) != EOF && (c != '\n'));
        if (c == EOF && tempy < longy - 1) { fclose(fp); return 0; } // 파일이 중간에 끝남
    }
    
    fclose(fp);
    return 1; // 불러오기 성공
}

void mon() /* 메뉴 화면 출력 */
{
    int x,y,tempy,myx,myy;
    myx=(2*longx)+whereX;
    myy=longy+whereX;
    tempy=myy;

    for(x=whereX;x<myx-2;x=x+2)
    {
        for(y = 0; y < longy; y++)
        {
            /*
              커서 위치 지정
              - 행 : whereY(3)부터 아래로 한칸씩 내려감
              - 열 : x는 2칸 간격 (3, 5, 7...)
            */
            gotoxy(x, whereY + y);
            putch(',');
        }
    }
    
    for(int tempy = 0; tempy < longy; tempy++) // y좌표
    {
        for(int tempx = 0; tempx < longx; tempx++) // x좌표
        {
            char m = picture[tempy][tempx]; // picture 2차원 배열

            // 공백이 아닌 그림 문자일 경우에만 출력
            if(m != ' ')
            {
                // 실제 커서가 위치할 화면 X좌표 계산
                // (tempx=0 -> 2, tempx=1 -> 4...)
                int screen_x = (tempx * 2) + (whereX - 1); 
                // 화면 Y좌표 계산
                int screen_y = whereY + tempy;

                gotoxy(screen_x, screen_y);
                putch(m);
            }
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
	prxy(48,14," p . replay");
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
