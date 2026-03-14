
#include <graphics.h>
#include <conio.h>
#include <math.h>
#include <dos.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

const float PI = 3.14159265;

// Draw a filled circle manually
void drawFilledCircle(int x, int y, int r, int color)
{
    setcolor(color);
    circle(x, y, r);
    setfillstyle(SOLID_FILL, color);
    floodfill(x, y, color);
}

// Draw stars
void drawStars(int starX[], int starY[], int n)
{
    setcolor(WHITE);
    for (int i = 0; i < n; i++)
    {
        putpixel(starX[i], starY[i], WHITE);
    }
}

// Draw info panel
void drawInfoPanel()
{
    setcolor(LIGHTGRAY);
    rectangle(10, 10, 220, 170);

    outtextxy(20, 20, "Satellite Orbit Visualization");
    outtextxy(20, 45, "Objects:");
    outtextxy(30, 65, "Earth");
    outtextxy(30, 85, "Moon");
    outtextxy(30, 105, "Sat-1: Circular Orbit");
    outtextxy(30, 125, "Sat-2: Elliptical Orbit");
    outtextxy(30, 145, "Sat-3: High Orbit");

    outtextxy(20, 185, "Press any key to stop");
}

// Draw satellite body
void drawSatellite(int x, int y, int bodyColor)
{
    // body
    setcolor(bodyColor);
    rectangle(x - 6, y - 4, x + 6, y + 4);
    setfillstyle(SOLID_FILL, bodyColor);
    floodfill(x, y, bodyColor);

    // solar panels
    setcolor(CYAN);
    rectangle(x - 14, y - 3, x - 7, y + 3);
    rectangle(x + 7, y - 3, x + 14, y + 3);

    setfillstyle(SOLID_FILL, CYAN);
    floodfill(x - 10, y, CYAN);
    floodfill(x + 10, y, CYAN);
}

// Draw orbit trail point
void drawTrail(int x, int y, int color)
{
    putpixel(x, y, color);
}

// Draw Earth details
void drawEarth(int x, int y, int r)
{
    drawFilledCircle(x, y, r, BLUE);

    // Continents-like patches
    setcolor(GREEN);
    setfillstyle(SOLID_FILL, GREEN);
    fillellipse(x - 10, y - 5, 10, 6);
    fillellipse(x + 8, y + 10, 8, 5);
    fillellipse(x + 12, y - 12, 6, 4);

    setcolor(WHITE);
    outtextxy(x - 15, y + r + 10, "Earth");
}

int main()
{
    int gd = DETECT, gm;
    char path[] = "";
    initgraph(&gd, &gm, path);

    int width = getmaxx();
    int height = getmaxy();

    int xc = width / 2;
    int yc = height / 2;

    // Random stars
    const int STAR_COUNT = 120;
    int starX[STAR_COUNT], starY[STAR_COUNT];
    srand(time(NULL));
    for (int i = 0; i < STAR_COUNT; i++)
    {
        starX[i] = rand() % width;
        starY[i] = rand() % height;
    }

    // Orbit angles
    float angle1 = 0.0;
    float angle2 = 0.0;
    float angle3 = 0.0;
    float moonAngle = 0.0;

    // Orbit dimensions
    int orbit1 = 100;      // circular
    int orbit2a = 170;     // ellipse horizontal radius
    int orbit2b = 100;     // ellipse vertical radius
    int orbit3 = 230;      // high orbit
    int moonOrbit = 70;

    while (!kbhit())
    {
        cleardevice();

        // background stars
        drawStars(starX, starY, STAR_COUNT);

        // title/info
        drawInfoPanel();

        // Earth
        drawEarth(xc, yc, 35);

        // Orbit paths
        setcolor(DARKGRAY);
        circle(xc, yc, orbit1);                  // circular orbit
        ellipse(xc, yc, 0, 360, orbit2a, orbit2b); // elliptical orbit
        circle(xc, yc, orbit3);                  // outer orbit

        // Moon orbit path
        setcolor(LIGHTGRAY);
        circle(xc, yc, moonOrbit);

        // Moon position
        int moonX = xc + moonOrbit * cos(moonAngle);
        int moonY = yc + moonOrbit * sin(moonAngle);

        drawTrail(moonX, moonY, LIGHTGRAY);
        drawFilledCircle(moonX, moonY, 8, LIGHTGRAY);
        outtextxy(moonX + 10, moonY, "Moon");

        // Satellite 1 - circular orbit
        int s1x = xc + orbit1 * cos(angle1);
        int s1y = yc + orbit1 * sin(angle1);

        setcolor(YELLOW);
        line(xc, yc, s1x, s1y);
        drawTrail(s1x, s1y, YELLOW);
        drawSatellite(s1x, s1y, YELLOW);
        outtextxy(s1x + 12, s1y - 10, "Sat-1");

        // Satellite 2 - elliptical orbit
        int s2x = xc + orbit2a * cos(angle2);
        int s2y = yc + orbit2b * sin(angle2);

        drawTrail(s2x, s2y, GREEN);
        drawSatellite(s2x, s2y, GREEN);
        outtextxy(s2x + 12, s2y - 10, "Sat-2");

        // Satellite 3 - high circular orbit
        int s3x = xc + orbit3 * cos(angle3);
        int s3y = yc + orbit3 * sin(angle3);

        drawTrail(s3x, s3y, RED);
        drawSatellite(s3x, s3y, RED);
        outtextxy(s3x + 12, s3y - 10, "Sat-3");

        // Decorative lines
        setcolor(LIGHTBLUE);
        line(s1x, s1y, s2x, s2y);
        line(s2x, s2y, s3x, s3y);

        // On-screen values
        char info1[100], info2[100], info3[100];
        sprintf(info1, "Sat-1 Angle: %.2f", angle1);
        sprintf(info2, "Sat-2 Angle: %.2f", angle2);
        sprintf(info3, "Sat-3 Angle: %.2f", angle3);

        outtextxy(width - 220, 30, info1);
        outtextxy(width - 220, 50, info2);
        outtextxy(width - 220, 70, info3);

        // Update angles with different speeds
        angle1 += 0.05;
        angle2 += 0.03;
        angle3 += 0.02;
        moonAngle += 0.06;

        // Keep angles bounded
        if (angle1 > 2 * PI) angle1 -= 2 * PI;
        if (angle2 > 2 * PI) angle2 -= 2 * PI;
        if (angle3 > 2 * PI) angle3 -= 2 * PI;
        if (moonAngle > 2 * PI) moonAngle -= 2 * PI;

        delay(40);
    }

    getch();
    closegraph();
    return 0;
}
