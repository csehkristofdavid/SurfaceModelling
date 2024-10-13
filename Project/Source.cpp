#include <GL/glew.h>
#include <GL/freeglut.h>

float rotationX = 0.0;
float rotationY = 0.0;

void drawScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT);

    // model view
    glLoadIdentity();

    // cam pos
    gluLookAt(5.0, 5.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // rotate
    glRotatef(rotationX, 1.0, 0.0, 0.0);
    glRotatef(rotationY, 0.0, 1.0, 0.0);

    // draw
    glBegin(GL_LINES);

    // X
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(3.0, 0.0, 0.0);

    // Y
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 3.0, 0.0);

    // Z
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 3.0);

    glEnd();

    glFlush();
}

// init
void setup(void)
{
    glClearColor(1.0, 1.0, 1.0, 0.0);
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
    glutPostRedisplay(); // display again
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    glutInitContextVersion(4, 3);
    glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);

    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);

    glutCreateWindow("Surface Modelling");

    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    glutSpecialFunc(specialInput); // Nyílbillentyûk kezeléséhez.

    glewExperimental = GL_TRUE;
    glewInit();

    setup();

    glutMainLoop();
}
