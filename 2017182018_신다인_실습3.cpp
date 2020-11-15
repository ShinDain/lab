#include <iostream>
#include <gl/glew.h> // 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <time.h>
#include <random>

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Mouse(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y);
void Timer(int value);


void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(0, 0); // 윈도우의 위치 지정
	glutInitWindowSize(800, 600); // 윈도우의 크기 지정
	glutCreateWindow("Example1"); // 윈도우 생성(윈도우 이름)
		//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew 초기화
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";
	glutDisplayFunc(drawScene); // 출력 콜백함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 콜백함수 지정
	glutMouseFunc(Mouse); // 키보드 입력 콜백함수 지정
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(1,Timer,1);
	glutMainLoop(); // 이벤트 처리 시작
}

GLfloat rectx1;
GLfloat recty1;
GLfloat rectx2;
GLfloat recty2;

GLfloat R = 0.2f;
GLfloat	G = 0.2f;
GLfloat	B = 0.2f;

GLfloat r;
GLfloat	g;
GLfloat	b;

bool bounce = false;
int direction = 0;

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	srand(time(NULL));

	//--- 변경된 배경색 설정
	glClearColor(R, G, B, 1.0f); // 바탕색을 변경
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기

	// 사각형 그리기
	glColor3f(r, g, b);
	glRectf(rectx1, recty1, rectx2, recty2);

	glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}

void Mouse(int button, int state, int x, int y)
{
	float Mx = x;
	float My = y;

	// 왼쪽 클릭
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		r = 0.01f * (rand() % 100);
		g = 0.01f * (rand() % 100);
		b = 0.01f * (rand() % 100);
		rectx1 = (Mx - 400) / 400 + 0.07;
		recty1 = -(My - 300) / 300 - 0.08;
		rectx2 = (Mx - 400) / 400 - 0.07;
		recty2 = -(My - 300) / 300 + 0.08;

		std::cout << Mx << " , " << My << std::endl;

		glutPostRedisplay();
	}
}
void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'a':
		bounce = true;
		break;
	case 's':
		bounce = false;
		break;
	}
	glutPostRedisplay(); //--- 배경색이 바뀔때마다 출력 콜백함수를 호출하여 화면을 refresh 한다
}

void Timer(int value)
{
	if (bounce == true)
	{
		// 좌상
		if (direction == 0)
		{
			rectx1 -= 0.01;
			rectx2 -= 0.01;
			recty1 += 0.01;
			recty2 += 0.01;
			if (rectx1 < -1)
				direction = 2;
			else if (recty2 > 1)
				direction = 1;
		}
		// 좌하
		else if (direction == 1)
		{
			rectx1 -= 0.01;
			rectx2 -= 0.01;
			recty1 -= 0.01;
			recty2 -= 0.01;
			if (rectx1 < -1)
				direction = 3;
			else if (recty1 < -1)
				direction = 0;
		}
		// 우상
		else if (direction == 2)
		{
			rectx1 += 0.01;
			rectx2 += 0.01;
			recty1 += 0.01;
			recty2 += 0.01;
			if (rectx2 > 1)
				direction = 0;
			else if (recty2 > 1)
				direction = 3;
		}
		// 우하
		else if (direction == 3)
		{
			rectx1 += 0.01;
			rectx2 += 0.01;
			recty1 -= 0.01;
			recty2 -= 0.01;
			if (rectx2 > 1)
				direction = 1;
			else if (recty1 < -1)
				direction = 3;
		}
	}


	glutPostRedisplay();
	glutTimerFunc(10, Timer, 1);
}