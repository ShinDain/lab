#include <iostream>
#include <gl/glew.h> // �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <fstream>

using namespace std;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Mouse(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y);\
void Timer(int value);
GLchar* filetobuf(const GLchar* filename);

// ���̴� ���� �б�
GLchar* filetobuf(const GLchar* filename)
{
	cout << "����" << endl;

	ifstream readFile;             //���� ������ ���� ����
	GLchar tmp[256];
	GLchar str[512] = { NULL };

	readFile.open(filename);    //���� ����
	if (readFile.is_open())    //������ ���ȴ��� Ȯ��
	{
		while (!readFile.eof())    //���� ������ �о����� Ȯ��
		{
			readFile.getline(tmp, 256);    //���پ� �о����
			if (str == NULL)
			{
				strcpy_s(str, tmp);
				strcat_s(str, "\n");
			}
			else
			{
				strcat_s(str, tmp);
				strcat_s(str, "\n");
			}
		}
		cout << "����2" << endl;
	}
	readFile.close();    //���� �ݱ�

	cout << str << endl;

	return str;
}

// ���̴� ����
GLuint make_shaderProgram()
{
	GLint result;
	GLchar errorLog[512];

	GLchar* vertexsource;
	vertexsource = filetobuf("vertex.glsl");

	//--- ���ؽ� ���̴� �о� �����ϰ� ������ �ϱ�
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexsource, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		cerr << "ERROR: vertex shader ������ ����\n" << errorLog << endl;
		return false;
	}

	GLchar* fragmentsource;
	fragmentsource = filetobuf("fragment.glsl");

	//--- �����׸�Ʈ ���̴� �о� �����ϰ� �������ϱ�
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentsource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		cerr << "ERROR: fragment shader ������ ����\n" << errorLog << endl;
		return false;
	}

	GLuint ShaderProgramID = glCreateProgram(); //--- ���̴� ���α׷� �����
	glAttachShader(ShaderProgramID, vertexShader); //--- ���̴� ���α׷��� ���ؽ� ���̴� ���̱�
	glAttachShader(ShaderProgramID, fragmentShader); //--- ���̴� ���α׷��� �����׸�Ʈ ���̴� ���̱�
	glLinkProgram(ShaderProgramID); //--- ���̴� ���α׷� ��ũ�ϱ�
	glDeleteShader(vertexShader); //--- ���̴� ���α׷��� ��ũ�Ͽ� ���̴� ��ü ��ü�� ���� ����
	glDeleteShader(fragmentShader);

	glGetProgramiv(ShaderProgramID, GL_LINK_STATUS, &result); // ---���̴��� �� ����Ǿ����� üũ�ϱ�
	if (!result)
	{
		glGetProgramInfoLog(ShaderProgramID, 512, NULL, errorLog);
		cerr << "ERROR: shader program ���� ����\n" << errorLog << endl;
		return false;
	}
	glUseProgram(ShaderProgramID); //--- ������� ���̴� ���α׷� ����ϱ�
	//--- ���� ���� ���̴����α׷� ���� �� �ְ�, Ư�� ���α׷��� ����Ϸ���
	//--- glUseProgram �Լ��� ȣ���Ͽ� ��� �� Ư�� ���α׷��� �����Ѵ�.
	//--- ����ϱ� ������ ȣ���� �� �ִ�.
	return ShaderProgramID;
}

GLfloat triShape1[3][3] = {// �ﰢ�� ������ ��ǥ��
{ 0.6f, -0.8f, 0.0f},
{ 0.8f, -0.8f, 0.0f },
{ 0.7f, -0.7f, 0.0f} };
GLfloat triShape2[3][3] = {// �ﰢ�� ������ ��ǥ��
{ -0.8f, -0.8f, 0.0f},
{ -0.6f, -0.8f, 0.0f },
{ -0.7f, -0.7f, 0.0f} };
GLfloat triShape3[3][3] = {// �ﰢ�� ������ ��ǥ��
{ 0.6f, 0.7f, 0.0f},
{ 0.8f, 0.7f, 0.0f },
{ 0.7f, 0.8f, 0.0f } };
GLfloat triShape4[3][3] = {// �ﰢ�� ������ ��ǥ��
{ -0.8f, +0.7f, 0.0f},
{ -0.6f, +0.7f, 0.0f },
{ -0.7f, +0.8f, 0.0f} };
GLfloat colors1[3][3] = { // �ﰢ�� ������ ����
{1.0f, 0.0f, 0.0f},
{1.0f, 0.0f, 0.0f},
{1.0f, 0.0f, 0.0f} };
GLfloat colors2[3][3] = { // �ﰢ�� ������ ����
{0.0f, 1.0f, 0.0f},
{0.0f, 1.0f, 0.0f},
{0.0f, 1.0f, 0.0f} };
GLfloat colors3[3][3] = { // �ﰢ�� ������ ����
{0.0f, 0.0f, 1.0f},
{0.0f, 0.0f, 1.0f},
{0.0f, 0.0f, 1.0f} };
GLfloat colors4[3][3] = { // �ﰢ�� ������ ����
{1.0f, 1.0f, 0.0f},
{1.0f, 1.0f, 0.0f},
{1.0f, 1.0f, 0.0f} };

//--- ���� ����
GLuint VAO[4], VBO1[2], VBO2[2], VBO3[2], VBO4[2];

void InitBuffer()
{
	glGenVertexArrays(1, &VAO[0]); //--- VAO �� �����ϰ� �Ҵ��ϱ�
	glBindVertexArray(VAO[0]); //--- VAO�� ���ε��ϱ�
	glGenBuffers(2, VBO1); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

	glBindBuffer(GL_ARRAY_BUFFER, VBO1[0]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), triShape1, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1[1]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colors1, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &VAO[1]); //--- VAO �� �����ϰ� �Ҵ��ϱ�
	glBindVertexArray(VAO[1]); //--- VAO�� ���ε��ϱ�
	glGenBuffers(2, VBO2); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

	glBindBuffer(GL_ARRAY_BUFFER, VBO2[0]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), triShape2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2[1]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colors2, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);


	glGenVertexArrays(1, &VAO[2]); //--- VAO �� �����ϰ� �Ҵ��ϱ�
	glBindVertexArray(VAO[2]); //--- VAO�� ���ε��ϱ�
	glGenBuffers(2, VBO3); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

	glBindBuffer(GL_ARRAY_BUFFER, VBO3[0]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), triShape3, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO3[1]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colors3, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &VAO[3]); //--- VAO �� �����ϰ� �Ҵ��ϱ�
	glBindVertexArray(VAO[3]); //--- VAO�� ���ε��ϱ�
	glGenBuffers(2, VBO4); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

	glBindBuffer(GL_ARRAY_BUFFER, VBO4[0]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), triShape4, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO4[1]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colors4, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

}

GLuint shaderID;

GLint width = 800;
GLint height = 800;
GLint Number = 0;

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	//--- ������ �����ϱ�
	glutInit(&argc, argv); // glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // ���÷��� ��� ����
	glutInitWindowPosition(100, 100); // �������� ��ġ ����
	glutInitWindowSize(width, height); // �������� ũ�� ����
	glutCreateWindow("Example1"); // ������ ����(������ �̸�)
		//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew �ʱ�ȭ
	{
		cerr << "Unable to initialize GLEW" << endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	shaderID = make_shaderProgram();
	InitBuffer();

	glutDisplayFunc(drawScene); // ��� �ݹ��Լ��� ����
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �ݹ��Լ� ����
	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(1, Timer, 1);

	glutMainLoop(); // �̺�Ʈ ó�� ����
}

bool DrawL = false;
bool Rotate = false;

int direction[4] = { rand() % 4 ,rand() % 4 ,rand() % 4 ,rand() % 4 };

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	//--- ����� ���� ����
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // �������� ����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�

	glUseProgram(shaderID);

	for (int i = 0; i < 4; i++)
	{
		glBindVertexArray(VAO[i]);
		if(DrawL == true)
			glDrawArrays(GL_LINE_LOOP, 0, 3);
		else
			glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
	glViewport(0, 0, w, h);
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'f':
		if(DrawL == false)
			DrawL = true;
		else
			DrawL = false;
		break;
	case 'm':
		Rotate = true;
		break;
	case 's':
		Rotate = false;
		break;
	case 'c':
		int a;
		a = rand() % 3;
		int b;
		b = rand() % 3;
		int c;
		c = rand() % 3;
		int d;
		d = rand() % 3;
		colors1[a][0] = (float)(rand() % 100) / 100;
		colors1[a][1] = (float)(rand() % 100) / 100;
		colors1[a][2] = (float)(rand() % 100) / 100;
		colors2[b][0] = (float)(rand() % 100) / 100;
		colors2[b][1] = (float)(rand() % 100) / 100;
		colors2[b][2] = (float)(rand() % 100) / 100;
		colors3[c][0] = (float)(rand() % 100) / 100;
		colors3[c][1] = (float)(rand() % 100) / 100;
		colors3[c][2] = (float)(rand() % 100) / 100;
		colors4[d][0] = (float)(rand() % 100) / 100;
		colors4[d][1] = (float)(rand() % 100) / 100;
		colors4[d][2] = (float)(rand() % 100) / 100;
		break;
	case 'q':
		exit(EXIT_FAILURE);
		break;
	}

	InitBuffer();

	glutPostRedisplay(); //--- ������ �ٲ𶧸��� ��� �ݹ��Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
}

GLvoid Mouse(int button, int state, int x, int y)
{
	cout << x << y << endl;

	// ���� Ŭ��
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		float Mx = x;
		float My = y;

		if (Number == 0)
		{
			triShape1[0][0] = ((Mx - width / 2) / (width / 2)) - 0.1;
			triShape1[0][1] = -((My - height / 2) / (height / 2)) - 0.1;
			triShape1[1][0] = ((Mx - width / 2) / (width / 2)) + 0.1;
			triShape1[1][1] = -((My - height / 2) / (height / 2)) - 0.1;
			triShape1[2][0] = ((Mx - width / 2) / (width / 2));
			triShape1[2][1] = -((My - height / 2) / (height / 2)) + 0.1;
		}
		else if (Number == 1)
		{
			triShape2[0][0] = ((Mx - width / 2) / (width / 2)) - 0.1;
			triShape2[0][1] = -((My - height / 2) / (height / 2)) - 0.1;
			triShape2[1][0] = ((Mx - width / 2) / (width / 2)) + 0.1;
			triShape2[1][1] = -((My - height / 2) / (height / 2)) - 0.1;
			triShape2[2][0] = ((Mx - width / 2) / (width / 2));
			triShape2[2][1] = -((My - height / 2) / (height / 2)) + 0.1;
		}
		else if (Number == 2)
		{
			triShape3[0][0] = ((Mx - width / 2) / (width / 2)) - 0.1;
			triShape3[0][1] = -((My - height / 2) / (height / 2)) - 0.1;
			triShape3[1][0] = ((Mx - width / 2) / (width / 2)) + 0.1;
			triShape3[1][1] = -((My - height / 2) / (height / 2)) - 0.1;
			triShape3[2][0] = ((Mx - width / 2) / (width / 2));
			triShape3[2][1] = -((My - height / 2) / (height / 2)) + 0.1;
		}
		else if (Number == 3)
		{
			triShape4[0][0] = ((Mx - width / 2) / (width / 2)) - 0.1;
			triShape4[0][1] = -((My - height / 2) / (height / 2)) - 0.1;
			triShape4[1][0] = ((Mx - width / 2) / (width / 2)) + 0.1;
			triShape4[1][1] = -((My - height / 2) / (height / 2)) - 0.1;
			triShape4[2][0] = ((Mx - width / 2) / (width / 2));
			triShape4[2][1] = -((My - height / 2) / (height / 2)) + 0.1;
		}
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		Number = (Number + 1) % 4;

	InitBuffer();

	glutPostRedisplay();
}

void Timer(int value)
{
	if (Rotate == true)
	{
		if (direction[0] == 0) // ��
		{
			triShape1[0][0] -= 0.01;
			triShape1[1][0] -= 0.01;
			triShape1[2][0] -= 0.01;
			if (triShape1[0][0] <= -0.9)
				direction[0] = 3;
		}
		else if (direction[0] == 1) // ��
		{
			triShape1[0][0] += 0.01;
			triShape1[1][0] += 0.01;
			triShape1[2][0] += 0.01;
			if (triShape1[1][0] >= 0.9)
				direction[0] = 2;
		}
		else if (direction[0] == 2) // ��
		{
			triShape1[0][1] += 0.01;
			triShape1[1][1] += 0.01;
			triShape1[2][1] += 0.01;
			if (triShape1[2][1] >= 0.9)
				direction[0] = 0;
		}
		else // ��
		{
			triShape1[0][1] -= 0.01;
			triShape1[1][1] -= 0.01;
			triShape1[2][1] -= 0.01;
			if (triShape1[0][1] <= -0.9)
				direction[0] = 1;
		}

		if (direction[1] == 0) // ��
		{
			triShape2[0][0] -= 0.01;
			triShape2[1][0] -= 0.01;
			triShape2[2][0] -= 0.01;
			if (triShape2[0][0] <= -0.9)
				direction[1] = 3;
		}
		else if (direction[1] == 1) // ��
		{
			triShape2[0][0] += 0.01;
			triShape2[1][0] += 0.01;
			triShape2[2][0] += 0.01;
			if (triShape2[1][0] >= 0.9)
				direction[1] = 2;
		}
		else if (direction[1] == 2) // ��
		{
			triShape2[0][1] += 0.01;
			triShape2[1][1] += 0.01;
			triShape2[2][1] += 0.01;
			if (triShape2[2][1] >= 0.9)
				direction[1] = 0;
		}
		else // ��
		{
			triShape2[0][1] -= 0.01;
			triShape2[1][1] -= 0.01;
			triShape2[2][1] -= 0.01;
			if (triShape2[0][1] <= -0.9)
				direction[1] = 1;
		}

		if (direction[2] == 0) // ��
		{
			triShape3[0][0] -= 0.01;
			triShape3[1][0] -= 0.01;
			triShape3[2][0] -= 0.01;
			if (triShape3[0][0] < -0.9)
				direction[2] = 3;
		}
		else if (direction[2] == 1) // ��
		{
			triShape3[0][0] += 0.01;
			triShape3[1][0] += 0.01;
			triShape3[2][0] += 0.01;
			if (triShape3[1][0] > 0.9)
				direction[2] = 2;
		}
		else if (direction[2] == 2) // ��
		{
			triShape3[0][1] += 0.01;
			triShape3[1][1] += 0.01;
			triShape3[2][1] += 0.01;
			if (triShape3[2][1] > 0.9)
				direction[2] = 0;
		}
		else // ��
		{
			triShape3[0][1] -= 0.01;
			triShape3[1][1] -= 0.01;
			triShape3[2][1] -= 0.01;
			if (triShape3[0][1] < -0.9)
				direction[2] = 1;
		}

		if (direction[3] == 0) // ��
		{
			triShape4[0][0] -= 0.01;
			triShape4[1][0] -= 0.01;
			triShape4[2][0] -= 0.01;
			if (triShape4[0][0] <= -0.9)
				direction[3] = 3;
		}
		else if (direction[3] == 1) // ��
		{
			triShape4[0][0] += 0.01;
			triShape4[1][0] += 0.01;
			triShape4[2][0] += 0.01;
			if (triShape4[1][0] >= 0.9)
				direction[3] = 2;
		}
		else if (direction[3] == 2) // ��
		{
			triShape4[0][1] += 0.01;
			triShape4[1][1] += 0.01;
			triShape4[2][1] += 0.01;
			if (triShape4[2][1] >= 0.9)
				direction[3] = 0;
		}
		else // ��
		{
			triShape4[0][1] -= 0.01;
			triShape4[1][1] -= 0.01;
			triShape4[2][1] -= 0.01;
			if (triShape4[0][1] <= -0.9)
				direction[3] = 1;
		}

	}
	InitBuffer();

	glutPostRedisplay();
	glutTimerFunc(1, Timer, 1);
}