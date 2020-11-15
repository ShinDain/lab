#include <iostream>
#include <gl/glew.h> // �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <fstream>

using namespace std;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Mouse(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y); \
void Timer(int value);
void InTimer(int value);
GLchar* filetobuf(const GLchar* filename);

void ChangeColor(GLfloat color[]);

void ChangeColor(GLfloat color[])
{
	float RColor = (float)(rand() % 100) / 100;
	float GColor = (float)(rand() % 100) / 100;
	float BColor = (float)(rand() % 100) / 100;

	for (int k = 0; k < 3; k++)
	{
		color[0] = RColor;
		color[1] = GColor;
		color[2] = BColor;
	}
}

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

GLfloat Intri[2][3][3] =
{
	{
		{0.05f,0.4f,0.0f},
		{-0.05f,0.4f,0.0f},
		{0.0f,0.2f,0.0f} },
	{
		{-0.05f,-0.4f,0.0f},
		{0.05f,-0.4f,0.0f},
		{0.0f,-0.2f,0.0f} }
};
GLfloat IntriC[3][3] =
{
	{0.0f,6.0f,6.0f},
	{1.0f,0.0f,0.0f},
	{0.6f,0.0f,6.0f}
};
GLfloat InLine[2][3][3] =
{
	{
		{-0.4f,-0.4f,0.0f},
		{-0.4f,0.4f,0.0f},
		{0.4f,0.4f,0.0f}
	},
	{
		{0.4f,0.4f,0.0f},
		{0.4f,-0.4f,0.0f},
		{-0.4f,-0.4f,0.0f}
	}
};
GLfloat LineC[3][3] =
{
	{0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f}
};
GLfloat triShape[10][3][3];
GLfloat colors[10][3][3];

//--- ���� ����
GLuint VAO[10], VAO1[2], VAO2[2], VBO[2];

void InitBuffer()
{
	// �ٱ� �ﰢ��
	for (int i = 0; i < 10; i++)
	{
		glGenVertexArrays(1, &VAO[i]); //--- VAO �� �����ϰ� �Ҵ��ϱ�
		glBindVertexArray(VAO[i]); //--- VAO�� ���ε��ϱ�
		glGenBuffers(2, VBO); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), triShape[i], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colors[i], GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
	}
	// �簢�� �׸���
	for (int i = 0; i < 2; i++)
	{
		glGenVertexArrays(1, &VAO1[i]); //--- VAO �� �����ϰ� �Ҵ��ϱ�
		glBindVertexArray(VAO1[i]); //--- VAO�� ���ε��ϱ�
		glGenBuffers(2, VBO); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), InLine[i], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), LineC, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
	}
	// �ٱ� �ﰢ��
	for (int i = 0; i < 2; i++)
	{
		glGenVertexArrays(1, &VAO2[i]); //--- VAO �� �����ϰ� �Ҵ��ϱ�
		glBindVertexArray(VAO2[i]); //--- VAO�� ���ε��ϱ�
		glGenBuffers(2, VBO); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), Intri[i], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), IntriC, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
	}
}

GLuint shaderID;

GLint width = 800;
GLint height = 600;
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
	glutTimerFunc(10, Timer, 1);
	glutTimerFunc(10, InTimer, 2);

	glutMainLoop(); // �̺�Ʈ ó�� ����
}

int direction[10];
int InDir[2] = { 3, 1 };
bool drawB = false;


GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	//--- ����� ���� ����
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // �������� ����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�

	glUseProgram(shaderID);

	if (drawB == true)
	{
		for (int i = 0; i < 10; i++)
		{
			glBindVertexArray(VAO[i]);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
	}
	for (int i = 0; i < 2; i++)
	{
		glBindVertexArray(VAO1[i]);
		glDrawArrays(GL_LINE_STRIP, 0, 3);
		glBindVertexArray(VAO2[i]);
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
	case 'q':
		exit(EXIT_FAILURE);
		break;
	}
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

		if (drawB == false)
			drawB = true;

		triShape[Number][0][0] = ((Mx - width / 2) / (width / 2)) - 0.05;
		triShape[Number][0][1] = -((My - height / 2) / (height / 2)) - 0.05;
		triShape[Number][1][0] = ((Mx - width / 2) / (width / 2)) + 0.05;
		triShape[Number][1][1] = -((My - height / 2) / (height / 2)) - 0.05;
		triShape[Number][2][0] = ((Mx - width / 2) / (width / 2));
		triShape[Number][2][1] = -((My - height / 2) / (height / 2)) + 0.2;

		direction[Number] = rand() % 4;

		float RColor = (float)(rand() % 100) / 100;
		float GColor = (float)(rand() % 100) / 100;
		float BColor = (float)(rand() % 100) / 100;

		for (int k = 0; k < 3; k++)
		{
			colors[Number][k][0] = RColor;
			colors[Number][k][1] = GColor;
			colors[Number][k][2] = BColor;
		}
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		Number = (Number + 1) % 10;

	InitBuffer();

	glutPostRedisplay();
}

void Timer(int value)
{
	for (int i = 0; i < 10; i++)
	{
		if (direction[i] == 0) // �»�
		{
			triShape[i][0][0] -= 0.01;
			triShape[i][1][0] -= 0.01;
			triShape[i][2][0] -= 0.01;
			triShape[i][0][1] += 0.01;
			triShape[i][1][1] += 0.01;
			triShape[i][2][1] += 0.01;
			if (triShape[i][0][0] <= -1.0)
			{
				direction[i] = 1;
				triShape[i][1][0] = triShape[i][0][0];
				triShape[i][1][1] = triShape[i][0][1] - 0.1;
				triShape[i][2][0] = triShape[i][0][0] + 0.2;
				triShape[i][2][1] = triShape[i][0][1] - 0.05;

				for (int k = 0; k < 3; k++)
				{
					ChangeColor(colors[i][k]);
				}
			}
			else if (triShape[i][1][1] >= 1.0)
			{
				direction[i] = 3;
				triShape[i][0][0] = triShape[i][1][0] + 0.1;
				triShape[i][0][1] = triShape[i][1][1];
				triShape[i][2][0] = triShape[i][1][0] + 0.05;
				triShape[i][2][1] = triShape[i][1][1] - 0.2;

				for (int k = 0; k < 3; k++)
				{
					ChangeColor(colors[i][k]);
				}
			}
			else if (triShape[i][2][1] >= -0.4 && triShape[i][2][1] < 0)
			{
				if (triShape[i][2][0] >= -0.4 && triShape[i][2][0] <= 0.4)
				{
					direction[i] = 3;
					triShape[i][0][0] = triShape[i][1][0] + 0.1;
					triShape[i][0][1] = triShape[i][1][1];
					triShape[i][2][0] = triShape[i][1][0] + 0.05;
					triShape[i][2][1] = triShape[i][1][1] - 0.2;
				}
			}
			else if (triShape[i][2][0] <= 0.4 && triShape[i][2][0] > 0)
			{
				if (triShape[i][2][1] >= -0.4 && triShape[i][2][1] <= 0.4)
				{
					direction[i] = 1;
					triShape[i][1][0] = triShape[i][0][0];
					triShape[i][1][1] = triShape[i][0][1] - 0.1;
					triShape[i][2][0] = triShape[i][0][0] + 0.2;
					triShape[i][2][1] = triShape[i][0][1] - 0.05;
				}
			}
		}
		else if (direction[i] == 1) // ���
		{
			triShape[i][0][0] += 0.01;
			triShape[i][1][0] += 0.01;
			triShape[i][2][0] += 0.01;
			triShape[i][0][1] += 0.01;
			triShape[i][1][1] += 0.01;
			triShape[i][2][1] += 0.01;
			if (triShape[i][1][0] >= 1.0)
			{
				direction[i] = 0;
				triShape[i][0][0] = triShape[i][1][0];
				triShape[i][0][1] = triShape[i][1][1] - 0.1;
				triShape[i][2][0] = triShape[i][1][0] - 0.2;
				triShape[i][2][1] = triShape[i][1][1] - 0.05;

				for (int k = 0; k < 3; k++)
				{
					ChangeColor(colors[i][k]);
				}
			}
			else if (triShape[i][0][1] >= 1.0)
			{
				direction[i] = 2;
				triShape[i][1][0] = triShape[i][0][0] - 0.1;
				triShape[i][1][1] = triShape[i][0][1];
				triShape[i][2][0] = triShape[i][0][0] - 0.05;
				triShape[i][2][1] = triShape[i][0][1] - 0.2;

				for (int k = 0; k < 3; k++)
				{
					ChangeColor(colors[i][k]);
				}
			}
			else if (triShape[i][2][1] >= -0.4 && triShape[i][2][1] < 0)
			{
				if (triShape[i][2][0] >= -0.4 && triShape[i][2][0] <= 0.4)
				{
					direction[i] = 2;
					triShape[i][1][0] = triShape[i][0][0] - 0.1;
					triShape[i][1][1] = triShape[i][0][1];
					triShape[i][2][0] = triShape[i][0][0] - 0.05;
					triShape[i][2][1] = triShape[i][0][1] - 0.2;
				}
			}
			else if (triShape[i][2][0] >= -0.4 && triShape[i][2][0] < 0)
			{
				if (triShape[i][2][1] >= -0.4 && triShape[i][2][1] <= 0.4)
				{
					direction[i] = 0;
					triShape[i][0][0] = triShape[i][1][0];
					triShape[i][0][1] = triShape[i][1][1] - 0.1;
					triShape[i][2][0] = triShape[i][1][0] - 0.2;
					triShape[i][2][1] = triShape[i][1][1] - 0.05;
				}
			}
		}
		else if (direction[i] == 2) // ����
		{
			triShape[i][0][0] += 0.01;
			triShape[i][1][0] += 0.01;
			triShape[i][2][0] += 0.01;
			triShape[i][0][1] -= 0.01;
			triShape[i][1][1] -= 0.01;
			triShape[i][2][1] -= 0.01;
			if (triShape[i][0][0] >= 1.0)
			{
				direction[i] = 3;
				triShape[i][1][0] = triShape[i][0][0];
				triShape[i][1][1] = triShape[i][0][1] + 0.1;
				triShape[i][2][0] = triShape[i][0][0] - 0.2;
				triShape[i][2][1] = triShape[i][0][1] + 0.05;

				for (int k = 0; k < 3; k++)
				{
					ChangeColor(colors[i][k]);
				}
			}
			else if (triShape[i][1][1] <= -1.0)
			{
				direction[i] = 1;
				triShape[i][0][0] = triShape[i][1][0] - 0.1;
				triShape[i][0][1] = triShape[i][1][1];
				triShape[i][2][0] = triShape[i][1][0] - 0.05;
				triShape[i][2][1] = triShape[i][1][1] + 0.2;

				for (int k = 0; k < 3; k++)
				{
					ChangeColor(colors[i][k]);
				}
			}
			else if (triShape[i][2][1] <= 0.4 && triShape[i][2][1] > 0)
			{
				if (triShape[i][2][0] >= -0.4 && triShape[i][2][0] <= 0.4)
				{
					direction[i] = 1;
					triShape[i][0][0] = triShape[i][1][0] - 0.1;
					triShape[i][0][1] = triShape[i][1][1];
					triShape[i][2][0] = triShape[i][1][0] - 0.05;
					triShape[i][2][1] = triShape[i][1][1] + 0.2;
				}
			}
			else if (triShape[i][2][0] >= -0.4 && triShape[i][2][0] < 0)
			{
				if (triShape[i][2][1] >= -0.4 && triShape[i][2][1] <= 0.4)
				{
					direction[i] = 3;
					triShape[i][1][0] = triShape[i][0][0];
					triShape[i][1][1] = triShape[i][0][1] + 0.1;
					triShape[i][2][0] = triShape[i][0][0] - 0.2;
					triShape[i][2][1] = triShape[i][0][1] + 0.05;
				}
			}
		}
		else // ����
		{
			triShape[i][0][0] -= 0.01;
			triShape[i][1][0] -= 0.01;
			triShape[i][2][0] -= 0.01;
			triShape[i][0][1] -= 0.01;
			triShape[i][1][1] -= 0.01;
			triShape[i][2][1] -= 0.01;
			if (triShape[i][1][0] <= -1.0)
			{
				direction[i] = 2;
				triShape[i][0][0] = triShape[i][1][0];
				triShape[i][0][1] = triShape[i][1][1] + 0.1;
				triShape[i][2][0] = triShape[i][1][0] + 0.2;
				triShape[i][2][1] = triShape[i][1][1] + 0.05;

				for (int k = 0; k < 3; k++)
				{
					ChangeColor(colors[i][k]);
				}
			}
			else if (triShape[i][0][1] <= -1.0)
			{
				direction[i] = 0;
				triShape[i][1][0] = triShape[i][0][0] + 0.1;
				triShape[i][1][1] = triShape[i][0][1];
				triShape[i][2][0] = triShape[i][0][0] + 0.05;
				triShape[i][2][1] = triShape[i][0][1] + 0.2;

				for (int k = 0; k < 3; k++)
				{
					ChangeColor(colors[i][k]);
				}
			}
			else if (triShape[i][2][1] <= 0.4 && triShape[i][2][1] > 0)
			{
				if (triShape[i][2][0] >= -0.4 && triShape[i][2][0] <= 0.4)
				{
					direction[i] = 0;
					triShape[i][1][0] = triShape[i][0][0] + 0.1;
					triShape[i][1][1] = triShape[i][0][1];
					triShape[i][2][0] = triShape[i][0][0] + 0.05;
					triShape[i][2][1] = triShape[i][0][1] + 0.2;
				}
			}
			else if (triShape[i][2][0] <= 0.4 && triShape[i][2][0] > 0)
			{
				if (triShape[i][2][1] >= -0.4 && triShape[i][2][1] <= 0.4)
				{
					direction[i] = 2;
					triShape[i][0][0] = triShape[i][1][0];
					triShape[i][0][1] = triShape[i][1][1] + 0.1;
					triShape[i][2][0] = triShape[i][1][0] + 0.2;
					triShape[i][2][1] = triShape[i][1][1] + 0.05;
				}
			}
		}
	}

	InitBuffer();

	glutPostRedisplay();
	glutTimerFunc(10, Timer, 1);
}

void InTimer(int value)
{
	for (int i = 0; i < 2; i++)
	{
		if (InDir[i] == 0) // �»�
		{
			Intri[i][0][0] -= 0.005;
			Intri[i][1][0] -= 0.005;
			Intri[i][2][0] -= 0.005;
			Intri[i][0][1] += 0.005;
			Intri[i][1][1] += 0.005;
			Intri[i][2][1] += 0.005;
			if (Intri[i][1][1] >= 0.4)
			{
				InDir[i] = 3;
				Intri[i][0][0] = Intri[i][1][0] + 0.1;
				Intri[i][0][1] = Intri[i][1][1];
				Intri[i][2][0] = Intri[i][1][0] + 0.05;
				Intri[i][2][1] = Intri[i][1][1] - 0.2;

				for (int k = 0; k < 3; k++)
				{
					ChangeColor(IntriC[k]);
				}
			}
		}
		else if (InDir[i] == 1) // ���
		{
			Intri[i][0][0] += 0.005;
			Intri[i][1][0] += 0.005;
			Intri[i][2][0] += 0.005;
			Intri[i][0][1] += 0.005;
			Intri[i][1][1] += 0.005;
			Intri[i][2][1] += 0.005;
			if (Intri[i][1][0] >= 0.4)
			{
				InDir[i] = 0;
				Intri[i][0][0] = Intri[i][1][0];
				Intri[i][0][1] = Intri[i][1][1] - 0.1;
				Intri[i][2][0] = Intri[i][1][0] - 0.2;
				Intri[i][2][1] = Intri[i][1][1] - 0.05;

				for (int k = 0; k < 3; k++)
				{
					ChangeColor(IntriC[k]);
				}
			}
		}
		else if (InDir[i] == 2) // ����
		{
			Intri[i][0][0] += 0.005;
			Intri[i][1][0] += 0.005;
			Intri[i][2][0] += 0.005;
			Intri[i][0][1] -= 0.005;
			Intri[i][1][1] -= 0.005;
			Intri[i][2][1] -= 0.005;
			if (Intri[i][1][1] <= -0.4)
			{
				InDir[i] = 1;
				Intri[i][0][0] = Intri[i][1][0] - 0.1;
				Intri[i][0][1] = Intri[i][1][1];
				Intri[i][2][0] = Intri[i][1][0] - 0.05;
				Intri[i][2][1] = Intri[i][1][1] + 0.2;

				for (int k = 0; k < 3; k++)
				{
					ChangeColor(IntriC[k]);
				}
			}
		}
		else // ����
		{
			Intri[i][0][0] -= 0.005;
			Intri[i][1][0] -= 0.005;
			Intri[i][2][0] -= 0.005;
			Intri[i][0][1] -= 0.005;
			Intri[i][1][1] -= 0.005;
			Intri[i][2][1] -= 0.005;
			if (Intri[i][1][0] <= -0.4)
			{
				InDir[i] = 2;
				Intri[i][0][0] = Intri[i][1][0];
				Intri[i][0][1] = Intri[i][1][1] + 0.1;
				Intri[i][2][0] = Intri[i][1][0] + 0.2;
				Intri[i][2][1] = Intri[i][1][1] + 0.05;

				for (int k = 0; k < 3; k++)
				{
					ChangeColor(IntriC[k]);
				}
			}
		}
	}

	InitBuffer();

	glutPostRedisplay();
	glutTimerFunc(10, InTimer, 2);
}