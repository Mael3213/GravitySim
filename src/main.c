#include <GL/freeglut.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <windows.h>
#include <time.h>


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif



/* === Planètes === */ //{x,y,z,vx,vy,vz,masse,r,g,b}
#define MAX_PLANETE 100000
double planete[MAX_PLANETE][10] ;
long nb_planete;

//= {{5,0,0,0,0,0,11,0,0,1},{-5,0,0,0,0,-1.25,10,0.5,1,0},{20,0,0,0,0,0,5,0,1,1}}

/* === timer === */
static double lastTime = 0.0;
double t;
double dt;

/* vistess de simulation */
float v_sim = 1;
float ans_v_sim;
long simCount = 0;
long sps = 0;
int lastSimTime = 0;

/* === gravité ===*/
float G = 4;

/* === FPS === */
static int frameCount = 0;
static int fps = 0;
static int lastFpsTime = 0;

/* ========= Caméra ========= */
static double camX = 0.0, camY = 20, camZ = 0;
static double yaw = -90.0, pitch = 0.0;
static double speed = 100.0;              // unités/sec
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
double dist_affichage = 10000.0;

/* Timer */
static int timerIntervalMs = 16;
static int lastTimeMs = 0;

static inline double deg2rad(double d) { return d * M_PI / 180.0; }

double getTimeSeconds() {
    static LARGE_INTEGER frequency;
    static BOOL initialized = FALSE;
    if (!initialized) {
        QueryPerformanceFrequency(&frequency);
        initialized = TRUE;
    }

    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / (double)frequency.QuadPart;
}


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
    /*  // affichage d'un repère
    glColor3f(1,0,0);
    // Dessine une ligne rouge
    glBegin(GL_LINES);
        glVertex3f(-10, 0, 0); 
        glVertex3f(10, 0, 0);
    glEnd();

    glColor3f(0,1,0);
    // Dessine une ligne verte
    glBegin(GL_LINES);
        glVertex3f(0,-10, 0); 
        glVertex3f(0, 10, 0);
    glEnd();

    glColor3f(0,0,1);
    // Dessine une ligne blue
    glBegin(GL_LINES);
        glVertex3f( 0, 0, -10); 
        glVertex3f( 0, 0, 10);
    glEnd();
    */

    for(int n = 0; n < nb_planete; n++){
        glColor3f(planete[n][7], planete[n][8], planete[n][9]); // couleur
        glPushMatrix();
        glTranslatef(planete[n][0], planete[n][1], planete[n][2]); // position dans la scène
        glutSolidSphere(sqrtf(planete[n][6]), 32, 32); // sphère remplie,
        glPopMatrix();
    }


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
    snprintf(buffer, sizeof(buffer), "FPS: %ld", fps);
    glColor3f(1, 1, 1); // blanc
    drawText(buffer, 10, winHeight - 20);

    snprintf(buffer, sizeof(buffer), "Sim/s: %ld", sps);
    glColor3f(1, 1, 1); // blanc
    drawText(buffer, 100, winHeight - 20);

    snprintf(buffer, sizeof(buffer), "Planetes: %ld", nb_planete);
    glColor3f(1, 1, 1); // blanc
    drawText(buffer, 210, winHeight - 20);

    snprintf(buffer, sizeof(buffer), "Vitesse: %.1f", v_sim);
    glColor3f(1, 1, 1); // blanc
    drawText(buffer, 320, winHeight - 20);



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
     switch (key) {
    case 38: v_sim = 0.5; break; // touche 1 (AZERTY)
    case 233: v_sim = 1.0; break; // touche 2
    case 34: v_sim = 2.0; break; // touche 3
    case 39: v_sim = 4.0; break; // touche 4
    case 40: v_sim = 8; break; // touche 5
    case 45: v_sim = 16; break; // touche 6
    case 232: v_sim = 32; break; // touche 7
    case 95: v_sim = 64; break; // touche 8
    case 231: v_sim = 128; break; // touche 9
    case 224: v_sim = 256; break; // touche 0
    }
    if (key == 'p') {// mettre en pause la simulation 
        if (v_sim != 0) {
            ans_v_sim = v_sim;
            v_sim = 0;
        }else{ 
            v_sim = ans_v_sim; 
        } 
    }
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
    if (key == GLUT_KEY_F11){
        glutFullScreenToggle();
    }
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

void fusion_planete(int p0,int p1){
    long N = nb_planete;
    double nouvelle_planete[10];
    double ancienne_planete[N][10];
    // Copie de toutes les planètes actuelles
    for (long n = 0; n < N; n++) {
        memcpy(ancienne_planete[n], planete[n], sizeof(planete[n]));
    }
    
        double masse_p0 = planete[p0][6];
        double masse_p1 = planete[p1][6];
        double masse_totale = masse_p0+masse_p1;
        nouvelle_planete[0]=(planete[p0][0]*masse_p0+planete[p1][0]*masse_p1)/masse_totale;
        nouvelle_planete[1]=(planete[p0][1]*masse_p0+planete[p1][1]*masse_p1)/masse_totale;
        nouvelle_planete[2]=(planete[p0][2]*masse_p0+planete[p1][2]*masse_p1)/masse_totale;
        nouvelle_planete[3]=(planete[p0][3]*masse_p0+planete[p1][3]*masse_p1)/masse_totale;
        nouvelle_planete[4]=(planete[p0][4]*masse_p0+planete[p1][4]*masse_p1)/masse_totale;
        nouvelle_planete[5]=(planete[p0][5]*masse_p0+planete[p1][5]*masse_p1)/masse_totale;
        nouvelle_planete[6]=masse_totale;
        nouvelle_planete[7]=(planete[p0][7]*masse_p0+planete[p1][7]*masse_p1)/masse_totale;
        nouvelle_planete[8]=(planete[p0][8]*masse_p0+planete[p1][8]*masse_p1)/masse_totale;
        nouvelle_planete[9]=(planete[p0][9]*masse_p0+planete[p1][9]*masse_p1)/masse_totale;

    // Réécriture du tableau des planètes
    long k = 0;
    for (long n = 0; n < N; n++) {
        if (n == p0 || n == p1) continue; // on saute les planètes fusionnées
        memcpy(planete[k], ancienne_planete[n], sizeof(planete[n]));
        k++;
    }

    // On ajoute la nouvelle planète fusionnée à la fin
    memcpy(planete[k], nouvelle_planete, sizeof(nouvelle_planete));
    nb_planete--;
}
void sim() {
    double time = getTimeSeconds();
    dt = time - t;
    t = time;
    simCount++;
    double now = glutGet(GLUT_ELAPSED_TIME);
    if (now - lastSimTime > 1000) {
        sps = simCount;
        lastSimTime = now;
        simCount = 0;
    }
    if (v_sim!=0){
    long N = nb_planete;
    for (long i = 0; i < N; i++) {
        for (long j = i + 1; j < N; j++) {
            float Fx = 0, Fy = 0, Fz = 0;

            float dx = planete[j][0] - planete[i][0];
            float dy = planete[j][1] - planete[i][1];
            float dz = planete[j][2] - planete[i][2];
            float dist2 = dx*dx + dy*dy + dz*dz;

            float dist = sqrtf(dist2);
            float F = G * planete[j][6]*planete[i][6]/dist2;

            Fx += F * dx / dist;
            Fy += F * dy / dist;
            Fz += F * dz / dist;

            planete[i][3] += Fx / planete[i][6] * dt * v_sim;
            planete[i][4] += Fy / planete[i][6] * dt * v_sim;
            planete[i][5] += Fz / planete[i][6] * dt * v_sim;

            planete[j][3] -= Fx / planete[j][6] * dt * v_sim;
            planete[j][4] -= Fy / planete[j][6] * dt * v_sim;
            planete[j][5] -= Fz / planete[j][6] * dt * v_sim;

            if (dist < sqrtf(planete[i][6])+sqrtf(planete[j][6])){
                fusion_planete(i,j);
                i = N;
                j = N;
            }
        }
    }for (long n = 0; n < nb_planete; n++) {
        planete[n][0] += planete[n][3] * dt * v_sim;
        planete[n][1] += planete[n][4] * dt * v_sim;
        planete[n][2] += planete[n][5] * dt * v_sim;
    }

}else{
    Sleep(1);
}
}

void rand_planete(){
    srand(time(NULL));
    nb_planete = rand() % 501;  // entre 0 et 500 planètes

    for (long a = 0; a < nb_planete; a++) {
        // position (x,y,z)
        for (int b = 0; b < 3; b++) {
            planete[a][b] = ((double)rand() / RAND_MAX) * 2000.0 - 1000.0;
        }

        // vitesse (vx,vy,vz)
        for (int b = 3; b < 6; b++) {
            planete[a][b] = ((double)rand() / RAND_MAX) * 5.0 - 2.5;
        }

        // masse
        planete[a][6] = 5.0 + ((double)rand() / RAND_MAX) * 15.0;

        // couleur RGB
        for (int b = 7; b < 10; b++) {
            planete[a][b] = ((double)rand() / RAND_MAX);
        }
    }
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    winWidth = w; winHeight = h;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0, (double)w/(double)h, 0.1, dist_affichage);
    glMatrixMode(GL_MODELVIEW);
}

/* ===== main ===== */
int main(int argc, char** argv) {
    if (planete[0]!=0){
        rand_planete();
    }
    t = getTimeSeconds();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(winWidth, winHeight);
    glutCreateWindow("GravitySim");

    glEnable(GL_DEPTH_TEST); glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // fond noir

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