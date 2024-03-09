#define _CRT_SECURE_NO_WARNINGS
#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <vector> 
#include <cstdlib>
using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846 //���� �� ����
#endif

//3���� ��ǥ�� �����ϴ� point structure
struct Point {
    GLfloat x, y, z;
};

vector<Point> clickedPoints; //Ŭ���� ���� �����ϴ� ���� ����ü ����
int sweepResolutionMod = 6; //������ 360�� ȸ����ų�� 6�� �����Ѵٴ� ����
double rotationAngle = 0.0; //���� ���� ���� �� �ʱ�ȭ

//�������� ����Ͽ� �޴��� �����ϴ� �ڵ�
enum Menu {
    SET_ANGLE,
    MENU_MODE_X,
    MENU_MODE_Y,
    MENU_MODE_Z,
    SAVE_MODE,
    MENU_CLEAR_POINTS,
    MENU_EXIT
};

//� ���� �������� ȸ������ �����ϴ� ���� �����ϴ� ���� ���� �� �ʱ�ȭ
//���� ���� �������� ������ Y���� �������� ȸ����Ŵ
int rotationMode = MENU_MODE_Y;

//clickedPoints ���Ϳ� ����� ������ �ʱ�ȭ��Ű�� �Լ�
void ClearPoints() {
    clickedPoints.clear();
    glutPostRedisplay();
}

// SaveModel �Լ��� ������ ���� �������� ȸ���� ������ ��ǥ�� �ε����� ���Ͽ� �����մϴ�.
void SaveModel() {
    // ���� ������ ���� �� ���� ���� �õ�
    FILE* fout;
    if (fopen_s(&fout, "..\\source\\myModel2.dat", "w") != 0) {
        cerr << "Error opening file for writing" << endl;  // ���� ���� ���� �� ���� �޽��� ���
        return;  // �Լ� ����
    }

    // VERTEX �κ� ����: Ŭ���� ���� ���� sweepResolutionMod + 1�� ���� ������ ���� ������ ���
    fprintf(fout, "VERTEX = %lu\n", clickedPoints.size() * (sweepResolutionMod + 1));

    // ��� Ŭ���� ���� ���� �ݺ�
    for (size_t i = 0; i < clickedPoints.size(); i++) {
        // sweepResolutionMod�� ���� �� ���� ȸ����Ű�� ��ǥ�� ���
        for (int j = 0; j <= sweepResolutionMod; j++) {
            // ȸ�� ���� ��� (360���� sweepResolutionMod�� ���� ���� j�� ����)
            double theta = (360.0 / sweepResolutionMod) * j;
            theta = theta * (M_PI / 180);  // �������� ��ȯ

            // Ŭ���� ���� ��ǥ�� ǥ��ȭ(����ȭ)
            GLfloat tempX = (clickedPoints[i].x - 250) / 250;
            GLfloat tempY = (clickedPoints[i].y - 250) / 250;
            GLfloat tempZ = 0.0;

            // ȸ�� ��忡 ���� X, Y, Z �� �� �ϳ��� �߽����� ȸ��
            if (rotationMode == MENU_MODE_X) {  // X �� ȸ��
                GLfloat newX = tempX;
                GLfloat newY = (tempZ * cos(rotationAngle + theta)) + (tempY * sin(rotationAngle + theta));
                GLfloat newZ = (tempY * cos(rotationAngle + theta)) - (tempZ * sin(rotationAngle + theta));
                fprintf(fout, "%.1f %.1f %.1f\n", newX * 250 + 250, newY * 250 + 250, newZ * 250 + 250);
            }

            else if (rotationMode == MENU_MODE_Y) {  // Y �� ȸ��
                GLfloat newX = (tempX * cos(rotationAngle + theta)) + (tempZ * sin(rotationAngle + theta));
                GLfloat newY = tempY;
                GLfloat newZ = (tempZ * cos(rotationAngle + theta)) - (tempX * sin(rotationAngle + theta));
                fprintf(fout, "%.1f %.1f %.1f\n", newX * 250 + 250, newY * 250 + 250, newZ * 250 + 250);
            }
            else if (rotationMode == MENU_MODE_Z) {  // Z �� ȸ��
                GLfloat newX = (tempX * cos(rotationAngle + theta)) - (tempY * sin(rotationAngle + theta));
                GLfloat newY = (tempY * cos(rotationAngle + theta)) + (tempX * sin(rotationAngle + theta));
                GLfloat newZ = tempZ;
                fprintf(fout, "%.1f %.1f %.1f\n", newX * 250 + 250, newY * 250 + 250, newZ * 250 + 250);
            }
        }
    }

    // FACE(�ﰢ��) �κ� ����: Ŭ���� �� ������ ���� ����
    fprintf(fout, "FACE = %lu\n", ((clickedPoints.size() - 1) * (sweepResolutionMod * 2)));
    for (size_t i = 0; i < clickedPoints.size() - 1; i++) {
        for (int j = 0; j < sweepResolutionMod; j++) {
            // ���� �� ���� ����Ʈ�� �ε��� ���
            int current = i * (sweepResolutionMod + 1) + j;
            int next = (i + 1) * (sweepResolutionMod + 1) + j;
            // �� ���� �����ϴ� 3���� ������ ���
            fprintf(fout, "%d %d %d\n", current, next + 1, current + 1);
            fprintf(fout, "%d %d %d\n", current, next, next + 1);
        }
    }
    // ���� �ݱ�
    fclose(fout);
}


// revolutionSurface �Լ��� ����ڰ� ���� ���� �������� ������ �Է¹��� ������ ���� ȸ����Ű�� ����Ʈ�� �ǽð����� �����ݴϴ�.
void revolutionSurface() {
    // ���� ���۸� ����ϴ�.
    glClear(GL_COLOR_BUFFER_BIT);

    // �𵨺� ��Ʈ���� ���� �����մϴ�.
    glMatrixMode(GL_MODELVIEW);
    // ���� ��Ʈ������ ���� ��ķ� �ʱ�ȭ�մϴ�.
    glLoadIdentity();

    // ī�޶��� ��ġ�� ������ �����մϴ�.
    gluLookAt(0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);

    // ����Ʈ�� X, Y, Z ���� �׸��� �ڵ�
    glBegin(GL_LINES);
    glVertex3f(500.0, 0.0, 0.0);  // X ��
    glVertex3f(-500.0, 0.0, 0.0); // X ��
    glVertex3f(0.0, -500.0, 0.0); // Y ��
    glVertex3f(0.0, 500.0, 0.0);  // Y ��
    glVertex3f(0.0, 0.0, -500.0); // Z ��
    glVertex3f(0.0, 0.0, 500.0);  // Z ��
    glEnd();

    // sweepResolutionMod�� ���� �� ���� ȸ����Ű�� ��ǥ�� ���
    for (int i = 0; i <= sweepResolutionMod; i++) {
        double theta = i > 0 ? (360 / sweepResolutionMod) * i : 0;
        theta = theta * (M_PI / 180);  // �������� ��ȯ

        glPointSize(3.0);
        glBegin(GL_POINTS);

        // ȸ�� ��忡 ���� X, Y, Z �� �� �ϳ��� �߽����� ȸ��
        if (rotationMode == MENU_MODE_X) {  // X �� ȸ��
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
        else if (rotationMode == MENU_MODE_Y) {  // Y �� ȸ��
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
        else if (rotationMode == MENU_MODE_Z) {  // Z �� ȸ��
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
    // ���� ���۸��� ����ϴ� ���, ���۸� ��ü�մϴ�.
    glutSwapBuffers();
}



// sor �𵨸� ���� Ȯ�� exe ���� ���� �Լ�
void runExeFile(const char* exePath) {
    system(exePath);
}


// createMenu �Լ��� ����Ʈ���� ��Ŭ���� �ϸ� �˾��Ǵ� �޴��� �����մϴ�.
void createMenu() {
    // ���� ǥ������ ����Ͽ� �޴� �׸� ���� �� ȣ��� �ݹ� �Լ��� �����մϴ�.
    glutCreateMenu([](int value) {
        switch (value) {
        case SET_ANGLE: // ȸ�� ���� ���� �ɼ�
            cout << "360�� ����� ������ �Է��Ͻʽÿ�: ";
            cin >> sweepResolutionMod;  // ����� �Է� ����
            sweepResolutionMod = 360 / sweepResolutionMod;  // ȸ�� �ػ� ���
            glutPostRedisplay();  // ȭ�� ����
            break;
        case MENU_MODE_X: // X�� ȸ�� ��� ����
            ClearPoints();  // ���� ������ ����
            rotationMode = MENU_MODE_X;  // ȸ�� ��带 X������ ����
            break;
        case MENU_MODE_Y: // Y�� ȸ�� ��� ����
            ClearPoints();  // ���� ������ ����
            rotationMode = MENU_MODE_Y;  // ȸ�� ��带 Y������ ����
            break;
        case MENU_MODE_Z: // Z�� ȸ�� ��� ����
            ClearPoints();  // ���� ������ ����
            rotationMode = MENU_MODE_Z;  // ȸ�� ��带 Z������ ����
            break;
        case SAVE_MODE: // �� ���� �ɼ�
            SaveModel();  // �� ���� �Լ� ȣ��
            break;
        case MENU_CLEAR_POINTS: // ������ ����� �ɼ�
            ClearPoints();  // ������ ����
            break;
        case MENU_EXIT: // ���α׷� ���� �ɼ�
            exit(0);  // ���α׷� ����
            break;
        }
        glutPostRedisplay();  // ȭ�� ����
        });

    // �޴��� �׸��� �߰��մϴ�.
    glutAddMenuEntry("Set Rotation Angle", SET_ANGLE);
    glutAddMenuEntry("Set Rotation Axis to X", MENU_MODE_X);
    glutAddMenuEntry("Set Rotation Axis to Y", MENU_MODE_Y);
    glutAddMenuEntry("Set Rotation Axis to Z", MENU_MODE_Z);
    glutAddMenuEntry("Save the data", SAVE_MODE);
    glutAddMenuEntry("Clear Points", MENU_CLEAR_POINTS);
    glutAddMenuEntry("Exit the SOR", MENU_EXIT);

    // ������ �޴��� ��Ŭ���� �����մϴ�.
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// mouseClick �Լ��� ���콺 Ŭ�� �̺�Ʈ�� ó���մϴ�.
void mouseClick(int button, int state, int x, int y) {
    // ���� ���콺 ��ư�� ������ ���� �̺�Ʈ�� ó���մϴ�.
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // ���ο� ��(Point) ��ü�� �����ϰ� ���콺 Ŭ�� ��ġ�� �������� ��ǥ�� �����մϴ�.
        Point newPoint;
        newPoint.x = static_cast<GLfloat>(x);            // ���콺 X ��ǥ
        newPoint.y = static_cast<GLfloat>(500 - y);      // ���콺 Y ��ǥ (����Ʈ ��ǥ��� ��ȯ)
        newPoint.z = 0.0;                                // Z ��ǥ�� 0���� ����

        // ������ ���� clickedPoints ���Ϳ� �߰��մϴ�.
        clickedPoints.push_back(newPoint);
    }

    // ȭ�� ������ ��û�մϴ�.
    glutPostRedisplay();
}



// keyboard �Լ��� Ű���� �̺�Ʈ�� ó���մϴ�.
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 's':  // 's' Ű: �� ����
        SaveModel();  // SaveModel �Լ��� ȣ���Ͽ� ���� �����͸� ���Ͽ� ����
        break;

    case 'r':  // 'r' Ű: ȸ�� ���� ����
        cout << "360�� ����� ������ �Է��Ͻʽÿ�: ";
        cin >> sweepResolutionMod;  // ����ڷκ��� �Է� ����
        sweepResolutionMod = 360 / sweepResolutionMod;  // ȸ�� �ػ� ���
        glutPostRedisplay();  // ȭ�� ����
        break;

    case 'f':  // 'f' Ű: �ܺ� ���α׷� ����
        runExeFile("Sor_Check.exe");  // �ܺ� ���� ���� ȣ��
        break;

    case 27:  // ESC Ű: ���α׷� ����
        exit(0);  // ���α׷� ����
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