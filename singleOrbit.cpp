#include <graphics.h>
#include <conio.h>
#include <math.h>
#include <dos.h>

int main()
{
    int gd = DETECT, gm;
    char path[] = "";
    initgraph(&gd, &gm, path);

    int xc = 320, yc = 240;   // center of Earth
    int earthRadius = 40;
    int orbitRadius = 140;

    float angle = 0;

    while (!kbhit())
    {
        cleardevice();

        // Title
        outtextxy(220, 20, "Satellite Orbit Visualization");

        // Earth
        setcolor(BLUE);
        circle(xc, yc, earthRadius);
        setfillstyle(SOLID_FILL, BLUE);
        floodfill(xc, yc, BLUE);
        outtextxy(xc - 15, yc - 5, "Earth");

        // Orbit path
        setcolor(WHITE);
        circle(xc, yc, orbitRadius);

        // Satellite position
        int sx = xc + orbitRadius * cos(angle);
        int sy = yc + orbitRadius * sin(angle);

        // Line from Earth to satellite
        setcolor(LIGHTGRAY);
        line(xc, yc, sx, sy);

        // Satellite
        setcolor(RED);
        circle(sx, sy, 8);
        setfillstyle(SOLID_FILL, RED);
        floodfill(sx, sy, RED);

        outtextxy(sx + 10, sy, "Satellite");

        // Update angle
        angle += 0.03;

        delay(40);
    }

    getch();
    closegraph();
    return 0;
}

