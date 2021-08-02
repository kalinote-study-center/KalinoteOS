/* 时钟app */
/*
#include <stdio.h>
#include "apilib.h"
*/
#include <stdio.h>
#include <conio.h>
#include <graphics.h>
#include <math.h>

#define High 480
#define Width 640
#define PI 3.14159

int main()
{
	initgraph(Width, High);
	int center_x = Width/2, center_y = High/2;

	int secondLength = Width/5;
	int minuteLength = Width/6;
	int hourLength = Width/7;

	int secondEnd_x, secondEnd_y;
	int minuteEnd_x, minuteEnd_y;
	int hourEnd_x, hourEnd_y;

	float secondAngle, minuteAngle, hourAngle;

	SYSTEMTIME ti;

	BeginBatchDraw();

	while(1)
	{
		int x, y, i;
		setlinestyle(PS_SOLID, 1);
		setcolor(WHITE);
		circle(center_x, center_y, Width/4);

		for(i = 0; i < 60; i++)
		{
			x = center_x + int( Width/4.3 * sin( PI * 2 * i/60));
			y = center_y + int( Width/4.3 * cos( PI * 2 * i/60));

			if( i % 15 == 0)
			{
				bar( x - 5, y - 5, x + 5, y + 5);
			}
			else if( i % 5 == 0)
			{
				circle(x, y, 3);
			}
			else
			{
				putpixel(x, y, WHITE);
			}
		}
		outtextxy(center_x - 25, center_y + Width/6, "我的时钟");

		GetLocalTime(&ti);

		secondAngle = ti.wSecond * 2*PI/60;
		minuteAngle = ti.wMinute * 2*PI/60 + secondAngle/60;
		hourAngle = ti.wHour * 2*PI/12 + minuteAngle/12;

		secondEnd_x = center_x + secondLength * sin(secondAngle);
		secondEnd_y = center_y - secondLength * cos(secondAngle);

		minuteEnd_x = center_x + minuteLength * sin(minuteAngle);
		minuteEnd_y = center_y - minuteLength * cos(minuteAngle);

		hourEnd_x = center_x + hourLength * sin(hourAngle);
		hourEnd_y = center_y - hourLength * cos(hourAngle);

		setlinestyle(PS_SOLID, 2);
		setcolor(YELLOW);
		line(center_x, center_y, secondEnd_x, secondEnd_y);

		setlinestyle(PS_SOLID, 5);
		setcolor(BLUE);
		line(center_x, center_y, minuteEnd_x, minuteEnd_y);

		setlinestyle(PS_SOLID, 10);
		setcolor(RED);
		line(center_x, center_y, hourEnd_x, hourEnd_y);

		FlushBatchDraw();
		Sleep(10);

		setcolor(BLACK);
		setlinestyle(PS_SOLID, 2);
		line(center_x, center_y, secondEnd_x, secondEnd_y);
		setlinestyle(PS_SOLID, 5);
		line(center_x, center_y, minuteEnd_x, minuteEnd_y);
		setlinestyle(PS_SOLID, 10);
		line(center_x, center_y, hourEnd_x, hourEnd_y);
		
	}

	EndBatchDraw();
	getch();
	closegraph();
	return 0;
}