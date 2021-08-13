/* 2048��Ϸ */
#include <stdio.h>
#include "apilib.h"

int rand(void);

int code[4][4]={
	{0,0,0,0},
	{0,0,0,0},
	{0,0,0,0},
	{0,0,0,0}/*��Ϸ�е�16������*/
};
int temp[5];/*�м����*/
int move=0;/*�ƶ�����*/
int score=0;/*����*/

void print(void)/*��ʾ��Ϸ����*/
{
    int i,j;
	char s[40];
	/* ���� */
	api_cls();
	
    api_putstr0("W����  S����  A����  D����  0���˳�\n");
	sprintf(s,"����:%d  �ƶ�����:%d\n",score,move);
    api_putstr0(s);
    api_putstr0("|-------------------|\n");/*��ʾ����ָ���*/
    for(i=0;i<=3;i++)
    {
        for(j=0;j<=3;j++)
        {
            if(code[i][j]==0)
            {
                api_putstr0("|    ");/*0��ʾ�ո�*/
            }
            else
            {
				sprintf(s,"|%4d",code[i][j]);
                api_putstr0(s);/*��ʾ���ֺͷָ���*/
            }
        }
        api_putstr0("|\n|-------------------|\n");/*��ʾ����ָ���*/
    }
}

int add(void)/*���м����������д���*/
{
    int i;
    int t=0;
    int change=0;/*�ж������Ƿ��иı䣬0���䣬1�仯*/

    do
    {
        for(i=0;i<=3;i++)
        {
            if(temp[i]==0)
            {
                if(temp[i]!=temp[i+1])
                    change=1;/*��һ��0���治��0ʱ����ı�*/
                temp[i]=temp[i+1];
                temp[i+1]=0;
            }
        }/*ȥ���м��0*/
        t++;
    }while(t<=3);/*�ظ����*/
    for(i=1;i<=3;i++)
    {
        if(temp[i]==temp[i-1])
        {

            if(temp[i]!=0)
            {
                change=1;/*������������ͬ�������ʱ����ı�*/
                score=score+temp[i];/*�ӷ�*/
            }
            temp[i-1]=temp[i-1]*2;
            temp[i]=0;
        }
    }/*���������ڵ���ͬ����������*/
    do
    {
        for(i=0;i<=3;i++)
        {
            if(temp[i]==0)
            {
                temp[i]=temp[i+1];
                temp[i+1]=0;
            }
        }/*ȥ���м��0*/
        t++;
    }while(t<=3);/*�ظ����*/
    return change;
}

void KaliMain(void){
    int gameover=0;/*�ж���Ϸ�Ƿ������1������0����*/
    int i,j;
    int change=1;/*�жϸ����е����Ƿ�ı䣬0����*/
    char input;
	
	api_initmalloc();
	
    while(gameover==0)
    {
        if(change>=1)/*�����������ı�ʱ�������*/
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
                code[i][j]=2;/*���ѡһ���ո�����2��4*/
            }
            move++;/*���Ӵ���*/
        }
        print();/*��ʾ*/
        input=api_getkey(1);/*���뷽��*/
        change=0;
        switch(input)
        {
            case '0':/*�˳�*/
                api_putstr0("��ȷ��Ҫ�˳���?(Y/N)");
                input=api_getkey(1);
                if(input=='y'||input=='Y')
                {
					api_putstr0("�ټ���\n");
                    api_end();
                }
                break;

            case 'W':
            case 'w':/*��*/
                for(j=0;j<=3;j++)
                {
                    for(i=0;i<=3;i++)
                    {
                        temp[i]=code[i][j];/*��һ�����Ƶ��м����*/
                    }
                    temp[4]=0;
                    change=change+add();
                    for(i=0;i<=3;i++)
                    {
                        code[i][j]=temp[i];/*�Ѵ���õ��м�����ƻ���*/
                    }
                }
                break;

            case 'A':
            case 'a':/*��*/
                for(i=0;i<=3;i++)
                {
                    for(j=0;j<=3;j++)
                    {
                        temp[j]=code[i][j];/*��һ�����Ƶ��м����*/
                    }
                    temp[4]=0;
                    change=change+add();
                    for(j=0;j<=3;j++)
                    {
                        code[i][j]=temp[j];/*�Ѵ���õ��м�����ƻ���*/
                    }
                }

                break;

            case 'S':
            case 's':/*��*/
                for(j=0;j<=3;j++)
                {
                    for(i=0;i<=3;i++)
                    {
                        temp[i]=code[3-i][j];/*��һ�����Ƶ��м����*/
                    }
                    temp[4]=0;
                    change=change+add();
                    for(i=0;i<=3;i++)
                    {
                        code[3-i][j]=temp[i];/*�Ѵ���õ��м�����ƻ���*/
                    }
                }
                break;

            case 'D':
            case 'd':/*��*/
                for(i=0;i<=3;i++)
                {
                    for(j=0;j<=3;j++)
                    {
                        temp[j]=code[i][3-j];/*��һ�����Ƶ��м����*/
                    }
                    temp[4]=0;
                    change=change+add();
                    for(j=0;j<=3;j++)
                    {
                        code[i][3-j]=temp[j];/*�Ѵ���õ��м�����ƻ���*/
                    }
                }
                break;
        }
        gameover=1;
        for(i=0;i<=3;i++)
            for(j=0;j<=3;j++)
                if(code[i][j]==0)
                    gameover=0;/*���и��Ӷ���������Ϸ����*/
    }
	api_putstr0("�㾹Ȼͨ���ˣ�\n");
	api_end();
}
