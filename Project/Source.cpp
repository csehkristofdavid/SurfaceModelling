#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cmath>
#include <vector>
#include <iostream>

float rotationX = 0.0;
float rotationY = 0.0;
float rotationZ = 0.0;
int selectedPoint[2] = { -1, -1 }; // A kiválasztott pont indexei (-1, -1, ha nincs kiválasztva)
bool showBezierSurface = false; // Induláskor a Bézier-felület nincs kirajzolva
bool showBSplineSurface = false;
bool showNURBSSurface = false;
float zoom = 1.0f;
int windowWidth = 800;
int windowHeight = 600;


// 4x4-es kontrollpontok vízszintes felülethez (X-Z síkban)
GLfloat ctrlPoints[4][4][3] = {
    { {-1.5, 0.0, -1.5}, {-0.5, 0.0, -1.5}, {0.5, 0.0, -1.5}, {1.5, 0.0, -1.5} },
    { {-1.5, 0.0, -0.5}, {-0.5, 0.5, -0.5}, {0.5, 0.5, -0.5}, {1.5, 0.0, -0.5} },
    { {-1.5, 0.0, 0.5}, {-0.5, 0.5, 0.5}, {0.5, 0.5, 0.5}, {1.5, 0.0, 0.5} },
    { {-1.5, 0.0, 1.5}, {-0.5, 0.0, 1.5}, {0.5, 0.0, 1.5}, {1.5, 0.0, 1.5} }
};


void drawCircle(GLfloat x, GLfloat y, GLfloat z, GLfloat radius)
{
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++)
    {
        float angle = 2.0f * 3.1415926f * float(i) / float(100);
        float dx = radius * cosf(angle);
        float dz = radius * sinf(angle);
        glVertex3f(x + dx, y, z + dz);
    }
    glEnd();
}

void drawControlPoints()
{
    glColor3f(0.0, 0.0, 0.0); // Fekete színű körök
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            drawCircle(ctrlPoints[i][j][0], ctrlPoints[i][j][1], ctrlPoints[i][j][2], 0.05);
        }
    }

    // Kiemeljük a kiválasztott pontot, ha van.
    if (selectedPoint[0] != -1 && selectedPoint[1] != -1)
    {
        glColor3f(1.0, 0.0, 0.0); // Piros színű kör a kiválasztott pont körül
        drawCircle(ctrlPoints[selectedPoint[0]][selectedPoint[1]][0],
            ctrlPoints[selectedPoint[0]][selectedPoint[1]][1],
            ctrlPoints[selectedPoint[0]][selectedPoint[1]][2], 0.1);
    }
}

void drawControlLines()
{
    glColor3f(0.0, 0.0, 0.0);  // Fekete színű vonalak

    // Sorok összekötése
    for (int i = 0; i < 4; ++i)
    {
        glBegin(GL_LINE_STRIP);
        for (int j = 0; j < 4; ++j)
        {
            glVertex3fv(ctrlPoints[i][j]);
        }
        glEnd();
    }

    // Oszlopok összekötése
    for (int j = 0; j < 4; ++j)
    {
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < 4; ++i)
        {
            glVertex3fv(ctrlPoints[i][j]);
        }
        glEnd();
    }
}

// Faktoriális függvény
int factorial(int n) {
    int result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

// Bernstein polinom kiszámítása
float bernstein(int i, int n, float t) {
    float binomialCoeff = 1.0;
    for (int j = 1; j <= i; ++j) {
        binomialCoeff *= (n - j + 1) / (float)j;
    }
    return binomialCoeff * pow(t, i) * pow(1 - t, n - i);
}

void drawBezierSurfaceWithLines() {
    int numDivisions = 20; // Felület felbontása
    float step = 1.0f / (float)(numDivisions - 1);

    glColor3f(0.5f, 0.5f, 0.5f); // Világos kék szín

    // Vízszintes vonalak rajzolása (u paraméter mentén)
    for (int i = 0; i < numDivisions; ++i) {
        float u = i * step;

        glBegin(GL_LINE_STRIP);
        for (int j = 0; j < numDivisions; ++j) {
            float v = j * step;
            GLfloat point[3] = { 0.0f, 0.0f, 0.0f };

            for (int k = 0; k < 4; ++k) {
                for (int l = 0; l < 4; ++l) {
                    float bU = bernstein(k, 3, u);
                    float bV = bernstein(l, 3, v);
                    point[0] += ctrlPoints[k][l][0] * bU * bV;
                    point[1] += ctrlPoints[k][l][1] * bU * bV;
                    point[2] += ctrlPoints[k][l][2] * bU * bV;
                }
            }

            glVertex3fv(point);
        }
        glEnd();
    }

    // Függőleges vonalak rajzolása (v paraméter mentén)
    for (int j = 0; j < numDivisions; ++j) {
        float v = j * step;

        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < numDivisions; ++i) {
            float u = i * step;
            GLfloat point[3] = { 0.0f, 0.0f, 0.0f };

            for (int k = 0; k < 4; ++k) {
                for (int l = 0; l < 4; ++l) {
                    float bU = bernstein(k, 3, u);
                    float bV = bernstein(l, 3, v);
                    point[0] += ctrlPoints[k][l][0] * bU * bV;
                    point[1] += ctrlPoints[k][l][1] * bU * bV;
                    point[2] += ctrlPoints[k][l][2] * bU * bV;
                }
            }

            glVertex3fv(point);
        }
        glEnd();
    }
}
float bsplineBasis(int i, int k, float t, const std::vector<float>& knots) {
    // B-spline alaptényező kiszámítása a Cox-de Boor képlettel
    if (k == 0) {
        return (knots[i] <= t && t < knots[i + 1]) ? 1.0f : 0.0f;
    }
    else {
        float denom1 = knots[i + k] - knots[i];
        float denom2 = knots[i + k + 1] - knots[i + 1];

        // Első tag számítása (recurszív)
        float term1 = (denom1 != 0) ? ((t - knots[i]) / denom1) * bsplineBasis(i, k - 1, t, knots) : 0.0f;

        // Második tag számítása (recurszív)
        float term2 = (denom2 != 0) ? ((knots[i + k + 1] - t) / denom2) * bsplineBasis(i + 1, k - 1, t, knots) : 0.0f;

        return term1 + term2;
    }
}

void drawBSplineSurfaceWithLines() {
    int numDivisions = 20; // Felület felbontása
    float step = 1.0f / (float)(numDivisions - 1);

    // 4x4-es kontrollpont hálóhoz megfelelő csomóponti vektor
    std::vector<float> knots = { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f };

    glColor3f(0.5f, 0.5f, 0.5f); // Szín beállítása

    // Vízszintes vonalak rajzolása (u paraméter mentén)
    for (int i = 0; i < numDivisions; ++i) {
        float u = i * step;

        glBegin(GL_LINE_STRIP);
        for (int j = 0; j < numDivisions-1; ++j) {
            float v = j * step;
            GLfloat point[3] = { 0.0f, 0.0f, 0.0f };

            for (int m = 0; m < 4; ++m) { // Kontrollpontok u irányban
                for (int n = 0; n < 4; ++n) { // Kontrollpontok v irányban
                    float bU = bsplineBasis(m, 3, u, knots); // Bspline alaptényező u-ra
                    float bV = bsplineBasis(n, 3, v, knots); // Bspline alaptényező v-re
                    point[0] += ctrlPoints[m][n][0] * bU * bV;
                    point[1] += ctrlPoints[m][n][1] * bU * bV;
                    point[2] += ctrlPoints[m][n][2] * bU * bV;
                }
            }

            glVertex3fv(point);
        }
        glEnd();
    }

    // Függőleges vonalak rajzolása (v paraméter mentén)
    for (int j = 0; j < numDivisions; ++j) {
        float v = j * step;

        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < numDivisions-1; ++i) {
            float u = i * step;
            GLfloat point[3] = { 0.0f, 0.0f, 0.0f };

            for (int m = 0; m < 4; ++m) { // Kontrollpontok u irányban
                for (int n = 0; n < 4; ++n) { // Kontrollpontok v irányban
                    float bU = bsplineBasis(m, 3, u, knots); // Bspline alaptényező u-ra
                    float bV = bsplineBasis(n, 3, v, knots); // Bspline alaptényező v-re
                    point[0] += ctrlPoints[m][n][0] * bU * bV;
                    point[1] += ctrlPoints[m][n][1] * bU * bV;
                    point[2] += ctrlPoints[m][n][2] * bU * bV;
                }
            }

            glVertex3fv(point);
        }
        glEnd();
    }
}


// Súlyok a kontrollpontokhoz
GLfloat weights[4][4] = {
    {1.0, 1.0, 1.0, 1.0},
    {1.0, 2.0, 2.0, 1.0},
    {1.0, 2.0, 2.0, 1.0},
    {1.0, 1.0, 1.0, 1.0}
};

// NURBS alaptényező (kiszámítja a NURBS alaptényezőt a megadott paraméterekkel)
float nurbsBasis(int i, int k, float t, std::vector<float>& knots) {
    if (k == 0) {
        return (knots[i] <= t && t < knots[i + 1]) ? 1.0f : 0.0f;
    }
    else {
        float coeff1 = 0.0f;
        float coeff2 = 0.0f;

        if (knots[i + k] != knots[i]) {
            coeff1 = (t - knots[i]) / (knots[i + k] - knots[i]) * nurbsBasis(i, k - 1, t, knots);
        }
        if (knots[i + k + 1] != knots[i + 1]) {
            coeff2 = (knots[i + k + 1] - t) / (knots[i + k + 1] - knots[i + 1]) * nurbsBasis(i + 1, k - 1, t, knots);
        }

        return coeff1 + coeff2;
    }
}

void drawNURBSSurface() {
    int numDivisions = 20; // Felület felbontása
    float step = 1.0f / (float)(numDivisions - 1);

    // Csomóponti vektor (példa: 4-es fokú NURBS esetén)
    std::vector<float> knots = { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f };

    glColor3f(0.5f, 0.5f, 0.5f); // Szín beállítása

    // Vízszintes vonalak rajzolása (u paraméter mentén)
    for (int i = 0; i < numDivisions; ++i) {
        float u = i * step;

        glBegin(GL_LINE_STRIP);
        for (int j = 0; j < numDivisions-1; ++j) {
            float v = j * step;
            GLfloat point[3] = { 0.0f, 0.0f, 0.0f };
            float sumWeights = 0.0f;

            for (int m = 0; m < 4; ++m) {
                for (int n = 0; n < 4; ++n) {
                    float bU = nurbsBasis(m, 3, u, knots);
                    float bV = nurbsBasis(n, 3, v, knots);
                    float weight = weights[m][n];
                    sumWeights += weight * bU * bV;
                    point[0] += ctrlPoints[m][n][0] * weight * bU * bV;
                    point[1] += ctrlPoints[m][n][1] * weight * bU * bV;
                    point[2] += ctrlPoints[m][n][2] * weight * bU * bV;
                }
            }

            if (sumWeights > 0.0f) {
                point[0] /= sumWeights;
                point[1] /= sumWeights;
                point[2] /= sumWeights;
            }

            glVertex3fv(point);
        }
        glEnd();
    }

    // Függőleges vonalak rajzolása (v paraméter mentén)
    for (int j = 0; j < numDivisions; ++j) {
        float v = j * step;

        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < numDivisions-1; ++i) {
            float u = i * step;
            GLfloat point[3] = { 0.0f, 0.0f, 0.0f };
            float sumWeights = 0.0f;

            for (int m = 0; m < 4; ++m) {
                for (int n = 0; n < 4; ++n) {
                    float bU = nurbsBasis(m, 3, u, knots);
                    float bV = nurbsBasis(n, 3, v, knots);
                    float weight = weights[m][n];
                    sumWeights += weight * bU * bV;
                    point[0] += ctrlPoints[m][n][0] * weight * bU * bV;
                    point[1] += ctrlPoints[m][n][1] * weight * bU * bV;
                    point[2] += ctrlPoints[m][n][2] * weight * bU * bV;
                }
            }

            if (sumWeights > 0.0f) {
                point[0] /= sumWeights;
                point[1] /= sumWeights;
                point[2] /= sumWeights;
            }

            glVertex3fv(point);
        }
        glEnd();
    }
}


void drawScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Model view
    glLoadIdentity();

    // Cam pos
   // gluLookAt(5.0, 5.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // Kamera pozíció frissítése zoom értékkel
    float cameraDistance = 5.0f * zoom;
    gluLookAt(cameraDistance, cameraDistance, cameraDistance,
        0.0, 0.0, 0.0,
        0.0, 1.0, 0.0);

    // Rotate
    glRotatef(rotationX, 1.0, 0.0, 0.0);
    glRotatef(rotationY, 0.0, 1.0, 0.0);
    glRotatef(rotationZ, 0.0, 0.0, 1.0);

    // Draw axes
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0); // X tengely
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(3.0, 0.0, 0.0);
    glColor3f(0.0, 1.0, 0.0); // Y tengely
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 3.0, 0.0);
    glColor3f(0.0, 0.0, 1.0); // Z tengely
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 3.0);
    glEnd();

    // Kontrollpontok és vonalak rajzolása
    drawControlPoints();
    drawControlLines();
    
    if (showBezierSurface) {
        drawBezierSurfaceWithLines(); // Bézier-felület kirajzolása
    }

    if (showBSplineSurface) {
        drawBSplineSurfaceWithLines();
    }

    if (showNURBSSurface) {
        drawNURBSSurface();
    }
    

    glFlush();
}

void setupCamera() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Perspektíva frissítése
    gluPerspective(45.0f, (float)windowWidth / windowHeight, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}



// Egérkattintás kezelése
void mouseFunc(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        // Képernyő koordinátákból OpenGL koordinátákra való átalakítás
        GLint viewport[4];
        GLdouble modelview[16];
        GLdouble projection[16];
        GLfloat winX, winY, winZ;
        GLdouble posX, posY, posZ;

        glGetIntegerv(GL_VIEWPORT, viewport);
        glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
        glGetDoublev(GL_PROJECTION_MATRIX, projection);

        winX = (float)x;
        winY = (float)viewport[3] - (float)y;
        glReadPixels(x, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
        gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

        // Kiválasztott pont keresése
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                float dx = posX - ctrlPoints[i][j][0];
                float dy = posY - ctrlPoints[i][j][1];
                float dz = posZ - ctrlPoints[i][j][2];
                float distance = sqrt(dx * dx + dy * dy + dz * dz);

                if (distance < 0.1) // Ha elég közel van a pont
                {
                    selectedPoint[0] = i;
                    selectedPoint[1] = j;
                    glutPostRedisplay();
                    return;
                }
            }
        }

        // Ha nem kattintottunk pont közelébe
        selectedPoint[0] = -1;
        selectedPoint[1] = -1;
        glutPostRedisplay();
    }
}

// Inicializációs rutin
void setup(void)
{
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_FLAT);
}

void resize(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / (float)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboardInput(unsigned char key, int x, int y)
{
    float angleStep = 5.0;
    switch (key)
    {
    case 'x': // Kis x -> X tengely körüli forgatás negatívan
        rotationX -= angleStep;
        break;
    case 'X': // Nagy X -> X tengely körüli forgatás pozitívan
        rotationX += angleStep;
        break;
    case 'y': // Kis y -> Y tengely körüli forgatás negatívan
        rotationY -= angleStep;
        break;
    case 'Y': // Nagy Y -> Y tengely körüli forgatás pozitívan
        rotationY += angleStep;
        break;
    case 'z': // Kis z -> Z tengely körüli forgatás negatívan
        rotationZ -= angleStep;
        break;
    case 'Z': // Nagy Z -> Z tengely körüli forgatás pozitívan
        rotationZ += angleStep;
        break;
    case 'b': // 'b' billentyű -> Bézier-felület ki/be kapcsolása
        showBezierSurface = !showBezierSurface;
        break;
    case 's': // B-spline felület kapcsoló
        showBSplineSurface = !showBSplineSurface;
        break;
    case 'n': // 'n' billentyű -> NURBS-felület ki/be kapcsolása
        showNURBSSurface = !showNURBSSurface;
        break;
    case '1': // Mozgás pozitív x irányba
        ctrlPoints[selectedPoint[0]][selectedPoint[1]][0] += 0.1f;
        break;
    case '2': // Mozgás negatív x irányba
        ctrlPoints[selectedPoint[0]][selectedPoint[1]][0] -= 0.1f;
        break;
    case '3': // Mozgás pozitív y irányba
        ctrlPoints[selectedPoint[0]][selectedPoint[1]][1] += 0.1f;
        break;
    case '4': // Mozgás negatív y irányba
        ctrlPoints[selectedPoint[0]][selectedPoint[1]][1] -= 0.1f;
        break;
    case '5': // Mozgás pozitív z irányba
        ctrlPoints[selectedPoint[0]][selectedPoint[1]][2] += 0.1f;
        break;
    case '6': // Mozgás negatív z irányba
        ctrlPoints[selectedPoint[0]][selectedPoint[1]][2] -= 0.1f;
        break;
    case '+':  // Ha a '+' jelet nyomjuk meg
        zoom = std::min(zoom + 0.05f, 2.0f); // Növeljük a zoomot
        break;
    case '-':  // Ha a '-' jelet nyomjuk meg
        zoom = std::max(zoom - 0.05f, 0.5f); // Csökkentjük a zoomot
        break;
    }
    setupCamera();
    glutPostRedisplay();
}


int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitContextVersion(4, 3);
    glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Surface Modelling");

    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyboardInput);
    glutMouseFunc(mouseFunc);
    setupCamera();
    //glutMouseWheelFunc(mouseWheel);

    glewExperimental = GL_TRUE;
    glewInit();

    setup();
    //glutMouseFunc(mouseWheel);

    glutMainLoop();
}
