#include <iostream>
#include <gl/glew.h> // �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <fstream>

using namespace std;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Mouse(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y);
GLchar* filetobuf(const GLchar* filename);
GLvoid Motion(int x, int y);


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

GLfloat point[4][3] = {
	{-0.5f,0.5f,0},
	{0.5f,0.5f,0},
	{0.5f,-0.5f,0},
	{-0.5f,-0.5f,0}
};
GLfloat color[4][3];

//--- ���� ����
GLuint VAO, VBO[2];

void InitBuffer()
{
	glGenVertexArrays(1, &VAO); //--- VAO �� �����ϰ� �Ҵ��ϱ�
	glBindVertexArray(VAO); //--- VAO�� ���ε��ϱ�
	glGenBuffers(2, VBO); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}

GLuint shaderID;

GLint width = 800;
GLint height = 600;

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
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);
	glutMainLoop(); // �̺�Ʈ ó�� ����
}

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	//--- ����� ���� ����
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // �������� ����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�

	glUseProgram(shaderID);

	glBindVertexArray(VAO);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
	
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

bool left_button = false;
bool select[4] = { false,false,false,false };
bool selectboundery[4] = { false,false,false,false };

GLvoid Mouse(int button, int state, int x, int y)
{
	cout << x << y << endl;

	// ���� Ŭ��
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		float Mx = (((float)x - width / 2) / (width / 2));
		float My = -(((float)y - height / 2) / (height / 2));

		left_button = true;

		for (int i = 0; i < 4; i++)
		{
			if (Mx - 0.1 <= point[i][0] && Mx + 0.1 >= point[i][0])
			{
				if (My - 0.1 <= point[i][1] && My + 0.1 >= point[i][1])
					select[i] = true;
			}
		}

		if (Mx >= point[0][0] && Mx <= point[1][0])
		{
			if (My >= point[0][1] - 1 && My <= point[1][1] + 1)
			{
				selectboundery[0] = true;
			}
		}

		float RColor = (float)(rand() % 100) / 100;
		float GColor = (float)(rand() % 100) / 100;
		float BColor = (float)(rand() % 100) / 100;

		for (int k = 0; k < 4; k++)
		{
			color[k][0] = RColor;
			color[k][1] = GColor;
			color[k][2] = BColor;
		}
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		left_button = false;
		for (int i = 0; i < 4; i++)
		{
			select[i] = false;
			selectboundery[i] = false;
		}
	}

	InitBuffer();

	glutPostRedisplay();
}

GLvoid Motion(int x, int y)
{

	float Mx = (((float)x - width / 2) / (width / 2));
	float My = -(((float)y - height / 2) / (height / 2));

	if (left_button == true)
	{
		cout << Mx << " , " << My << endl;
		for (int i = 0; i < 4; i++)
		{
			if (select[i] == true)
			{
				point[i][0] = Mx;
				point[i][1] = My;
			}
		}

		if (selectboundery[0] == true)
		{
			point[0][1] = My;
			point[1][1] = My;
		}
		else if (selectboundery[1] == true)
		{
			point[1][1] = My;
			point[2][1] = My;
		}
		else if (selectboundery[2] == true)
		{
			point[2][1] = My;
			point[3][1] = My;
		}
		else if (selectboundery[3] == true)
		{
			point[0][1] = My;
			point[3][1] = My;
		}

	}
	InitBuffer();

	glutPostRedisplay();
}