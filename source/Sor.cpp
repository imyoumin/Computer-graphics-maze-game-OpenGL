#define _CRT_SECURE_NO_WARNINGS
#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <vector> 
#include <cstdlib>
using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846 //파이 값 정의
#endif

//3차원 좌표를 저장하는 point structure
struct Point {
    GLfloat x, y, z;
};

vector<Point> clickedPoints; //클릭한 점을 저장하는 벡터 구조체 선언
int sweepResolutionMod = 6; //정점을 360도 회전시킬때 6번 저장한다는 변수
double rotationAngle = 0.0; //각도 변수 선언 및 초기화

//열거형을 사용하여 메뉴를 정의하는 코드
enum Menu {
    SET_ANGLE,
    MENU_MODE_X,
    MENU_MODE_Y,
    MENU_MODE_Z,
    SAVE_MODE,
    MENU_CLEAR_POINTS,
    MENU_EXIT
};

//어떤 축을 기준으로 회전할지 결정하는 값을 저장하는 변수 선언 및 초기화
//따로 축을 설정하지 않으면 Y축을 기준으로 회전시킴
int rotationMode = MENU_MODE_Y;

//clickedPoints 벡터에 저장된 값들을 초기화시키는 함수
void ClearPoints() {
    clickedPoints.clear();
    glutPostRedisplay();
}

// SaveModel 함수는 임의의 축을 기준으로 회전된 점들의 좌표와 인덱스를 파일에 저장합니다.
void SaveModel() {
    // 파일 포인터 선언 및 파일 열기 시도
    FILE* fout;
    if (fopen_s(&fout, "..\\source\\myModel2.dat", "w") != 0) {
        cerr << "Error opening file for writing" << endl;  // 파일 열기 실패 시 오류 메시지 출력
        return;  // 함수 종료
    }

    // VERTEX 부분 저장: 클릭된 점의 수에 sweepResolutionMod + 1을 곱한 값으로 정점 개수를 기록
    fprintf(fout, "VERTEX = %lu\n", clickedPoints.size() * (sweepResolutionMod + 1));

    // 모든 클릭된 점에 대해 반복
    for (size_t i = 0; i < clickedPoints.size(); i++) {
        // sweepResolutionMod에 따라 각 점을 회전시키며 좌표를 계산
        for (int j = 0; j <= sweepResolutionMod; j++) {
            // 회전 각도 계산 (360도를 sweepResolutionMod로 나눈 값에 j를 곱함)
            double theta = (360.0 / sweepResolutionMod) * j;
            theta = theta * (M_PI / 180);  // 라디안으로 변환

            // 클릭된 점의 좌표를 표준화(정규화)
            GLfloat tempX = (clickedPoints[i].x - 250) / 250;
            GLfloat tempY = (clickedPoints[i].y - 250) / 250;
            GLfloat tempZ = 0.0;

            // 회전 모드에 따라 X, Y, Z 축 중 하나를 중심으로 회전
            if (rotationMode == MENU_MODE_X) {  // X 축 회전
                GLfloat newX = tempX;
                GLfloat newY = (tempZ * cos(rotationAngle + theta)) + (tempY * sin(rotationAngle + theta));
                GLfloat newZ = (tempY * cos(rotationAngle + theta)) - (tempZ * sin(rotationAngle + theta));
                fprintf(fout, "%.1f %.1f %.1f\n", newX * 250 + 250, newY * 250 + 250, newZ * 250 + 250);
            }

            else if (rotationMode == MENU_MODE_Y) {  // Y 축 회전
                GLfloat newX = (tempX * cos(rotationAngle + theta)) + (tempZ * sin(rotationAngle + theta));
                GLfloat newY = tempY;
                GLfloat newZ = (tempZ * cos(rotationAngle + theta)) - (tempX * sin(rotationAngle + theta));
                fprintf(fout, "%.1f %.1f %.1f\n", newX * 250 + 250, newY * 250 + 250, newZ * 250 + 250);
            }
            else if (rotationMode == MENU_MODE_Z) {  // Z 축 회전
                GLfloat newX = (tempX * cos(rotationAngle + theta)) - (tempY * sin(rotationAngle + theta));
                GLfloat newY = (tempY * cos(rotationAngle + theta)) + (tempX * sin(rotationAngle + theta));
                GLfloat newZ = tempZ;
                fprintf(fout, "%.1f %.1f %.1f\n", newX * 250 + 250, newY * 250 + 250, newZ * 250 + 250);
            }
        }
    }

    // FACE(삼각형) 부분 저장: 클릭된 점 사이의 면을 저장
    fprintf(fout, "FACE = %lu\n", ((clickedPoints.size() - 1) * (sweepResolutionMod * 2)));
    for (size_t i = 0; i < clickedPoints.size() - 1; i++) {
        for (int j = 0; j < sweepResolutionMod; j++) {
            // 현재 및 다음 포인트의 인덱스 계산
            int current = i * (sweepResolutionMod + 1) + j;
            int next = (i + 1) * (sweepResolutionMod + 1) + j;
            // 각 면을 구성하는 3개의 정점을 기록
            fprintf(fout, "%d %d %d\n", current, next + 1, current + 1);
            fprintf(fout, "%d %d %d\n", current, next, next + 1);
        }
    }
    // 파일 닫기
    fclose(fout);
}


// revolutionSurface 함수는 사용자가 정한 축을 기준으로 점들을 입력받은 각도에 따라 회전시키고 뷰포트에 실시간으로 보여줍니다.
void revolutionSurface() {
    // 색상 버퍼를 지웁니다.
    glClear(GL_COLOR_BUFFER_BIT);

    // 모델뷰 매트릭스 모드로 설정합니다.
    glMatrixMode(GL_MODELVIEW);
    // 현재 매트릭스를 단위 행렬로 초기화합니다.
    glLoadIdentity();

    // 카메라의 위치와 방향을 설정합니다.
    gluLookAt(0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);

    // 뷰포트에 X, Y, Z 축을 그리는 코드
    glBegin(GL_LINES);
    glVertex3f(500.0, 0.0, 0.0);  // X 축
    glVertex3f(-500.0, 0.0, 0.0); // X 축
    glVertex3f(0.0, -500.0, 0.0); // Y 축
    glVertex3f(0.0, 500.0, 0.0);  // Y 축
    glVertex3f(0.0, 0.0, -500.0); // Z 축
    glVertex3f(0.0, 0.0, 500.0);  // Z 축
    glEnd();

    // sweepResolutionMod에 따라 각 점을 회전시키며 좌표를 계산
    for (int i = 0; i <= sweepResolutionMod; i++) {
        double theta = i > 0 ? (360 / sweepResolutionMod) * i : 0;
        theta = theta * (M_PI / 180);  // 라디안으로 변환

        glPointSize(3.0);
        glBegin(GL_POINTS);

        // 회전 모드에 따라 X, Y, Z 축 중 하나를 중심으로 회전
        if (rotationMode == MENU_MODE_X) {  // X 축 회전
            for (size_t i = 0; i < clickedPoints.size(); i++) {
                GLfloat tempX = (clickedPoints[i].x - 250) / 250;
                GLfloat tempY = (clickedPoints[i].y - 250) / 250;
                GLfloat tempZ = 0.0;

                GLfloat newX = tempX;
                GLfloat newY = (tempY * cos(theta)) - (tempZ * sin(theta));
                GLfloat newZ = (tempY * sin(theta)) + (tempZ * cos(theta));

                glVertex3f(newX, newY, newZ);
            }
            glEnd();
        }
        else if (rotationMode == MENU_MODE_Y) {  // Y 축 회전
            for (size_t i = 0; i < clickedPoints.size(); i++) {
                GLfloat tempX = (clickedPoints[i].x - 250) / 250;
                GLfloat tempY = (clickedPoints[i].y - 250) / 250;
                GLfloat tempZ = 0.0;

                GLfloat newX = (tempX * cos(theta)) + (tempZ * sin(theta));
                GLfloat newY = tempY;
                GLfloat newZ = (tempZ * cos(theta)) - (tempX * sin(theta));

                glVertex3f(newX, newY, newZ);
            }
            glEnd();
        }
        else if (rotationMode == MENU_MODE_Z) {  // Z 축 회전
            for (size_t i = 0; i < clickedPoints.size(); i++) {
                GLfloat tempX = (clickedPoints[i].x - 250) / 250;
                GLfloat tempY = (clickedPoints[i].y - 250) / 250;
                GLfloat tempZ = 0.0;

                GLfloat newX = (tempX * cos(theta)) - (tempY * sin(theta));
                GLfloat newY = (tempX * sin(theta)) + (tempY * cos(theta));
                GLfloat newZ = tempZ;

                glVertex3f(newX, newY, newZ);
            }
            glEnd();
        }
    }
    // 이중 버퍼링을 사용하는 경우, 버퍼를 교체합니다.
    glutSwapBuffers();
}



// sor 모델링 파일 확인 exe 파일 실행 함수
void runExeFile(const char* exePath) {
    system(exePath);
}


// createMenu 함수는 뷰포트에서 우클릭을 하면 팝업되는 메뉴를 구현합니다.
void createMenu() {
    // 람다 표현식을 사용하여 메뉴 항목 선택 시 호출될 콜백 함수를 정의합니다.
    glutCreateMenu([](int value) {
        switch (value) {
        case SET_ANGLE: // 회전 각도 설정 옵션
            cout << "360의 약수로 각도를 입력하십시오: ";
            cin >> sweepResolutionMod;  // 사용자 입력 받음
            sweepResolutionMod = 360 / sweepResolutionMod;  // 회전 해상도 계산
            glutPostRedisplay();  // 화면 갱신
            break;
        case MENU_MODE_X: // X축 회전 모드 설정
            ClearPoints();  // 기존 점들을 지움
            rotationMode = MENU_MODE_X;  // 회전 모드를 X축으로 설정
            break;
        case MENU_MODE_Y: // Y축 회전 모드 설정
            ClearPoints();  // 기존 점들을 지움
            rotationMode = MENU_MODE_Y;  // 회전 모드를 Y축으로 설정
            break;
        case MENU_MODE_Z: // Z축 회전 모드 설정
            ClearPoints();  // 기존 점들을 지움
            rotationMode = MENU_MODE_Z;  // 회전 모드를 Z축으로 설정
            break;
        case SAVE_MODE: // 모델 저장 옵션
            SaveModel();  // 모델 저장 함수 호출
            break;
        case MENU_CLEAR_POINTS: // 점들을 지우는 옵션
            ClearPoints();  // 점들을 지움
            break;
        case MENU_EXIT: // 프로그램 종료 옵션
            exit(0);  // 프로그램 종료
            break;
        }
        glutPostRedisplay();  // 화면 갱신
        });

    // 메뉴에 항목을 추가합니다.
    glutAddMenuEntry("Set Rotation Angle", SET_ANGLE);
    glutAddMenuEntry("Set Rotation Axis to X", MENU_MODE_X);
    glutAddMenuEntry("Set Rotation Axis to Y", MENU_MODE_Y);
    glutAddMenuEntry("Set Rotation Axis to Z", MENU_MODE_Z);
    glutAddMenuEntry("Save the data", SAVE_MODE);
    glutAddMenuEntry("Clear Points", MENU_CLEAR_POINTS);
    glutAddMenuEntry("Exit the SOR", MENU_EXIT);

    // 생성된 메뉴를 우클릭에 연결합니다.
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// mouseClick 함수는 마우스 클릭 이벤트를 처리합니다.
void mouseClick(int button, int state, int x, int y) {
    // 왼쪽 마우스 버튼이 눌렸을 때의 이벤트를 처리합니다.
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // 새로운 점(Point) 객체를 생성하고 마우스 클릭 위치를 기준으로 좌표를 설정합니다.
        Point newPoint;
        newPoint.x = static_cast<GLfloat>(x);            // 마우스 X 좌표
        newPoint.y = static_cast<GLfloat>(500 - y);      // 마우스 Y 좌표 (뷰포트 좌표계로 변환)
        newPoint.z = 0.0;                                // Z 좌표는 0으로 설정

        // 생성된 점을 clickedPoints 벡터에 추가합니다.
        clickedPoints.push_back(newPoint);
    }

    // 화면 갱신을 요청합니다.
    glutPostRedisplay();
}



// keyboard 함수는 키보드 이벤트를 처리합니다.
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 's':  // 's' 키: 모델 저장
        SaveModel();  // SaveModel 함수를 호출하여 현재 데이터를 파일에 저장
        break;

    case 'r':  // 'r' 키: 회전 각도 설정
        cout << "360의 약수로 각도를 입력하십시오: ";
        cin >> sweepResolutionMod;  // 사용자로부터 입력 받음
        sweepResolutionMod = 360 / sweepResolutionMod;  // 회전 해상도 계산
        glutPostRedisplay();  // 화면 갱신
        break;

    case 'f':  // 'f' 키: 외부 프로그램 실행
        runExeFile("Sor_Check.exe");  // 외부 실행 파일 호출
        break;

    case 27:  // ESC 키: 프로그램 종료
        exit(0);  // 프로그램 종료
        break;
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Sor");
    createMenu();

    glutDisplayFunc(revolutionSurface);
    glutMouseFunc(mouseClick);
    glutKeyboardFunc(keyboard);

    glutMainLoop();

    return 0;
}