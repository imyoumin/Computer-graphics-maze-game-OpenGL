#include <GL/glut.h>
#include <stdio.h>
#include <string>  // �߰��� �κ�

typedef struct {
    float x;
    float y;
    float z;
} Point;

typedef struct {
    unsigned int ip[3];
} Face;

int pnum;
int fnum;
Point* mpoint = NULL;
Face* mface = NULL;

std::string fname = "..\\source\\myModel2.dat";  // ����� �κ�

// ReadModel �Լ��� ���Ͽ��� �� �����͸� �о�� �޸𸮿� �����մϴ�.
void ReadModel() {
    FILE* f1;  // ���� ������
    char s[81];  // ���ڿ��� �б� ���� ����
    int i;

    // ������ �޸� �Ҵ� ����
    if (mpoint != NULL) delete[] mpoint;
    if (mface != NULL) delete[] mface;

    // ���� ����. ���� �� ���� �޽��� ��� �� �Լ� ����
    if ((f1 = fopen(fname.c_str(), "rt")) == NULL) {
        printf("No file\n");
        exit(0);
    }

    // ���Ͽ��� ����(Vertex) ���� �б�
    fscanf(f1, "%s", s); // Read and print the first string
    fscanf(f1, "%s", s); // Read and print the second string
    fscanf(f1, "%d", &pnum);

    // ���� ������ ���� �Ҵ� �� �б�
    mpoint = new Point[pnum];
    for (i = 0; i < pnum; i++) {
        fscanf(f1, "%f", &mpoint[i].x);
        fscanf(f1, "%f", &mpoint[i].y);
        fscanf(f1, "%f", &mpoint[i].z);
    }

    // ���Ͽ��� ��(Face) ���� �б�
    fscanf(f1, "%s", s); // Read and print the third string
    fscanf(f1, "%s", s); // Read and print the fourth string
    fscanf(f1, "%d", &fnum);
    // �� ������ ���� �Ҵ� �� �б�
    mface = new Face[fnum];
    for (i = 0; i < fnum; i++) {
        fscanf(f1, "%d", &mface[i].ip[0]);
        fscanf(f1, "%d", &mface[i].ip[1]);
        fscanf(f1, "%d", &mface[i].ip[2]);
    }

    // ���� �ݱ�
    fclose(f1);
}



// InitLight �Լ��� ����� ���� �Ӽ��� �����մϴ�.
void InitLight() {
    // ���� �Ӽ� ����
    GLfloat mat_diffuse[] = { 0.5, 0.4, 0.3, 1.0 };
    GLfloat mat_specular[] = { 0.3, 0.3, 0.3, 1.0 };
    GLfloat mat_ambient[] = { 0.5, 0.4, 0.3, 1.0 };
    GLfloat mat_shininess[] = { 2.0 };

    // ���� �Ӽ� ����
    GLfloat light_specular[] = { 0.5, 0.5, 0.5, 0.5 };
    GLfloat light_diffuse[] = { 0.5, 0.5, 0.5, 0.5 };
    GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 0.5 };
    GLfloat light_position[] = { 300.0, 300.0, 300.0, 1.0 };

    // ����� ���� �Ӽ� ����
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}



// cnormal �Լ��� �־��� �� ���� ����� ���� ���� ���͸� ����մϴ�.
Point cnormal(Point a, Point b, Point c) {
    Point p, q, r;
    double val;

    // ���� p, q ���
    p.x = a.x - b.x; p.y = a.y - b.y; p.z = a.z - b.z;
    q.x = c.x - b.x; q.y = c.y - b.y; q.z = c.z - b.z;

    // ���� ������ ����� ���� ���� r ���
    r.x = p.y * q.z - p.z * q.y;
    r.y = p.z * q.x - p.x * q.z;
    r.z = p.x * q.y - p.y * q.x;

    // ���� ����ȭ
    val = sqrt(r.x * r.x + r.y * r.y + r.z * r.z);
    r.x = r.x / val; r.y = r.y / val;
    r.z = r.z / val;
    return r;
}


// DrawModelAtOffset �Լ��� ���� �׸��� �Լ��Դϴ�.
void DrawModelAtOffset(void) {
    glTranslatef(0.0f, 0.0f, 0.0f);  // �� ��ġ �̵�
    glScalef(0.5f, 0.5f, 0.5f);      // �� ũ�� ����
    glColor3f(0, 0, 1);              // ���� ����

    glBegin(GL_TRIANGLES);  // �ﰢ������ ���� �׸�
    for (int i = 0; i < fnum; i++) {
        Point norm = cnormal(mpoint[mface[i].ip[2]], mpoint[mface[i].ip[1]], mpoint[mface[i].ip[0]]);
        glNormal3f(norm.x, norm.y, norm.z);  // ���� ���� ����
        glVertex3f(mpoint[mface[i].ip[0]].x, mpoint[mface[i].ip[0]].y, mpoint[mface[i].ip[0]].z);
        glVertex3f(mpoint[mface[i].ip[1]].x, mpoint[mface[i].ip[1]].y, mpoint[mface[i].ip[1]].z);
        glVertex3f(mpoint[mface[i].ip[2]].x, mpoint[mface[i].ip[2]].y, mpoint[mface[i].ip[2]].z);
    }
    glEnd();  // �ﰢ�� �׸��� ����
}



// GLSetupRC �Լ��� OpenGL ������ ���ؽ�Ʈ�� �����մϴ�.
void GLSetupRC(void) {
    glEnable(GL_DEPTH_TEST);  // ���� �׽�Ʈ Ȱ��ȭ

    // ���� ��Ʈ���� ����
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0, 1.0, 1.0, 2000.0);

    // �𵨺� ��Ʈ���� ����
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(500.0, 400.0, 500.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);  // ī�޶� ��ġ �� ���� ����
}


void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // ��ȯ ����� �ٽ� ����
    glPushMatrix();

    // �̵��� ��ġ�� Ƽ�� �׸���
    DrawModelAtOffset();

    // ��ȯ ����� �ʱ� ���·� �ǵ�����
    glPopMatrix();

    glutSwapBuffers();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Push/Pop Matrix Example");
    glutDisplayFunc(display);

    ReadModel();
    GLSetupRC();
    InitLight();

    glutMainLoop();
    return 0;
}