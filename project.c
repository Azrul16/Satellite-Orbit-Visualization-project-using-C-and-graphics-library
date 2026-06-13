#include <graphics.h>
#include <conio.h>
#include <math.h>
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 700
#define EARTH_X 500
#define EARTH_Y 340
#define EARTH_RADIUS 55
#define STAR_COUNT 160
#define SATELLITE_COUNT 3
#define STATION_COUNT 3
#define PI 3.14159265
#define EARTH_RADIUS_KM 6371.0
#define KM_PER_PIXEL 35.0
#define EARTH_GRAVITY 398600.0
#define MAX_LINK_DISTANCE 320.0

typedef struct
{
    int x;
    int y;
    int color;
} Star;

typedef struct
{
    char name[12];
    float angle;
    float speedFactor;
    int orbitOffsetX;
    int orbitOffsetY;
    int color;
    int active;
    int missionMode;
    float fuel;
    long completedOrbits;
} Satellite;

typedef struct
{
    char name[12];
    int angle;
    int color;
} GroundStation;

typedef struct
{
    float angle;
    float speed;
    int orbitRadiusX;
    int orbitRadiusY;
    int paused;
    int direction;
    int selectedSatellite;
    int showLabels;
    int showLinks;
    unsigned long missionFrames;
} Simulation;

const char *MISSION_MODES[] =
{
    "STANDBY",
    "COMMUNICATION",
    "EARTH SCAN",
    "WEATHER"
};

#define MISSION_MODE_COUNT 4

void initializeStars(Star stars[])
{
    int i;

    srand(7);
    for (i = 0; i < STAR_COUNT; i++)
    {
        stars[i].x = rand() % SCREEN_WIDTH;
        stars[i].y = 45 + rand() % (SCREEN_HEIGHT - 90);
        stars[i].color = (i % 6 == 0) ? LIGHTCYAN : WHITE;
    }
}

void drawStars(const Star stars[], int frame)
{
    int i;

    for (i = 0; i < STAR_COUNT; i++)
    {
        int color = stars[i].color;

        if ((i + frame / 8) % 17 == 0)
            color = DARKGRAY;

        putpixel(stars[i].x, stars[i].y, color);
        if (i % 15 == 0)
        {
            putpixel(stars[i].x - 1, stars[i].y, color);
            putpixel(stars[i].x + 1, stars[i].y, color);
        }
    }
}

void drawHeader(void)
{
    setfillstyle(SOLID_FILL, BLUE);
    bar(0, 0, SCREEN_WIDTH, 44);

    setcolor(WHITE);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(270, 13, "MULTI SATELLITE ORBIT SYSTEM");
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
}

void drawEarth(void)
{
    setcolor(LIGHTBLUE);
    setfillstyle(SOLID_FILL, BLUE);
    fillellipse(EARTH_X, EARTH_Y, EARTH_RADIUS, EARTH_RADIUS);

    /* Simple continent shapes make the planet easier to recognize. */
    setcolor(GREEN);
    setfillstyle(SOLID_FILL, GREEN);
    fillellipse(EARTH_X - 13, EARTH_Y - 12, 12, 7);
    fillellipse(EARTH_X + 16, EARTH_Y + 8, 10, 14);
    fillellipse(EARTH_X - 8, EARTH_Y + 20, 7, 9);

    setcolor(LIGHTCYAN);
    arc(EARTH_X, EARTH_Y, 25, 155, EARTH_RADIUS - 4);
    arc(EARTH_X, EARTH_Y, 205, 335, EARTH_RADIUS - 4);

    setcolor(WHITE);
    outtextxy(EARTH_X - 18, EARTH_Y - 4, "EARTH");
}

void drawMoon(int frame)
{
    int moonX = 875;
    int moonY = 145 + (int)(10 * sin(frame * 0.015));

    setcolor(LIGHTGRAY);
    setfillstyle(SOLID_FILL, LIGHTGRAY);
    fillellipse(moonX, moonY, 18, 18);

    setcolor(DARKGRAY);
    circle(moonX - 6, moonY - 4, 3);
    circle(moonX + 5, moonY + 6, 4);
    circle(moonX + 7, moonY - 7, 2);

    setcolor(WHITE);
    outtextxy(moonX - 15, moonY + 24, "MOON");
}

void drawOrbit(int radiusX, int radiusY, int color)
{
    setcolor(color);
    setlinestyle(DOTTED_LINE, 0, NORM_WIDTH);
    ellipse(EARTH_X, EARTH_Y, 0, 360, radiusX, radiusY);
    setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
}

void getSatellitePosition(
    const Simulation *sim,
    const Satellite *satellite,
    int *x,
    int *y)
{
    int radiusX = sim->orbitRadiusX + satellite->orbitOffsetX;
    int radiusY = sim->orbitRadiusY + satellite->orbitOffsetY;

    *x = EARTH_X + (int)(radiusX * cos(satellite->angle));
    *y = EARTH_Y + (int)(radiusY * sin(satellite->angle));
}

void getStationPosition(
    const GroundStation *station,
    int *x,
    int *y)
{
    float radians = station->angle * PI / 180.0;

    *x = EARTH_X + (int)((EARTH_RADIUS + 5) * cos(radians));
    *y = EARTH_Y + (int)((EARTH_RADIUS + 5) * sin(radians));
}

double getAltitudeKm(int satelliteX, int satelliteY)
{
    double dx = satelliteX - EARTH_X;
    double dy = satelliteY - EARTH_Y;
    double distancePixels = sqrt(dx * dx + dy * dy);
    double altitude = (distancePixels - EARTH_RADIUS) * KM_PER_PIXEL;

    return altitude > 0.0 ? altitude : 0.0;
}

double getOrbitalVelocity(double altitudeKm)
{
    return sqrt(EARTH_GRAVITY / (EARTH_RADIUS_KM + altitudeKm));
}

int getConnectionPercentage(
    const Satellite *satellite,
    const GroundStation *station,
    int satelliteX,
    int satelliteY)
{
    int stationX;
    int stationY;
    double dx;
    double dy;
    double linkDistance;
    double quality;

    if (!satellite->active || satellite->fuel <= 0.0)
        return 0;

    getStationPosition(station, &stationX, &stationY);
    dx = satelliteX - stationX;
    dy = satelliteY - stationY;
    linkDistance = sqrt(dx * dx + dy * dy);
    quality = 100.0 * (1.0 - linkDistance / MAX_LINK_DISTANCE);

    if (satellite->missionMode == 1)
        quality += 12.0;
    else if (satellite->missionMode == 0)
        quality -= 15.0;

    quality *= 0.75 + satellite->fuel / 400.0;

    if (quality < 0.0)
        quality = 0.0;
    if (quality > 100.0)
        quality = 100.0;

    return (int)(quality + 0.5);
}

int findNearestStation(
    const GroundStation stations[],
    int satelliteX,
    int satelliteY)
{
    int i;
    int nearestStation = 0;
    double nearestDistance = 1.0e30;

    for (i = 0; i < STATION_COUNT; i++)
    {
        int stationX;
        int stationY;
        double dx;
        double dy;
        double distance;

        getStationPosition(&stations[i], &stationX, &stationY);
        dx = satelliteX - stationX;
        dy = satelliteY - stationY;
        distance = dx * dx + dy * dy;

        if (distance < nearestDistance)
        {
            nearestDistance = distance;
            nearestStation = i;
        }
    }

    return nearestStation;
}

void drawSatellite(
    int x,
    int y,
    const Satellite *satellite,
    int selected,
    int showLabel)
{
    int bodyColor = satellite->active ? LIGHTGRAY : DARKGRAY;

    /* Solar panels */
    setcolor(satellite->active ? satellite->color : DARKGRAY);
    setfillstyle(SOLID_FILL, satellite->active ? BLUE : DARKGRAY);
    bar(x - 20, y - 4, x - 8, y + 4);
    bar(x + 8, y - 4, x + 20, y + 4);
    rectangle(x - 20, y - 4, x - 8, y + 4);
    rectangle(x + 8, y - 4, x + 20, y + 4);

    line(x - 14, y - 4, x - 14, y + 4);
    line(x + 14, y - 4, x + 14, y + 4);

    /* Main body and antenna */
    setcolor(selected ? YELLOW : satellite->color);
    setfillstyle(SOLID_FILL, selected ? WHITE : bodyColor);
    bar(x - 7, y - 7, x + 7, y + 7);
    rectangle(x - 7, y - 7, x + 7, y + 7);
    line(x, y - 7, x, y - 13);
    arc(x, y - 15, 200, 340, 6);

    if (selected)
        circle(x, y, 25);

    if (showLabel)
    {
        setcolor(selected ? YELLOW : (satellite->active ? WHITE : DARKGRAY));
        outtextxy(x + 10, y + 9, satellite->name);
    }
}

void drawGroundStations(
    const GroundStation stations[],
    const Simulation *sim,
    const Satellite satellites[],
    int showLinks)
{
    int i;
    int selectedX;
    int selectedY;
    int selectedStation;

    getSatellitePosition(
        sim, &satellites[sim->selectedSatellite], &selectedX, &selectedY);
    selectedStation = findNearestStation(
        stations, selectedX, selectedY);

    for (i = 0; i < STATION_COUNT; i++)
    {
        int stationX;
        int stationY;
        int selected = i == selectedStation;

        getStationPosition(&stations[i], &stationX, &stationY);

        setcolor(selected ? YELLOW : stations[i].color);
        setfillstyle(SOLID_FILL, selected ? YELLOW : stations[i].color);
        fillellipse(stationX, stationY, 3, 3);
        line(stationX, stationY, stationX, stationY - 8);
        arc(stationX, stationY - 8, 30, 150, 5);

        if (selected)
            circle(stationX, stationY, 7);
    }

    if (!showLinks)
        return;

    for (i = 0; i < SATELLITE_COUNT; i++)
    {
        int satelliteX;
        int satelliteY;
        int stationX;
        int stationY;
        int nearest;
        int connection;

        if (!satellites[i].active)
            continue;

        getSatellitePosition(sim, &satellites[i], &satelliteX, &satelliteY);
        nearest = findNearestStation(stations, satelliteX, satelliteY);
        connection = getConnectionPercentage(
            &satellites[i], &stations[nearest], satelliteX, satelliteY);

        if (connection <= 0)
            continue;

        getStationPosition(&stations[nearest], &stationX, &stationY);
        setcolor(i == sim->selectedSatellite ? YELLOW : satellites[i].color);
        setlinestyle(DOTTED_LINE, 0, NORM_WIDTH);
        line(stationX, stationY, satelliteX, satelliteY);
        setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
    }
}

void drawManagementPanel(
    const Satellite *satellite,
    const GroundStation stations[],
    int satelliteX,
    int satelliteY)
{
    char text[80];
    int stationIndex = findNearestStation(
        stations, satelliteX, satelliteY);
    int connection = getConnectionPercentage(
        satellite,
        &stations[stationIndex],
        satelliteX,
        satelliteY);

    setfillstyle(SOLID_FILL, BLACK);
    setcolor(DARKGRAY);
    rectangle(12, 88, 250, 174);

    setcolor(YELLOW);
    sprintf(text, "CONTROL: %s", satellite->name);
    outtextxy(22, 98, text);

    setcolor(WHITE);
    sprintf(text, "Mode: %s", MISSION_MODES[satellite->missionMode]);
    outtextxy(22, 116, text);

    sprintf(text, "Auto station: %s", stations[stationIndex].name);
    outtextxy(22, 134, text);

    sprintf(text, "Power: %s  Fuel: %3.0f%%",
        satellite->active ? "ON" : "OFF", satellite->fuel);
    outtextxy(22, 152, text);

    setcolor(connection >= 70 ? LIGHTGREEN :
        (connection >= 35 ? YELLOW : LIGHTRED));
    rectangle(268, 126, 410, 154);
    setfillstyle(SOLID_FILL, connection >= 70 ? LIGHTGREEN :
        (connection >= 35 ? YELLOW : LIGHTRED));
    bar(272, 130, 272 + connection, 150);
    sprintf(text, "LINK %d%%", connection);
    setcolor(WHITE);
    outtextxy(302, 136, text);
}

void drawFleetPanel(
    const Simulation *sim,
    const Satellite satellites[],
    const GroundStation stations[])
{
    int i;
    int activeCount = 0;
    char text[100];

    setfillstyle(SOLID_FILL, BLACK);
    setcolor(DARKGRAY);
    rectangle(748, 88, 985, 270);

    setcolor(LIGHTCYAN);
    outtextxy(760, 98, "FLEET STATUS");

    for (i = 0; i < SATELLITE_COUNT; i++)
    {
        int x;
        int y;
        int stationIndex;
        int connection;
        int rowY = 122 + i * 42;

        getSatellitePosition(sim, &satellites[i], &x, &y);
        stationIndex = findNearestStation(stations, x, y);
        connection = getConnectionPercentage(
            &satellites[i], &stations[stationIndex], x, y);
        if (satellites[i].active)
            activeCount++;

        setcolor(i == sim->selectedSatellite ? YELLOW :
            (satellites[i].active ? WHITE : DARKGRAY));
        sprintf(text, "%d. %-5s  %s", i + 1, satellites[i].name,
            satellites[i].active ? "ONLINE" : "OFFLINE");
        outtextxy(760, rowY, text);

        sprintf(text, "%-7s Fuel %3.0f%% Link %3d%%",
            stations[stationIndex].name, satellites[i].fuel, connection);
        outtextxy(772, rowY + 16, text);
    }

    setcolor(activeCount == SATELLITE_COUNT ? LIGHTGREEN : YELLOW);
    sprintf(text, "Active satellites: %d/%d", activeCount, SATELLITE_COUNT);
    outtextxy(760, 252, text);
}

void drawTelemetry(
    const Simulation *sim,
    const Satellite *satellite,
    const GroundStation *station,
    int satelliteX,
    int satelliteY)
{
    char text[80];
    float degrees = satellite->angle * 180.0 / PI;
    double altitude = getAltitudeKm(satelliteX, satelliteY);
    double earthDistance = EARTH_RADIUS_KM + altitude;
    double velocity = getOrbitalVelocity(altitude) *
        satellite->speedFactor;
    int connection = getConnectionPercentage(
        satellite, station, satelliteX, satelliteY);
    unsigned long seconds = sim->missionFrames / 28;
    unsigned long minutes = seconds / 60;

    if (degrees < 0)
        degrees += 360.0;

    setfillstyle(SOLID_FILL, BLACK);
    setcolor(DARKGRAY);
    rectangle(12, 574, 985, 685);

    setcolor(LIGHTGREEN);
    sprintf(text, "LIVE TELEMETRY - %s", satellite->name);
    outtextxy(24, 586, text);

    setcolor(WHITE);
    sprintf(text, "Altitude: %.0f km", altitude);
    outtextxy(24, 610, text);

    sprintf(text, "Earth center: %.0f km", earthDistance);
    outtextxy(24, 630, text);

    sprintf(text, "Angle: %6.1f deg", degrees);
    outtextxy(24, 650, text);

    sprintf(text, "Velocity: %.2f km/s", velocity);
    outtextxy(285, 610, text);

    sprintf(text, "Direction: %s", sim->direction > 0 ? "COUNTER-CLOCKWISE" : "CLOCKWISE");
    outtextxy(285, 630, text);

    sprintf(text, "Completed orbits: %ld", satellite->completedOrbits);
    outtextxy(285, 650, text);

    sprintf(text, "Connection: %d%%", connection);
    outtextxy(650, 610, text);

    sprintf(text, "Mission: %02lu:%02lu", minutes, seconds % 60);
    outtextxy(650, 630, text);

    setcolor(!satellite->active ? LIGHTRED :
        (connection < 25 ? LIGHTRED : (sim->paused ? YELLOW : LIGHTGREEN)));
    outtextxy(650, 650, !satellite->active ? "STATUS: OFFLINE" :
        (connection < 25 ? "STATUS: WEAK SIGNAL" :
        (sim->paused ? "STATUS: PAUSED" : "STATUS: RUNNING")));
}

void drawControls(void)
{
    setcolor(LIGHTGRAY);
    outtextxy(12, 52, "[1-3] Satellite  [AUTO] Nearest station  [E] Power  [F] Refuel");
    outtextxy(12, 68, "[M] Mission  [ARROWS] Orbit  [A/D] Speed  [+/-] Time  [R] Reverse");
    outtextxy(650, 52, "[C] Links  [L] Labels  [X] Reset");
    outtextxy(650, 68, "[SPACE] Pause  [ESC] Exit");
}

void resetSatellite(Satellite *satellite)
{
    satellite->angle = 0.0;
    satellite->speedFactor = 1.0;
    satellite->orbitOffsetX = 0;
    satellite->orbitOffsetY = 0;
    satellite->active = 1;
    satellite->missionMode = 0;
    satellite->fuel = 100.0;
    satellite->completedOrbits = 0;
}

void handleInput(
    Simulation *sim,
    Satellite satellites[],
    int *running)
{
    int key;
    Satellite *selected = &satellites[sim->selectedSatellite];

    if (!kbhit())
        return;

    key = getch();

    if (key == 0 || key == 224)
    {
        key = getch();
        if (key == 72 && selected->orbitOffsetY > -50)
            selected->orbitOffsetY -= 3;
        else if (key == 80 && selected->orbitOffsetY < 75)
            selected->orbitOffsetY += 3;
        else if (key == 75 && selected->orbitOffsetX > -70)
            selected->orbitOffsetX -= 5;
        else if (key == 77 && selected->orbitOffsetX < 80)
            selected->orbitOffsetX += 5;
        return;
    }

    switch (key)
    {
        case 27:
            *running = 0;
            break;
        case ' ':
            sim->paused = !sim->paused;
            break;
        case '+':
        case '=':
            if (sim->speed < 0.12)
                sim->speed += 0.005;
            break;
        case '-':
        case '_':
            if (sim->speed > 0.01)
                sim->speed -= 0.005;
            break;
        case 'a':
        case 'A':
            if (selected->speedFactor > 0.25)
                selected->speedFactor -= 0.05;
            break;
        case 'd':
        case 'D':
            if (selected->speedFactor < 2.5)
                selected->speedFactor += 0.05;
            break;
        case 'e':
        case 'E':
            if (selected->fuel > 0.0)
                selected->active = !selected->active;
            break;
        case 'f':
        case 'F':
            selected->fuel = 100.0;
            selected->active = 1;
            break;
        case 'm':
        case 'M':
            selected->missionMode =
                (selected->missionMode + 1) % MISSION_MODE_COUNT;
            break;
        case 'x':
        case 'X':
            resetSatellite(selected);
            break;
        case 'r':
        case 'R':
            sim->direction *= -1;
            break;
        case '1':
        case '2':
        case '3':
            sim->selectedSatellite = key - '1';
            break;
        case 'c':
        case 'C':
            sim->showLinks = !sim->showLinks;
            break;
        case 'l':
        case 'L':
            sim->showLabels = !sim->showLabels;
            break;
    }
}

void updateSimulation(Simulation *sim, Satellite satellites[])
{
    int i;

    if (sim->paused)
        return;

    sim->missionFrames++;

    for (i = 0; i < SATELLITE_COUNT; i++)
    {
        if (!satellites[i].active)
            continue;

        satellites[i].angle +=
            sim->speed * sim->direction * satellites[i].speedFactor;

        if (satellites[i].angle >= 2 * PI)
        {
            satellites[i].angle -= 2 * PI;
            satellites[i].completedOrbits++;
        }
        else if (satellites[i].angle < 0)
        {
            satellites[i].angle += 2 * PI;
            satellites[i].completedOrbits++;
        }

        if (sim->missionFrames % 180 == 0 && satellites[i].fuel > 0.0)
        {
            satellites[i].fuel -= 0.2 +
                0.1 * satellites[i].speedFactor;
            if (satellites[i].fuel <= 0.0)
            {
                satellites[i].fuel = 0.0;
                satellites[i].active = 0;
            }
        }
    }
}

int main(void)
{
    int running = 1;
    int frame = 0;
    int activePage = 1;
    int i;
    int satelliteX;
    int satelliteY;
    int selectedX;
    int selectedY;
    int selectedStation;
    Star stars[STAR_COUNT];
    Simulation simulation;
    Satellite satellites[SATELLITE_COUNT] =
    {
        {"SAT-1", 0.0, 1.0, 0, 0, LIGHTBLUE, 1, 2, 100.0, 0},
        {"COM-2", 2.1, 0.65, 32, 18, LIGHTMAGENTA, 1, 1, 100.0, 0},
        {"OBS-3", 4.2, 1.35, -38, -24, LIGHTGREEN, 1, 3, 100.0, 0}
    };
    GroundStation stations[STATION_COUNT] =
    {
        {"DHAKA", 215, LIGHTCYAN},
        {"TOKYO", 315, LIGHTMAGENTA},
        {"HOUSTON", 75, YELLOW}
    };

    simulation.angle = 0.0;
    simulation.speed = 0.03;
    simulation.orbitRadiusX = 245;
    simulation.orbitRadiusY = 165;
    simulation.paused = 0;
    simulation.direction = 1;
    simulation.selectedSatellite = 0;
    simulation.showLabels = 1;
    simulation.showLinks = 1;
    simulation.missionFrames = 0;

    initwindow(
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        "Multi Satellite Orbit System",
        20,
        20,
        false,
        true);
    initializeStars(stars);

    while (running)
    {
        handleInput(&simulation, satellites, &running);
        updateSimulation(&simulation, satellites);

        getSatellitePosition(
            &simulation,
            &satellites[simulation.selectedSatellite],
            &selectedX,
            &selectedY);
        selectedStation = findNearestStation(
            stations, selectedX, selectedY);

        setactivepage(activePage);
        cleardevice();
        drawStars(stars, frame);
        drawHeader();
        drawControls();
        drawManagementPanel(
            &satellites[simulation.selectedSatellite],
            stations,
            selectedX,
            selectedY);
        drawFleetPanel(&simulation, satellites, stations);
        drawMoon(frame);

        for (i = 0; i < SATELLITE_COUNT; i++)
        {
            drawOrbit(
                simulation.orbitRadiusX + satellites[i].orbitOffsetX,
                simulation.orbitRadiusY + satellites[i].orbitOffsetY,
                i == simulation.selectedSatellite ? LIGHTGRAY : DARKGRAY);
        }

        drawEarth();
        drawGroundStations(
            stations,
            &simulation,
            satellites,
            simulation.showLinks);

        for (i = 0; i < SATELLITE_COUNT; i++)
        {
            getSatellitePosition(
                &simulation,
                &satellites[i],
                &satelliteX,
                &satelliteY);
            drawSatellite(
                satelliteX,
                satelliteY,
                &satellites[i],
                i == simulation.selectedSatellite,
                simulation.showLabels);
        }

        drawTelemetry(
            &simulation,
            &satellites[simulation.selectedSatellite],
            &stations[selectedStation],
            selectedX,
            selectedY);

        setvisualpage(activePage);
        activePage = 1 - activePage;
        frame++;
        delay(35);
    }

    closegraph();
    return 0;
}
