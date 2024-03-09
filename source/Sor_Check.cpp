#include <GL/glut.h>
#include <stdio.h>
#include <string>  // 추가된 부분

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

std::string fname = "..\\source\\myModel2.dat";  // 변경된 부분

// ReadModel 함수는 파일에서 모델 데이터를 읽어와 메모리에 저장합니다.
void ReadModel() {
    FILE* f1;  // 파일 포인터
    char s[81];  // 문자열을 읽기 위한 버퍼
    int i;

    // 기존의 메모리 할당 해제
    if (mpoint != NULL) delete[] mpoint;
    if (mface != NULL) delete[] mface;

    // 파일 열기. 실패 시 오류 메시지 출력 후 함수 종료
    if ((f1 = fopen(fname.c_str(), "rt")) == NULL) {
        printf("No file\n");
        exit(0);
    }

    // 파일에서 정점(Vertex) 개수 읽기
    fscanf(f1, "%s", s); // Read and print the first string
    fscanf(f1, "%s", s); // Read and print the second string
    fscanf(f1, "%d", &pnum);

    // 정점 데이터 동적 할당 및 읽기
    mpoint = new Point[pnum];
    for (i = 0; i < pnum; i++) {
        fscanf(f1, "%f", &mpoint[i].x);
        fscanf(f1, "%f", &mpoint[i].y);
        fscanf(f1, "%f", &mpoint[i].z);
    }

    // 파일에서 면(Face) 개수 읽기
    fscanf(f1, "%s", s); // Read and print the third string
    fscanf(f1, "%s", s); // Read and print the fourth string
    fscanf(f1, "%d", &fnum);
    // 면 데이터 동적 할당 및 읽기
    mface = new Face[fnum];
    for (i = 0; i < fnum; i++) {
        fscanf(f1, "%d", &mface[i].ip[0]);
        fscanf(f1, "%d", &mface[i].ip[1]);
        fscanf(f1, "%d", &mface[i].ip[2]);
    }

    // 파일 닫기
    fclose(f1);
}



// InitLight 함수는 조명과 재질 속성을 설정합니다.
void InitLight() {
    // 재질 속성 설정
    GLfloat mat_diffuse[] = { 0.5, 0.4, 0.3, 1.0 };
    GLfloat mat_specular[] = { 0.3, 0.3, 0.3, 1.0 };
    GLfloat mat_ambient[] = { 0.5, 0.4, 0.3, 1.0 };
    GLfloat mat_shininess[] = { 2.0 };

    // 조명 속성 설정
    GLfloat light_specular[] = { 0.5, 0.5, 0.5, 0.5 };
    GLfloat light_diffuse[] = { 0.5, 0.5, 0.5, 0.5 };
    GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 0.5 };
    GLfloat light_position[] = { 300.0, 300.0, 300.0, 1.0 };

    // 조명과 재질 속성 적용
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



// cnormal 함수는 주어진 세 점을 사용해 면의 법선 벡터를 계산합니다.
Point cnormal(Point a, Point b, Point c) {
    Point p, q, r;
    double val;

    // 벡터 p, q 계산
    p.x = a.x - b.x; p.y = a.y - b.y; p.z = a.z - b.z;
    q.x = c.x - b.x; q.y = c.y - b.y; q.z = c.z - b.z;

    // 벡터 외적을 사용해 법선 벡터 r 계산
    r.x = p.y * q.z - p.z * q.y;
    r.y = p.z * q.x - p.x * q.z;
    r.z = p.x * q.y - p.y * q.x;

    // 벡터 정규화
    val = sqrt(r.x * r.x + r.y * r.y + r.z * r.z);
    r.x = r.x / val; r.y = r.y / val;
    r.z = r.z / val;
    return r;
}


// DrawModelAtOffset 함수는 모델을 그리는 함수입니다.
void DrawModelAtOffset(void) {
    glTranslatef(0.0f, 0.0f, 0.0f);  // 모델 위치 이동
    glScalef(0.5f, 0.5f, 0.5f);      // 모델 크기 조정
    glColor3f(0, 0, 1);              // 색상 설정

    glBegin(GL_TRIANGLES);  // 삼각형으로 모델을 그림
    for (int i = 0; i < fnum; i++) {
        Point norm = cnormal(mpoint[mface[i].ip[2]], mpoint[mface[i].ip[1]], mpoint[mface[i].ip[0]]);
        glNormal3f(norm.x, norm.y, norm.z);  // 법선 벡터 설정
        glVertex3f(mpoint[mface[i].ip[0]].x, mpoint[mface[i].ip[0]].y, mpoint[mface[i].ip[0]].z);
        glVertex3f(mpoint[mface[i].ip[1]].x, mpoint[mface[i].ip[1]].y, mpoint[mface[i].ip[1]].z);
        glVertex3f(mpoint[mface[i].ip[2]].x, mpoint[mface[i].ip[2]].y, mpoint[mface[i].ip[2]].z);
    }
    glEnd();  // 삼각형 그리기 종료
}



// GLSetupRC 함수는 OpenGL 렌더링 컨텍스트를 설정합니다.
void GLSetupRC(void) {
    glEnable(GL_DEPTH_TEST);  // 깊이 테스트 활성화

    // 투영 매트릭스 설정
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0, 1.0, 1.0, 2000.0);

    // 모델뷰 매트릭스 설정
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(500.0, 400.0, 500.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);  // 카메라 위치 및 방향 설정
}


void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // 변환 행렬을 다시 저장
    glPushMatrix();

    // 이동한 위치에 티팟 그리기
    DrawModelAtOffset();

    // 변환 행렬을 초기 상태로 되돌리기
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