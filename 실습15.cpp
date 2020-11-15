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
	vertexsource = filetobuf("vertex_view.glsl");

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
	fragmentsource = filetobuf("fragment_view.glsl");

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

GLfloat circle[6][30][3];
GLfloat circleC[30];

//--- ���� ����
GLuint VAO[6], VBO[2];

void InitBuffer()
{
	for (int i = 0; i < 6; i++)
	{
		glGenVertexArrays(1, &VAO[i]); //--- VAO �� �����ϰ� �Ҵ��ϱ�
		glBindVertexArray(VAO[i]); //--- VAO�� ���ε��ϱ�
		glGenBuffers(2, VBO); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(circle[i]), circle[i], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(circleC), circleC, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
	}
}

GLuint shaderID;

float width = 1200;
float height = 1200;

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

GLUquadricObj* qobj[7];

float timecount = 0.0f;

bool Wire = true;
bool Rdirection = false;
float Howfar = 0.0f;
float Hmove[2] = { 0.0f,0.0f};

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	//--- ����� ���� ����
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // �������� ����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	glUseProgram(shaderID);
	glEnable(GL_DEPTH_TEST);

	float num = (3.14 / 180) * 12;

	// �˵� �׸���
	for (int i = 0; i < 3; i++)
	{
		for (int k = 0; k < 30; k++)
		{
			circle[i][k][0] = 2 * cos(num * k);
			circle[i][k][1] = 0.0;
			circle[i][k][2] = 2 * sin(num * k);
		}
	}
	for (int i = 3; i < 6; i++)
	{
		for (int k = 0; k < 30; k++)
		{
			circle[i][k][0] = 0.8 * cos(num * k);
			circle[i][k][1] = 0.0;
			circle[i][k][2] = 0.8 * sin(num * k);
		}
	}

	glm::mat4 Rself = glm::mat4(1.0f);
	glm::mat4 Rlarge = glm::mat4(1.0f);
	glm::mat4 R1 = glm::mat4(1.0f);
	glm::mat4 R2 = glm::mat4(1.0f);
	glm::mat4 T1 = glm::mat4(1.0f);
	glm::mat4 T2 = glm::mat4(1.0f);
	glm::mat4 S1 = glm::mat4(1.0f);
	glm::mat4 S2 = glm::mat4(1.0f);

	Rlarge = glm::rotate(Rlarge, glm::radians(timecount), glm::vec3(0.0f, 1.0f, 0.0f));
	Rself = glm::rotate(Rself, glm::radians(timecount), glm::vec3(0.0f, 1.0f, 0.0f));
	R1 = glm::rotate(R1, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	R2 = glm::rotate(R2, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	T1 = glm::translate(T1, glm::vec3(2 * cos(0),0,0));
	T2 = glm::translate(T2, glm::vec3(0.8 * cos(0),0,0));

	S1 = R1 * Rlarge * Rself;
	S2 = R2 * Rlarge * Rself;

	float xAngle = 10.0f;

	int modelLoc = glGetUniformLocation(shaderID, "Transform"); //--- ���ؽ� ���̴����� �𵨸� ��ȯ ��� �������� �޾ƿ´�.
	int viewLoc = glGetUniformLocation(shaderID, "viewTransform"); //--- ���ؽ� ���̴����� ���� ��ȯ ��� �������� �޾ƿ´�.
	int projLoc = glGetUniformLocation(shaderID, "projectionTransform"); //--- ���ؽ� ���̴����� ���� ��ȯ ��� �������� �޾ƿ´�.
	//--- �𵨸� ��ȯ
	glm::mat4 mTransform = glm::mat4(1.0f);
	glm::mat4 mT = glm::mat4(1.0f);
	mTransform = glm::rotate(mTransform, glm::radians(xAngle), glm::vec3(1.0f, 0.0f, 0.0f));
	mT = glm::translate(mT, glm::vec3(Hmove[0], Hmove[1], Howfar));
	mTransform = mT * mTransform;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * Rlarge * Rself));
	//--- ���� ��ȯ
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 vTransform = glm::mat4(1.0f);
	vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(vTransform));
	//--- ���� ��ȯ ����� ������ ��, ���ؽ� ���̴��� �����Ѵ�.
	glm::mat4 pTransform = glm::mat4(1.0f);
	pTransform = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.1f, 200.0f);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pTransform));

	qobj[0] = gluNewQuadric();
	if(Wire == false)
		gluQuadricDrawStyle(qobj[0], GLU_FILL);
	else
		gluQuadricDrawStyle(qobj[0], GLU_LINE);
	gluQuadricNormals(qobj[0], GLU_SMOOTH);
	gluQuadricOrientation(qobj[0], GLU_OUTSIDE);
	gluSphere(qobj[0], 0.5, 50, 50);

	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_LINE_LOOP, 0, 30);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * Rlarge * Rself * T1));
	qobj[1] = gluNewQuadric();
	if (Wire == false)
		gluQuadricDrawStyle(qobj[1], GLU_FILL);
	else
	gluQuadricDrawStyle(qobj[1], GLU_LINE);
	gluQuadricNormals(qobj[1], GLU_SMOOTH);
	gluQuadricOrientation(qobj[1], GLU_OUTSIDE);
	gluSphere(qobj[1], 0.2, 50, 50);
	glBindVertexArray(VAO[3]);
	glDrawArrays(GL_LINE_LOOP, 0, 30);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * Rlarge * Rself * T1 * T2));
	qobj[4] = gluNewQuadric();
	if (Wire == false)
		gluQuadricDrawStyle(qobj[4], GLU_FILL);
	else
	gluQuadricDrawStyle(qobj[4], GLU_LINE);
	gluQuadricNormals(qobj[4], GLU_SMOOTH);
	gluQuadricOrientation(qobj[4], GLU_OUTSIDE);
	gluSphere(qobj[4], 0.1, 50, 50);

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * S1));
	glBindVertexArray(VAO[1]);
	glDrawArrays(GL_LINE_LOOP, 0, 30);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform  * S1 * T1));
	qobj[2] = gluNewQuadric();
	if (Wire == false)
		gluQuadricDrawStyle(qobj[2], GLU_FILL);
	else
	gluQuadricDrawStyle(qobj[2], GLU_LINE);
	gluQuadricNormals(qobj[2], GLU_SMOOTH);
	gluQuadricOrientation(qobj[2], GLU_OUTSIDE);
	gluSphere(qobj[2], 0.2, 50, 50);
	glBindVertexArray(VAO[4]);
	glDrawArrays(GL_LINE_LOOP, 0, 30);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * S1 * T1 * T2));
	qobj[5] = gluNewQuadric();
	if (Wire == false)
		gluQuadricDrawStyle(qobj[5], GLU_FILL);
	else
	gluQuadricDrawStyle(qobj[5], GLU_LINE);
	gluQuadricNormals(qobj[5], GLU_SMOOTH);
	gluQuadricOrientation(qobj[5], GLU_OUTSIDE);
	gluSphere(qobj[5], 0.1, 50, 50);

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * S2));
	glBindVertexArray(VAO[2]);
	glDrawArrays(GL_LINE_LOOP, 0, 30);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * S2 * T1));
	qobj[3] = gluNewQuadric();
	if (Wire == false)
		gluQuadricDrawStyle(qobj[3], GLU_FILL);
	else
	gluQuadricDrawStyle(qobj[3], GLU_LINE);
	gluQuadricNormals(qobj[3], GLU_SMOOTH);
	gluQuadricOrientation(qobj[3], GLU_OUTSIDE);
	gluSphere(qobj[3], 0.2, 50, 50);
	glBindVertexArray(VAO[5]);
	glDrawArrays(GL_LINE_LOOP, 0, 30);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * S2 * T1 * T2));
	qobj[6] = gluNewQuadric();
	if (Wire == false)
		gluQuadricDrawStyle(qobj[6], GLU_FILL);
	else
	gluQuadricDrawStyle(qobj[6], GLU_LINE);
	gluQuadricNormals(qobj[6], GLU_SMOOTH);
	gluQuadricOrientation(qobj[6], GLU_OUTSIDE);
	gluSphere(qobj[6], 0.1, 50, 50);
	
	glutSwapBuffers(); // ȭ�鿡 ����ϱ�1
}
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
	glViewport(0, 0, w, h);
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'm':
		if (Wire == false)
			Wire = true;
		else
			Wire = false;
		break;
	case 'w':
		Hmove[1] = Hmove[1] + 0.1;
		break;
	case 'a':
		Hmove[0] = Hmove[0] - 0.1;
		break;
	case 's':
		Hmove[1] = Hmove[1] - 0.1;
		break;
	case 'd':
		Hmove[0] = Hmove[0] + 0.1;
		break;
	case 'z':
		Howfar = Howfar + 0.1;
		break;
	case 'x':
		Howfar = Howfar - 0.1;
		break;
	case 'y':
		if (Rdirection == false)
			Rdirection = true;
		else
			Rdirection = false;
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
	if (Rdirection == true)
		timecount = timecount + 1;
	else
		timecount = timecount - 1;

	InitBuffer();
	glutPostRedisplay();
	glutTimerFunc(15, Timer, 1);
}
