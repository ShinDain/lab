#include <iostream>
#include <gl/glew.h> // �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <fstream>

using namespace std;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
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
	vertexsource = filetobuf("vertex_glm.glsl");

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
	fragmentsource = filetobuf("fragment_glm.glsl");

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

GLfloat DrawLine[3][2][3] =
{
	{
		{1.0f,0.0f,0.0f},
		{-1.0f,0.0f,0.0f}
	},
	{
		{0.0f,1.0f,0.0f},
		{0.0f,-1.0f,0.0f}
	},
	{
		{0.0f,0.0f,1.0f},
		{0.0f,0.0f,-1.0f}
	},
};

GLfloat LineC[2][3] = {
	{0.3f,0.5f,0.3f},
	{0.8f,0.6f,0.4f}
};

GLfloat Square[36][3] = {
	{-0.5f,0.5f,0.0f},
	{-0.5f,-0.5f,0.0f},
	{0.0f,-0.5f,0.5f},

	{-0.5f,0.5f,0.0f},
	{0.0f,-0.5f,0.5f},
	{0.0f,0.5f,0.5f},

	{0.0f,0.5f,0.5f},
	{0.0f,-0.5f,0.5f},
	{0.5f,-0.5f,0.0f},

	{0.0f,0.5f,0.5f},
	{0.5f,-0.5f,0.0f},
	{0.5f,0.5f,0.0f},

	{0.5f,0.5f,0.0f},
	{0.5f,-0.5f,0.0f},
	{0.0f,-0.5f,-0.5f},

	{0.5f,0.5f,0.0f},
	{0.0f,-0.5f,-0.5f},
	{0.0f,0.5f,-0.5f},

	{0.0f,0.5f,-0.5f},
	{0.0f,-0.5f,-0.5f},
	{-0.5f,-0.5f,0.0f},

	{0.0f,0.5f,-0.5f},
	{-0.5f,-0.5f,0.0f},
	{-0.5f,0.5f,0.0f},

	{0.0f,0.5f,-0.5f},
	{-0.5f,0.5f,0.0f},
	{0.0f,0.5f,0.5f},

	{0.0f,0.5f,-0.5f},
	{0.0f,0.5f,0.5f},
	{0.5f,0.5f,0.0f},

	{-0.5f,-0.5f,0.0f},
	{0.0f,-0.5f,-0.5f},
	{0.0f,-0.5f,0.5f},

	{0.0f,-0.5f,-0.5f},
	{0.5f,-0.5f,0.0f},
	{0.0f,-0.5f,0.5f}
};
GLfloat SquareC[36][3];

//--- ���� ����
GLuint VAO[4], VBO[2];

void InitBuffer()
{
	for (int i = 0; i < 3; i++)
	{
		glGenVertexArrays(1, &VAO[i]); //--- VAO �� �����ϰ� �Ҵ��ϱ�
		glBindVertexArray(VAO[i]); //--- VAO�� ���ε��ϱ�
		glGenBuffers(2, VBO); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(DrawLine[i]), DrawLine[i], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(LineC), LineC, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
	}
	glGenVertexArrays(1, &VAO[3]); //--- VAO �� �����ϰ� �Ҵ��ϱ�
	glBindVertexArray(VAO[3]); //--- VAO�� ���ε��ϱ�
	glGenBuffers(2, VBO); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Square), Square, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SquareC), SquareC, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}

GLuint shaderID;

GLint width = 1000;
GLint height = 1000;

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	//--- ������ �����ϱ�
	glutInit(&argc, argv); // glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // ���÷��� ��� ����
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

GLUquadricObj* qobj;

bool change = false;
bool animation = false;
int aniSphere = 0;
int aniSquare = 0;
int Allani = 0;

float TransSquare[3] = { -0.5f,0.0f,0.0f };
float TransSphere[3] = { 0.5f,0.0f,0.0f };
float RotSquare = 0.0f;
float RotSphere = 0.0f;

float Alldegree = 0.0f;

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	//--- ����� ���� ����
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // �������� ����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�

	glUseProgram(shaderID);

	glEnable(GL_DEPTH_TEST);

	glm::mat4 TSphere = glm::mat4(1.0f);
	glm::mat4 TSquare = glm::mat4(1.0f);
	glm::mat4 RSphere = glm::mat4(1.0f);
	glm::mat4 RSquare = glm::mat4(1.0f);
	glm::mat4 RAll = glm::mat4(1.0f);
	glm::mat4 R = glm::mat4(1.0f);
	glm::mat4 R2 = glm::mat4(1.0f);
	glm::mat4 AR = glm::mat4(1.0f);
	glm::mat4 SSquare = glm::mat4(1.0f);
	glm::mat4 Sphere = glm::mat4(1.0f);
	glm::mat4 Square = glm::mat4(1.0f);

	TSphere = glm::translate(TSphere, glm::vec3(TransSquare[0], TransSquare[1], TransSquare[2]));
	TSquare = glm::translate(TSquare, glm::vec3(TransSphere[0], TransSphere[1], TransSphere[2]));
	SSquare = glm::scale(SSquare, glm::vec3(0.5f, 0.5f, 0.5f));
	RSphere = glm::rotate(RSphere, glm::radians(RotSphere), glm::vec3(0.0f, 1.0f, 0.0f));
	RSquare = glm::rotate(RSquare, glm::radians(RotSquare), glm::vec3(1.0f, 0.0f, 0.0f));

	R = glm::rotate(R, glm::radians(15.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	R2 = glm::rotate(R2, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	RAll = glm::rotate(RAll, glm::radians(Alldegree), glm::vec3(0.0f, 1.0f, 0.0f));

	AR = R2 * R * RAll;

	Square = AR * RAll * TSquare * RSquare * SSquare;
	Sphere = AR * RAll * TSphere * RSphere * SSquare;

	unsigned int modelLocation = glGetUniformLocation(shaderID, "transform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(AR));
	for (int i = 0; i < 3; i++)
	{
		glBindVertexArray(VAO[i]);
		glDrawArrays(GL_LINE_STRIP, 0, 2);
	}

	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GLU_LINE);
	gluQuadricNormals(qobj, GLU_SMOOTH);
	gluQuadricOrientation(qobj, GLU_OUTSIDE);
	if(change == false)
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Sphere));
	else
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Square));
	gluSphere(qobj, 0.1, 50, 50);

	if (change == false)
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Square));
	else
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Sphere));
	glBindVertexArray(VAO[3]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glutSwapBuffers(); // ȭ�鿡 ����ϱ�1
}
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
	glViewport(0, 0, w, h);
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'x':
		animation = true;
		aniSquare++;
		aniSquare = aniSquare % 3;
		break;
	case 'y':
		animation = true;
		aniSphere++;
		aniSphere = aniSphere % 3;
		break;
		break;
	case 'b':
		animation = true;
		Allani++;
		Allani = Allani % 3;
		break;
	case 'c':
		if (change == false)
			change = true;
		else
			change = false;
		break;
	case 's':
		animation = false;
		break;
	case 'q':
		exit(EXIT_FAILURE);
		break;
	}
	InitBuffer();
	glutPostRedisplay(); //--- ������ �ٲ𶧸��� ��� �ݹ��Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
}

void Timer(int value)
{
	if (animation == true)
	{
		if (aniSquare == 1)
			RotSquare += 1.0f;
		else if (aniSquare == 2)
			RotSquare -= 1.0f;
		if (aniSphere == 1)
			RotSphere += 1.0f;
		else if (aniSphere == 2)
			RotSphere -= 1.0f;
		if (Allani == 1)
			Alldegree += 1.0f;
		else if (Allani == 2)
			Alldegree -= 1.0f;
	}
	InitBuffer();
	glutPostRedisplay();
	glutTimerFunc(10, Timer, 1);
}
