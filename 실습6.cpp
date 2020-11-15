#include <iostream>
#include <gl/glew.h> // �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <fstream>
#include <math.h>

using namespace std;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Mouse(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y);
void Timer(int value);
GLchar* filetobuf(const GLchar* filename);

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

GLfloat circle[10][90][3];
GLfloat colors[10][90][3];

//--- ���� ����
GLuint VAO[10], VBO[2];

void InitBuffer()
{
	for (int i = 0; i < 10; i++)
	{
		glGenVertexArrays(1, &VAO[i]); //--- VAO �� �����ϰ� �Ҵ��ϱ�

		glGenBuffers(2, VBO); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

		glBindVertexArray(VAO[i]); //--- VAO�� ���ε��ϱ�
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(circle), circle[i], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors[i], GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(1);

	}
	
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
	glutTimerFunc(10, Timer, 1);
	glutMainLoop(); // �̺�Ʈ ó�� ����
}

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	//--- ����� ���� ����
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // �������� ����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�

	glUseProgram(shaderID);

	for (int i = 0; i < 10; i++)
	{
		glBindVertexArray(VAO[i]);

		glDrawArrays(GL_LINE_LOOP, 0, 90);
	}

	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
	glViewport(0, 0, w, h);
}

float r = 0.1;

GLvoid Mouse(int button, int state, int x, int y)
{
	cout << x << y << endl;

	// ���� Ŭ��
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		float Mx = (((float)x - width / 2) / (width / 2));
		float My = -(((float)y - width / 2) / (width / 2));

		float num = (3.14 / 180) * 12;

		for (int i = 0; i < 30; i++)
		{
			circle[Number][(i * 3)][0] = Mx;
			circle[Number][(i * 3)][1] = My;
			circle[Number][(i * 3) + 1][0] = Mx + r * cos(num * i);
			circle[Number][(i * 3) + 1][1] = My + r * sin(num * i);
			circle[Number][(i * 3) + 2][0] = Mx + r * cos(num * (i + 1));
			circle[Number][(i * 3) + 2][1] = My + r * sin(num * (i+1));
		}
		
		float RColor = (float)(rand() % 100) / 100; 
		float GColor = (float)(rand() % 100) / 100;
		float BColor = (float)(rand() % 100) / 100;

		for (int k = 0; k < 90; k++)
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

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'q':
		exit(EXIT_FAILURE);
		break;
	}
	glutPostRedisplay(); //--- ������ �ٲ𶧸��� ��� �ݹ��Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
}

bool bigsize = true;

void Timer(int value)
{
	if(bigsize == true)
		r += 0.01;
	else
		r -= 0.01;

	if (r >= 0.5)
		bigsize = false;
	else if (r <= 0.1)
		bigsize = true;

	float Mx;
	float My;

	float num = (3.14 / 180) * 12;

	for (int k = 0; k < 10; k++)
	{
		Mx = circle[k][0][0];
		My = circle[k][0][1];


		for (int i = 0; i < 30; i++)
		{
			circle[k][(i * 3)][0] = Mx;
			circle[k][(i * 3)][1] = My;
			circle[k][(i * 3) + 1][0] = Mx + r * cos(num * i);
			circle[k][(i * 3) + 1][1] = My + r * sin(num * i);
			circle[k][(i * 3) + 2][0] = Mx + r * cos(num * (i + 1));
			circle[k][(i * 3) + 2][1] = My + r * sin(num * (i + 1));
		}
	}
	

	InitBuffer();

	glutPostRedisplay();
	glutTimerFunc(100, Timer, 1);
}