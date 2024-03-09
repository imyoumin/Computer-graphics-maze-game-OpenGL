#include <GL/glut.h>
#include <Windows.h>
#include <mmsystem.h>
#include <cmath>
#include <stdio.h>
#include <iostream>
#include <filesystem>
#include <string>


float cameraX = 0.0f; //초기 카메라 위치 x좌표
float cameraY = 1.0f; //초기 카메라 위치 y좌표
float cameraZ = 5.0f; //초기 카메라 위치 z좌표

float cameraAngleY = 0.0f; //초기 카메라 y각도
float cameraAngleX = 0.0f; //초기 카메라 x각도

float cameraSpeed = 0.005f; //카메라 움직임 속도 조절
float rotateSpeed = 0.05f; //카메라 회전 속도 조절

bool isMenuDisplayed = false;

int lastMouseX = -1;
int lastMouseY = -1;

GLuint textureIDs[9]; // 텍스처 ID 배열
GLubyte* images[9];   // 각 면에 대한 이미지 배열

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

GLfloat MyVertices[8][3] = {  //직육면체 정점 정보
    {-1.5f, -1.5f, 0.5f},
    {1.5f, -1.5f, 0.5f},
    {1.5f, 1.5f, 0.5f},
    {-1.5f, 1.5f, 0.5f},
    {-1.5f, -1.5f, -0.5f},
    {1.5f, -1.5f, -0.5f},
    {1.5f, 1.5f, -0.5f},
    {-1.5f, 1.5f, -0.5f}
};


GLfloat MyColors[8][3] = {   //직육면체 색상 정보
    { 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0 }
};

GLfloat MyTextureCoordinates[4][2] = {   //직육면체 텍스쳐 좌표
    { 0.0, 1.0 },
    { 1.0, 1.0 },
    { 1.0, 0.0 }
};

GLubyte MyVertexList[24] = {   //직육면체 면
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

// ReadModel 함수는 파일에서 3D 모델 데이터를 읽어오는 함수입니다.
void ReadModel()
{
    // 모델 파일의 경로를 문자열로 정의합니다.
    std::string fname = "..\\source\\myModel.dat";

    // 파일 포인터를 선언합니다. 이것은 파일을 읽기 위한 핸들 역할을 합니다.
    FILE* f1;
    // 임시 문자열 버퍼를 선언합니다. 파일로부터 읽은 문자열을 저장하는 데 사용됩니다.
    char s[81];
    // 반복문에서 사용할 인덱스 변수를 선언합니다.
    int i;

    // 이미 할당된 메모리가 있다면 해제합니다. 이는 이전에 읽은 데이터를 클리어하는 용도입니다.
    if (mpoint != NULL) delete[] mpoint;
    if (mface != NULL) delete[] mface;

    // 파일을 열고, 파일이 열리지 않았다면 오류 메시지를 출력하고 프로그램을 종료합니다.
    if ((f1 = fopen(fname.c_str(), "rt")) == NULL) {
        printf("No file\n");
        exit(0);
    }

    // 파일에서 문자열을 두 번 읽어와서 s 변수에 저장합니다. 이 부분은 파일 형식에 따라 다를 수 있습니다.
    fscanf(f1, "%s", s); // 첫 번째 문자열 읽기
    fscanf(f1, "%s", s); // 두 번째 문자열 읽기

    // 파일에서 정점의 개수를 읽어와 pnum 변수에 저장합니다.
    fscanf(f1, "%d", &pnum);

    // 정점 데이터를 저장할 배열을 동적으로 할당합니다.
    mpoint = new Point[pnum];
    // 모든 정점에 대해 반복하면서 x, y, z 좌표를 파일에서 읽어옵니다.
    for (i = 0; i < pnum; i++) {
        fscanf(f1, "%f", &mpoint[i].x);
        fscanf(f1, "%f", &mpoint[i].y);
        fscanf(f1, "%f", &mpoint[i].z);
    }

    // 파일에서 면의 개수를 읽어와 fnum 변수에 저장합니다.
    fscanf(f1, "%s", s); // Read and print the third string
    fscanf(f1, "%s", s); // Read and print the fourth string
    fscanf(f1, "%d", &fnum);

    // 면 데이터를 저장할 배열을 동적으로 할당합니다.
    mface = new Face[fnum];
    // 모든 면에 대해 반복하면서 각 면을 구성하는 세 개의 정점 인덱스를 파일에서 읽어옵니다.
    for (i = 0; i < fnum; i++) {
        fscanf(f1, "%d", &mface[i].ip[0]);
        fscanf(f1, "%d", &mface[i].ip[1]);
        fscanf(f1, "%d", &mface[i].ip[2]);
    }

    // 파일을 닫습니다.
    fclose(f1);
}


// ReadModel2 함수는 또 다른 3D 모델 데이터를 파일에서 읽어오는 함수입니다.
void ReadModel2()
{
    // 모델 파일의 경로를 문자열로 정의합니다.
    std::string f2name = "..\\source\\myModel2.dat";

    // 파일 포인터를 선언합니다. 이것은 파일을 읽기 위한 핸들 역할을 합니다.
    FILE* f2;
    // 임시 문자열 버퍼를 선언합니다. 파일로부터 읽은 문자열을 저장하는 데 사용됩니다.
    char d[81];
    // 반복문에서 사용할 인덱스 변수를 선언합니다.
    int j;

    // 이미 할당된 메모리가 있다면 해제합니다. 이는 이전에 읽은 데이터를 클리어하는 용도입니다.
    if (m2point != NULL) delete[] m2point;
    if (m2face != NULL) delete[] m2face;

    // 파일을 열고, 파일이 열리지 않았다면 오류 메시지를 출력하고 프로그램을 종료합니다.
    if ((f2 = fopen(f2name.c_str(), "rt")) == NULL) {
        printf("No file\n");
        exit(0);
    }

    // 파일에서 문자열을 두 번 읽어와서 d 변수에 저장합니다. 이 부분은 파일 형식에 따라 다를 수 있습니다.
    fscanf(f2, "%s", d); // 첫 번째 문자열 읽기
    fscanf(f2, "%s", d); // 두 번째 문자열 읽기

    // 파일에서 정점의 개수를 읽어와 pnum2 변수에 저장합니다.
    fscanf(f2, "%d", &pnum2);

    // 정점 데이터를 저장할 배열을 동적으로 할당합니다.
    m2point = new Point2[pnum2];
    // 모든 정점에 대해 반복하면서 x, y, z 좌표를 파일에서 읽어옵니다.
    for (j = 0; j < pnum2; j++) {
        fscanf(f2, "%f", &m2point[j].x);
        fscanf(f2, "%f", &m2point[j].y);
        fscanf(f2, "%f", &m2point[j].z);
    }

    // 파일에서 면의 개수를 읽어와 fnum2 변수에 저장합니다.
    fscanf(f2, "%s", d); // Read and print the third string
    fscanf(f2, "%s", d); // Read and print the fourth string
    fscanf(f2, "%d", &fnum2);

    // 면 데이터를 저장할 배열을 동적으로 할당합니다.
    m2face = new Face2[fnum2];
    // 모든 면에 대해 반복하면서 각 면을 구성하는 세 개의 정점 인덱스를 파일에서 읽어옵니다.
    for (j = 0; j < fnum2; j++) {
        fscanf(f2, "%d", &m2face[j].ip[0]);
        fscanf(f2, "%d", &m2face[j].ip[1]);
        fscanf(f2, "%d", &m2face[j].ip[2]);
    }

    // 파일을 닫습니다.
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


// DrawModelAtOffset 함수는 저장된 3D 모델 데이터를 화면에 그리는 함수입니다.
void DrawModelAtOffset(void)
{
    // 모델을 이동시키는 함수입니다. 여기서는 (2.0f, -0.2f, 3.2f)만큼 이동합니다.
    // 이는 X, Y, Z 축을 따라 모델을 이동시킵니다.
    glTranslatef(2.0f, -0.2f, 3.2f);

    // 모델의 크기를 조정하는 함수입니다. 여기서는 각 축에 대해 0.003배로 축소합니다.
    glScalef(0.003f, 0.003f, 0.003f);

    /*
    // 모델의 재질 속성을 설정하는 부분입니다. 
    GLfloat model_diffuse[] = { 0.5, 0.4, 0.3, 1.0 };
    GLfloat model_specular[] = { 0.1, 0.1, 0.1, 1.0 };
    GLfloat model_ambient[] = { 0.5, 0.4, 0.3, 1.0 };
    GLfloat model_shininess[] = { 500.0 };

    glMaterialfv(GL_FRONT, GL_DIFFUSE, model_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, model_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, model_ambient);
    glMaterialfv(GL_FRONT, GL_SHININESS, model_shininess);
    */

    // 모델의 색상을 설정합니다. 여기서는 (0.7, 0.2, 0.3) RGB 색상을 사용합니다.
    glColor3f(0.7, 0.2, 0.3);

    // 쉐이딩 모델을 설정합니다. GL_SMOOTH는 부드러운 그라데이션 쉐이딩을 의미합니다.
    glShadeModel(GL_SMOOTH);

    // 삼각형을 그리기 시작합니다. 이는 모델의 면을 구성하는 기본 단위입니다.
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < fnum; i++) {
        // 각 면을 구성하는 정점들을 그립니다. mpoint 배열에서 정점의 좌표를 가져와서 사용합니다.
        glVertex3f(mpoint[mface[i].ip[0]].x, mpoint[mface[i].ip[0]].y, mpoint[mface[i].ip[0]].z);
        glVertex3f(mpoint[mface[i].ip[1]].x, mpoint[mface[i].ip[1]].y, mpoint[mface[i].ip[1]].z);
        glVertex3f(mpoint[mface[i].ip[2]].x, mpoint[mface[i].ip[2]].y, mpoint[mface[i].ip[2]].z);
    }
    // 삼각형 그리기를 종료합니다.
    glEnd();
}


// DrawModelAtOffset2 함수는 또 다른 3D 모델 데이터를 화면에 그리는 함수입니다.
void DrawModelAtOffset2(void)
{
    // 모델을 이동시키는 함수입니다. 여기서는 (-5.0f, 0.0f, 3.0f)만큼 이동합니다.
    // 이는 X, Y, Z 축을 따라 모델을 이동시킵니다.
    glTranslatef(-5.0f, -0.0f, 3.0f);

    // 모델의 크기를 조정하는 함수입니다. 여기서는 각 축에 대해 0.003배로 축소합니다.
    glScalef(0.003f, 0.003f, 0.003f);

    /*
    // 모델의 재질 속성을 설정하는 부분입니다. 여기서는 주석 처리되어 있어 적용되지 않습니다.
    GLfloat model_diffuse[] = { 0.5, 0.4, 0.3, 1.0 };
    GLfloat model_specular[] = { 0.1, 0.1, 0.1, 1.0 };
    GLfloat model_ambient[] = { 0.5, 0.4, 0.3, 1.0 };
    GLfloat model_shininess[] = { 500.0 };

    glMaterialfv(GL_FRONT, GL_DIFFUSE, model_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, model_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, model_ambient);
    glMaterialfv(GL_FRONT, GL_SHININESS, model_shininess);
    */

    // 모델의 색상을 설정합니다. 여기서는 (0.0, 0.7, 0.3) RGB 색상을 사용합니다.
    glColor3f(0.0, 0.7, 0.3);

    // 쉐이딩 모델을 설정합니다. GL_SMOOTH는 부드러운 그라데이션 쉐이딩을 의미합니다.
    glShadeModel(GL_SMOOTH);

    // 삼각형을 그리기 시작합니다. 이는 모델의 면을 구성하는 기본 단위입니다.
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < fnum2; i++) {
        // 각 면을 구성하는 정점들을 그립니다. m2point 배열에서 정점의 좌표를 가져와서 사용합니다.
        glVertex3f(m2point[m2face[i].ip[0]].x, m2point[m2face[i].ip[0]].y, m2point[m2face[i].ip[0]].z);
        glVertex3f(m2point[m2face[i].ip[1]].x, m2point[m2face[i].ip[1]].y, m2point[m2face[i].ip[1]].z);
        glVertex3f(m2point[m2face[i].ip[2]].x, m2point[m2face[i].ip[2]].y, m2point[m2face[i].ip[2]].z);
    }
    // 삼각형 그리기를 종료합니다.
    glEnd();
}


// init 함수는 OpenGL 그래픽스 프로그램의 초기 설정을 위한 함수입니다.
void init() {
    // glClearColor 함수는 화면을 지울 때 사용할 색상을 설정합니다.
    // 여기서 설정된 색상은 검은색(0.0f, 0.0f, 0.0f)이며, 알파 값은 1.0f입니다.
    // 이 색상은 glClear 함수를 호출할 때 색 버퍼를 지울 때 사용됩니다.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // glEnable 함수를 사용하여 깊이 테스트를 활성화합니다.
    // 깊이 테스트는 3D 그래픽스에서 물체가 다른 물체 뒤에 숨겨져야 할 때 이를 처리하는 기능입니다.
    // GL_DEPTH_TEST는 물체의 깊이(즉, Z 값)를 고려하여 물체의 가시성을 결정하는 데 사용됩니다.
    glEnable(GL_DEPTH_TEST);
}


// JumpSoundFunction 함수는 점프 소리를 재생하거나 중단합니다.
void JumpSoundFunction(bool start) {
    // 점프 소리 파일의 경로입니다.
    LPCWSTR soundFilePath = L"../source/sounds/jump.wav";

    if (start) {
        // start가 true일 경우, 점프 소리를 비동기적으로 재생합니다.
        // SND_ASYNC는 소리 재생을 비동기적으로 수행하며, SND_FILENAME은 파일 이름을 사용한다는 것을 의미합니다.
        sndPlaySound(soundFilePath, SND_ASYNC | SND_FILENAME);
    }
    else {
        // start가 false일 경우, 현재 재생중인 소리를 중단합니다.
        sndPlaySound(NULL, SND_ASYNC);
    }
}

// PlaySoundFunction 함수는 걷는 소리를 재생하거나 중단합니다.
void PlaySoundFunction(bool start) {
    // 걷는 소리 파일의 경로입니다.
    LPCWSTR soundFilePath = L"../source/sounds/walking.wav";

    if (start) {
        // start가 true일 경우, 걷는 소리를 비동기적으로 재생하며, 이를 반복합니다.
        // SND_LOOP는 소리를 반복 재생하도록 설정합니다.
        sndPlaySound(soundFilePath, SND_ASYNC | SND_FILENAME | SND_LOOP);
    }
    else {
        // start가 false일 경우, 현재 재생중인 소리를 중단합니다.
        sndPlaySound(NULL, SND_ASYNC);
    }
}

// BackgroundSoundFunction 함수는 배경음악을 재생하거나 중단합니다.
void BackgroundSoundFunction(bool start) {
    // 배경음악 파일의 경로입니다.
    LPCWSTR soundFilePath = L"../source/sounds/background_sound.wav";

    if (start) {
        // 시작할 때 소리를 비동기적으로 재생하며, 이를 반복합니다.
        // SND_LOOP는 소리를 반복 재생하도록 설정합니다.
        sndPlaySound(soundFilePath, SND_ASYNC | SND_FILENAME | SND_LOOP);
    }
}


// reshape 함수는 창 크기가 변경될 때 호출되어 그래픽스 뷰포트와 투영 매트릭스를 설정합니다.
void reshape(int width, int height) {
    // glViewport 함수는 화면에 그려질 영역의 크기와 위치를 설정합니다.
    // 여기서는 전체 창 크기를 뷰포트로 설정합니다.
    glViewport(0, 0, width, height);

    // glMatrixMode 함수는 현재 작업할 매트릭스 모드를 설정합니다.
    // GL_PROJECTION은 투영 매트릭스(카메라 뷰) 설정을 위한 모드입니다.
    glMatrixMode(GL_PROJECTION);

    // glLoadIdentity 함수는 선택된 매트릭스 모드의 매트릭스를 단위 행렬로 초기화합니다.
    // 이는 이전에 적용된 변환을 초기화하는 역할을 합니다.
    glLoadIdentity();

    // gluPerspective 함수는 원근 투영 행렬을 설정합니다.
    // 이 함수는 시야각(fov), 종횡비(aspect ratio), 근접 평면(near plane), 원거리 평면(far plane)을 매개변수로 받습니다.
    gluPerspective(45.0f, (float)width / height, 0.1f, 100.0f);

    // 다시 모델뷰 매트릭스 모드로 전환합니다. 이 모드에서는 객체의 변환을 처리합니다.
    glMatrixMode(GL_MODELVIEW);
}



// LoadBMP 함수는 BMP 형식의 이미지 파일을 읽어와서 OpenGL에서 사용할 수 있도록 메모리에 로드합니다.
void LoadBMP(const char* filename, GLubyte** imagePtr) {
    // 파일을 이진 읽기 모드로 엽니다.
    FILE* file = fopen(filename, "rb");

    // BMP 파일의 헤더를 저장할 배열입니다. BMP 헤더는 54바이트로 구성됩니다.
    unsigned char header[54];
    // 파일에서 54바이트를 읽어와 헤더 배열에 저장합니다.
    fread(header, sizeof(unsigned char), 54, file);

    // 헤더에서 이미지의 너비와 높이를 추출합니다.
    // 너비는 헤더의 18번째 바이트부터 시작하며, 높이는 22번째 바이트부터 시작합니다.
    width = *(int*)&header[18];
    height = *(int*)&header[22];

    // 이미지의 총 바이트 크기를 계산합니다. BMP 파일은 픽셀 당 3바이트(RGB)를 사용합니다.
    int imageSize = width * height * 3;

    // 메모리에 이미지 데이터를 저장할 공간을 할당합니다.
    *imagePtr = (GLubyte*)malloc(imageSize);

    // 파일에서 이미지 데이터를 읽어와 할당된 메모리에 저장합니다.
    fread(*imagePtr, sizeof(GLubyte), imageSize, file);

    // 파일을 닫습니다.
    fclose(file);
}


// MakeTextures 함수는 여러 텍스처를 생성하고 설정하는 함수입니다.
void MakeTextures() {
    // glGenTextures 함수를 사용하여 텍스처 ID를 생성합니다.
    // 여기서는 8개의 텍스처 ID를 생성합니다.
    glGenTextures(8, textureIDs);

    // 텍스처 파일의 경로를 담고 있는 문자열 배열입니다.
    const char* filenames[9] = {
        "../source/textures/dungeon_wall3.bmp", //벽 이미지1
        "../source/textures/dungeon_wall3.bmp", //벽 이미지1
        "../source/textures/dungeon_wall2.bmp", //벽 이미지1
        "../source/textures/dungeon_wall1.bmp", //벽 이미지1
        "../source/textures/dungeon_wall3.bmp", //벽 이미지1
        "../source/textures/dungeon_wall3.bmp", //벽 이미지1
        "../source/textures/dungeon_floor.bmp", //바닥 이미지1
        "../source/textures/dungeon_floor.bmp", //천장 이미지1
        "../source/textures/title.bmp" 
    };

    // 각 텍스처에 대해 반복합니다.
    for (int i = 0; i < 9; ++i) {
        // glBindTexture 함수를 사용하여 현재 작업할 텍스처를 바인딩합니다.
        glBindTexture(GL_TEXTURE_2D, textureIDs[i]);

        // LoadBMP 함수를 사용하여 BMP 파일로부터 텍스처 이미지를 로드합니다.
        LoadBMP(filenames[i], &images[i]);

        // glTexImage2D 함수를 사용하여 로드된 이미지 데이터를 OpenGL 텍스처로 설정합니다.
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, images[i]);

        // 텍스처 필터링 옵션을 설정합니다. 여기서는 선형 필터링을 사용합니다.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
}


// MakeWallList 함수는 벽의 텍스처와 정점 데이터를 설정하는 디스플레이 리스트를 생성합니다.
void MakeWallList() {
    // glGenLists 함수를 사용하여 새로운 디스플레이 리스트를 생성합니다.
    WallListID = glGenLists(1);

    // glNewList 함수로 새 리스트의 정의를 시작합니다.
    // GL_COMPILE은 이 리스트가 컴파일 모드로 생성됨을 의미합니다.
    glNewList(WallListID, GL_COMPILE);

    // 정점, 색상, 텍스처 좌표 배열을 사용할 수 있도록 설정합니다.
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    // 텍스처 매핑을 활성화합니다.
    glEnable(GL_TEXTURE_2D);

    // 정점을 그릴 때의 정점 순서를 시계 방향(Clockwise)으로 설정합니다.
    glFrontFace(GL_CW);

    // 벽의 각 면에 대해 반복합니다.
    for (GLint i = 0; i < 6; i++) {
        // glBindTexture 함수로 현재 사용할 텍스처를 바인딩합니다.
        glBindTexture(GL_TEXTURE_2D, textureIDs[i]);

        // glBegin 함수로 쿼드(사각형) 그리기를 시작합니다.
        glBegin(GL_QUADS);

        // 현재 면의 색상을 설정합니다.
        glColor3fv(MyColors[i]);

        // 면을 구성하는 각 정점에 대해 반복합니다.
        for (GLint j = 0; j < 4; j++) {
            // 현재 정점의 텍스처 좌표를 설정합니다.
            glTexCoord2fv(MyTextureCoordinates[j]);

            // 현재 정점의 좌표를 설정합니다.
            glVertex3fv(MyVertices[MyVertexList[i * 4 + j]]);
        }

        // 사각형 그리기를 종료합니다.
        glEnd();
    }

    // 텍스처 매핑을 비활성화합니다.
    glDisable(GL_TEXTURE_2D);

    // 정점, 색상, 텍스처 좌표 배열 사용을 중지합니다.
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    // 디스플레이 리스트 정의를 종료합니다.
    glEndList();
}


// MakeFloorList 함수는 바닥의 텍스처와 정점 데이터를 설정하는 디스플레이 리스트를 생성합니다.
void MakeFloorList() {
    // glGenLists 함수를 사용하여 새로운 디스플레이 리스트를 생성합니다.
    FloorListID = glGenLists(1);

    // glNewList 함수로 새 리스트의 정의를 시작합니다.
    // GL_COMPILE은 이 리스트가 컴파일 모드로 생성됨을 의미합니다.
    glNewList(FloorListID, GL_COMPILE);

    // glBindTexture 함수로 현재 사용할 텍스처를 바인딩합니다.
    // 여기서는 바닥 텍스처(7번째 텍스처)를 사용합니다.
    glBindTexture(GL_TEXTURE_2D, textureIDs[6]);

    // 텍스처 매핑을 활성화합니다.
    glEnable(GL_TEXTURE_2D);

    // glColor3f 함수로 현재 색상을 흰색으로 설정합니다.
    // 이는 텍스처의 색상을 그대로 사용하기 위함입니다.
    glColor3f(1.0f, 1.0f, 1.0f);

    // glBegin 함수로 쿼드(사각형) 그리기를 시작합니다.
    glBegin(GL_QUADS);

    // 바닥의 각 정점에 대해 텍스처 좌표와 정점 좌표를 설정합니다.
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-3.5f, 0.0f, -3.5f);

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-3.5f, 0.0f, 3.5f);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(3.5f, 0.0f, 3.5f);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(3.5f, 0.0f, -3.5f);

    // 사각형 그리기를 종료합니다.
    glEnd();

    // 텍스처 매핑을 비활성화합니다.
    glDisable(GL_TEXTURE_2D);

    // 디스플레이 리스트 정의를 종료합니다.
    glEndList();
}


// MakeCeilingList 함수는 천장의 텍스처와 정점 데이터를 설정하는 디스플레이 리스트를 생성합니다.
void MakeCeilingList() {
    // glGenLists 함수를 사용하여 새로운 디스플레이 리스트를 생성합니다.
    CeilingListID = glGenLists(1);

    // glNewList 함수로 새 리스트의 정의를 시작합니다.
    // GL_COMPILE은 이 리스트가 컴파일 모드로 생성됨을 의미합니다.
    glNewList(CeilingListID, GL_COMPILE);

    // glBindTexture 함수로 현재 사용할 텍스처를 바인딩합니다.
    // 여기서는 천장 텍스처(8번째 텍스처)를 사용합니다.
    glBindTexture(GL_TEXTURE_2D, textureIDs[7]);

    // 텍스처 매핑을 활성화합니다.
    glEnable(GL_TEXTURE_2D);

    // glColor3f 함수로 현재 색상을 흰색으로 설정합니다.
    // 이는 텍스처의 색상을 그대로 사용하기 위함입니다.
    glColor3f(1.0f, 1.0f, 1.0f);

    // glBegin 함수로 쿼드(사각형) 그리기를 시작합니다.
    glBegin(GL_QUADS);

    // 천장의 각 정점에 대해 텍스처 좌표와 정점 좌표를 설정합니다.
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-3.5f, 0.0f, -3.5f);

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-3.5f, 0.0f, 3.5f);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(3.5f, 0.0f, 3.5f);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(3.5f, 0.0f, -3.5f);

    // 사각형 그리기를 종료합니다.
    glEnd();

    // 텍스처 매핑을 비활성화합니다.
    glDisable(GL_TEXTURE_2D);

    // 디스플레이 리스트 정의를 종료합니다.
    glEndList();
}


    // updateCamera 함수는 카메라의 뷰 매트릭스를 업데이트하는 함수입니다.
void updateCamera() {
        // glMatrixMode 함수로 현재 작업할 매트릭스 모드를 설정합니다.
        // GL_MODELVIEW 모드는 카메라 뷰 및 모델 변환을 위한 모드입니다.
        glMatrixMode(GL_MODELVIEW);

        // glLoadIdentity 함수로 현재 매트릭스를 단위 행렬로 초기화합니다.
        glLoadIdentity();

        // 카메라가 바라보는 위치를 계산합니다.
        // 이 위치는 카메라의 현재 위치와 방향(각도)에 기반합니다.
        float lookAtX = cameraX + 5.0f * sin(+cameraAngleY) * cos(cameraAngleX);
        float lookAtY = cameraY + 5.0f * sin(-cameraAngleX);
        float lookAtZ = cameraZ - 5.0f * cos(+cameraAngleY) * cos(cameraAngleX);

        // gluLookAt 함수로 카메라의 위치와 카메라가 바라보는 위치를 설정합니다.
        // 마지막 세 매개변수(0.0f, 1.0f, 0.0f)는 카메라의 '업' 방향을 나타냅니다.
        gluLookAt(cameraX, cameraY, cameraZ, lookAtX, lookAtY, lookAtZ, 0.0f, 1.0f, 0.0f);
    }


// display 함수는 OpenGL의 렌더링 루프에서 호출되어 화면에 그림을 그립니다.
void display() {
        // 화면을 지우는 함수입니다. 색상 버퍼와 깊이 버퍼를 모두 지웁니다.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 정점을 그릴 때의 정점 순서를 반시계 방향(Counter-Clockwise)으로 설정합니다.
        glFrontFace(GL_CCW);

        // 면 제거(Back-face culling)를 활성화합니다. 이는 뒤집힌 면을 렌더링하지 않도록 합니다.
        glEnable(GL_CULL_FACE);

        // 깊이 테스트를 활성화합니다. 이는 객체의 깊이를 고려하여 렌더링합니다.
        glEnable(GL_DEPTH_TEST);

        // 모델뷰 매트릭스 모드를 설정합니다.
        glMatrixMode(GL_MODELVIEW);

        // 현재 매트릭스를 단위 행렬로 초기화합니다.
        glLoadIdentity();

        // 카메라의 뷰 매트릭스를 업데이트합니다.
        updateCamera();

        // 모델을 그리기 전에 매트릭스 상태를 저장합니다.
        glPushMatrix();

        DrawModelAtOffset();

        // 저장했던 매트릭스 상태를 복원합니다.
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


        //미로 내부 배치
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

        // 메뉴가 활성화되어 있으면 메뉴를 그리는 코드가 여기에 위치합니다.
        if (isMenuDisplayed) {
            // 메뉴 그리기 관련 코드
        }

        // 이중 버퍼링을 사용하는 경우, 버퍼를 교체합니다.
        // 이는 현재 그려진 이미지를 화면에 표시하고, 다음 그림을 그리기 시작합니다.
        glutSwapBuffers();
    }


// keyboard 함수는 키보드의 키가 눌렸을 때 호출되는 콜백 함수입니다.
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'w':  // 'w' 키: 앞으로 이동
        if (!isWalking) {
            isWalking = true;
            PlaySoundFunction(true);  // 걷는 소리 시작
        }
        moveForward = true;
        break;
    case 's':  // 's' 키: 뒤로 이동
        if (!isWalking) {
            isWalking = true;
            PlaySoundFunction(true);  // 걷는 소리 시작
        }
        moveBackward = true;
        break;
    case 'a':  // 'a' 키: 왼쪽으로 이동
        if (!isWalking) {
            isWalking = true;
            PlaySoundFunction(true);  // 걷는 소리 시작
        }
        moveLeft = true;
        break;
    case 'd':  // 'd' 키: 오른쪽으로 이동
        if (!isWalking) {
            isWalking = true;
            PlaySoundFunction(true);  // 걷는 소리 시작
        }
        moveRight = true;
        break;
    case 'q':  // 'q' 키: 카메라를 왼쪽으로 회전
        cameraAngleY -= rotateSpeed;
        break;
    case 'e':  // 'e' 키: 카메라를 오른쪽으로 회전
        cameraAngleY += rotateSpeed;
        break;
    case '1':  // '1' 키: 카메라 속도 증가
        cameraSpeed += 0.003f;
        break;
    case '2':  // '2' 키: 카메라 속도 감소
        cameraSpeed = std::max(cameraSpeed - 0.003f, 0.0f);
        break;
    case ' ':  // 스페이스바: 점프
        JumpSoundFunction(true);
        jumping = true;  // 점프 활성화
        break;
    case 27:  // ESC 키: 프로그램 종료
        exit(0);
        break;
    }

    // 화면을 다시 그리도록 요청합니다.
    glutPostRedisplay();
}


// keyboardUp 함수는 키보드의 키가 떼어졌을 때 호출되는 콜백 함수입니다.
void keyboardUp(unsigned char key, int x, int y) {
    switch (key) {
    case 'w':  // 'w' 키: 앞으로 이동 중지
        moveForward = false;
        break;
    case 's':  // 's' 키: 뒤로 이동 중지
        moveBackward = false;
        break;
    case 'a':  // 'a' 키: 왼쪽으로 이동 중지
        moveLeft = false;
        break;
    case 'd':  // 'd' 키: 오른쪽으로 이동 중지
        moveRight = false;
        break;
    }

    // 이동 중이지 않다면 걷는 소리 중지
    if (!moveForward && !moveBackward && !moveLeft && !moveRight) {
        isWalking = false;
        PlaySoundFunction(false);
    }

    // 화면을 다시 그리도록 요청합니다.
    glutPostRedisplay();
}


// idle 함수는 이벤트가 발생하지 않을 때 호출되어, 지속적으로 변화하는 상태를 업데이트합니다.
void idle() {
    // 카메라 이동에 대한 X, Z 축 변화량을 초기화합니다.
    float moveX = 0.0f;
    float moveZ = 0.0f;

    // 카메라 이동 처리
    if (moveForward) {  // 앞으로 이동
        moveX += cameraSpeed * sin(cameraAngleY);
        moveZ -= cameraSpeed * cos(cameraAngleY);
    }
    if (moveBackward) {  // 뒤로 이동
        moveX -= cameraSpeed * sin(cameraAngleY);
        moveZ += cameraSpeed * cos(cameraAngleY);
    }
    if (moveLeft) {  // 왼쪽으로 이동
        moveX -= cameraSpeed * cos(cameraAngleY);
        moveZ -= cameraSpeed * sin(cameraAngleY);
    }
    if (moveRight) {  // 오른쪽으로 이동
        moveX += cameraSpeed * cos(cameraAngleY);
        moveZ += cameraSpeed * sin(cameraAngleY);
    }

    // 점프 처리
    if (jumping) {
        // 카메라의 Y축 위치가 2.0f에 도달하면 점프 상태를 중지합니다.
        if (cameraY < 2.0f) {
            cameraY += 0.01f;  // 점프 중에는 Y축 위치를 증가시킵니다.
        }
        else {
            jumping = false;  // 최대 높이에 도달하면 점프 상태를 중지합니다.
        }
    }
    else {
        // 중력 효과 적용: 카메라가 공중에 있으면 Y축 위치를 감소시킵니다.
        if (cameraY > 1.0f) {
            cameraY -= 0.01f;
        }
        else {
            cameraY = 1.0f;  // 땅에 도달하면 Y축 위치를 고정합니다.
        }
    }

    // 계산된 이동량을 적용하여 카메라 위치를 업데이트합니다.
    cameraX += moveX;
    cameraZ += moveZ;

    // 화면을 다시 그리도록 요청합니다.
    glutPostRedisplay();
}


// mouseMotion 함수는 마우스가 움직일 때 호출되는 콜백 함수입니다.
void mouseMotion(int x, int y) {
    // 이전 마우스 위치가 설정되었는지 확인합니다.
    if (lastMouseX != -1 && lastMouseY != -1) {
        // 마우스의 이동량을 계산합니다.
        float deltaX = x - lastMouseX;
        float deltaY = y - lastMouseY;

        // 마우스의 이동량에 따라 카메라 각도를 조절합니다.
        cameraAngleY += (deltaX) * 0.001f;
        cameraAngleX += (deltaY) * 0.001f;

        // 카메라의 상하 각도가 너무 크지 않도록 제한합니다.
        if (cameraAngleX > 1.5f) cameraAngleX = 1.5f;
        if (cameraAngleX < -1.5f) cameraAngleX = -1.5f;

        // 화면을 다시 그리도록 요청합니다.
        glutPostRedisplay();
    }

    // 현재 마우스 위치를 저장합니다.
    lastMouseX = x;
    lastMouseY = y;
}


// mouse 함수는 마우스 버튼 이벤트가 발생할 때 호출되는 콜백 함수입니다.
void mouse(int button, int state, int x, int y) {
    // 왼쪽 마우스 버튼 이벤트를 확인합니다.
    if (button == GLUT_LEFT_BUTTON) {
        // 마우스 버튼을 누른 경우
        if (state == GLUT_DOWN) {
            // 현재 마우스 위치를 저장합니다.
            lastMouseX = x;
            lastMouseY = y;
        }
        // 마우스 버튼을 떼는 경우
        else if (state == GLUT_UP) {
            // 마우스 위치 추적을 중지합니다.
            lastMouseX = -1;
            lastMouseY = -1;
        }
    }
}


// ResetCameraPosition 함수는 카메라의 위치와 각도를 초기 상태로 재설정합니다.
void ResetCameraPosition(int value) {
    // 카메라의 X, Y, Z 좌표를 초기 위치로 설정합니다.
    // 여기서 카메라는 원점 근처에 위치하며, 약간 높이 올라가고 약간 뒤로 물러난 위치에 있습니다.
    cameraX = 0.0f;
    cameraY = 1.0f;
    cameraZ = 5.0f;

    // 카메라의 수평 및 수직 회전 각도를 초기화합니다.
    cameraAngleY = 0.0f;
    cameraAngleX = 0.0f;

    // 화면을 다시 그리도록 요청합니다.
    glutPostRedisplay();
}


// update 함수는 주기적으로 호출되어 애니메이션 상태를 업데이트하는 함수입니다.
void update(int value) {
    // 큐브의 Y축 위치를 변경하여 큐브를 상하로 이동시킵니다.
    cubeY += cubeU;

    // 큐브가 특정 범위를 벗어나면 이동 방향을 반대로 전환합니다.
    if (cubeY > 5.0 || cubeY < 2.5) {
        cubeU = -cubeU;
    }

    // 화면 갱신을 요청합니다.
    glutPostRedisplay();

    // 16밀리초(약 60FPS에 해당)마다 이 함수를 다시 호출하여 애니메이션을 계속합니다.
    glutTimerFunc(16, update, 0);
}



// handleMouse 함수는 마우스 이벤트를 처리합니다.
void handleMouse(int button, int state, int x, int y) {
    // 마우스 오른쪽 버튼을 클릭했을 때의 이벤트를 처리합니다.
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        // 메뉴를 활성화하고 화면을 갱신합니다.
        isMenuDisplayed = true;
        glutPostRedisplay();
    }
}


// processMenu 함수는 메뉴 이벤트를 처리합니다.
void processMenu(int option) {
    switch (option) {
    case 1: // '1' 옵션 선택 시 초기 카메라 위치로 이동
        ResetCameraPosition(0);
        break;
        // 이곳에 다른 메뉴 옵션들을 추가할 수 있습니다.
    }

    // 메뉴 선택 후 메뉴를 비활성화하고 화면을 갱신합니다.
    isMenuDisplayed = false;
    glutPostRedisplay();
}


// main 함수는 프로그램의 시작점입니다.
int main(int argc, char** argv) {
    // GLUT 라이브러리를 초기화합니다.
    glutInit(&argc, argv);

    // 디스플레이 모드를 설정합니다. 여기서는 이중 버퍼링, RGB 색상 모드, 깊이 버퍼를 사용합니다.
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // 초기 윈도우 크기를 설정합니다.
    glutInitWindowSize(800, 600);

    // 윈도우를 생성하고 제목을 설정합니다.
    glutCreateWindow("3D_Maze");

    // OpenGL 관련 초기 설정을 수행하는 사용자 정의 함수를 호출합니다.
    init();
    //InitLight(); // 조명 설정 함수 (현재 비활성화)

    // 배경음악을 시작합니다.
    BackgroundSoundFunction(true);

    // 이벤트 처리 함수들을 등록합니다.
    glutDisplayFunc(display);        // 화면 그리기 함수
    glutReshapeFunc(reshape);        // 창 크기 변경 처리 함수
    glutKeyboardFunc(keyboard);      // 키보드 입력 처리 함수
    glutKeyboardUpFunc(keyboardUp);  // 키보드 입력 해제 처리 함수

    glutMouseFunc(mouse);            // 마우스 클릭 처리 함수
    glutMotionFunc(mouseMotion);     // 마우스 움직임 처리 함수

    glutIdleFunc(idle);              // 아무 이벤트도 발생하지 않을 때 호출할 함수

    // 텍스처와 디스플레이 리스트를 생성합니다.
    MakeTextures();
    MakeWallList();
    MakeFloorList();
    MakeCeilingList();
    ReadModel();
    ReadModel2();

    // 메뉴를 생성하고 설정합니다.
    glutCreateMenu(processMenu);
    glutAddMenuEntry("restart", 1);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    // 타이머 함수를 설정하여 주기적으로 `update` 함수를 호출합니다.
    glutTimerFunc(25, update, 0);

    // GLUT의 메인 이벤트 루프를 시작합니다.
    glutMainLoop();

    return 0;
}
