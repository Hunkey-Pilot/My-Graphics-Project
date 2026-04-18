#include <GL/freeglut.h>
#include<bits/stdc++.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
using namespace std;

int xs, ys, xe, ye;
int scene = 0;
int trafficsignal = 2;// 2=Green, 1=Yellow, 0=Red
int currentSound = -1; // -1 = no sound, 0 = birds, 1 = rain
float carX = 0;
float carSpeed = 5.0f;
float maxSpeed = 5.0f;

const float stopLineX = 1620.0f;
const float slowStartX = 1350.0f;
int starx[100], stary[100];
float rippleOffset = 0.0f;
float boatX = 1800.0f;
float cloudLayer1 = 0.0f;
float cloudLayer2 = 0.0f;
float cloudLayer3 = 0.0f;

// basic algos
void drawPixel(int x, int y) {
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
}

void DDA(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;

    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

    float xInc = dx / (float)steps;
    float yInc = dy / (float)steps;

    float x = x1;
    float y = y1;

    for (int i = 0; i <= steps; i++) {
        drawPixel(round(x), round(y));
        x += xInc;
        y += yInc;
    }
}

void plotCirclePoints(int xc, int yc, int x, int y)
{
    float xScale = 0.53f;

    drawPixel(xc + x * xScale, yc + y);
    drawPixel(xc - x * xScale, yc + y);
    drawPixel(xc + x * xScale, yc - y);
    drawPixel(xc - x * xScale, yc - y);
    drawPixel(xc + y * xScale, yc + x);
    drawPixel(xc - y * xScale, yc + x);
    drawPixel(xc + y * xScale, yc - x);
    drawPixel(xc - y * xScale, yc - x);
}

void midpointCircle(int xc, int yc, int r)
{
    int x = 0;
    int y = r;
    int p = 1 - r;

    while (x <= y)
    {
        plotCirclePoints(xc, yc, x, y);

        x++;

        if (p < 0)
        {
            p = p + 2 * x + 3;
        }
        else
        {
            y--;
            p = p + 2 * x + 5 - 2 * y;
        }
    }
}

void drawCircle(int xc, int yc, int r)
{
    for (int i = 1; i <= r; i++)
    {
        midpointCircle(xc, yc, i);
    }
}

void drawSimpleCircle(GLfloat rx, GLfloat ry, GLfloat cx, GLfloat cy)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);

    for (int i = 0; i <= 100; i++) {
        float angle = 2.0f * 3.1416f * i / 100.0f;

        float x = rx * cosf(angle);
        float y = ry * sinf(angle);

        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}

void playBirdSound()
{
    PlaySound(TEXT("assets\\birds-sfx_74bpm.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}

void playRainSound()
{
    PlaySound(TEXT("assets\\thunder-rain.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}

void playNightSound()
{
    PlaySound(TEXT("assets\\schorsch1964-night-atmosphere-with-crickets-374652.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}

void stopAllSounds()
{
    PlaySound(0, 0, 0);
}

void updatedSceneSound()
{
    if (scene == 0)
    {
        playBirdSound();
    }
    else if (scene == 2)
    {
        playNightSound();
    }
    else if (scene == 1 || scene == 3)
    {
        playRainSound();
    }
}


// scenario
void drawHouse()
{
    // Roof
    glColor3ub(160, 82, 45);
    glBegin(GL_POLYGON);
    glVertex2d(0, 220);
    glVertex2d(135, 220);
    glVertex2d(110, 270);
    glVertex2d(25, 270);
    glEnd();

    // Left upper triangle/front
    glColor3ub(255, 222, 173);
    glBegin(GL_POLYGON);
    glVertex2d(10, 220);
    glVertex2d(50, 220);
    glVertex2d(25, 255);
    glEnd();

    // Left wall
    glColor3ub(255, 222, 173);
    glBegin(GL_POLYGON);
    glVertex2d(10, 150);
    glVertex2d(50, 150);
    glVertex2d(50, 220);
    glVertex2d(10, 220);
    glEnd();

    // Right wall
    glColor3ub(222, 184, 135);
    glBegin(GL_POLYGON);
    glVertex2d(50, 150);
    glVertex2d(125, 150);
    glVertex2d(125, 220);
    glVertex2d(50, 220);
    glEnd();

    // Bottom border
    glColor3ub(160, 82, 45);
    glBegin(GL_POLYGON);
    glVertex2d(10, 150);
    glVertex2d(125, 150);
    glVertex2d(125, 140);
    glVertex2d(10, 140);
    glEnd();

    // Door
    glColor3ub(120, 60, 20);
    glBegin(GL_POLYGON);
    glVertex2d(75, 150);
    glVertex2d(95, 150);
    glVertex2d(95, 195);
    glVertex2d(75, 195);
    glEnd();

    // Window
    if (scene == 2 || scene == 3)
    {
        glColor3f(0.9, 0.9, 0.5);
    }
    else
    {
        glColor3f(0.9, 0.95, 1.0);
    }

    glBegin(GL_POLYGON);
    glVertex2d(20, 200);
    glVertex2d(35, 200);
    glVertex2d(35, 175);
    glVertex2d(20, 175);
    glEnd();
}

void drawHouseAt(float x, float y, float scale)
{
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(scale, scale, 1.0);
    drawHouse();
    glPopMatrix();
}

void drawBush(float x, float y)
{
    // Lower layer
    glColor3f(0.0f, 0.35f, 0.0f);
    drawSimpleCircle(30, 30, x, y);

    // Middle layer
    glColor3f(0.0f, 0.45f, 0.0f);
    drawSimpleCircle(28, 28, x + 20, y + 10);
    drawSimpleCircle(28, 28, x - 20, y + 10);

    // Upper layer
    glColor3f(0.0f, 0.55f, 0.0f);
    drawCircle(x, y + 25, 25);
    drawCircle(x + 15, y + 30, 22);
    drawCircle(x - 15, y + 30, 22);

    // Extra fill for gaps
    glColor3f(0.0f, 0.4f, 0.0f);
    drawSimpleCircle(20, 20, x + 5, y + 5);
    drawSimpleCircle(20, 20, x - 5, y + 5);
}

void drawBigTree(float x, float y)
{
    // Trunk
    glColor3f(0.45f, 0.22f, 0.07f);
    glBegin(GL_QUADS);
    glVertex2f(x - 20, y);
    glVertex2f(x - 20, y + 190);
    glVertex2f(x + 20, y + 190);
    glVertex2f(x + 20, y);
    glEnd();

    // Leaf
    glColor3f(0.0f, 0.35f, 0.0f);
    drawSimpleCircle(55, 55, x, y + 220);
    drawSimpleCircle(45, 45, x - 35, y + 200);
    drawSimpleCircle(45, 45, x + 35, y + 200);

    glColor3f(0.0f, 0.45f, 0.0f);
    drawSimpleCircle(40, 40, x - 20, y + 245);
    drawSimpleCircle(40, 40, x + 20, y + 245);
    drawSimpleCircle(38, 38, x, y + 270);

    glColor3f(0.0f, 0.55f, 0.0f);
    drawSimpleCircle(30, 30, x, y + 235);
    drawSimpleCircle(28, 28, x - 12, y + 215);
    drawSimpleCircle(28, 28, x + 12, y + 215);
}

void drawGround()
{
    glColor3f(0.4f, 0.85f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(0, 120);
    glVertex2f(0, -80);
    glVertex2f(1900, -80);
    glVertex2f(1900, 120);
    glEnd();
}

void drawFootpath()
{
    glColor3f(0.6, 0.6, 0.6);
    glBegin(GL_QUADS);
    glVertex2f(0, -80);
    glVertex2f(0, -120);
    glVertex2f(1900, -120);
    glVertex2f(1900, -80);
    glEnd();
}

void drawRain()
{
    glColor3f(0.7, 0.7, 1.0);
    for (int i = 0; i < 2500; i++)
    {
        int x = rand() % 1900;
        int y = (rand() % 1400) - 600;
        glBegin(GL_LINES);
        glVertex2f(x, y);
        glVertex2f(x, y - 20);
        glEnd();
    }
}
//supti

void drawTrafficLight(float x, float y)
{
    glColor3f(0.55f, 0.27f, 0.07f);
    glBegin(GL_QUADS);
    glVertex2f(x - 6, y);
    glVertex2f(x - 6, y + 220);
    glVertex2f(x + 6, y + 220);
    glVertex2f(x + 6, y);
    glEnd();

    glColor3f(0.05, 0.05, 0.05);
    glBegin(GL_QUADS);
    glVertex2f(x - 35, y + 350);
    glVertex2f(x - 35, y + 365);
    glVertex2f(x + 35, y + 365);
    glVertex2f(x + 35, y + 350);
    glEnd();

    glColor3f(0.1, 0.1, 0.1);
    glBegin(GL_QUADS);
    glVertex2f(x - 30, y + 210);
    glVertex2f(x - 30, y + 350);
    glVertex2f(x + 30, y + 350);
    glVertex2f(x + 30, y + 210);
    glEnd();

    if (trafficsignal == 0)
        glColor3f(1.0, 0.0, 0.0);
    else
        glColor3f(0.5, 0.1, 0.1);
    drawCircle(x, y + 330, 20);

    if (trafficsignal == 1)
        glColor3f(1.0, 1.0, 0.0);
    else
        glColor3f(0.25, 0.25, 0.0);
    drawCircle(x, y + 285, 20);

    if (trafficsignal == 2)
        glColor3f(0.0, 1.0, 0.0);
    else
        glColor3f(0.0, 0.25, 0.0);
    drawCircle(x, y + 240, 20);
}

void drawTrafficLightReflection(float x, float y)
{
    glPushMatrix();

    glTranslatef(0, -600, 0);
    glTranslatef(0, sin(glutGet(GLUT_ELAPSED_TIME) * 0.002) * 3, 0);
    glScalef(1, -1, 1);

    glColor3f(0.3, 0.3, 0.3);

    glBegin(GL_QUADS);
    glVertex2f(x - 6, y);
    glVertex2f(x - 6, y + 220);
    glVertex2f(x + 6, y + 220);
    glVertex2f(x + 6, y);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(x - 30, y + 210);
    glVertex2f(x - 30, y + 350);
    glVertex2f(x + 30, y + 350);
    glVertex2f(x + 30, y + 210);
    glEnd();

    glColor3f(0.5, 0.1, 0.1);
    drawCircle(x, y + 330, 20);

    glColor3f(0.5, 0.5, 0.1);
    drawCircle(x, y + 285, 20);

    glColor3f(0.1, 0.5, 0.1);
    drawCircle(x, y + 240, 20);

    glPopMatrix();
}

void drawStreetLight(float x, float y)
{
    glColor3f(0.2, 0.2, 0.2);
    glBegin(GL_QUADS);
    glVertex2f(x - 5, y);
    glVertex2f(x - 5, y + 350);
    glVertex2f(x + 5, y + 350);
    glVertex2f(x + 5, y);
    glEnd();

    if (scene == 2 || scene == 3)
        glColor3f(1.0, 1.0, 0.5);
    else
        glColor3f(1.0, 1.0, 0.8);

    drawSimpleCircle(20, 20, x, y + 350);
}

void drawMainRoad()
{
    glColor3f(0.2, 0.2, 0.2);
    glBegin(GL_QUADS);
    glVertex2f(0, -120);
    glVertex2f(0, -300);
    glVertex2f(1900, -300);
    glVertex2f(1900, -120);
    glEnd();
}

void drawMainRoadDivider()
{
    glColor3f(1, 1, 1);

    glBegin(GL_QUADS);
    for (int x = 50; x < 1850; x += 220)
    {
        glVertex2f(x, -210);
        glVertex2f(x + 100, -210);
        glVertex2f(x + 100, -190);
        glVertex2f(x, -190);
    }
    glEnd();
}

void drawPond()
{
    glColor3f(0.1, 0.5, 0.9);
    glBegin(GL_QUADS);
    glVertex2f(0, -300);
    glVertex2f(0, -800);
    glVertex2f(1900, -800);
    glVertex2f(1900, -300);
    glEnd();
}

void drawPondLines()
{
    glColor3f(0.9, 0.9, 0.9);
    for (int y = -350; y >= -750; y -= 100)
    {
        for (int x = 50; x < 1850; x += 220)
        {
            DDA(x, y, x + 100, y);
        }
    }
}
//bulbul



void drawCloud(float x, float y, float scale = 1.0f)
{
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(scale, scale, 1.0f);

    drawSimpleCircle(35, 25, 0, 0);
    drawSimpleCircle(30, 22, 30, 10);
    drawSimpleCircle(32, 24, 60, 0);
    drawSimpleCircle(28, 20, 15, -15);
    drawSimpleCircle(28, 20, 45, -15);
    drawSimpleCircle(22, 16, 75, -10);

    glPopMatrix();
}

void drawAllClouds()
{
    if (scene == 2 || scene == 3)
        glColor3f(0.75f, 0.75f, 0.78f);
    else
        glColor3f(1.0f, 1.0f, 1.0f);

    // Layer 1
    drawCloud(cloudLayer1 + 50, 730, 1.2f);
    drawCloud(cloudLayer1 + 320, 760, 0.9f);
    drawCloud(cloudLayer1 + 620, 720, 1.1f);
    drawCloud(cloudLayer1 + 920, 750, 1.0f);
    drawCloud(cloudLayer1 + 1240, 735, 1.15f);
    drawCloud(cloudLayer1 + 1540, 765, 0.95f);
    drawCloud(cloudLayer1 + 1820, 725, 1.1f);
    drawCloud(cloudLayer1 - 250, 745, 1.0f);
    drawCloud(cloudLayer1 + 2120, 740, 1.1f);

    // Layer 2
    drawCloud(cloudLayer2 + 100, 660, 1.0f);
    drawCloud(cloudLayer2 + 420, 690, 1.25f);
    drawCloud(cloudLayer2 + 760, 670, 0.95f);
    drawCloud(cloudLayer2 + 1080, 700, 1.1f);
    drawCloud(cloudLayer2 + 1420, 675, 1.0f);
    drawCloud(cloudLayer2 + 1740, 690, 1.2f);
    drawCloud(cloudLayer2 - 300, 680, 1.05f);
    drawCloud(cloudLayer2 + 2150, 670, 1.0f);

    // Layer 3
    drawCloud(cloudLayer3 + 0, 590, 0.9f);
    drawCloud(cloudLayer3 + 260, 610, 1.0f);
    drawCloud(cloudLayer3 + 540, 585, 0.95f);
    drawCloud(cloudLayer3 + 820, 620, 1.05f);
    drawCloud(cloudLayer3 + 1120, 600, 0.9f);
    drawCloud(cloudLayer3 + 1400, 615, 1.0f);
    drawCloud(cloudLayer3 + 1700, 590, 0.95f);
    drawCloud(cloudLayer3 - 220, 605, 1.0f);
    drawCloud(cloudLayer3 + 2050, 600, 0.9f);
}

void drawSmallTree(float x, float y)
{
    glColor3f(0.55, 0.27, 0.07);
    glBegin(GL_QUADS);
    glVertex2f(x - 10, y);
    glVertex2f(x - 10, y + 80);
    glVertex2f(x + 10, y + 80);
    glVertex2f(x + 10, y);
    glEnd();

    glColor3f(0.0, 0.6, 0.0);
    drawSimpleCircle(50, 50, x, y + 100);

    glColor3f(0.0f, 0.45f, 0.0f);
    drawSimpleCircle(40, 40, x, y + 130);

    glColor3f(0.0f, 0.55f, 0.0f);
    drawSimpleCircle(30, 30, x, y + 155);
}

void drawCar()
{
    glPushMatrix();

    glTranslatef(carX, 0, 0);
    glTranslatef(1600, 0, 0);
    glScalef(-1, 1, 1);

    // Car body
    glColor3f(0.9, 0.1, 0.1);
    glBegin(GL_QUADS);
    glVertex2f(700, -250);
    glVertex2f(700, -180);
    glVertex2f(900, -180);
    glVertex2f(900, -250);
    glEnd();

    // Car upper body
    glColor3f(0.8, 0.0, 0.0);
    glBegin(GL_QUADS);
    glVertex2f(740, -180);
    glVertex2f(790, -130);
    glVertex2f(860, -130);
    glVertex2f(890, -180);
    glEnd();

    // Front glass
    glColor3f(0.7, 0.9, 1.0);
    glBegin(GL_QUADS);
    glVertex2f(800, -175);
    glVertex2f(815, -140);
    glVertex2f(850, -140);
    glVertex2f(850, -175);
    glEnd();

    // Back glass
    glBegin(GL_QUADS);
    glVertex2f(750, -175);
    glVertex2f(770, -140);
    glVertex2f(800, -140);
    glVertex2f(790, -175);
    glEnd();

    // Wheels
    glColor3f(0.0, 0.0, 0.0);
    drawCircle(760, -250, 25);
    drawCircle(860, -250, 25);

    // Wheel center
    glColor3f(0.7, 0.7, 0.7);
    drawCircle(760, -250, 10);
    drawCircle(860, -250, 10);

    // Headlight
    if (scene == 2 || scene == 3)
    {
        glColor3f(1.0, 1.0, 0.4);
    }
    glBegin(GL_QUADS);
    glVertex2f(900, -220);
    glVertex2f(900, -200);
    glVertex2f(910, -200);
    glVertex2f(910, -220);
    glEnd();

    glPopMatrix();
}

void drawCarReflection()
{
    glPushMatrix();

    glTranslatef(0, -600, 0);
    glTranslatef(0, sin(glutGet(GLUT_ELAPSED_TIME) * 0.002) * 3, 0);
    glScalef(1, -1, 1);

    glTranslatef(carX, 0, 0);
    glTranslatef(1600, 0, 0);
    glScalef(-1, 1, 1);

    glColor3f(0.35, 0.08, 0.08);
    glBegin(GL_QUADS);
    glVertex2f(700, -250);
    glVertex2f(700, -180);
    glVertex2f(900, -180);
    glVertex2f(900, -250);
    glEnd();

    glColor3f(0.30, 0.05, 0.05);
    glBegin(GL_QUADS);
    glVertex2f(740, -180);
    glVertex2f(790, -130);
    glVertex2f(860, -130);
    glVertex2f(890, -180);
    glEnd();

    glColor3f(0.45, 0.55, 0.60);
    glBegin(GL_QUADS);
    glVertex2f(800, -175);
    glVertex2f(815, -140);
    glVertex2f(850, -140);
    glVertex2f(850, -175);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(750, -175);
    glVertex2f(770, -140);
    glVertex2f(800, -140);
    glVertex2f(790, -175);
    glEnd();

    glColor3f(0.08, 0.08, 0.08);
    drawSimpleCircle(25, 25, 760, -250);
    drawSimpleCircle(25, 25, 860, -250);

    glColor3f(0.35, 0.35, 0.35);
    drawSimpleCircle(10, 10, 760, -250);
    drawSimpleCircle(10, 10, 860, -250);

    glColor3f(0.50, 0.50, 0.20);
    glBegin(GL_QUADS);
    glVertex2f(900, -220);
    glVertex2f(900, -200);
    glVertex2f(910, -200);
    glVertex2f(910, -220);
    glEnd();

    glPopMatrix();
}

void drawBoat(float x, float y)
{
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(1.8, 1.8, 1.0f);

    // Boat base
    glColor3f(0.55, 0.27, 0.07);
    glBegin(GL_POLYGON);
    glVertex2f(-80, 0);
    glVertex2f(80, 0);
    glVertex2f(50, -40);
    glVertex2f(-50, -40);
    glEnd();

    // Upper border
    glColor3f(0.65, 0.32, 0.10);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-80, 0);
    glVertex2f(80, 0);
    glVertex2f(50, -40);
    glVertex2f(-50, -40);
    glEnd();

    // Mast
    glColor3f(0.4, 0.2, 0.1);
    glBegin(GL_QUADS);
    glVertex2f(-3, 0);
    glVertex2f(-3, 90);
    glVertex2f(3, 90);
    glVertex2f(3, 0);
    glEnd();

    // Sail (pal)
    glColor3f(1.0, 1.0, 1.0); // white sail
    glBegin(GL_TRIANGLES);
    glVertex2f(3, 80);
    glVertex2f(3, 20);
    glVertex2f(60, 50);
    glEnd();



    // Rope line look
    glColor3f(0.2, 0.2, 0.2);
    glBegin(GL_LINES);
    glVertex2f(0, 90);
    glVertex2f(60, 50);

    glEnd();

    glPopMatrix();
}

void drawBoatReflection(float x, float y)
{
    glPushMatrix();

    glTranslatef(x, y, 0);
    glTranslatef(0, sin(glutGet(GLUT_ELAPSED_TIME) * 0.002) * 3, 0);
    glScalef(1.8f, -1.8f, 1.0f);

    // Boat base
    glColor3f(0.30, 0.15, 0.05);
    glBegin(GL_POLYGON);
    glVertex2f(-80, 0);
    glVertex2f(80, 0);
    glVertex2f(50, -40);
    glVertex2f(-50, -40);
    glEnd();

    // Upper border
    glColor3f(0.35, 0.18, 0.06);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-80, 0);
    glVertex2f(80, 0);
    glVertex2f(50, -40);
    glVertex2f(-50, -40);
    glEnd();

    // Mast
    glColor3f(0.4, 0.2, 0.1);
    glBegin(GL_QUADS);
    glVertex2f(-3, 0);
    glVertex2f(-3, 90);
    glVertex2f(3, 90);
    glVertex2f(3, 0);
    glEnd();

    // Sail (pal)
    glColor3f(1.0, 1.0, 1.0); // white sail
    glBegin(GL_TRIANGLES);
    glVertex2f(3, 80);
    glVertex2f(3, 20);
    glVertex2f(60, 50);
    glEnd();



    // Rope line look
    glColor3f(0.2, 0.2, 0.2);
    glBegin(GL_LINES);
    glVertex2f(0, 90);
    glVertex2f(60, 50);

    glEnd();

    glPopMatrix();
}

void generateStars()
{
    for (int i = 0; i < 100; i++) {
        starx[i] = rand() % 1900;
        stary[i] = rand() % 800;
    }
}

void drawStar()
{
    for (int i = 0;i <= 100;i++) {
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_POINTS);
        for (int i = 0; i < 100; i++)
        {
            glVertex2f(starx[i], stary[i]);
        }
        glEnd();
    }
}

void switchToGreen(int value)
{
    trafficsignal = 2;
}



void update(int value)
{
    float carFrontX = carX + 900.0f;
    float targetSpeed = maxSpeed;

    if (trafficsignal == 0 || trafficsignal == 1)
    {
        if (carFrontX < 1702) {
            if (carFrontX >= stopLineX)
            {
                targetSpeed = 0.0f;
            }
        }
    }

    // smooth acceleration / deceleration
    carSpeed += (targetSpeed - carSpeed) * 0.08f;

    if (targetSpeed == 0.0f && carSpeed < 0.05f)
        carSpeed = 0.0f;

    carX += carSpeed;

    if (carX > 1200)
        carX = -1000;

    // Cloud motion
    cloudLayer1 -= 0.35f;
    cloudLayer2 -= 0.60f;
    cloudLayer3 -= 0.90f;

    if (cloudLayer1 < -400) cloudLayer1 = 0;
    if (cloudLayer2 < -500) cloudLayer2 = 0;
    if (cloudLayer3 < -600) cloudLayer3 = 0;


    boatX -= 1.5f;
    if (boatX < -200)
        boatX = 2000;



    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void normalScenario()
{

    if (scene == 2 || scene == 3)
    {
        glColor3f(0.8, 0.8, 0.8);
        drawStar();
    }
    else
        glColor3f(1.0, 0.8, 0.0);
    drawCircle(1500, 700, 90); // sun or moon

    drawAllClouds();

    drawGround();
    drawBigTree(50, 80);
    drawBigTree(650, 80);
    drawBigTree(1250, 80);
    drawBigTree(1850, 80);

    for (int x = 0; x <= 1900; x += 50) {
        drawBush(x, 100);
    }

    drawHouseAt(50, -280, 2);
    drawHouseAt(400, -280, 2);
    drawHouseAt(750, -280, 2);
    drawHouseAt(1100, -280, 2);
    drawHouseAt(1450, -280, 2);
    drawHouseAt(1800, -280, 2);

    drawFootpath();
    drawMainRoad();
    drawPond();
    drawPondLines();



    drawMainRoadDivider();

    drawSmallTree(100, -80);
    drawSmallTree(400, -80);
    drawSmallTree(700, -80);
    drawSmallTree(1000, -80);
    drawSmallTree(1300, -80);
    drawSmallTree(1600, -80);
    drawSmallTree(1885, -80);

    drawStreetLight(300, -120);
    drawStreetLight(600, -120);
    drawStreetLight(900, -120);
    drawStreetLight(1200, -120);
    drawStreetLight(1500, -120);
    drawStreetLight(1800, -120);

    drawCar();
    drawCarReflection();
    drawTrafficLight(1700, -300);
    drawTrafficLightReflection(1700, -300);
    if (scene == 0 || scene == 1)
    {
        drawBoat(boatX, -400);
        drawBoatReflection(boatX, -550);
    }
}

void rainyScenario()
{
    normalScenario();
    drawRain();
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);


    if (scene == 0)
    {
        glClearColor(0.53, 0.81, 0.98, 1.0);
        normalScenario();
        glutPostRedisplay();
    }
    else if (scene == 1)
    {
        glClearColor(0.53, 0.81, 0.98, 1.0);
        rainyScenario();
        glutPostRedisplay();
    }
    else if (scene == 2)
    {
        glClearColor(0.01, 0.01, 0.1, 1.0);
        normalScenario();
        glutPostRedisplay();
    }
    else if (scene == 3)
    {
        glClearColor(0.01, 0.01, 0.1, 1.0);
        rainyScenario();
        glutPostRedisplay();
    }

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
    if (key == 'r' || key == 'R')
    {
        trafficsignal = 0; // Red
    }
    else if (key == 'G' || key == 'g')
    {
        if (trafficsignal == 0)
            trafficsignal = 1; // Yellow
        glutTimerFunc(2000, switchToGreen, 0);
    }

    if (key == 'W' || key == 'w')
    {
        scene = 1; // Rainy Day
        updatedSceneSound();
    }
    else if (key == 'D' || key == 'd')
    {
        scene = 0; // Clear Day
        updatedSceneSound();

    }
    else if (key == 'N' || key == 'n')
    {
        scene = 2; // Night
        updatedSceneSound();
    }
    else if (key == 'M' || key == 'm')
    {
        scene = 3; // Rainy Night
        updatedSceneSound();
    }

    glutPostRedisplay();
}

void init(void)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1900, -600, 800, -1.0, 1.0);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1800, 900);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Town Scenario");

    generateStars();
    glutTimerFunc(16, update, 0);
    updatedSceneSound();
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMainLoop();

    return 0;
}