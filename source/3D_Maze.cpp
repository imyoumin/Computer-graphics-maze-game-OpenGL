#include <GL/glut.h>
#include <Windows.h>
#include <mmsystem.h>
#include <cmath>
#include <stdio.h>
#include <iostream>
#include <filesystem>
#include <string>


float cameraX = 0.0f; //�ʱ� ī�޶� ��ġ x��ǥ
float cameraY = 1.0f; //�ʱ� ī�޶� ��ġ y��ǥ
float cameraZ = 5.0f; //�ʱ� ī�޶� ��ġ z��ǥ

float cameraAngleY = 0.0f; //�ʱ� ī�޶� y����
float cameraAngleX = 0.0f; //�ʱ� ī�޶� x����

float cameraSpeed = 0.005f; //ī�޶� ������ �ӵ� ����
float rotateSpeed = 0.05f; //ī�޶� ȸ�� �ӵ� ����

bool isMenuDisplayed = false;

int lastMouseX = -1;
int lastMouseY = -1;

GLuint textureIDs[9]; // �ؽ�ó ID �迭
GLubyte* images[9];   // �� �鿡 ���� �̹��� �迭

int width, height;

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

typedef struct {
    float x;
    float y;
    float z;
} Point2;

typedef struct {
    unsigned int ip[3];
} Face2;

int pnum2;
int fnum2;
Point2* m2point = NULL;
Face2* m2face = NULL;

GLfloat MyVertices[8][3] = {  //������ü ���� ����
    {-1.5f, -1.5f, 0.5f},
    {1.5f, -1.5f, 0.5f},
    {1.5f, 1.5f, 0.5f},
    {-1.5f, 1.5f, 0.5f},
    {-1.5f, -1.5f, -0.5f},
    {1.5f, -1.5f, -0.5f},
    {1.5f, 1.5f, -0.5f},
    {-1.5f, 1.5f, -0.5f}
};


GLfloat MyColors[8][3] = {   //������ü ���� ����
    { 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0 }
};

GLfloat MyTextureCoordinates[4][2] = {   //������ü �ؽ��� ��ǥ
    { 0.0, 1.0 },
    { 1.0, 1.0 },
    { 1.0, 0.0 }
};

GLubyte MyVertexList[24] = {   //������ü ��
    0, 3, 2, 1,
    2, 3, 7, 6,
    0, 4, 7, 3,
    1, 2, 6, 5,
    4, 5, 6, 7,
    0, 1, 5, 4
};

bool moveForward = false;
bool moveBackward = false;
bool moveLeft = false;
bool moveRight = false;
bool isWalking = false;
bool jumping = false;

int WallListID, FloorListID, CeilingListID;

float cubeY = 2.5;
float cubeU = 0.01;

// ReadModel �Լ��� ���Ͽ��� 3D �� �����͸� �о���� �Լ��Դϴ�.
void ReadModel()
{
    // �� ������ ��θ� ���ڿ��� �����մϴ�.
    std::string fname = "..\\source\\myModel.dat";

    // ���� �����͸� �����մϴ�. �̰��� ������ �б� ���� �ڵ� ������ �մϴ�.
    FILE* f1;
    // �ӽ� ���ڿ� ���۸� �����մϴ�. ���Ϸκ��� ���� ���ڿ��� �����ϴ� �� ���˴ϴ�.
    char s[81];
    // �ݺ������� ����� �ε��� ������ �����մϴ�.
    int i;

    // �̹� �Ҵ�� �޸𸮰� �ִٸ� �����մϴ�. �̴� ������ ���� �����͸� Ŭ�����ϴ� �뵵�Դϴ�.
    if (mpoint != NULL) delete[] mpoint;
    if (mface != NULL) delete[] mface;

    // ������ ����, ������ ������ �ʾҴٸ� ���� �޽����� ����ϰ� ���α׷��� �����մϴ�.
    if ((f1 = fopen(fname.c_str(), "rt")) == NULL) {
        printf("No file\n");
        exit(0);
    }

    // ���Ͽ��� ���ڿ��� �� �� �о�ͼ� s ������ �����մϴ�. �� �κ��� ���� ���Ŀ� ���� �ٸ� �� �ֽ��ϴ�.
    fscanf(f1, "%s", s); // ù ��° ���ڿ� �б�
    fscanf(f1, "%s", s); // �� ��° ���ڿ� �б�

    // ���Ͽ��� ������ ������ �о�� pnum ������ �����մϴ�.
    fscanf(f1, "%d", &pnum);

    // ���� �����͸� ������ �迭�� �������� �Ҵ��մϴ�.
    mpoint = new Point[pnum];
    // ��� ������ ���� �ݺ��ϸ鼭 x, y, z ��ǥ�� ���Ͽ��� �о�ɴϴ�.
    for (i = 0; i < pnum; i++) {
        fscanf(f1, "%f", &mpoint[i].x);
        fscanf(f1, "%f", &mpoint[i].y);
        fscanf(f1, "%f", &mpoint[i].z);
    }

    // ���Ͽ��� ���� ������ �о�� fnum ������ �����մϴ�.
    fscanf(f1, "%s", s); // Read and print the third string
    fscanf(f1, "%s", s); // Read and print the fourth string
    fscanf(f1, "%d", &fnum);

    // �� �����͸� ������ �迭�� �������� �Ҵ��մϴ�.
    mface = new Face[fnum];
    // ��� �鿡 ���� �ݺ��ϸ鼭 �� ���� �����ϴ� �� ���� ���� �ε����� ���Ͽ��� �о�ɴϴ�.
    for (i = 0; i < fnum; i++) {
        fscanf(f1, "%d", &mface[i].ip[0]);
        fscanf(f1, "%d", &mface[i].ip[1]);
        fscanf(f1, "%d", &mface[i].ip[2]);
    }

    // ������ �ݽ��ϴ�.
    fclose(f1);
}


// ReadModel2 �Լ��� �� �ٸ� 3D �� �����͸� ���Ͽ��� �о���� �Լ��Դϴ�.
void ReadModel2()
{
    // �� ������ ��θ� ���ڿ��� �����մϴ�.
    std::string f2name = "..\\source\\myModel2.dat";

    // ���� �����͸� �����մϴ�. �̰��� ������ �б� ���� �ڵ� ������ �մϴ�.
    FILE* f2;
    // �ӽ� ���ڿ� ���۸� �����մϴ�. ���Ϸκ��� ���� ���ڿ��� �����ϴ� �� ���˴ϴ�.
    char d[81];
    // �ݺ������� ����� �ε��� ������ �����մϴ�.
    int j;

    // �̹� �Ҵ�� �޸𸮰� �ִٸ� �����մϴ�. �̴� ������ ���� �����͸� Ŭ�����ϴ� �뵵�Դϴ�.
    if (m2point != NULL) delete[] m2point;
    if (m2face != NULL) delete[] m2face;

    // ������ ����, ������ ������ �ʾҴٸ� ���� �޽����� ����ϰ� ���α׷��� �����մϴ�.
    if ((f2 = fopen(f2name.c_str(), "rt")) == NULL) {
        printf("No file\n");
        exit(0);
    }

    // ���Ͽ��� ���ڿ��� �� �� �о�ͼ� d ������ �����մϴ�. �� �κ��� ���� ���Ŀ� ���� �ٸ� �� �ֽ��ϴ�.
    fscanf(f2, "%s", d); // ù ��° ���ڿ� �б�
    fscanf(f2, "%s", d); // �� ��° ���ڿ� �б�

    // ���Ͽ��� ������ ������ �о�� pnum2 ������ �����մϴ�.
    fscanf(f2, "%d", &pnum2);

    // ���� �����͸� ������ �迭�� �������� �Ҵ��մϴ�.
    m2point = new Point2[pnum2];
    // ��� ������ ���� �ݺ��ϸ鼭 x, y, z ��ǥ�� ���Ͽ��� �о�ɴϴ�.
    for (j = 0; j < pnum2; j++) {
        fscanf(f2, "%f", &m2point[j].x);
        fscanf(f2, "%f", &m2point[j].y);
        fscanf(f2, "%f", &m2point[j].z);
    }

    // ���Ͽ��� ���� ������ �о�� fnum2 ������ �����մϴ�.
    fscanf(f2, "%s", d); // Read and print the third string
    fscanf(f2, "%s", d); // Read and print the fourth string
    fscanf(f2, "%d", &fnum2);

    // �� �����͸� ������ �迭�� �������� �Ҵ��մϴ�.
    m2face = new Face2[fnum2];
    // ��� �鿡 ���� �ݺ��ϸ鼭 �� ���� �����ϴ� �� ���� ���� �ε����� ���Ͽ��� �о�ɴϴ�.
    for (j = 0; j < fnum2; j++) {
        fscanf(f2, "%d", &m2face[j].ip[0]);
        fscanf(f2, "%d", &m2face[j].ip[1]);
        fscanf(f2, "%d", &m2face[j].ip[2]);
    }

    // ������ �ݽ��ϴ�.
    fclose(f2);
}



/*
void InitLight() {

    GLfloat light_specular[] = { 0.00000000005, 0.00000000005, 0.00000000005, 0.5 };
    GLfloat light_diffuse[] = { 0.005, 0.005, 0.005, 0.005 };
    GLfloat light_ambient[] = { 0.002, 0.002, 0.002, 0.005 };
    GLfloat light_position[] = { 100000000000000.0, 10000000000000.0, 1000000000.0, 0.0 };
    glShadeModel(GL_SMOOTH); glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
}
*/



/*
Point cnormal(Point a, Point b, Point c) {
    Point p, q, r;
    double val;
    p.x = a.x - b.x; p.y = a.y - b.y; p.z = a.z - b.z;
    q.x = c.x - b.x; q.y = c.y - b.y; q.z = c.z - b.z;
    r.x = p.y * q.z - p.z * q.y;
    r.y = p.z * q.x - p.x * q.z;
    r.z = p.x * q.y - p.y * q.x;
    val = sqrt(r.x * r.x + r.y * r.y + r.z * r.z);
    r.x = r.x / val; r.y = r.y / val; r.z = r.z / val;
    return r;
}
*/


// DrawModelAtOffset �Լ��� ����� 3D �� �����͸� ȭ�鿡 �׸��� �Լ��Դϴ�.
void DrawModelAtOffset(void)
{
    // ���� �̵���Ű�� �Լ��Դϴ�. ���⼭�� (2.0f, -0.2f, 3.2f)��ŭ �̵��մϴ�.
    // �̴� X, Y, Z ���� ���� ���� �̵���ŵ�ϴ�.
    glTranslatef(2.0f, -0.2f, 3.2f);

    // ���� ũ�⸦ �����ϴ� �Լ��Դϴ�. ���⼭�� �� �࿡ ���� 0.003��� ����մϴ�.
    glScalef(0.003f, 0.003f, 0.003f);

    /*
    // ���� ���� �Ӽ��� �����ϴ� �κ��Դϴ�. 
    GLfloat model_diffuse[] = { 0.5, 0.4, 0.3, 1.0 };
    GLfloat model_specular[] = { 0.1, 0.1, 0.1, 1.0 };
    GLfloat model_ambient[] = { 0.5, 0.4, 0.3, 1.0 };
    GLfloat model_shininess[] = { 500.0 };

    glMaterialfv(GL_FRONT, GL_DIFFUSE, model_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, model_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, model_ambient);
    glMaterialfv(GL_FRONT, GL_SHININESS, model_shininess);
    */

    // ���� ������ �����մϴ�. ���⼭�� (0.7, 0.2, 0.3) RGB ������ ����մϴ�.
    glColor3f(0.7, 0.2, 0.3);

    // ���̵� ���� �����մϴ�. GL_SMOOTH�� �ε巯�� �׶��̼� ���̵��� �ǹ��մϴ�.
    glShadeModel(GL_SMOOTH);

    // �ﰢ���� �׸��� �����մϴ�. �̴� ���� ���� �����ϴ� �⺻ �����Դϴ�.
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < fnum; i++) {
        // �� ���� �����ϴ� �������� �׸��ϴ�. mpoint �迭���� ������ ��ǥ�� �����ͼ� ����մϴ�.
        glVertex3f(mpoint[mface[i].ip[0]].x, mpoint[mface[i].ip[0]].y, mpoint[mface[i].ip[0]].z);
        glVertex3f(mpoint[mface[i].ip[1]].x, mpoint[mface[i].ip[1]].y, mpoint[mface[i].ip[1]].z);
        glVertex3f(mpoint[mface[i].ip[2]].x, mpoint[mface[i].ip[2]].y, mpoint[mface[i].ip[2]].z);
    }
    // �ﰢ�� �׸��⸦ �����մϴ�.
    glEnd();
}


// DrawModelAtOffset2 �Լ��� �� �ٸ� 3D �� �����͸� ȭ�鿡 �׸��� �Լ��Դϴ�.
void DrawModelAtOffset2(void)
{
    // ���� �̵���Ű�� �Լ��Դϴ�. ���⼭�� (-5.0f, 0.0f, 3.0f)��ŭ �̵��մϴ�.
    // �̴� X, Y, Z ���� ���� ���� �̵���ŵ�ϴ�.
    glTranslatef(-5.0f, -0.0f, 3.0f);

    // ���� ũ�⸦ �����ϴ� �Լ��Դϴ�. ���⼭�� �� �࿡ ���� 0.003��� ����մϴ�.
    glScalef(0.003f, 0.003f, 0.003f);

    /*
    // ���� ���� �Ӽ��� �����ϴ� �κ��Դϴ�. ���⼭�� �ּ� ó���Ǿ� �־� ������� �ʽ��ϴ�.
    GLfloat model_diffuse[] = { 0.5, 0.4, 0.3, 1.0 };
    GLfloat model_specular[] = { 0.1, 0.1, 0.1, 1.0 };
    GLfloat model_ambient[] = { 0.5, 0.4, 0.3, 1.0 };
    GLfloat model_shininess[] = { 500.0 };

    glMaterialfv(GL_FRONT, GL_DIFFUSE, model_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, model_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, model_ambient);
    glMaterialfv(GL_FRONT, GL_SHININESS, model_shininess);
    */

    // ���� ������ �����մϴ�. ���⼭�� (0.0, 0.7, 0.3) RGB ������ ����մϴ�.
    glColor3f(0.0, 0.7, 0.3);

    // ���̵� ���� �����մϴ�. GL_SMOOTH�� �ε巯�� �׶��̼� ���̵��� �ǹ��մϴ�.
    glShadeModel(GL_SMOOTH);

    // �ﰢ���� �׸��� �����մϴ�. �̴� ���� ���� �����ϴ� �⺻ �����Դϴ�.
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < fnum2; i++) {
        // �� ���� �����ϴ� �������� �׸��ϴ�. m2point �迭���� ������ ��ǥ�� �����ͼ� ����մϴ�.
        glVertex3f(m2point[m2face[i].ip[0]].x, m2point[m2face[i].ip[0]].y, m2point[m2face[i].ip[0]].z);
        glVertex3f(m2point[m2face[i].ip[1]].x, m2point[m2face[i].ip[1]].y, m2point[m2face[i].ip[1]].z);
        glVertex3f(m2point[m2face[i].ip[2]].x, m2point[m2face[i].ip[2]].y, m2point[m2face[i].ip[2]].z);
    }
    // �ﰢ�� �׸��⸦ �����մϴ�.
    glEnd();
}


// init �Լ��� OpenGL �׷��Ƚ� ���α׷��� �ʱ� ������ ���� �Լ��Դϴ�.
void init() {
    // glClearColor �Լ��� ȭ���� ���� �� ����� ������ �����մϴ�.
    // ���⼭ ������ ������ ������(0.0f, 0.0f, 0.0f)�̸�, ���� ���� 1.0f�Դϴ�.
    // �� ������ glClear �Լ��� ȣ���� �� �� ���۸� ���� �� ���˴ϴ�.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // glEnable �Լ��� ����Ͽ� ���� �׽�Ʈ�� Ȱ��ȭ�մϴ�.
    // ���� �׽�Ʈ�� 3D �׷��Ƚ����� ��ü�� �ٸ� ��ü �ڿ� �������� �� �� �̸� ó���ϴ� ����Դϴ�.
    // GL_DEPTH_TEST�� ��ü�� ����(��, Z ��)�� ����Ͽ� ��ü�� ���ü��� �����ϴ� �� ���˴ϴ�.
    glEnable(GL_DEPTH_TEST);
}


// JumpSoundFunction �Լ��� ���� �Ҹ��� ����ϰų� �ߴ��մϴ�.
void JumpSoundFunction(bool start) {
    // ���� �Ҹ� ������ ����Դϴ�.
    LPCWSTR soundFilePath = L"../source/sounds/jump.wav";

    if (start) {
        // start�� true�� ���, ���� �Ҹ��� �񵿱������� ����մϴ�.
        // SND_ASYNC�� �Ҹ� ����� �񵿱������� �����ϸ�, SND_FILENAME�� ���� �̸��� ����Ѵٴ� ���� �ǹ��մϴ�.
        sndPlaySound(soundFilePath, SND_ASYNC | SND_FILENAME);
    }
    else {
        // start�� false�� ���, ���� ������� �Ҹ��� �ߴ��մϴ�.
        sndPlaySound(NULL, SND_ASYNC);
    }
}

// PlaySoundFunction �Լ��� �ȴ� �Ҹ��� ����ϰų� �ߴ��մϴ�.
void PlaySoundFunction(bool start) {
    // �ȴ� �Ҹ� ������ ����Դϴ�.
    LPCWSTR soundFilePath = L"../source/sounds/walking.wav";

    if (start) {
        // start�� true�� ���, �ȴ� �Ҹ��� �񵿱������� ����ϸ�, �̸� �ݺ��մϴ�.
        // SND_LOOP�� �Ҹ��� �ݺ� ����ϵ��� �����մϴ�.
        sndPlaySound(soundFilePath, SND_ASYNC | SND_FILENAME | SND_LOOP);
    }
    else {
        // start�� false�� ���, ���� ������� �Ҹ��� �ߴ��մϴ�.
        sndPlaySound(NULL, SND_ASYNC);
    }
}

// BackgroundSoundFunction �Լ��� ��������� ����ϰų� �ߴ��մϴ�.
void BackgroundSoundFunction(bool start) {
    // ������� ������ ����Դϴ�.
    LPCWSTR soundFilePath = L"../source/sounds/background_sound.wav";

    if (start) {
        // ������ �� �Ҹ��� �񵿱������� ����ϸ�, �̸� �ݺ��մϴ�.
        // SND_LOOP�� �Ҹ��� �ݺ� ����ϵ��� �����մϴ�.
        sndPlaySound(soundFilePath, SND_ASYNC | SND_FILENAME | SND_LOOP);
    }
}


// reshape �Լ��� â ũ�Ⱑ ����� �� ȣ��Ǿ� �׷��Ƚ� ����Ʈ�� ���� ��Ʈ������ �����մϴ�.
void reshape(int width, int height) {
    // glViewport �Լ��� ȭ�鿡 �׷��� ������ ũ��� ��ġ�� �����մϴ�.
    // ���⼭�� ��ü â ũ�⸦ ����Ʈ�� �����մϴ�.
    glViewport(0, 0, width, height);

    // glMatrixMode �Լ��� ���� �۾��� ��Ʈ���� ��带 �����մϴ�.
    // GL_PROJECTION�� ���� ��Ʈ����(ī�޶� ��) ������ ���� ����Դϴ�.
    glMatrixMode(GL_PROJECTION);

    // glLoadIdentity �Լ��� ���õ� ��Ʈ���� ����� ��Ʈ������ ���� ��ķ� �ʱ�ȭ�մϴ�.
    // �̴� ������ ����� ��ȯ�� �ʱ�ȭ�ϴ� ������ �մϴ�.
    glLoadIdentity();

    // gluPerspective �Լ��� ���� ���� ����� �����մϴ�.
    // �� �Լ��� �þ߰�(fov), ��Ⱦ��(aspect ratio), ���� ���(near plane), ���Ÿ� ���(far plane)�� �Ű������� �޽��ϴ�.
    gluPerspective(45.0f, (float)width / height, 0.1f, 100.0f);

    // �ٽ� �𵨺� ��Ʈ���� ���� ��ȯ�մϴ�. �� ��忡���� ��ü�� ��ȯ�� ó���մϴ�.
    glMatrixMode(GL_MODELVIEW);
}



// LoadBMP �Լ��� BMP ������ �̹��� ������ �о�ͼ� OpenGL���� ����� �� �ֵ��� �޸𸮿� �ε��մϴ�.
void LoadBMP(const char* filename, GLubyte** imagePtr) {
    // ������ ���� �б� ���� ���ϴ�.
    FILE* file = fopen(filename, "rb");

    // BMP ������ ����� ������ �迭�Դϴ�. BMP ����� 54����Ʈ�� �����˴ϴ�.
    unsigned char header[54];
    // ���Ͽ��� 54����Ʈ�� �о�� ��� �迭�� �����մϴ�.
    fread(header, sizeof(unsigned char), 54, file);

    // ������� �̹����� �ʺ�� ���̸� �����մϴ�.
    // �ʺ�� ����� 18��° ����Ʈ���� �����ϸ�, ���̴� 22��° ����Ʈ���� �����մϴ�.
    width = *(int*)&header[18];
    height = *(int*)&header[22];

    // �̹����� �� ����Ʈ ũ�⸦ ����մϴ�. BMP ������ �ȼ� �� 3����Ʈ(RGB)�� ����մϴ�.
    int imageSize = width * height * 3;

    // �޸𸮿� �̹��� �����͸� ������ ������ �Ҵ��մϴ�.
    *imagePtr = (GLubyte*)malloc(imageSize);

    // ���Ͽ��� �̹��� �����͸� �о�� �Ҵ�� �޸𸮿� �����մϴ�.
    fread(*imagePtr, sizeof(GLubyte), imageSize, file);

    // ������ �ݽ��ϴ�.
    fclose(file);
}


// MakeTextures �Լ��� ���� �ؽ�ó�� �����ϰ� �����ϴ� �Լ��Դϴ�.
void MakeTextures() {
    // glGenTextures �Լ��� ����Ͽ� �ؽ�ó ID�� �����մϴ�.
    // ���⼭�� 8���� �ؽ�ó ID�� �����մϴ�.
    glGenTextures(8, textureIDs);

    // �ؽ�ó ������ ��θ� ��� �ִ� ���ڿ� �迭�Դϴ�.
    const char* filenames[9] = {
        "../source/textures/dungeon_wall3.bmp", //�� �̹���1
        "../source/textures/dungeon_wall3.bmp", //�� �̹���1
        "../source/textures/dungeon_wall2.bmp", //�� �̹���1
        "../source/textures/dungeon_wall1.bmp", //�� �̹���1
        "../source/textures/dungeon_wall3.bmp", //�� �̹���1
        "../source/textures/dungeon_wall3.bmp", //�� �̹���1
        "../source/textures/dungeon_floor.bmp", //�ٴ� �̹���1
        "../source/textures/dungeon_floor.bmp", //õ�� �̹���1
        "../source/textures/title.bmp" 
    };

    // �� �ؽ�ó�� ���� �ݺ��մϴ�.
    for (int i = 0; i < 9; ++i) {
        // glBindTexture �Լ��� ����Ͽ� ���� �۾��� �ؽ�ó�� ���ε��մϴ�.
        glBindTexture(GL_TEXTURE_2D, textureIDs[i]);

        // LoadBMP �Լ��� ����Ͽ� BMP ���Ϸκ��� �ؽ�ó �̹����� �ε��մϴ�.
        LoadBMP(filenames[i], &images[i]);

        // glTexImage2D �Լ��� ����Ͽ� �ε�� �̹��� �����͸� OpenGL �ؽ�ó�� �����մϴ�.
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, images[i]);

        // �ؽ�ó ���͸� �ɼ��� �����մϴ�. ���⼭�� ���� ���͸��� ����մϴ�.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
}


// MakeWallList �Լ��� ���� �ؽ�ó�� ���� �����͸� �����ϴ� ���÷��� ����Ʈ�� �����մϴ�.
void MakeWallList() {
    // glGenLists �Լ��� ����Ͽ� ���ο� ���÷��� ����Ʈ�� �����մϴ�.
    WallListID = glGenLists(1);

    // glNewList �Լ��� �� ����Ʈ�� ���Ǹ� �����մϴ�.
    // GL_COMPILE�� �� ����Ʈ�� ������ ���� �������� �ǹ��մϴ�.
    glNewList(WallListID, GL_COMPILE);

    // ����, ����, �ؽ�ó ��ǥ �迭�� ����� �� �ֵ��� �����մϴ�.
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    // �ؽ�ó ������ Ȱ��ȭ�մϴ�.
    glEnable(GL_TEXTURE_2D);

    // ������ �׸� ���� ���� ������ �ð� ����(Clockwise)���� �����մϴ�.
    glFrontFace(GL_CW);

    // ���� �� �鿡 ���� �ݺ��մϴ�.
    for (GLint i = 0; i < 6; i++) {
        // glBindTexture �Լ��� ���� ����� �ؽ�ó�� ���ε��մϴ�.
        glBindTexture(GL_TEXTURE_2D, textureIDs[i]);

        // glBegin �Լ��� ����(�簢��) �׸��⸦ �����մϴ�.
        glBegin(GL_QUADS);

        // ���� ���� ������ �����մϴ�.
        glColor3fv(MyColors[i]);

        // ���� �����ϴ� �� ������ ���� �ݺ��մϴ�.
        for (GLint j = 0; j < 4; j++) {
            // ���� ������ �ؽ�ó ��ǥ�� �����մϴ�.
            glTexCoord2fv(MyTextureCoordinates[j]);

            // ���� ������ ��ǥ�� �����մϴ�.
            glVertex3fv(MyVertices[MyVertexList[i * 4 + j]]);
        }

        // �簢�� �׸��⸦ �����մϴ�.
        glEnd();
    }

    // �ؽ�ó ������ ��Ȱ��ȭ�մϴ�.
    glDisable(GL_TEXTURE_2D);

    // ����, ����, �ؽ�ó ��ǥ �迭 ����� �����մϴ�.
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    // ���÷��� ����Ʈ ���Ǹ� �����մϴ�.
    glEndList();
}


// MakeFloorList �Լ��� �ٴ��� �ؽ�ó�� ���� �����͸� �����ϴ� ���÷��� ����Ʈ�� �����մϴ�.
void MakeFloorList() {
    // glGenLists �Լ��� ����Ͽ� ���ο� ���÷��� ����Ʈ�� �����մϴ�.
    FloorListID = glGenLists(1);

    // glNewList �Լ��� �� ����Ʈ�� ���Ǹ� �����մϴ�.
    // GL_COMPILE�� �� ����Ʈ�� ������ ���� �������� �ǹ��մϴ�.
    glNewList(FloorListID, GL_COMPILE);

    // glBindTexture �Լ��� ���� ����� �ؽ�ó�� ���ε��մϴ�.
    // ���⼭�� �ٴ� �ؽ�ó(7��° �ؽ�ó)�� ����մϴ�.
    glBindTexture(GL_TEXTURE_2D, textureIDs[6]);

    // �ؽ�ó ������ Ȱ��ȭ�մϴ�.
    glEnable(GL_TEXTURE_2D);

    // glColor3f �Լ��� ���� ������ ������� �����մϴ�.
    // �̴� �ؽ�ó�� ������ �״�� ����ϱ� �����Դϴ�.
    glColor3f(1.0f, 1.0f, 1.0f);

    // glBegin �Լ��� ����(�簢��) �׸��⸦ �����մϴ�.
    glBegin(GL_QUADS);

    // �ٴ��� �� ������ ���� �ؽ�ó ��ǥ�� ���� ��ǥ�� �����մϴ�.
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-3.5f, 0.0f, -3.5f);

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-3.5f, 0.0f, 3.5f);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(3.5f, 0.0f, 3.5f);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(3.5f, 0.0f, -3.5f);

    // �簢�� �׸��⸦ �����մϴ�.
    glEnd();

    // �ؽ�ó ������ ��Ȱ��ȭ�մϴ�.
    glDisable(GL_TEXTURE_2D);

    // ���÷��� ����Ʈ ���Ǹ� �����մϴ�.
    glEndList();
}


// MakeCeilingList �Լ��� õ���� �ؽ�ó�� ���� �����͸� �����ϴ� ���÷��� ����Ʈ�� �����մϴ�.
void MakeCeilingList() {
    // glGenLists �Լ��� ����Ͽ� ���ο� ���÷��� ����Ʈ�� �����մϴ�.
    CeilingListID = glGenLists(1);

    // glNewList �Լ��� �� ����Ʈ�� ���Ǹ� �����մϴ�.
    // GL_COMPILE�� �� ����Ʈ�� ������ ���� �������� �ǹ��մϴ�.
    glNewList(CeilingListID, GL_COMPILE);

    // glBindTexture �Լ��� ���� ����� �ؽ�ó�� ���ε��մϴ�.
    // ���⼭�� õ�� �ؽ�ó(8��° �ؽ�ó)�� ����մϴ�.
    glBindTexture(GL_TEXTURE_2D, textureIDs[7]);

    // �ؽ�ó ������ Ȱ��ȭ�մϴ�.
    glEnable(GL_TEXTURE_2D);

    // glColor3f �Լ��� ���� ������ ������� �����մϴ�.
    // �̴� �ؽ�ó�� ������ �״�� ����ϱ� �����Դϴ�.
    glColor3f(1.0f, 1.0f, 1.0f);

    // glBegin �Լ��� ����(�簢��) �׸��⸦ �����մϴ�.
    glBegin(GL_QUADS);

    // õ���� �� ������ ���� �ؽ�ó ��ǥ�� ���� ��ǥ�� �����մϴ�.
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-3.5f, 0.0f, -3.5f);

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-3.5f, 0.0f, 3.5f);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(3.5f, 0.0f, 3.5f);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(3.5f, 0.0f, -3.5f);

    // �簢�� �׸��⸦ �����մϴ�.
    glEnd();

    // �ؽ�ó ������ ��Ȱ��ȭ�մϴ�.
    glDisable(GL_TEXTURE_2D);

    // ���÷��� ����Ʈ ���Ǹ� �����մϴ�.
    glEndList();
}


    // updateCamera �Լ��� ī�޶��� �� ��Ʈ������ ������Ʈ�ϴ� �Լ��Դϴ�.
void updateCamera() {
        // glMatrixMode �Լ��� ���� �۾��� ��Ʈ���� ��带 �����մϴ�.
        // GL_MODELVIEW ���� ī�޶� �� �� �� ��ȯ�� ���� ����Դϴ�.
        glMatrixMode(GL_MODELVIEW);

        // glLoadIdentity �Լ��� ���� ��Ʈ������ ���� ��ķ� �ʱ�ȭ�մϴ�.
        glLoadIdentity();

        // ī�޶� �ٶ󺸴� ��ġ�� ����մϴ�.
        // �� ��ġ�� ī�޶��� ���� ��ġ�� ����(����)�� ����մϴ�.
        float lookAtX = cameraX + 5.0f * sin(+cameraAngleY) * cos(cameraAngleX);
        float lookAtY = cameraY + 5.0f * sin(-cameraAngleX);
        float lookAtZ = cameraZ - 5.0f * cos(+cameraAngleY) * cos(cameraAngleX);

        // gluLookAt �Լ��� ī�޶��� ��ġ�� ī�޶� �ٶ󺸴� ��ġ�� �����մϴ�.
        // ������ �� �Ű�����(0.0f, 1.0f, 0.0f)�� ī�޶��� '��' ������ ��Ÿ���ϴ�.
        gluLookAt(cameraX, cameraY, cameraZ, lookAtX, lookAtY, lookAtZ, 0.0f, 1.0f, 0.0f);
    }


// display �Լ��� OpenGL�� ������ �������� ȣ��Ǿ� ȭ�鿡 �׸��� �׸��ϴ�.
void display() {
        // ȭ���� ����� �Լ��Դϴ�. ���� ���ۿ� ���� ���۸� ��� ����ϴ�.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ������ �׸� ���� ���� ������ �ݽð� ����(Counter-Clockwise)���� �����մϴ�.
        glFrontFace(GL_CCW);

        // �� ����(Back-face culling)�� Ȱ��ȭ�մϴ�. �̴� ������ ���� ���������� �ʵ��� �մϴ�.
        glEnable(GL_CULL_FACE);

        // ���� �׽�Ʈ�� Ȱ��ȭ�մϴ�. �̴� ��ü�� ���̸� ����Ͽ� �������մϴ�.
        glEnable(GL_DEPTH_TEST);

        // �𵨺� ��Ʈ���� ��带 �����մϴ�.
        glMatrixMode(GL_MODELVIEW);

        // ���� ��Ʈ������ ���� ��ķ� �ʱ�ȭ�մϴ�.
        glLoadIdentity();

        // ī�޶��� �� ��Ʈ������ ������Ʈ�մϴ�.
        updateCamera();

        // ���� �׸��� ���� ��Ʈ���� ���¸� �����մϴ�.
        glPushMatrix();

        DrawModelAtOffset();

        // �����ߴ� ��Ʈ���� ���¸� �����մϴ�.
        glPopMatrix();

        glPushMatrix();
        DrawModelAtOffset2();
        glPopMatrix();

        glPushMatrix();
        glScalef(2.0f, 2.0f, 2.0f);
        glTranslatef(0.0f, 0.0f, 0.0f);
        glCallList(FloorListID);
        glPopMatrix();

        glPushMatrix();
        glScalef(2.0f, 2.0f, 2.0f);
        glTranslatef(0.0f, 1.5f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        glCallList(CeilingListID);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.0f, 1.5f, -7.0f);
        glCallList(WallListID);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(3.0f, 1.5f, -7.0f);
        glCallList(WallListID);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-3.0f, 1.5f, -7.0f);
        glCallList(WallListID);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(6.0f, 1.5f, -7.0f);
        glCallList(WallListID);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-6.0f, 1.5f, -7.0f);
        glCallList(WallListID);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.0f, 1.5f, 7.0f);
        glCallList(WallListID);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(3.0f, 1.5f, 7.0f);
        glCallList(WallListID);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-3.0f, 1.5f, 7.0f);
        glCallList(WallListID);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(6.0f, 1.5f, 7.0f);
        glCallList(WallListID);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-6.0f, 1.5f, 7.0f);
        glCallList(WallListID);
        glPopMatrix();

        glPushMatrix();
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(0.0f, 1.5f, 7.0f);
        glCallList(WallListID);
        glPopMatrix();

        glPushMatrix();
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(3.0f, 1.5f, 7.0f);
        glCallList(WallListID);
        glPopMatrix();

        glPushMatrix();
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(6.0f, 1.5f, 7.0f);
        glCallList(WallListID);
        glPopMatrix();

        glPushMatrix();
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(-3.0f, 1.5f, 7.0f);
        glCallList(WallListID);
        glPopMatrix();

        glPushMatrix();
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(-6.0f, 1.5f, 7.0f);
        glCallList(WallListID);
        glPopMatrix();

        glPushMatrix();
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(0.0f, 1.5f, -7.0f);
        glCallList(WallListID);
        glPopMatrix();

        glPushMatrix();
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(3.0f, 1.5f, -7.0f);
        glCallList(WallListID);
        glPopMatrix();

        glPushMatrix();
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(6.0f, 1.5f, -7.0f);
        glCallList(WallListID);
        glPopMatrix();

        glPushMatrix();
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(-3.0f, 1.5f, -7.0f);
        glCallList(WallListID);
        glPopMatrix();

        glPushMatrix();
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(-6.0f, 1.5f, -7.0f);
        glCallList(WallListID);
        glPopMatrix();


        //�̷� ���� ��ġ
        glPushMatrix();
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(-5.0f, 1.5f, -1.5f);
        glCallList(WallListID);
        glPopMatrix();

        glPushMatrix();
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(-5.0f, 1.5f, +1.5f);
        glCallList(WallListID);
        glPopMatrix();

        glPushMatrix();
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(-2.0f, 1.5f, -1.5f);
        glCallList(WallListID);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-0.5f, 1.5f, -0.0f);
        glCallList(WallListID);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(cubeY, 1.5f, -0.0f);
        glCallList(WallListID);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(2.5f, 1.5f, 3.0f);
        glCallList(WallListID);
        glPopMatrix();

        glPushMatrix();
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(2.0f, 1.5f, 0.5f);
        glCallList(WallListID);
        glPopMatrix();

        glPushMatrix();
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(2.0f, 1.5f, 3.75f);
        glCallList(WallListID);
        glPopMatrix();


        glPushMatrix();
        glTranslatef(2.75f, 1.5f, -4.0f);
        glCallList(WallListID);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-0.25f, 1.5f, -4.0f);
        glCallList(WallListID);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-3.25f, 1.5f, -4.0f);
        glCallList(WallListID);
        glPopMatrix();


        glPushMatrix();
        glTranslatef(-5.0f, 1.5f, -0.0f);
        glCallList(WallListID);
        glPopMatrix();

        // �޴��� Ȱ��ȭ�Ǿ� ������ �޴��� �׸��� �ڵ尡 ���⿡ ��ġ�մϴ�.
        if (isMenuDisplayed) {
            // �޴� �׸��� ���� �ڵ�
        }

        // ���� ���۸��� ����ϴ� ���, ���۸� ��ü�մϴ�.
        // �̴� ���� �׷��� �̹����� ȭ�鿡 ǥ���ϰ�, ���� �׸��� �׸��� �����մϴ�.
        glutSwapBuffers();
    }


// keyboard �Լ��� Ű������ Ű�� ������ �� ȣ��Ǵ� �ݹ� �Լ��Դϴ�.
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'w':  // 'w' Ű: ������ �̵�
        if (!isWalking) {
            isWalking = true;
            PlaySoundFunction(true);  // �ȴ� �Ҹ� ����
        }
        moveForward = true;
        break;
    case 's':  // 's' Ű: �ڷ� �̵�
        if (!isWalking) {
            isWalking = true;
            PlaySoundFunction(true);  // �ȴ� �Ҹ� ����
        }
        moveBackward = true;
        break;
    case 'a':  // 'a' Ű: �������� �̵�
        if (!isWalking) {
            isWalking = true;
            PlaySoundFunction(true);  // �ȴ� �Ҹ� ����
        }
        moveLeft = true;
        break;
    case 'd':  // 'd' Ű: ���������� �̵�
        if (!isWalking) {
            isWalking = true;
            PlaySoundFunction(true);  // �ȴ� �Ҹ� ����
        }
        moveRight = true;
        break;
    case 'q':  // 'q' Ű: ī�޶� �������� ȸ��
        cameraAngleY -= rotateSpeed;
        break;
    case 'e':  // 'e' Ű: ī�޶� ���������� ȸ��
        cameraAngleY += rotateSpeed;
        break;
    case '1':  // '1' Ű: ī�޶� �ӵ� ����
        cameraSpeed += 0.003f;
        break;
    case '2':  // '2' Ű: ī�޶� �ӵ� ����
        cameraSpeed = std::max(cameraSpeed - 0.003f, 0.0f);
        break;
    case ' ':  // �����̽���: ����
        JumpSoundFunction(true);
        jumping = true;  // ���� Ȱ��ȭ
        break;
    case 27:  // ESC Ű: ���α׷� ����
        exit(0);
        break;
    }

    // ȭ���� �ٽ� �׸����� ��û�մϴ�.
    glutPostRedisplay();
}


// keyboardUp �Լ��� Ű������ Ű�� �������� �� ȣ��Ǵ� �ݹ� �Լ��Դϴ�.
void keyboardUp(unsigned char key, int x, int y) {
    switch (key) {
    case 'w':  // 'w' Ű: ������ �̵� ����
        moveForward = false;
        break;
    case 's':  // 's' Ű: �ڷ� �̵� ����
        moveBackward = false;
        break;
    case 'a':  // 'a' Ű: �������� �̵� ����
        moveLeft = false;
        break;
    case 'd':  // 'd' Ű: ���������� �̵� ����
        moveRight = false;
        break;
    }

    // �̵� ������ �ʴٸ� �ȴ� �Ҹ� ����
    if (!moveForward && !moveBackward && !moveLeft && !moveRight) {
        isWalking = false;
        PlaySoundFunction(false);
    }

    // ȭ���� �ٽ� �׸����� ��û�մϴ�.
    glutPostRedisplay();
}


// idle �Լ��� �̺�Ʈ�� �߻����� ���� �� ȣ��Ǿ�, ���������� ��ȭ�ϴ� ���¸� ������Ʈ�մϴ�.
void idle() {
    // ī�޶� �̵��� ���� X, Z �� ��ȭ���� �ʱ�ȭ�մϴ�.
    float moveX = 0.0f;
    float moveZ = 0.0f;

    // ī�޶� �̵� ó��
    if (moveForward) {  // ������ �̵�
        moveX += cameraSpeed * sin(cameraAngleY);
        moveZ -= cameraSpeed * cos(cameraAngleY);
    }
    if (moveBackward) {  // �ڷ� �̵�
        moveX -= cameraSpeed * sin(cameraAngleY);
        moveZ += cameraSpeed * cos(cameraAngleY);
    }
    if (moveLeft) {  // �������� �̵�
        moveX -= cameraSpeed * cos(cameraAngleY);
        moveZ -= cameraSpeed * sin(cameraAngleY);
    }
    if (moveRight) {  // ���������� �̵�
        moveX += cameraSpeed * cos(cameraAngleY);
        moveZ += cameraSpeed * sin(cameraAngleY);
    }

    // ���� ó��
    if (jumping) {
        // ī�޶��� Y�� ��ġ�� 2.0f�� �����ϸ� ���� ���¸� �����մϴ�.
        if (cameraY < 2.0f) {
            cameraY += 0.01f;  // ���� �߿��� Y�� ��ġ�� ������ŵ�ϴ�.
        }
        else {
            jumping = false;  // �ִ� ���̿� �����ϸ� ���� ���¸� �����մϴ�.
        }
    }
    else {
        // �߷� ȿ�� ����: ī�޶� ���߿� ������ Y�� ��ġ�� ���ҽ�ŵ�ϴ�.
        if (cameraY > 1.0f) {
            cameraY -= 0.01f;
        }
        else {
            cameraY = 1.0f;  // ���� �����ϸ� Y�� ��ġ�� �����մϴ�.
        }
    }

    // ���� �̵����� �����Ͽ� ī�޶� ��ġ�� ������Ʈ�մϴ�.
    cameraX += moveX;
    cameraZ += moveZ;

    // ȭ���� �ٽ� �׸����� ��û�մϴ�.
    glutPostRedisplay();
}


// mouseMotion �Լ��� ���콺�� ������ �� ȣ��Ǵ� �ݹ� �Լ��Դϴ�.
void mouseMotion(int x, int y) {
    // ���� ���콺 ��ġ�� �����Ǿ����� Ȯ���մϴ�.
    if (lastMouseX != -1 && lastMouseY != -1) {
        // ���콺�� �̵����� ����մϴ�.
        float deltaX = x - lastMouseX;
        float deltaY = y - lastMouseY;

        // ���콺�� �̵����� ���� ī�޶� ������ �����մϴ�.
        cameraAngleY += (deltaX) * 0.001f;
        cameraAngleX += (deltaY) * 0.001f;

        // ī�޶��� ���� ������ �ʹ� ũ�� �ʵ��� �����մϴ�.
        if (cameraAngleX > 1.5f) cameraAngleX = 1.5f;
        if (cameraAngleX < -1.5f) cameraAngleX = -1.5f;

        // ȭ���� �ٽ� �׸����� ��û�մϴ�.
        glutPostRedisplay();
    }

    // ���� ���콺 ��ġ�� �����մϴ�.
    lastMouseX = x;
    lastMouseY = y;
}


// mouse �Լ��� ���콺 ��ư �̺�Ʈ�� �߻��� �� ȣ��Ǵ� �ݹ� �Լ��Դϴ�.
void mouse(int button, int state, int x, int y) {
    // ���� ���콺 ��ư �̺�Ʈ�� Ȯ���մϴ�.
    if (button == GLUT_LEFT_BUTTON) {
        // ���콺 ��ư�� ���� ���
        if (state == GLUT_DOWN) {
            // ���� ���콺 ��ġ�� �����մϴ�.
            lastMouseX = x;
            lastMouseY = y;
        }
        // ���콺 ��ư�� ���� ���
        else if (state == GLUT_UP) {
            // ���콺 ��ġ ������ �����մϴ�.
            lastMouseX = -1;
            lastMouseY = -1;
        }
    }
}


// ResetCameraPosition �Լ��� ī�޶��� ��ġ�� ������ �ʱ� ���·� �缳���մϴ�.
void ResetCameraPosition(int value) {
    // ī�޶��� X, Y, Z ��ǥ�� �ʱ� ��ġ�� �����մϴ�.
    // ���⼭ ī�޶�� ���� ��ó�� ��ġ�ϸ�, �ణ ���� �ö󰡰� �ణ �ڷ� ������ ��ġ�� �ֽ��ϴ�.
    cameraX = 0.0f;
    cameraY = 1.0f;
    cameraZ = 5.0f;

    // ī�޶��� ���� �� ���� ȸ�� ������ �ʱ�ȭ�մϴ�.
    cameraAngleY = 0.0f;
    cameraAngleX = 0.0f;

    // ȭ���� �ٽ� �׸����� ��û�մϴ�.
    glutPostRedisplay();
}


// update �Լ��� �ֱ������� ȣ��Ǿ� �ִϸ��̼� ���¸� ������Ʈ�ϴ� �Լ��Դϴ�.
void update(int value) {
    // ť���� Y�� ��ġ�� �����Ͽ� ť�긦 ���Ϸ� �̵���ŵ�ϴ�.
    cubeY += cubeU;

    // ť�갡 Ư�� ������ ����� �̵� ������ �ݴ�� ��ȯ�մϴ�.
    if (cubeY > 5.0 || cubeY < 2.5) {
        cubeU = -cubeU;
    }

    // ȭ�� ������ ��û�մϴ�.
    glutPostRedisplay();

    // 16�и���(�� 60FPS�� �ش�)���� �� �Լ��� �ٽ� ȣ���Ͽ� �ִϸ��̼��� ����մϴ�.
    glutTimerFunc(16, update, 0);
}



// handleMouse �Լ��� ���콺 �̺�Ʈ�� ó���մϴ�.
void handleMouse(int button, int state, int x, int y) {
    // ���콺 ������ ��ư�� Ŭ������ ���� �̺�Ʈ�� ó���մϴ�.
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        // �޴��� Ȱ��ȭ�ϰ� ȭ���� �����մϴ�.
        isMenuDisplayed = true;
        glutPostRedisplay();
    }
}


// processMenu �Լ��� �޴� �̺�Ʈ�� ó���մϴ�.
void processMenu(int option) {
    switch (option) {
    case 1: // '1' �ɼ� ���� �� �ʱ� ī�޶� ��ġ�� �̵�
        ResetCameraPosition(0);
        break;
        // �̰��� �ٸ� �޴� �ɼǵ��� �߰��� �� �ֽ��ϴ�.
    }

    // �޴� ���� �� �޴��� ��Ȱ��ȭ�ϰ� ȭ���� �����մϴ�.
    isMenuDisplayed = false;
    glutPostRedisplay();
}


// main �Լ��� ���α׷��� �������Դϴ�.
int main(int argc, char** argv) {
    // GLUT ���̺귯���� �ʱ�ȭ�մϴ�.
    glutInit(&argc, argv);

    // ���÷��� ��带 �����մϴ�. ���⼭�� ���� ���۸�, RGB ���� ���, ���� ���۸� ����մϴ�.
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // �ʱ� ������ ũ�⸦ �����մϴ�.
    glutInitWindowSize(800, 600);

    // �����츦 �����ϰ� ������ �����մϴ�.
    glutCreateWindow("3D_Maze");

    // OpenGL ���� �ʱ� ������ �����ϴ� ����� ���� �Լ��� ȣ���մϴ�.
    init();
    //InitLight(); // ���� ���� �Լ� (���� ��Ȱ��ȭ)

    // ��������� �����մϴ�.
    BackgroundSoundFunction(true);

    // �̺�Ʈ ó�� �Լ����� ����մϴ�.
    glutDisplayFunc(display);        // ȭ�� �׸��� �Լ�
    glutReshapeFunc(reshape);        // â ũ�� ���� ó�� �Լ�
    glutKeyboardFunc(keyboard);      // Ű���� �Է� ó�� �Լ�
    glutKeyboardUpFunc(keyboardUp);  // Ű���� �Է� ���� ó�� �Լ�

    glutMouseFunc(mouse);            // ���콺 Ŭ�� ó�� �Լ�
    glutMotionFunc(mouseMotion);     // ���콺 ������ ó�� �Լ�

    glutIdleFunc(idle);              // �ƹ� �̺�Ʈ�� �߻����� ���� �� ȣ���� �Լ�

    // �ؽ�ó�� ���÷��� ����Ʈ�� �����մϴ�.
    MakeTextures();
    MakeWallList();
    MakeFloorList();
    MakeCeilingList();
    ReadModel();
    ReadModel2();

    // �޴��� �����ϰ� �����մϴ�.
    glutCreateMenu(processMenu);
    glutAddMenuEntry("restart", 1);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    // Ÿ�̸� �Լ��� �����Ͽ� �ֱ������� `update` �Լ��� ȣ���մϴ�.
    glutTimerFunc(25, update, 0);

    // GLUT�� ���� �̺�Ʈ ������ �����մϴ�.
    glutMainLoop();

    return 0;
}
