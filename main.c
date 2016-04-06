#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include<signal.h>
#include<sys/time.h>
//snake的结构体
typedef struct Snake{
	int x;
	int y;
	struct Snake *per;
	struct Snake *next;
} Snode;
//食物结构体
typedef struct Sbean{
	int x;
	int y;
}Bean;
Snode *head,*tail;
Bean *food;
void movesnake();
void init();
void gameover();
int  eatfood();
void moveup();
void movedown();
void moveleft();
void moveright();
void newhead();
void Key_Ctrl();
void randfood();
int set_ticker(int n_msecs);

int peraction=4; //上次的动作，下次不能跟上次相反，1上2下3左4右
int nowaction=4;//下次动作，1上2下3左4右
int tadd=100;//速度标示
int GAME_HANG;//游戏区域Y坐标起始值
int LIE;//x坐标最大
int TOP_HANG=8;//标题区域Y左边结束值
int score=0;//分数

//主函数
int main(){
	init();
	Key_Ctrl();
	endwin();
	return 1;
}

//初始化函数
void init(){
	initscr();//curses初始化
	GAME_HANG=TOP_HANG+1;
	LIE=COLS;
	curs_set(0);//隐藏光标
	noecho();//输入不回显
	nonl();
	box(stdscr,'|','-');//边框
	//最上面显示信息
	int i=0;
	for( i=0;i<LIE;i++){//标题区域下边框
		move(8,i);
		addch('-');
	}
	//标题区域内文字
	move(2,2);
	printw("author:dandinglong");
	move(2,40);
	printw("now speed:");
	move(2,52);
	printw("%d",200-tadd);
	move(4,2);
	printw("speed faster:f");
	move(4,40);
	printw("speed slower:s");
	move(6,2);
	printw("quit:q");
	move(6,40);
	printw("now score:");
	move(6,52);
	printw("%d",score);
	//头尾食物分配内存
	head=malloc(sizeof(Snode));
	head->x=2;
	head->y=GAME_HANG;
	head->per=NULL;
	tail=malloc(sizeof(Snode));
	tail->x=1;
	tail->y=GAME_HANG;
	tail->next=NULL;
	tail->per=head;
	food=malloc(sizeof(Snode));
	//随机第一个食物
	randfood();
	//开启时钟中断
    signal(SIGALRM, movesnake);
    //设置中间间隔
    set_ticker(tadd);
}
//随机显示食物
void randfood(){
	food->x=rand() % (COLS - 2) + 1;
	food->y=rand() % (LINES - 2-TOP_HANG) + 1+TOP_HANG;
	move(food->y,food->x);
	addch('#');
}
//键盘监听
void Key_Ctrl()
{
    int c;
    keypad(stdscr, true);       /* use little keyboard Navigation Key */
    while(c = getch(), c != 'q')
    {
        if(c == 's')
        {
        	if(tadd>200){
        		tadd=200;
        	}else{
        		tadd+=10;
        	}
        	move(2,52);
        	printw("   ");
        	move(2,52);
        	printw("%d",200-tadd);
        	set_ticker(tadd);
        }
        else if(c == 'f')
        {
        	if(tadd<1){
        		tadd=0;
        	}else{
        		tadd-=10;
        	}
        	move(2,52);
        	printw("   ");
        	move(2,52);
        	printw("%d",200-tadd);
        	set_ticker(tadd);
        }
        if(c == KEY_LEFT&&peraction!=4)
        {
        	nowaction=3;
        }
        else if(c == KEY_RIGHT&&peraction!=3)
        {
        	nowaction=4;
        }
        else if(c == KEY_UP&&peraction!=2)
        {
        	nowaction=1;
        }
        else if(c == KEY_DOWN&&peraction!=1)
        {
        	nowaction=2;
        }
    }
}
int set_ticker(int n_msecs)
{
    struct itimerval new_timeset;
    long n_sec, n_usecs;

    n_sec = n_msecs / 1000;                 /* int second part */
    n_usecs = (n_msecs % 1000) * 1000L;     /* microsecond part */

    new_timeset.it_interval.tv_sec = n_sec; /* set reload */
    new_timeset.it_interval.tv_usec = n_usecs;

    new_timeset.it_value.tv_sec = n_sec;    /* set new ticker value */
    new_timeset.it_value.tv_usec = n_usecs;

    return setitimer(ITIMER_REAL, &new_timeset, NULL);
}
//移动时候，新开一个头
void newhead(int x,int y){
	Snode *tmp=head;
	head=malloc(sizeof(Snode));
	head->x=x;
	head->y=y;
	head->next=tmp;
	head->per=NULL;
	tmp->per=head;
	gameover();
	move(head->y,head->x);
	addch('@');
}
//移动时候，如果吃到食物则尾巴节点不去除，没迟到则去除尾巴节点
void cuttail(){
	if(!eatfood()){
		move(tail->y,tail->x);
		addch(' ');
		Snode *tmp=tail->per;
		free(tail);
		tail=tmp;
		tail->next=NULL;
	}
}
//判断游戏结束，头节点下一个位置是边框，或者是以存在节点，则游戏结束
void gameover(){
	if(head->x>COLS-1||head->x<1||head->y>LINES-2||head->y<GAME_HANG){
		clear();
		move(5,10);
		printw("game over");
		refresh();
		set_ticker(0);
		sleep(4);
		endwin();
		exit(1);
	}
	Snode *tmp=head->next;
	while(tmp->next){
		if(tmp->x==head->x&&tmp->y==head->y){
			clear();
			move(5,10);
			printw("game over self");
			refresh();
			set_ticker(0);
			sleep(4);
			endwin();
			exit(1);
		}
		tmp=tmp->next;
	}
}
//上下左右移动
void moveup(){
	newhead(head->x,head->y-1);
	cuttail();
}
void movedown(){
	newhead(head->x,head->y+1);
	cuttail();
}
void moveleft(){
	newhead(head->x-1,head->y);
	cuttail();

}
void moveright(){
	newhead(head->x+1,head->y);
	cuttail();
}
//判断是否迟到食物
int  eatfood(){
	int x=head->x,y=head->y;
	if(x>COLS){
			x=0;
			y++;
		}
	if(x==food->x&&y==food->y){
		score++;
		move(6,52);
		printw("     ");
		move(6,52);
		printw("%d",score);
		randfood();
		return 1;
	}
	return 0;
}
//移动蛇
void movesnake(){
    signal(SIGALRM, SIG_IGN);//忽略时钟信号
	switch(nowaction){
		case 1:
			moveup();
			break;
		case 2:
			movedown();
			break;
		case 3:
			moveleft();
			break;
		case 4:
			moveright();
			break;
		default:
			break;
	}
	peraction=nowaction;
	refresh();
    signal(SIGALRM, movesnake);//开启时钟信号
}
