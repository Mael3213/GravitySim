#include <GL/freeglut.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* === Planètes === */


/* === FPS === */
static int frameCount = 0;
static int fps = 0;
static int lastFpsTime = 0;

/* ========= Caméra ========= */
static double camX = 0.0, camY = 1.6, camZ = 5.0;
static double yaw = -90.0, pitch = 0.0;
static double speed = 5.0;              // unités/sec
static double mouseSensitivity = 0.15;  // sensibilité souris

/* États des touches */
static bool moveForward = false;
static bool moveBackward = false;
static bool moveLeft = false;
static bool moveRight = false;
static bool moveUp = false;     // espace
static bool moveDown = false;   // shift

/* Fenêtre */
static int winWidth = 800, winHeight = 600;

/* Timer */
static int timerIntervalMs = 16;
static int lastTimeMs = 0;

static inline double deg2rad(double d) { return d * M_PI / 180.0; }

void drawText(const char *text, float x, float y) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, winWidth, 0, winHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glRasterPos2f(x, y);
    for (const char *c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

/* Direction */
static void computeDirection(double *dx, double *dy, double *dz) {
    double yawR = deg2rad(yaw);
    double pitchR = deg2rad(pitch);
    *dx = cos(yawR) * cos(pitchR);
    *dy = sin(pitchR);
    *dz = sin(yawR) * cos(pitchR);
}

/* Caméra */
static void updateViewMatrix() {
    double dx, dy, dz;
    computeDirection(&dx, &dy, &dz);
    double cx = camX + dx;
    double cy = camY + dy;
    double cz = camZ + dz;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camX, camY, camZ, cx, cy, cz, 0.0, 1.0, 0.0);
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    updateViewMatrix();

    glColor3f(0.0f, 0.5f, 1.0f); // couleur
    glPushMatrix();
    glTranslatef(2.0f, 1.0f, -5.0f); // position dans la scène
    glutSolidSphere(1.0, 32, 32); // sphère remplie, rayon 1, 32 slices/stacks
    glPopMatrix();



    // === compteur FPS ===
    frameCount++;
    int now = glutGet(GLUT_ELAPSED_TIME);
    if (now - lastFpsTime > 1000) {
        fps = frameCount * 1000 / (now - lastFpsTime);
        lastFpsTime = now;
        frameCount = 0;
    }

    // afficher FPS en haut à gauche
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "FPS: %d", fps);
    glColor3f(1, 1, 1); // blanc
    drawText(buffer, 10, winHeight - 20);

    glutSwapBuffers();
}

void keyDown(unsigned char key, int x, int y) {
    (void)x; (void)y;
    if (key == 27) exit(0); // ESC
    if (key == 'w' || key == 'z') moveForward = true;
    if (key == 's') moveBackward = true;
    if (key == 'a' || key == 'q') moveLeft = true;
    if (key == 'd') moveRight = true;
    if (key == 32) moveUp = true;       // espace
}

void keyUp(unsigned char key, int x, int y) {
    (void)x; (void)y;
    if (key == 'w' || key == 'z') moveForward = false;
    if (key == 's') moveBackward = false;
    if (key == 'a' || key == 'q') moveLeft = false;
    if (key == 'd') moveRight = false;
    if (key == 32) moveUp = false;      // espace
}

void specialDown(int key, int x, int y) {
    (void)x; (void)y;
    if (key == GLUT_KEY_UP) moveForward = true;
    if (key == GLUT_KEY_DOWN) moveBackward = true;
    if (key == GLUT_KEY_LEFT) moveLeft = true;
    if (key == GLUT_KEY_RIGHT) moveRight = true;
    if (key == GLUT_KEY_SHIFT_L || key == GLUT_KEY_SHIFT_R) moveDown = true;
}

void specialUp(int key, int x, int y) {
    (void)x; (void)y;
    if (key == GLUT_KEY_UP) moveForward = false;
    if (key == GLUT_KEY_DOWN) moveBackward = false;
    if (key == GLUT_KEY_LEFT) moveLeft = false;
    if (key == GLUT_KEY_RIGHT) moveRight = false;
    if (key == GLUT_KEY_SHIFT_L || key == GLUT_KEY_SHIFT_R) moveDown = false;
}

/* ===== Souris ===== */
static bool justWarped = false;
static const int WARP_IGNORE_RADIUS = 2;

void mousePassive(int x, int y) {
    int centerX = winWidth / 2;
    int centerY = winHeight / 2;
    int dx = x - centerX;
    int dy = y - centerY;

    if (justWarped) {
        if (abs(dx) <= WARP_IGNORE_RADIUS && abs(dy) <= WARP_IGNORE_RADIUS) {
            justWarped = false;
            return;
        }
        justWarped = false;
    }

    yaw   += dx * mouseSensitivity;
    pitch -= dy * mouseSensitivity;

    if (pitch > 89.0) pitch = 89.0;
    if (pitch < -89.0) pitch = -89.0;

    glutWarpPointer(centerX, centerY);
    justWarped = true;
}

/* ===== Update ===== */
void timerUpdate(int value) {
    (void)value;
    int now = glutGet(GLUT_ELAPSED_TIME);
    if (lastTimeMs == 0) lastTimeMs = now;
    int dt_ms = now - lastTimeMs;
    double dt = dt_ms / 1000.0;
    lastTimeMs = now;
    if (dt > 0.25) dt = 0.25;

    double dirX, dirY, dirZ;
    computeDirection(&dirX, &dirY, &dirZ);

    double rightX = -dirZ, rightY = 0.0, rightZ = dirX;
    double lenDir = sqrt(dirX*dirX + dirZ*dirZ);
    if (lenDir != 0.0) { dirX /= lenDir; dirZ /= lenDir; }
    double lenRight = sqrt(rightX*rightX + rightZ*rightZ);
    if (lenRight != 0.0) { rightX /= lenRight; rightZ /= lenRight; }

    double step = speed * dt;

    if (moveForward) { camX += dirX * step; camZ += dirZ * step; }
    if (moveBackward){ camX -= dirX * step; camZ -= dirZ * step; }
    if (moveLeft)    { camX -= rightX * step; camZ -= rightZ * step; }
    if (moveRight)   { camX += rightX * step; camZ += rightZ * step; }
    if (moveUp)      { camY += step; }
    if (moveDown)    { camY -= step; }


    glutPostRedisplay();
    glutTimerFunc(timerIntervalMs, timerUpdate, 0);
}

void sim(){
    return;
}

/* ===== Init ===== */
void initGL() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // fond noir
}
void reshape(int w, int h) {
    if (h == 0) h = 1;
    winWidth = w; winHeight = h;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0, (double)w/(double)h, 0.1, 200.0);
    glMatrixMode(GL_MODELVIEW);
}

/* ===== main ===== */
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(winWidth, winHeight);
    glutCreateWindow("FPS camera");

    initGL();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(specialDown);
    glutSpecialUpFunc(specialUp);

    glutPassiveMotionFunc(mousePassive);
    glutSetCursor(GLUT_CURSOR_NONE);
    glutWarpPointer(winWidth/2, winHeight/2);
    justWarped = true;

    lastTimeMs = glutGet(GLUT_ELAPSED_TIME);
    glutTimerFunc(timerIntervalMs, timerUpdate, 0);
    glutIdleFunc(sim);

    glutMainLoop();
    return 0;
}
