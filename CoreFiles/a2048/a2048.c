/* 2048游戏 */
#include <stdio.h>
#include "apilib.h"

int rand(void);

int code[4][4]={
	{0,0,0,0},
	{0,0,0,0},
	{0,0,0,0},
	{0,0,0,0}/*游戏中的16个格子*/
};
int temp[5];/*中间变量*/
int move=0;/*移动次数*/
int score=0;/*分数*/

void print(void)/*显示游戏界面*/
{
    int i,j;
	char s[40];
	/* 清屏 */
	api_cls();
	
    api_putstr0("W━上  S━下  A━左  D━右  0━退出\n");
	sprintf(s,"分数:%d  移动次数:%d\n",score,move);
    api_putstr0(s);
    api_putstr0("|-------------------|\n");/*显示横向分隔线*/
    for(i=0;i<=3;i++)
    {
        for(j=0;j<=3;j++)
        {
            if(code[i][j]==0)
            {
                api_putstr0("|    ");/*0显示空格*/
            }
            else
            {
				sprintf(s,"|%4d",code[i][j]);
                api_putstr0(s);/*显示数字和分隔线*/
            }
        }
        api_putstr0("|\n|-------------------|\n");/*显示横向分隔线*/
    }
}

int add(void)/*对中间变量数组进行处理*/
{
    int i;
    int t=0;
    int change=0;/*判断数组是否有改变，0不变，1变化*/

    do
    {
        for(i=0;i<=3;i++)
        {
            if(temp[i]==0)
            {
                if(temp[i]!=temp[i+1])
                    change=1;/*当一个0后面不是0时数组改变*/
                temp[i]=temp[i+1];
                temp[i+1]=0;
            }
        }/*去掉中间的0*/
        t++;
    }while(t<=3);/*重复多次*/
    for(i=1;i<=3;i++)
    {
        if(temp[i]==temp[i-1])
        {

            if(temp[i]!=0)
            {
                change=1;/*当两个非零相同的数相加时数组改变*/
                score=score+temp[i];/*加分*/
            }
            temp[i-1]=temp[i-1]*2;
            temp[i]=0;
        }
    }/*把两个相邻的相同的数加起来*/
    do
    {
        for(i=0;i<=3;i++)
        {
            if(temp[i]==0)
            {
                temp[i]=temp[i+1];
                temp[i+1]=0;
            }
        }/*去掉中间的0*/
        t++;
    }while(t<=3);/*重复多次*/
    return change;
}

void KaliMain(void){
    int gameover=0;/*判断游戏是否结束，1结束，0继续*/
    int i,j;
    int change=1;/*判断格子中的数是否改变，0不变*/
    char input;
	
	api_initmalloc();
	
    while(gameover==0)
    {
        if(change>=1)/*仅当数发生改变时添加新数*/
        {
            do
            {
                i=((unsigned)rand())%4;
                j=((unsigned)rand())%4;
            }while(code[i][j]!=0);
            if(((unsigned)rand())%4==0)
            {
                code[i][j]=4;
            }
            else
            {
                code[i][j]=2;/*随机选一个空格填上2或4*/
            }
            move++;/*增加次数*/
        }
        print();/*显示*/
        input=api_getkey(1);/*输入方向*/
        change=0;
        switch(input)
        {
            case '0':/*退出*/
                api_putstr0("你确定要退出吗?(Y/N)");
                input=api_getkey(1);
                if(input=='y'||input=='Y')
                {
					api_putstr0("再见！\n");
                    api_end();
                }
                break;

            case 'W':
            case 'w':/*上*/
                for(j=0;j<=3;j++)
                {
                    for(i=0;i<=3;i++)
                    {
                        temp[i]=code[i][j];/*把一列数移到中间变量*/
                    }
                    temp[4]=0;
                    change=change+add();
                    for(i=0;i<=3;i++)
                    {
                        code[i][j]=temp[i];/*把处理好的中间变量移回来*/
                    }
                }
                break;

            case 'A':
            case 'a':/*左*/
                for(i=0;i<=3;i++)
                {
                    for(j=0;j<=3;j++)
                    {
                        temp[j]=code[i][j];/*把一行数移到中间变量*/
                    }
                    temp[4]=0;
                    change=change+add();
                    for(j=0;j<=3;j++)
                    {
                        code[i][j]=temp[j];/*把处理好的中间变量移回来*/
                    }
                }

                break;

            case 'S':
            case 's':/*下*/
                for(j=0;j<=3;j++)
                {
                    for(i=0;i<=3;i++)
                    {
                        temp[i]=code[3-i][j];/*把一列数移到中间变量*/
                    }
                    temp[4]=0;
                    change=change+add();
                    for(i=0;i<=3;i++)
                    {
                        code[3-i][j]=temp[i];/*把处理好的中间变量移回来*/
                    }
                }
                break;

            case 'D':
            case 'd':/*右*/
                for(i=0;i<=3;i++)
                {
                    for(j=0;j<=3;j++)
                    {
                        temp[j]=code[i][3-j];/*把一行数移到中间变量*/
                    }
                    temp[4]=0;
                    change=change+add();
                    for(j=0;j<=3;j++)
                    {
                        code[i][3-j]=temp[j];/*把处理好的中间变量移回来*/
                    }
                }
                break;
        }
        gameover=1;
        for(i=0;i<=3;i++)
            for(j=0;j<=3;j++)
                if(code[i][j]==0)
                    gameover=0;/*所有格子都填满则游戏结束*/
    }
	api_putstr0("你竟然通关了！\n");
	api_end();
}
