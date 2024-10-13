#include <GL/glew.h>
#include <GL/freeglut.h>

float rotationX = 0.0;
float rotationY = 0.0;

// 4x4-es kontrollpontok vízszintes felülethez (X-Z síkban)
GLfloat ctrlPoints[4][4][3] = {
    { {-1.5, 0.0, -1.5}, {-0.5, 0.0, -1.5}, {0.5, 0.0, -1.5}, {1.5, 0.0, -1.5} },
    { {-1.5, 0.0, -0.5}, {-0.5, 0.0, -0.5}, {0.5, 0.0, -0.5}, {1.5, 0.0, -0.5} },
    { {-1.5, 0.0, 0.5}, {-0.5, 0.0, 0.5}, {0.5, 0.0, 0.5}, {1.5, 0.0, 0.5} },
    { {-1.5, 0.0, 1.5}, {-0.5, 0.0, 1.5}, {0.5, 0.0, 1.5}, {1.5, 0.0, 1.5} }
};

void drawControlPoints()
{
    glPointSize(5.0);
    glColor3f(0.0, 0.0, 0.0);  // Fekete színű pontok
    glBegin(GL_POINTS);
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            glVertex3fv(ctrlPoints[i][j]);
        }
    }
    glEnd();
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

    glFlush();
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

void specialInput(int key, int x, int y)
{
    float angleStep = 5.0;
    switch (key)
    {
    case GLUT_KEY_RIGHT:
        rotationY += angleStep;
        break;
    case GLUT_KEY_LEFT:
        rotationY -= angleStep;
        break;
    case GLUT_KEY_UP:
        rotationX -= angleStep;
        break;
    case GLUT_KEY_DOWN:
        rotationX += angleStep;
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
    glutSpecialFunc(specialInput);

    glewExperimental = GL_TRUE;
    glewInit();

    setup();

    glutMainLoop();
}