F#include <iostream>
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
void Timer2(int value);
GLchar* filetobuf(const GLchar* filename);
void init();


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

GLfloat DrawLine[2][2][3] =
{
	{
		{-1.0f,0.0f,0.0f},
		{1.0f,0.0f,0.0f}
	},
	{
		{0.0f,1.0f,0.0f},
		{0.0f,-1.0f,0.0f}
	},
};

GLfloat sinLine[120][3] ;
GLfloat circleLine[120][3];
GLfloat zigLine[6][3] = {
	{-1.0f,0.0f,0.0f},
	{-0.8f,0.5f,0.0f},
	{-0.2f,-0.5f,0.0f},
	{0.2f,0.5f,0.0f},
	{0.8f,-0.5f,0.0f},
	{1.0f,0.0f,0.0f}
};

GLfloat LineC[6][3] = {
	{0.0f,0.0f,0.0f}
};

GLfloat tri[3][3] = 
{
	{0.0f,0.1f,0.0f},
	{-0.05f,-0.05f,0.0f},
	{0.05f,-0.05f,0.0f}
};
GLfloat triC[3][3] = 
{
	{0.3f,0.0f,0.0f},
	{0.3f,0.0f,0.0f},
	{0.3f,0.0f,0.0f}
};

//--- ���� ����
GLuint VAO[6], VBO[2];

void InitBuffer()
{
	for (int i = 0; i < 6; i++)
	{
		glGenVertexArrays(1, &VAO[i]); //--- VAO �� �����ϰ� �Ҵ��ϱ�
		glBindVertexArray(VAO[i]); //--- VAO�� ���ε��ϱ�
		glGenBuffers(2, VBO); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

		if (i == 2)
		{
			glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(sinLine), sinLine, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(LineC), LineC, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}
		else if (i == 3)
		{
			glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(circleLine), circleLine, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(LineC), LineC, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}
		else if (i == 4)
		{
			glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(zigLine), zigLine, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(LineC), LineC, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}
		else if (i == 5)
		{
			glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(tri), tri, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(triC), triC, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}
		else
		{
			glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(DrawLine[i]), DrawLine[i], GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(LineC), LineC, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}
	}
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
	glutTimerFunc(100, Timer2, 2);

	glutMainLoop(); // �̺�Ʈ ó�� ����
}

bool dimention = true;
bool Move = false;
bool Shrink = false;
bool Rotate = false;

bool Size = false;

float sx = 1.0f;
float sy = 1.0f;
float sz = 1.0f;

float dx = -1.0f;
float dy = 0.0f;
float Ry = 0.0f;

int shape = 1;
int counter = 0;

GLUquadricObj* qobj;

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	for (int i = 0; i < 120; i++)
	{
		sinLine[i][0] = -1.0f + ((float)i*0.03f);
		sinLine[i][1] = -glm::sin(6 * (float)i)/2;
		sinLine[i][2] = 0.0f;
	}
	for (int i = 0; i < 120; i++)
	{
		circleLine[i][0] = glm::cos(6 * i) * 0.2 + ((float)i * 0.02f) - 1;
		circleLine[i][1] = glm::sin(6 * i) * 0.2;
		circleLine[i][2] = 0.0f;
	}

	//--- ����� ���� ����
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // �������� ����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�

	glUseProgram(shaderID);

	glm::mat4 T = glm::mat4(1.0f);
	glm::mat4 R = glm::mat4(1.0f);
	glm::mat4 S = glm::mat4(1.0f);
	glm::mat4 RS = glm::mat4(1.0f);
	glm::mat4 TRS = glm::mat4(1.0f);
	T = glm::translate(T, glm::vec3(dx, dy, 0.0f));
	R = glm::rotate(R, glm::radians(Ry), glm::vec3(0.0f, 1.0f, 0.0f));
	S = glm::scale(S, glm::vec3(sx, sy, sz));
	RS = R * S;
	TRS = R * T * S;
	unsigned int modelLocation = glGetUniformLocation(shaderID, "transform");

	if (dimention == false)
	{
		qobj = gluNewQuadric();
		gluQuadricDrawStyle(qobj, GLU_LINE);
		gluQuadricNormals(qobj, GLU_SMOOTH);
		gluQuadricOrientation(qobj, GLU_OUTSIDE);
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TRS));
		gluSphere(qobj, 0.1, 50, 50);
	}

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	for (int i = 0; i < 2; i++)
	{
		glBindVertexArray(VAO[i]);
		glDrawArrays(GL_LINE_STRIP, 0, 2);
	}
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(R));
	if (shape == 1)
	{
		glBindVertexArray(VAO[2]);
		glDrawArrays(GL_LINE_STRIP, 0, 120);
	}
	else if(shape == 2)
	{
		glBindVertexArray(VAO[3]);
		glDrawArrays(GL_LINE_STRIP, 0, 120);
	}
	else
	{
		glBindVertexArray(VAO[4]);
		glDrawArrays(GL_LINE_STRIP, 0, 6);
	}

	if (dimention == true)
	{
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TRS));
		glBindVertexArray(VAO[5]);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	
	glutSwapBuffers(); // ȭ�鿡 ����ϱ�1
}
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
	glViewport(0, 0, w, h);
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case '1':
		dx = (-1.0f);
		dy = 0.0f;
		shape = 1;
		counter = 0;
		break;
	case '2':
		dx = glm::cos(6) * 0.2 + 0.02f - 1;
		dy = glm::sin(6) * 0.2;
		shape = 2;
		counter = 0;
		break;
	case '3':
		dx = -1.0f;
		dy = 0.0f;
		shape = 3;
		counter = 0;
		break;
	case 'd':
		if (dimention == false)
			dimention = true;
		else
			dimention = false;
		break;
	case 't':
		Move = true;
		break;
	case 's':
		Shrink = true;
		break;
	case 'r':
		Rotate = true;
		break;
	case 'c':
		dimention = true;
		Move = false;
		Shrink = false;
		Rotate = false;

		Size = false;

		sx = 1.0f;
		sy = 1.0f;
		sz = 1.0f;
		dx = -1.0f;
		dy = -glm::sin(12) / 2;
		Ry = 0.0f;

		counter = 0;
		shape = 1;
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
	if (Shrink == true)
	{
		if (Size == false)
		{
			sx += 0.01f;
			sy += 0.01f;
			sz += 0.01f;
			if (sx >= 1.5)
				Size = true;
		}
		else
		{
			sx -= 0.01f;
			sy -= 0.01f;
			sz -= 0.01f;
			if (sx <= 1)
				Size = false;
		}
	}
	if (Rotate == true)
	{
		Ry += 0.3f;
	}
	InitBuffer();
	glutPostRedisplay();
	glutTimerFunc(1, Timer, 1);
}

void Timer2(int value)
{
	if (Move == true)
	{
		if (shape == 1)
		{
			dx = dx + 0.03f;
			dy = -glm::sin(6 * counter)/2;

			counter++;
			counter = counter % 120;
		}
		else if (shape == 2)
		{
			dx = glm::cos(6 * counter) * 0.2 + ((float)counter * 0.02f) - 1;
			dy = glm::sin(6 * counter) * 0.2;
			counter++;
			counter = counter % 120;
		}
		else
		{
			dx = dx + 0.02f;
			dy = dy + (zigLine[counter + 1][1] - zigLine[counter][1]) / (zigLine[counter + 1][0] - zigLine[counter][0]) * 0.02f;

			if (dx >= zigLine[counter+1][0])
				counter++;
			counter = counter % 5;
		}
	}
	InitBuffer();
	glutPostRedisplay();
	glutTimerFunc(100, Timer2, 2);
}