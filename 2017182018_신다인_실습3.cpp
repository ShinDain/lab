#include <iostream>
#include <gl/glew.h> // �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <time.h>
#include <random>

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Mouse(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y);
void Timer(int value);


void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	//--- ������ �����ϱ�
	glutInit(&argc, argv); // glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // ���÷��� ��� ����
	glutInitWindowPosition(0, 0); // �������� ��ġ ����
	glutInitWindowSize(800, 600); // �������� ũ�� ����
	glutCreateWindow("Example1"); // ������ ����(������ �̸�)
		//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew �ʱ�ȭ
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";
	glutDisplayFunc(drawScene); // ��� �ݹ��Լ��� ����
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �ݹ��Լ� ����
	glutMouseFunc(Mouse); // Ű���� �Է� �ݹ��Լ� ����
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(1,Timer,1);
	glutMainLoop(); // �̺�Ʈ ó�� ����
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

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	srand(time(NULL));

	//--- ����� ���� ����
	glClearColor(R, G, B, 1.0f); // �������� ����
	glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�

	// �簢�� �׸���
	glColor3f(r, g, b);
	glRectf(rectx1, recty1, rectx2, recty2);

	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
	glViewport(0, 0, w, h);
}

void Mouse(int button, int state, int x, int y)
{
	float Mx = x;
	float My = y;

	// ���� Ŭ��
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
	glutPostRedisplay(); //--- ������ �ٲ𶧸��� ��� �ݹ��Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
}

void Timer(int value)
{
	if (bounce == true)
	{
		// �»�
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
		// ����
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
		// ���
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
		// ����
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