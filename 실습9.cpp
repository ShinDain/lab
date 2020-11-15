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

GLfloat triShape[4][3][3] = {
	{
		{-0.5f,0.5f,0.0f },
		{-0.5f,-0.5f,0.0f},
		{0.0f,0.0f,0.0f} 
	},
	{ 
		{-0.5f,-0.5f,0.0f },
		{0.5f,-0.5f,0.0f},
		{0.0f,0.0f,0.0f}
	},
	{
		{0.5f,-0.5f,0.0f },
		{0.5f,0.5f,0.0f},
		{0.0f,0.0f,0.0f}
	},
	{
		{0.5f,0.5f,0.0f },
		{-0.5f,0.5f,0.0f},
		{0.0f,0.0f,0.0f}
	}
};
GLfloat colors[4][3][3] = {
	{
		{0.9f,0.2f,0.0f},
		{0.4f,0.0f,0.6f},
		{0.0f,0.8f,0.9f}
	},
	{
		{0.4f,0.5f,0.0f},
		{0.7f,0.0f,0.9f},
		{0.0f,0.2f,0.9f}
	},
	{
		{0.7f,0.8f,0.0f},
		{0.9f,0.0f,0.3f},
		{0.0f,0.5f,0.6f}
	},
	{
		{0.9f,0.8f,0.0f},
		{0.6f,0.0f,0.4f},
		{0.0f,0.8f,0.4f}
	}
};

//--- ���� ����
GLuint VAO[4], VBO[2];

void InitBuffer()
{
	// �ٱ� �ﰢ��
	for (int i = 0; i < 4; i++)
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
}

GLuint shaderID;

GLint width = 800;
GLint height = 600;

bool animation = false;
bool shrink = false;
bool direction[4] = { false,false,false,false };
bool Trisize[4] = { false,false,false,false };

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

	for (int i = 0; i < 4; i++)
	{
		glBindVertexArray(VAO[i]);
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
	case 't':
		if (animation == false)
			animation = true;
		else
			animation = false;
		break;
	case 's':
		if (shrink == false)
			shrink = true;
		else
			shrink = false;
		break;
	case 'q':
		exit(EXIT_FAILURE);
		break;
	}
	glutPostRedisplay(); //--- ������ �ٲ𶧸��� ��� �ݹ��Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
}

void Timer(int value)
{
	if (animation == true)
	{
		if (direction[0] == false)
		{
			triShape[0][0][0] -= 0.01;
			triShape[0][1][0] -= 0.01;
			triShape[0][2][0] -= 0.01;
			if (triShape[0][0][0] <= -1)
				direction[0] = true;
		}
		else
		{
			triShape[0][0][0] += 0.01;
			triShape[0][1][0] += 0.01;
			triShape[0][2][0] += 0.01;
			if (triShape[0][2][0] >= 0)
				direction[0] = false;
		}
		if (direction[2] == false)
		{
			triShape[2][0][0] += 0.01;
			triShape[2][1][0] += 0.01;
			triShape[2][2][0] += 0.01;
			if (triShape[2][0][0] >= 1)
				direction[2] = true;
		}
		else
		{
			triShape[2][0][0] -= 0.01;
			triShape[2][1][0] -= 0.01;
			triShape[2][2][0] -= 0.01;
			if (triShape[2][2][0] <= 0)
				direction[2] = false;

		}

		if (direction[1] == false)
		{
			triShape[1][0][1] -= 0.01;
			triShape[1][1][1] -= 0.01;
			triShape[1][2][1] -= 0.01;
			if (triShape[1][0][1] <= -1)
				direction[1] = true;
		}
		else
		{
			triShape[1][0][1] += 0.01;
			triShape[1][1][1] += 0.01;
			triShape[1][2][1] += 0.01;
			if (triShape[1][2][1] >= 0)
				direction[1] = false;
		}
		if (direction[3] == false)
		{
			triShape[3][0][1] += 0.01;
			triShape[3][1][1] += 0.01;
			triShape[3][2][1] += 0.01;
			if (triShape[3][0][1] >= 1)
				direction[3] = true;
		}				   
		else
		{
			triShape[3][0][1] -= 0.01;
			triShape[3][1][1] -= 0.01;
			triShape[3][2][1] -= 0.01;
			if (triShape[3][2][1] <= 0)
				direction[3] = false;
		}
		// ����
		if (shrink == true)
		{
			if (Trisize[0] == false)
			{
				triShape[0][0][1] -= 0.01;
				triShape[0][1][1] += 0.01;
				if (triShape[0][0][1] <= 0)
					Trisize[0] = true;
			}
			else
			{
				triShape[0][0][1] += 0.01;
				triShape[0][1][1] -= 0.01;
				if (triShape[0][0][1] >= 0.5)
					Trisize[0] = false;
			}
			if (Trisize[2] == false)
			{
				triShape[2][0][1] += 0.01;
				triShape[2][1][1] -= 0.01;
				if (triShape[2][0][1] >= 0)
					Trisize[2] = true;
			}
			else
			{
				triShape[2][0][1] -= 0.01;
				triShape[2][1][1] += 0.01;
				if (triShape[2][0][1] <= -0.5)
					Trisize[2] = false;

			}

			if (Trisize[1] == false)
			{
				triShape[1][0][0] -= 0.01;
				triShape[1][1][0] += 0.01;
				if (triShape[1][0][0] <= 0)
					Trisize[1] = true;
			}
			else
			{
				triShape[1][0][0] += 0.01;
				triShape[1][1][0] -= 0.01;
				if (triShape[1][0][0] >= 0.5)
					Trisize[1] = false;
			}
			if (Trisize[3] == false)
			{
				triShape[3][0][0] += 0.01;
				triShape[3][1][0] -= 0.01;
				if (triShape[3][0][0] >= 0)
					Trisize[3] = true;
			}
			else
			{
				triShape[3][0][0] -= 0.01;
				triShape[3][1][0] += 0.01;
				if (triShape[3][0][0] <= -0.5)
					Trisize[3] = false;
			}
		}
	}
	InitBuffer();

	glutPostRedisplay();
	glutTimerFunc(10, Timer, 1);
}
