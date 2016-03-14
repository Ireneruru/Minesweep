#undef UNICODE
#undef _UNICODE
#include<iostream>
#include<Windows.h>
#include<stdlib.h>
#include<ctime>
#include<fstream>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <math.h>
#include "resource.h"
#include <time.h>
#include <graphics.h>//EasyX图形库
#pragma comment(lib,"Winmm.lib")      //给游戏添加音乐

using namespace std;

#define side			25					//方块宽度
#define gap			    3					  //方块间隔宽度
#define C_BLOCKPANEL	RGB(255, 255, 221)	//底板颜色
#define MAXSTAR			3500	                       // 星星总数

int GamePhase = 0;				  //游戏阶段 0开始页面    1帮助     2 结束   3 帮助界面
int row;
int mouseX;						  //鼠标位置坐标X
int mouseY;						  //鼠标位置坐标Y
int field[101][101] = {0};                //记录周围的雷的个数
char table[101][101] = {'\0'};               //桌面显示
int total_mine;                     //总雷数
bool  spread[101][101] = { 0 };     //记录是否是雷
bool isMouseDown;				    //鼠标按下
bool win = 1;                           //0是失败
int flag[101][101] = { 0 };
int sp, ep;
bool youjian[101][101] = { 0 };         //右键，0代表未按，1代表已按
time_t t_start, t_end;

//函数声明
void startdraw();          //开始绘图  
void  helpdraw();          //帮助绘图
void youlose();            //失败界面
void youwin();             //成功界面 
void time();               //时间函数

//游戏进程中
void game();										//游戏控制中心
void pailei(int x, int y);							//排雷
void  out();										//界面输出
void expand(int x, int y);							//拓展函数
void firstgame(int startx, int starty);				//第一次游戏
void nextgame(int x, int y);						//下次游戏
void key();											//答案
void gamedraw();                                    //游戏初始画图
void hongqi(int ,int);
int mousex(int x);									//获得鼠标
int mousey(int y);
int save();                                        //存盘函数
void memory();                                      //读盘
int kaiju = 0;

//为开场动画而做
void InitStar(int); 
void MoveStar(int i);
void startflash();


struct STAR
{
		double	x;
		int		y;
		double	step;
		int		color;
};

STAR star[MAXSTAR];               //星星

//排雷
void pailei(int x, int y)
{
	int r, c, now_mine = 0;
	srand((unsigned)time(NULL));               //初始化随机数
	while (now_mine<total_mine){
		r = rand() % row;
		if (r == 0)r += row;
		c = rand() % row;
		if (c == 0)c += row;
		if (spread[r][c] == 0 && r != x&&c != y)
		{
			spread[r][c] = 1; now_mine++;
		}
	}                                            //布雷
	for (int i = 1; i <= row; i++)
	{
		for (int j = 1; j <= row; j++)
		{
			table[i][j] = '*';
		}
	}
	cout << endl;
	for (int i = 1; i <= row; i++){
		for (int j = 1; j <= row; j++){
			if (spread[i][j] == 0)
			{
				int num = 0;
				for (int m = i - 1; m <= i + 1; m++){
					for (int n = j - 1; n <= j + 1; n++){
						if (m >= 1 && m <= row&&n >= 1 && n <= row&&spread[m][n] == 1)
							num++;
					}
				}
				field[i][j] = num;
			}
			else field[i][j] = 9;                    //赋值field
		}

	}

}

//控制输出
void  out()
{
	int left = 0;
	for (int i = 1; i <= row; i++)
	{
		for (int j = 1; j <= row; j++)
		{
			int x, y;
			x = 300 - (row*side + (row - 1)*gap) / 2 + (side + gap)*(i - 1);          //i,j点对应的坐标
			y = 300 - (row*side + (row - 1)*gap) / 2 + (side + gap)*(j - 1);
			if (table[i][j] == '*')
			{
				left++;
				IMAGE img;
				loadimage(&img, "fore.jpg");
				putimage(x, y, &img);
			}
			else if (table[i][j] == '#')
			{
				IMAGE img;
				loadimage(&img, "flag.jpg");
				putimage(x, y, &img);
				left++;
			}
			else if (table[i][j] == '?')
			{
				IMAGE img;
				loadimage(&img, "exp.jpg");
				putimage(x, y, &img);
			}
			else if (table[i][j] == '1')
			{
				IMAGE img;
				loadimage(&img, "1.jpg");
				putimage(x, y, &img);
			}
			else if (table[i][j] == '2')
			{
				IMAGE img;
				loadimage(&img, "2.jpg");
				putimage(x, y, &img);
			}
			else if (table[i][j] == '3')
			{
				IMAGE img;
				loadimage(&img, "3.jpg");
				putimage(x, y, &img);
			}
			else if (table[i][j] == '4')
			{
				IMAGE img;
				loadimage(&img, "4.jpg");
				putimage(x, y, &img);
			}
			else if (table[i][j] == '5')
			{
				IMAGE img;
				loadimage(&img, "5.jpg");
				putimage(x, y, &img);
			}
			else if (table[i][j] == '6')
			{
				IMAGE img;
				loadimage(&img, "6.jpg");
				putimage(x, y, &img);
			}
			else if (table[i][j] == '7')
			{
				IMAGE img;
				loadimage(&img, "7.jpg");
				putimage(x, y, &img);
			}
			else if (table[i][j] == '8')
			{
				IMAGE img;
				loadimage(&img, "8.jpg");
				putimage(x, y, &img);
			}

		}
	}
	if (left == total_mine)
	{
		Sleep(240);
		youwin();
	}
}

//拓展
void expand(int x, int y)
{
	if (field[x][y] == 9)
	{
		win = 0;
		return;
	}
	if (flag[x][y] == 0)
	{
	if (field[x][y] >= 1 && field[x][y] <= 8)
	{
		table[x][y] = (char)(field[x][y] + 48); flag[x][y] = 1;
		return;
	}
	else
	{
		table[x][y] = '?'; flag[x][y] = 1;
		for (int i = x - 1; i <= x + 1; i++){
			for (int j = y - 1; j <= y + 1; j++)
			{
				if (i >= 1 && i <= row&&j >= 1 && j <= row && flag[i][j] == 0)
				{
					expand(i, j);
				}
			}
		}
	}
	}
}

//游戏初始
void firstgame(int startx, int starty)
{
	pailei(startx, starty);
	expand(startx, starty);
	if (win)
	{
		out();
	}

}

//游戏进行
void nextgame(int x, int y)
{
	expand(x, y);
	if (win)
		out();
	else
		key();
}

//输出雷
void key()
{
	for (int i = 1; i <= row; i++)
	{
		for (int j = 1; j <= row; j++)
		{
			int x, y;
			x = 300 - (row*side + (row - 1)*gap) / 2 + (side + gap)*(i - 1);          //i,j点对应的坐标
			y = 300 - (row*side + (row - 1)*gap) / 2 + (side + gap)*(j - 1);
		   if (spread[i][j] == 1)
			{
				IMAGE img;
				loadimage(&img, "boom.jpg");
				putimage(x, y, &img);
			}
		}
	}
	Sleep(5000);
	if (win==0)
	youlose();

}

void gamedraw()
{
	initgraph(800, 600);
	setbkcolor(WHITE);
	cleardevice();
	int len;
	//底板
	setlinecolor(RGB(255, 25, 22));
	setfillcolor(C_BLOCKPANEL);
	len = (row*side + (row - 1)*gap) / 2;
	fillrectangle(300 - len - 2, 300 - len - 2, 300 + len + 2, 300 + len + 2);
	sp = 300 - len;
	ep = 300 - len;
	//画方块
	for (int i = 1; i <= row; i++)
	{
		for (int j = 1; j <= row; j++)
		{
			int x, y;
			x = 300 - len + (side + gap)*(i - 1);
			y = 300 - len + (side + gap)*(j - 1);
			IMAGE img;
			loadimage(&img, "fore.jpg");
			putimage(x, y, &img);
		}
	}

	//关于存盘save，退出exit,重新开始restart,time
	
	setlinecolor(RGB(255, 153, 0));
	setfillcolor(WHITE);
	fillrectangle(640, 50, 760, 250);
	settextstyle(45, 15, _T("Verdana"));
	settextcolor(RGB(255, 17, 102));
	outtextxy(665, 70, _T("Time"));


	settextstyle(45, 15, _T("Verdana"));
	settextcolor(RGB(255, 17, 102));
	outtextxy(652, 170, _T("Mines"));
	if (total_mine == 20)
	{
		settextstyle(28, 12, _T("Verdana"));
		settextcolor(RGB(255, 17, 102));
		outtextxy(685, 215, _T("20"));
	}
	if (total_mine == 50)
	{
		settextstyle(28, 12, _T("Verdana"));
		settextcolor(RGB(255, 17, 102));
		outtextxy(685, 215, _T("50"));
	}
	if (total_mine == 70)
	{
		settextstyle(28, 12, _T("Verdana"));
		settextcolor(RGB(255, 17, 102));
		outtextxy(685, 215, _T("70"));
	}
	setlinecolor(RGB(255, 153, 0));
	setfillcolor(WHITE);
	fillrectangle(640, 280, 760, 350);
	settextstyle(45, 15, _T("Verdana"));
	settextcolor(RGB(255, 17, 102));
	outtextxy(665, 300, _T("Exit"));

	setlinecolor(RGB(255, 153, 0));
	setfillcolor(WHITE);
	fillrectangle(640, 380, 760, 450);
	settextstyle(45, 15, _T("Verdana"));
	settextcolor(RGB(255, 17, 102));
	outtextxy(645, 400, _T("Restart"));

	setlinecolor(RGB(255, 153, 0));
	setfillcolor(WHITE);
	fillrectangle(640, 480, 760, 550);
	settextstyle(45, 15, _T("Verdana"));
	settextcolor(RGB(255, 17, 102));
	outtextxy(665, 500, _T("Save"));

}

int mousex(int x)
{
	int hang;
	for (int i = 1; i <= row; i++)
	{
		if ((x >= (i - 1) * 28 + sp) && (x <= (28 * (i - 1) + 28 + sp)))
		{
			hang = i;
		}
	}
	return hang;
}

int mousey(int y)
{
	int lie;
	for (int i = 1; i <= row; i++)
	{
		if ((y >= (i - 1) * 28 + sp) && (y <= (28 * (i - 1) + 28 + sp)))
		{
			lie = i;
		}
	}
	return lie;
}

void hongqi(int i ,int j)
{

	int x, y;
	x = 300 - (row*side + (row - 1)*gap) / 2 + (side + gap)*(i - 1);          //i,j点对应的坐标
	y = 300 - (row*side + (row - 1)*gap) / 2 + (side + gap)*(j - 1);

   if (!youjian[i][j]&&flag[i][j]==0)                        //如果是0，则要标记红旗
   {
			IMAGE img;
			loadimage(&img, "flag.jpg");
			putimage(x, y, &img);
			youjian[i][j] = 1;
			flag[i][j] = 1;
			table[i][j] = '#';
	}
	else if (youjian[i][j])
	  {
		IMAGE img;
		loadimage(&img, "fore.jpg");
		putimage(x, y, &img);
		youjian[i][j] = 0;
		flag[i][j] = 0;
		table[i][j] = '*';
	 }
}

void time()
{
	t_end = time(NULL);
	int t = int(difftime(t_end, t_start)) % 10;
	int u = (int(difftime(t_end, t_start) - t)/10) %10;
	int v = (int(difftime(t_end, t_start) - t - u * 10) / 100) & 10;
	char s, e, f;
	s = t + '0';
	e = u + '0';
	f = v + '0';
	settextstyle(45, 15, _T("Verdana"));
	settextcolor(RGB(255, 17, 102));
	outtextxy(652, 112, f );
	outtextxy(670, 112, e );
	outtextxy(688, 112, s );
	outtextxy(706, 112,'s');
}

int save()
{
	ofstream fout("date");
	if (!fout)
	{
		exit(1);
	}
	fout << kaiju << ' ';
	fout << row << ' ';
	fout << total_mine << ' ';
	fout << win << ' ';
	for (int i = 0; i <= 100; i++)
	{
		for (int j = 0; j <= 100; j++)
		{
			fout << field[i][j] << ' ' << table[i][j] << ' ' << spread[i][j] << ' ' << flag[i][j] << ' ' << youjian[i][j] << ' ';
		}
	}
	fout.close();
	Sleep(2000);
	startdraw();
	return 0;
}

void memory()
{
	ifstream fin("date");

	if (!fin)
	{
		startdraw();
	}
	fin >> kaiju;
	fin >> row;
	fin >> total_mine;
	fin >> win;
	for (int i = 0; i <= 100; i++)
    {
		for (int j = 0; j <= 100; j++)
        {
			fin >> field[i][j];
			fin >> table[i][j];
			fin >> spread[i][j];
			fin >> flag[i][j];
			fin >> youjian[i][j];
		}
    }
	fin.close();
}

void game()
{
	int sp = 300 - (row*side + (row - 1)*gap) / 2;
	int ep = 300 + (row*side + (row - 1)*gap) / 2;
    t_start = time(NULL);

	while (true){

		time();
		
		if (!MouseHit())
			continue;
         
		int xzuo = 0, yzuo = 0;

		MOUSEMSG p;   //鼠标变量的获取
		 p = GetMouseMsg();

		switch (p.uMsg)
		{
		case WM_MOUSEMOVE:	     break;
		case WM_LBUTTONDOWN:    xzuo = p.x; yzuo = p.y;    //如按左键，则记录下那个点的坐标
			if (xzuo >= sp&&xzuo <= ep&&yzuo >= sp&&yzuo <= ep)
			{
				if (kaiju == 0){
					firstgame(mousex(xzuo), mousey(yzuo));
					kaiju = 1;
				}
				else{
					if (win == 1){
						nextgame(mousex(xzuo), mousey(yzuo));
					}
					else break;
				}
			}
			else if (xzuo >= 640 && xzuo <= 760 && yzuo >= 380 && yzuo <= 450)
				startdraw();
			else if (xzuo >= 640 && xzuo <= 760 && yzuo >= 280 && yzuo <= 350)
				closegraph();
			else if (xzuo >= 640 && xzuo <= 760 && yzuo >= 480 && yzuo <= 550)
				save();
			else break;
	   case WM_RBUTTONDOWN: xzuo = p.x; yzuo = p.y;    //如按右键，则记录下那个点的坐标
			if (xzuo >= sp&&xzuo <= ep&&yzuo >= sp&&yzuo <= ep)
			{
				hongqi(mousex(xzuo),mousey(yzuo));
			}
			else break;
		default:break;
		}
	}

}

//失败界面
void youlose()
{
	setbkmode(TRANSPARENT);//设置透明背景
	setbkcolor(WHITE);
	cleardevice();

	IMAGE img1;
	loadimage(&img1, "youlose1.jpg");
	putimage(10, 140, &img1);
	IMAGE img2;
	loadimage(&img2, "youlose.jpg");
	putimage(500, 70, &img2);



	//返回
	setlinecolor(RGB(255, 17, 102));
	setfillcolor(RGB(255, 255, 204));
	fillrectangle(330, 400, 460, 450);
	settextstyle(25, 9, _T("Verdana"));
	settextcolor(RGB(255, 51, 68));
	outtextxy(342, 412, _T("BACK MENU"));

	MOUSEMSG p;
	while (true){
		p = GetMouseMsg();
		switch (p.uMsg)
		{
		case WM_MOUSEMOVE:	    break;
		case WM_LBUTTONDOWN:
			if (p.x >= 330 && p.x <= 460 && p.y >= 400 && p.y <= 450)
				startdraw();
			else break;
		}
	}
	
}

//成功界面
void youwin()
{
	setbkmode(TRANSPARENT);//设置透明背景
	setbkcolor(WHITE);
	cleardevice();
	settextstyle(115, 80, _T("Impact"));
	settextcolor(RGB(255, 17, 152));
	outtextxy(163, 186, _T("YOU WIN!"));
	settextcolor(RGB(215, 193, 238));
	outtextxy(163, 180, _T("YOU WIN!"));

	IMAGE img1;
	loadimage(&img1, "youwin.jpg");
	putimage(550, 330, &img1);

	//返回
	setlinecolor(RGB(255, 17, 102));
	setfillcolor(RGB(255, 255, 204));
	fillrectangle(330, 400, 460, 450);
	settextstyle(25, 9, _T("Verdana"));
	settextcolor(RGB(255, 51, 68));
	outtextxy(342, 412, _T("BACK MENU"));
	MOUSEMSG p;
	while (true){
		p = GetMouseMsg();
		switch (p.uMsg)
		{
		case WM_MOUSEMOVE:	    break;
		case WM_LBUTTONDOWN:
			if (p.x >= 330 && p.x <= 460 && p.y >= 400 && p.y <= 450)
				startdraw();
			else break;
		}
	}
	
}

//初始化
void Init()
{
	win = 1; row = 0;
	total_mine = 0;
	kaiju = 0;
	for (int i = 0; i <= 100; i++){
		for (int j = 0; j <= 100; j++){
			field[i][j] =0 ;
			table[i][j] = '\0';
			spread[i][j] = 0;
			flag[i][j] = 0;
			youjian[i][j] = 0;
		}
	}

}

//开始界面
void startdraw()
{
	setbkmode(TRANSPARENT);//设置透明背景
	setbkcolor(WHITE);
	cleardevice();
	
	IMAGE img1;
	loadimage(&img1, "title1.jpg");
	putimage(10, 0, &img1);
	IMAGE img2;
	loadimage(&img2, "花纹.jpg");
	putimage(550, 408, &img2);
	//版本
	settextstyle(22, 10, _T("Verdana"));
	settextcolor(RGB(255, 51, 68));
	outtextxy(440, 240, _T(" By  Chen Yiru "));
	//开始游戏按钮
	setlinecolor(RGB(255, 192, 203));
	setfillcolor(RGB(245,222,179));
	fillrectangle(200, 255, 340, 305);
	settextstyle(28, 12, _T("Verdana"));
	settextcolor(RGB(46, 39, 87));
	outtextxy(235, 265, _T("Easy"));

	setlinecolor(RGB(255, 192, 203));
	setfillcolor(RGB(245, 222, 179));
	fillrectangle(200, 345, 340, 395);
	settextstyle(28, 12, _T("Verdana"));
	settextcolor(RGB(46, 39, 87));
	outtextxy(225, 355, _T("Medium"));

	setlinecolor(RGB(255, 192, 203));
	setfillcolor(RGB(245, 222, 179));
	fillrectangle(200, 435, 340, 485);
	settextstyle(28, 12, _T("Verdana"));
	settextcolor(RGB(46, 39, 87));
	outtextxy(235, 445, _T("Hard"));

	setlinecolor(RGB(255, 192, 203));
	setfillcolor(RGB(245, 222, 179));
	fillrectangle(470, 295, 600, 350);
	settextstyle(28, 12, _T("Verdana"));
	settextcolor(RGB(46, 39, 87));
	outtextxy(505, 309, _T("Help"));

	setlinecolor(RGB(255, 192, 203));
	setfillcolor(RGB(245, 222, 179));
	fillrectangle(470, 385, 600, 441);
	settextstyle(28, 12, _T("Verdana"));
	settextcolor(RGB(46, 39, 87));
	outtextxy(492, 399, _T("Memory"));
	
	MOUSEMSG m;		// 定义鼠标消息
	while (true)
	{
		//处理鼠标消息
		m = GetMouseMsg();
		switch (m.uMsg)
		{
		case WM_MOUSEMOVE:	    break;
		case WM_LBUTTONDOWN:   if (m.x >= 200 && m.x <= 340 && m.y >= 255 && m.y <= 305)
		                         {
								   Init(); row = 12;	total_mine = 20; gamedraw();  game();
		                       }
							   else if (m.x >= 200 && m.x <= 340 && m.y >= 345 && m.y <= 395)
							   {
								   Init();   row = 15;    total_mine = 50; gamedraw(); game();
							   }
							   else if (m.x >= 200 && m.x <= 340 && m.y >= 435 && m.y <= 485)
							   {
								   Init();      row = 20;      total_mine = 70;    gamedraw();    game();
							   }
							   else if (m.x >= 470 && m.x <= 600 && m.y >= 295 && m.y <= 350)
								   helpdraw();
							   else if (m.x >= 470 && m.x <= 600 && m.y >= 385 && m.y <= 441)
							   {
								   Init();    memory();   gamedraw();  out();   game();
							   }
							   else   break;
		}
	}
	
}

//帮助绘图
void  helpdraw()
{      
	setbkmode(TRANSPARENT);//设置透明背景
	  setbkcolor(RGB(240, 255, 255));
	   cleardevice();
		//游戏帮助绘图
		settextstyle(82, 50, _T("Impact"));
		settextcolor(RGB(255, 17, 152));
		outtextxy(313, 40, _T("HELP"));
		settextcolor(RGB(215, 193, 238));
		outtextxy(313, 37, _T("HELP"));

		//底板
		setlinecolor(RGB(255, 17, 204));
		setfillcolor(RGB(255, 221, 255));
		fillrectangle(100, 110, 700, 500);

		//版本
		settextstyle(18, 10, _T("黑体"));
		settextcolor(RGB(245, 126, 92));
		outtextxy(115, 125, _T("扫雷是一个经典的小游戏"));
		outtextxy(115, 155, _T("【玩法】"));
		outtextxy(125, 185, _T("1.左键点击后扫雷"));
		outtextxy(125, 215, _T("2.右键点击后标记为红旗,左右键均可消除红旗"));
		outtextxy(125, 245, _T("3.Mine显示本局的雷数"));
		outtextxy(125, 275, _T("4.SAVE为存盘，可以用MEMORY调出，恢复残局"));
		outtextxy(145, 305, _T("还有很多玩法，且玩且体会>__<  ! !"));
		//返回
		setlinecolor(RGB(255, 17, 102));
		setfillcolor(RGB(255, 255, 204));
		fillrectangle(330, 400, 460, 450);
		settextstyle(25, 9, _T("Verdana"));
		settextcolor(RGB(255, 51, 68));
		outtextxy(342, 412, _T("BACK MENU"));

		MOUSEMSG p;
		while (true){
			p = GetMouseMsg();
			switch (p.uMsg)
			{
			case WM_MOUSEMOVE:	    break;
			case WM_LBUTTONDOWN:
				if (p.x >= 330 && p.x <= 460 && p.y >= 400 && p.y <= 450)
					startdraw();
				else break;
			}
		}
}

// 初始化星星//关于开始界面的动画
void InitStar(int i)
{
		star[i].x = 0;
		star[i].y = rand() % 600;
		star[i].step = (rand() % 5000) / 1000.0 + 1;
		star[i].color = (int)(star[i].step * 255 / 0.1 + 0.5);	// 速度越快，颜色越亮
		star[i].color = RGB(star[i].color + 100, star[i].color + 50, star[i].color);
}

// 移动星星
void MoveStar(int i)
	{
		// 擦掉原来的星星
		putpixel((int)star[i].x, star[i].y, 0);

		// 计算新位置
		star[i].x += star[i].step * 30;
		if (star[i].x > 800)	InitStar(i);

		// 画新星星
		putpixel((int)star[i].x, star[i].y, star[i].color);
	}

//开始动画
void startflash()
{
    
		srand((unsigned)time(NULL));	// 随机种子
		initgraph(800, 600);			// 创建绘图窗口

		settextstyle(100, 50, _T("Impact"));
		//标题
		settextcolor(RGB(255, 153, 238));
		outtextxy(270, 140, _T("MineSweeper"));
		settextstyle(30, 15, _T("Verdana"));
		settextcolor(RGB(100, 187, 119));
		outtextxy(440, 270, _T(" By:Chen Yiru"));
		// 初始化所有星星
		for (int i = 0; i < MAXSTAR; i++)
		{
			InitStar(i);
			star[i].x = rand() % 800;
		}

		// 绘制星空，
		for (int i = 0; i <= 100; i++){
			for (int i = 0; i < MAXSTAR; i++)
				MoveStar(i);
			}
		
}

int main()
{
	startflash();    //开始动画
	mciSendString("open bk.mp3 alias music_bg", NULL, 0, NULL);
	mciSendString("play music_bg from 0", NULL, 0, NULL);	    //播放音乐
    initgraph(800, 600);
	cleardevice();
	setbkmode(TRANSPARENT);//设置透明背景
	startdraw();
	//关闭
	mciSendString("clase all", NULL, 0, NULL);      //关闭所有
	closegraph();
    return 0;
}


