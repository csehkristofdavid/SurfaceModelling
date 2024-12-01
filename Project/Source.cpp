#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cmath>

float rotationX = 0.0;
float rotationY = 0.0;
float rotationZ = 0.0;
int selectedPoint[2] = { -1, -1 }; // A kiválasztott pont indexei (-1, -1, ha nincs kiválasztva)
bool showBezierSurface = false; // Induláskor a Bézier-felület nincs kirajzolva


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




void drawScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Model view
    glLoadIdentity();

    // Cam pos
    gluLookAt(5.0, 5.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

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

    glFlush();
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
    }
    glutPostRedisplay();
}


int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitContextVersion(4, 3);
    glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Surface Modelling");

    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyboardInput);
    glutMouseFunc(mouseFunc);

    glewExperimental = GL_TRUE;
    glewInit();

    setup();

    glutMainLoop();
}