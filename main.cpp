// ============================================================
//  WINDMILL FARM  +  STARS  +  RAIN  +  WEATHER HUD
//  ------------------------------------------------------------
//  CONTROLS:
//    LEFT-DRAG   : Rotate camera
//    Z / X       : Zoom IN / OUT
//    R           : Toggle RAIN
//    D / N       : Day / Night
//    S           : Toggle STARS (only visible at NIGHT)
//    A           : Toggle AIR FLOW (wind)
//    B           : Toggle birds
//    F           : Toggle flowers
//    G           : Toggle scattered grass
//    SPACE       : Pause / Resume
//    ESC         : Exit
// ============================================================

#include <windows.h>
#include <GL/glut.h>
#include <math.h>
#include <cstdio>
#include <cstring>

// ------------------------------------------------------------
//  GLOBAL STATE
// ------------------------------------------------------------
float cameraAngleX   = 30.0f;
float cameraAngleY   = 45.0f;
float cameraDistance = 65.0f;      // pulled back a bit for 30 windmills

int   lastMouseX = 0;
int   lastMouseY = 0;
bool  isMouseDragging = false;

float bladeRotation  = 0.0f;
float birdFlightTime = 0.0f;
float windTime       = 0.0f;
float starTwinkle    = 0.0f;
bool  isDay        = true;
bool  isPaused     = false;
bool  showBirds    = false;
bool  showFlowers  = false;
bool  showGrass    = false;
bool  showAirFlow  = false;
bool  showStars    = false;
bool  showRain     = false;

int   winWidth  = 1024;
int   winHeight = 768;

// ------------------------------------------------------------
//  FLOWER DATA
// ------------------------------------------------------------
const int NUM_FLOWERS = 500;
float flowerX[NUM_FLOWERS], flowerZ[NUM_FLOWERS], flowerH[NUM_FLOWERS];
float flowerC[NUM_FLOWERS][3];

// ------------------------------------------------------------
//  GRASS DATA
// ------------------------------------------------------------
const int NUM_GRASS = 800;
float grassX[NUM_GRASS], grassZ[NUM_GRASS], grassH[NUM_GRASS];

// ------------------------------------------------------------
//  WIND STREAMS
// ------------------------------------------------------------
const int NUM_WIND_LINES = 60;
float windY[NUM_WIND_LINES], windZ[NUM_WIND_LINES], windX[NUM_WIND_LINES];
float windSpeed[NUM_WIND_LINES], windLen[NUM_WIND_LINES];

// ------------------------------------------------------------
//  STARS DATA  (1000 stars)
// ------------------------------------------------------------
const int NUM_STARS = 1000;
float starX[NUM_STARS], starY[NUM_STARS], starZ[NUM_STARS], starSize[NUM_STARS];

// ------------------------------------------------------------
//  RAIN DATA  (600 drops)
// ------------------------------------------------------------
const int NUM_RAIN = 600;
float rainX[NUM_RAIN], rainY[NUM_RAIN], rainZ[NUM_RAIN];
float rainSpeed[NUM_RAIN], rainLen[NUM_RAIN];

// ------------------------------------------------------------
//  FORWARD DECLARATIONS
// ------------------------------------------------------------
void init();
void generateFlowers();
void generateGrass();
void generateWind();
void generateStars();
void generateRain();
void display();
void update(int value);
void handleKeyboard(unsigned char key, int x, int y);
void handleMouseClick(int button, int state, int x, int y);
void handleMouseMotion(int x, int y);
void handleReshape(int w, int h);
void drawGround();
void drawWindmill(float x, float y, float z, float scale);
void drawSky();
void drawSunOrMoon();
void drawStars();
void drawRain();
void drawBirds();
void drawOneBird(float x, float y, float z, float wingAngle);
void drawFlower(float x, float y, float z, float height,
                float pr, float pg, float pb);
void drawAllFlowers();
void drawOneGrassTuft(float x, float z, float height);
void drawAllGrass();
void drawAirFlow();
void drawHUD();
void drawText(float x, float y, const char* text, void* font);
void setupLights();

// ============================================================
//  MAIN
// ============================================================
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(winWidth, winHeight);
    glutInitWindowPosition(100, 60);
    glutCreateWindow("Windmill Farm - 30 Windmills + Weather + Rain + Stars");

    init();
    generateFlowers();
    generateGrass();
    generateWind();
    generateStars();
    generateRain();

    glutDisplayFunc(display);
    glutReshapeFunc(handleReshape);
    glutKeyboardFunc(handleKeyboard);
    glutMouseFunc(handleMouseClick);
    glutMotionFunc(handleMouseMotion);

    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}

// ============================================================
//  INIT
// ============================================================
void init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.55f, 0.75f, 1.0f, 1.0f);
}

// ============================================================
//  RANDOM DATA GENERATORS
// ============================================================
void generateFlowers()
{
    for (int i = 0; i < NUM_FLOWERS; i++) {
        flowerX[i] = -60.0f + 120.0f * fabsf(sinf(i * 12.9898f));
        flowerZ[i] = -60.0f + 120.0f * fabsf(sinf(i * 78.2330f));
        flowerH[i] = 0.5f + 0.5f * fabsf(sinf(i * 4.5678f));
        int c = i % 6;
        switch (c) {
            case 0: flowerC[i][0]=1.0f;  flowerC[i][1]=0.25f; flowerC[i][2]=0.25f; break;
            case 1: flowerC[i][0]=1.0f;  flowerC[i][1]=0.90f; flowerC[i][2]=0.15f; break;
            case 2: flowerC[i][0]=1.0f;  flowerC[i][1]=0.45f; flowerC[i][2]=0.70f; break;
            case 3: flowerC[i][0]=1.0f;  flowerC[i][1]=1.00f; flowerC[i][2]=1.00f; break;
            case 4: flowerC[i][0]=0.75f; flowerC[i][1]=0.40f; flowerC[i][2]=0.95f; break;
            case 5: flowerC[i][0]=1.0f;  flowerC[i][1]=0.55f; flowerC[i][2]=0.10f; break;
        }
    }
}

void generateGrass()
{
    for (int i = 0; i < NUM_GRASS; i++) {
        grassX[i] = -60.0f + 120.0f * fabsf(sinf(i * 23.1337f));
        grassZ[i] = -60.0f + 120.0f * fabsf(sinf(i * 91.7711f));
        grassH[i] = 0.3f + 0.5f * fabsf(sinf(i * 5.3121f));
    }
}

void generateWind()
{
    for (int i = 0; i < NUM_WIND_LINES; i++) {
        windY[i]     = 1.0f + 12.0f * fabsf(sinf(i * 3.123f));
        windZ[i]     = -50.0f + 100.0f * fabsf(sinf(i * 41.77f));
        windX[i]     = -60.0f + 120.0f * fabsf(sinf(i * 7.777f));
        windSpeed[i] = 0.4f + 0.8f * fabsf(sinf(i * 2.313f));
        windLen[i]   = 2.0f + 4.0f * fabsf(sinf(i * 9.191f));
    }
}

void generateStars()
{
    for (int i = 0; i < NUM_STARS; i++) {
        float theta = 2.0f * 3.14159f * fabsf(sinf(i * 1.234f));
        float phi   = 1.5708f * fabsf(sinf(i * 3.456f));
        float R     = 150.0f;

        starX[i] = R * cosf(phi) * cosf(theta);
        starY[i] = R * sinf(phi) + 5.0f;
        starZ[i] = R * cosf(phi) * sinf(theta);

        starSize[i] = 0.5f + 1.5f * fabsf(sinf(i * 7.77f));
    }
}

void generateRain()
{
    for (int i = 0; i < NUM_RAIN; i++) {
        rainX[i]     = -60.0f + 120.0f * fabsf(sinf(i * 13.31f));
        rainY[i]     = 5.0f + 25.0f * fabsf(sinf(i * 22.77f));
        rainZ[i]     = -60.0f + 120.0f * fabsf(sinf(i * 47.91f));
        rainSpeed[i] = 0.7f + 0.9f * fabsf(sinf(i * 3.91f));
        rainLen[i]   = 0.6f + 0.8f * fabsf(sinf(i * 8.13f));
    }
}

// ============================================================
//  LIGHTS
// ============================================================
void setupLights()
{
    GLfloat light0_pos[]     = { 10.0f, 15.0f, 10.0f, 1.0f };
    GLfloat light0_diffuse[] = { 1.0f, 0.98f, 0.90f, 1.0f };
    GLfloat light0_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light0_diffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light0_ambient);

    GLfloat light1_pos[]     = { -10.0f, 5.0f, -8.0f, 1.0f };
    GLfloat light1_diffuse[] = { 0.35f, 0.4f, 0.5f, 1.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  light1_diffuse);
}

// ============================================================
//  SKY
// ============================================================
void drawSky()
{
    if (isDay) glClearColor(0.55f, 0.75f, 1.0f, 1.0f);
    else       glClearColor(0.02f, 0.02f, 0.10f, 1.0f);
}

// ============================================================
//  SUN / MOON
// ============================================================
void drawSunOrMoon()
{
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    glTranslatef(35.0f, 28.0f, 0.0f);

    if (isDay) {
        glColor4f(1.0f, 0.9f, 0.3f, 0.35f);
        glutSolidSphere(4.5f, 24, 24);
        glColor4f(1.0f, 0.95f, 0.4f, 1.0f);
        glutSolidSphere(2.5f, 24, 24);
    } else {
        glColor4f(0.85f, 0.9f, 1.0f, 0.30f);
        glutSolidSphere(4.0f, 24, 24);
        glColor4f(0.95f, 0.95f, 0.98f, 1.0f);
        glutSolidSphere(2.2f, 24, 24);
    }
    glPopMatrix();

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

// ============================================================
//  GROUND
// ============================================================
void drawGround()
{
    if (isDay) glColor3f(0.25f, 0.55f, 0.20f);
    else       glColor3f(0.05f, 0.12f, 0.05f);

    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-80.0f, 0.0f, -80.0f);
        glVertex3f( 80.0f, 0.0f, -80.0f);
        glVertex3f( 80.0f, 0.0f,  80.0f);
        glVertex3f(-80.0f, 0.0f,  80.0f);
    glEnd();
}

// ============================================================
//  STARS
// ============================================================
void drawStars()
{
    if (isDay || !showStars) return;

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < NUM_STARS; i++)
    {
        float twinkle = 0.55f + 0.45f * sinf(starTwinkle * 3.0f + i * 0.7f);
        glPointSize(starSize[i]);
        glColor4f(1.0f, 1.0f, 1.0f, twinkle);

        glBegin(GL_POINTS);
            glVertex3f(starX[i], starY[i], starZ[i]);
        glEnd();
    }

    glPointSize(1.0f);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

// ============================================================
//  RAIN
// ============================================================
void drawRain()
{
    if (!showRain) return;

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1.5f);

    for (int i = 0; i < NUM_RAIN; i++)
    {
        float x = rainX[i];
        float y = rainY[i];
        float z = rainZ[i];
        float len = rainLen[i];

        glColor4f(0.65f, 0.80f, 1.00f, 0.75f);

        glBegin(GL_LINES);
            glVertex3f(x, y,       z);
            glVertex3f(x, y - len, z);
        glEnd();
    }

    glLineWidth(1.0f);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

// ============================================================
//  BIRDS
// ============================================================
void drawOneBird(float x, float y, float z, float wingAngle)
{
    glPushMatrix();
    glTranslatef(x, y, z);

    glColor3f(0.15f, 0.15f, 0.15f);
    glPushMatrix();
    glScalef(1.6f, 0.5f, 0.5f);
    glutSolidSphere(0.35, 12, 12);
    glPopMatrix();

    glColor3f(0.2f, 0.2f, 0.2f);
    glPushMatrix();
    glTranslatef(0.55f, 0.12f, 0.0f);
    glutSolidSphere(0.16, 10, 10);
    glPopMatrix();

    glColor3f(0.1f, 0.1f, 0.1f);
    glPushMatrix();
    glTranslatef(0.0f, 0.1f, 0.15f);
    glRotatef(wingAngle, 1.0f, 0.0f, 0.0f);
    glScalef(0.9f, 0.05f, 0.6f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 0.1f, -0.15f);
    glRotatef(-wingAngle, 1.0f, 0.0f, 0.0f);
    glScalef(0.9f, 0.05f, 0.6f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix();
}

void drawBirds()
{
    if (!showBirds) return;

    const int N = 10;
    float radius = 40.0f;
    float height = 16.0f;
    float wingAngle = 40.0f * sinf(birdFlightTime * 6.0f);

    for (int i = 0; i < N; i++) {
        float angle = birdFlightTime * 0.8f + (i * 2.0f * 3.14159f / N);
        float bx = radius * cosf(angle);
        float bz = radius * sinf(angle);
        float by = height + 0.8f * sinf(birdFlightTime * 2.0f + i);

        glPushMatrix();
        glTranslatef(bx, by, bz);
        glRotatef(-angle * 180.0f / 3.14159f - 90.0f, 0.0f, 1.0f, 0.0f);
        drawOneBird(0.0f, 0.0f, 0.0f, wingAngle);
        glPopMatrix();
    }
}

// ============================================================
//  FLOWERS
// ============================================================
void drawFlower(float x, float y, float z, float height,
                float pr, float pg, float pb)
{
    glPushMatrix();
    glTranslatef(x, y, z);

    glColor3f(0.15f, 0.6f, 0.15f);
    glPushMatrix();
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    GLUquadricObj* stem = gluNewQuadric();
    gluQuadricNormals(stem, GLU_SMOOTH);
    gluCylinder(stem, 0.025, 0.025, height, 6, 6);
    gluDeleteQuadric(stem);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, height, 0.0f);

    for (int i = 0; i < 5; i++) {
        glPushMatrix();
        glRotatef(i * 72.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(0.10f, 0.0f, 0.0f);
        glColor3f(pr, pg, pb);
        glPushMatrix();
        glScalef(1.0f, 0.25f, 0.55f);
        glutSolidSphere(0.10, 6, 6);
        glPopMatrix();
        glPopMatrix();
    }

    glColor3f(1.0f, 0.85f, 0.1f);
    glPushMatrix();
    glScalef(1.0f, 0.35f, 1.0f);
    glutSolidSphere(0.06, 6, 6);
    glPopMatrix();
    glPopMatrix();

    glPopMatrix();
}

void drawAllFlowers()
{
    if (!showFlowers) return;
    for (int i = 0; i < NUM_FLOWERS; i++)
        drawFlower(flowerX[i], 0.0f, flowerZ[i], flowerH[i],
                   flowerC[i][0], flowerC[i][1], flowerC[i][2]);
}

// ============================================================
//  GRASS
// ============================================================
void drawOneGrassTuft(float x, float z, float height)
{
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glColor3f(0.10f, 0.45f, 0.10f);

    for (int i = 0; i < 3; i++) {
        glPushMatrix();
        glRotatef(i * 40.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(15.0f * sinf(i * 1.7f), 1.0f, 0.0f, 0.0f);

        glBegin(GL_TRIANGLES);
            glNormal3f(0.0f, 0.0f, 1.0f);
            glVertex3f(-0.03f, 0.0f,   0.0f);
            glVertex3f( 0.03f, 0.0f,   0.0f);
            glVertex3f( 0.0f,  height, 0.0f);
        glEnd();
        glPopMatrix();
    }
    glPopMatrix();
}

void drawAllGrass()
{
    if (!showGrass) return;
    for (int i = 0; i < NUM_GRASS; i++)
        drawOneGrassTuft(grassX[i], grassZ[i], grassH[i]);
}

// ============================================================
//  AIR FLOW
// ============================================================
void drawAirFlow()
{
    if (!showAirFlow) return;

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(2.0f);

    for (int i = 0; i < NUM_WIND_LINES; i++) {
        float x = windX[i];
        float y = windY[i];
        float z = windZ[i];
        float len = windLen[i];

        float alpha = 0.55f - 0.03f * y;
        if (alpha < 0.10f) alpha = 0.10f;

        if (isDay) glColor4f(0.90f, 0.95f, 1.00f, alpha);
        else       glColor4f(0.60f, 0.75f, 0.95f, alpha * 0.85f);

        glBegin(GL_LINES);
            glVertex3f(x,       y, z);
            glVertex3f(x + len, y, z);
        glEnd();
    }

    glLineWidth(1.0f);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

// ============================================================
//  WINDMILL
// ============================================================
void drawWindmill(float x, float y, float z, float scale)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);

    glColor3f(0.5f, 0.5f, 0.5f);
    glPushMatrix();
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    GLUquadricObj* base = gluNewQuadric();
    gluQuadricNormals(base, GLU_SMOOTH);
    gluCylinder(base, 0.55, 0.55, 0.3, 20, 20);
    gluDeleteQuadric(base);
    glPopMatrix();

    glColor3f(0.85f, 0.85f, 0.88f);
    glPushMatrix();
    glTranslatef(0.0f, 0.3f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glutSolidCone(0.35f, 6.0f, 20, 20);
    glPopMatrix();

    glColor3f(0.55f, 0.55f, 0.6f);
    glPushMatrix();
    glTranslatef(0.0f, 6.4f, 0.0f);
    glutSolidSphere(0.45, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 6.4f, 0.0f);

    glColor3f(0.4f, 0.4f, 0.45f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.45f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glutSolidCone(0.25f, 0.4f, 16, 16);
    glPopMatrix();

    glRotatef(bladeRotation, 0.0f, 0.0f, 1.0f);

    glColor3f(0.97f, 0.97f, 0.97f);
    for (int i = 0; i < 3; i++) {
        glPushMatrix();
        glRotatef(i * 120.0f, 0.0f, 0.0f, 1.0f);

        glPushMatrix();
        glTranslatef(0.0f, 1.3f, 0.0f);
        glScalef(0.15f, 2.4f, 0.05f);
        glutSolidCube(1.0);
        glPopMatrix();
        glPopMatrix();
    }
    glPopMatrix();

    glPopMatrix();
}

// ============================================================
//  HUD
// ============================================================
void drawText(float x, float y, const char* text, void* font)
{
    glRasterPos2f(x, y);
    while (*text) glutBitmapCharacter(font, *text++);
}

void drawHUD()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, winWidth, 0, winHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.0f, 0.0f, 0.0f, 0.55f);
    glBegin(GL_QUADS);
        glVertex2f(15,  winHeight - 15);
        glVertex2f(320, winHeight - 15);
        glVertex2f(320, winHeight - 140);
        glVertex2f(15,  winHeight - 140);
    glEnd();

    char line1[64];
    char line2[64];
    char line3[64];

    sprintf(line1, "Time    : %s", isDay ? "DAY" : "NIGHT");

    if (showAirFlow) {
        float avgSpeed = 0.0f;
        for (int i = 0; i < NUM_WIND_LINES; i++) avgSpeed += windSpeed[i];
        avgSpeed /= NUM_WIND_LINES;
        float speedKmh = avgSpeed * 30.0f;
        sprintf(line2, "Wind    : ON  (%.0f km/h)", speedKmh);
    } else {
        sprintf(line2, "Wind    : OFF");
    }

    const char* weather = "Clear";
    if (showRain && !isDay)             weather = "Rainy Night";
    else if (showRain && isDay)         weather = "Rainy Day";
    else if (showStars && !isDay)       weather = "Starry Night";
    else if (!isDay)                    weather = "Clear Night";
    else                                weather = "Sunny Day";

    sprintf(line3, "Weather : %s", weather);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(30, winHeight - 40,  line1, GLUT_BITMAP_HELVETICA_18);
    drawText(30, winHeight - 70,  line2, GLUT_BITMAP_HELVETICA_18);
    drawText(30, winHeight - 100, line3, GLUT_BITMAP_HELVETICA_18);

    glColor3f(0.75f, 0.85f, 1.0f);
    drawText(30, winHeight - 125,
             "N=night  S=stars  A=wind  R=rain",
             GLUT_BITMAP_HELVETICA_12);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

// ============================================================
//  DISPLAY
// ============================================================
void display()
{
    drawSky();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)winWidth / winHeight, 0.5, 500.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float radX = cameraAngleX * 3.14159f / 180.0f;
    float radY = cameraAngleY * 3.14159f / 180.0f;

    float camX = cameraDistance * cosf(radX) * sinf(radY);
    float camY = cameraDistance * sinf(radX);
    float camZ = cameraDistance * cosf(radX) * cosf(radY);

    gluLookAt(camX, camY + 12.0f, camZ,
              0.0f, 4.0f, 0.0f,
              0.0f, 1.0f, 0.0f);

    setupLights();

    drawStars();

    drawGround();
    drawSunOrMoon();

    drawAllGrass();
    drawAllFlowers();

    // ============================================================
    //  30 WINDMILLS  —  6 columns × 5 rows
    //  x from -25 to +25  step 10  (6 columns)
    //  z from -20 to +20  step 10  (5 rows)
    // ============================================================
    for (int col = 0; col < 6; col++)
    {
        for (int row = 0; row < 5; row++)
        {
            float wx = -25.0f + col * 10.0f;
            float wz = -20.0f + row * 10.0f;
            drawWindmill(wx, 0.0f, wz, 1.0f);
        }
    }

    drawBirds();
    drawAirFlow();
    drawRain();

    drawHUD();

    glutSwapBuffers();
}

// ============================================================
//  UPDATE
// ============================================================
void update(int value)
{
    if (!isPaused) {
        bladeRotation += 2.5f;
        if (bladeRotation > 360.0f) bladeRotation -= 360.0f;

        birdFlightTime += 0.02f;
        if (birdFlightTime > 1000.0f) birdFlightTime = 0.0f;

        starTwinkle += 0.05f;
        if (starTwinkle > 1000.0f) starTwinkle = 0.0f;

        windTime += 0.02f;
        for (int i = 0; i < NUM_WIND_LINES; i++) {
            windX[i] += windSpeed[i];
            if (windX[i] > 65.0f) windX[i] = -65.0f - windLen[i];
        }

        for (int i = 0; i < NUM_RAIN; i++) {
            rainY[i] -= rainSpeed[i];
            if (rainY[i] < 0.0f) {
                rainY[i] = 25.0f + 10.0f * fabsf(sinf(i * 3.7f));
                rainX[i] = -60.0f + 120.0f * fabsf(sinf(i * 13.31f + starTwinkle));
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// ============================================================
//  KEYBOARD
// ============================================================
void handleKeyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27:  exit(0); break;

        case 'z': case 'Z':
            cameraDistance -= 2.5f;
            if (cameraDistance < 8.0f) cameraDistance = 8.0f;
            break;

        case 'x': case 'X':
            cameraDistance += 2.5f;
            if (cameraDistance > 150.0f) cameraDistance = 150.0f;
            break;

        case 'r': case 'R':
            showRain = !showRain;
            printf(">> Rain %s\n", showRain ? "FALLING" : "stopped");
            break;

        case 'd': case 'D':
            isDay = true;
            printf(">> Day mode\n");
            break;

        case 'n': case 'N':
            isDay = false;
            printf(">> Night mode - press S to show stars\n");
            break;

        case 's': case 'S':
            if (!isDay) {
                showStars = !showStars;
                printf(">> Stars %s\n", showStars ? "VISIBLE" : "hidden");
            } else {
                printf(">> Press N first (stars only appear at night)\n");
            }
            break;

        case 'a': case 'A':
            showAirFlow = !showAirFlow;
            printf(">> Wind %s\n", showAirFlow ? "ON" : "OFF");
            break;

        case 'b': case 'B':
            showBirds = !showBirds;
            break;

        case 'f': case 'F':
            showFlowers = !showFlowers;
            break;

        case 'g': case 'G':
            showGrass = !showGrass;
            break;

        case ' ':
            isPaused = !isPaused;
            break;
    }
    glutPostRedisplay();
}

// ============================================================
//  MOUSE
// ============================================================
void handleMouseClick(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            isMouseDragging = true;
            lastMouseX = x;  lastMouseY = y;
        } else if (state == GLUT_UP) {
            isMouseDragging = false;
        }
    }
    else if (button == 3) { cameraDistance -= 2.0f; if (cameraDistance < 8.0f) cameraDistance = 8.0f; }
    else if (button == 4) { cameraDistance += 2.0f; if (cameraDistance > 150.0f) cameraDistance = 150.0f; }
}

void handleMouseMotion(int x, int y)
{
    if (isMouseDragging) {
        cameraAngleY += (x - lastMouseX) * 0.5f;
        cameraAngleX += (y - lastMouseY) * 0.5f;

        if (cameraAngleX >  85.0f) cameraAngleX =  85.0f;
        if (cameraAngleX <   5.0f) cameraAngleX =   5.0f;

        lastMouseX = x;  lastMouseY = y;
        glutPostRedisplay();
    }
}

// ============================================================
//  RESHAPE
// ============================================================
void handleReshape(int w, int h)
{
    if (h == 0) h = 1;
    winWidth  = w;
    winHeight = h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w / h, 0.5f, 500.0f);
    glMatrixMode(GL_MODELVIEW);
}
