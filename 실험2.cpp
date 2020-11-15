#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <gl/glew.h> // �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <stdio.h>

using namespace std;

FILE* Sphere = fopen("sphere.obj", "r");


GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void Timer(int value);
GLchar* filetobuf(const GLchar* filename);
void ReadObj(FILE* obhFile);

// ���̴� ���� �б�
GLchar* filetobuf(const GLchar* filename)
{
	cout << "����" << endl;

	ifstream readFile;             //���� ������ ���� ����
	GLchar tmp[1024];
	GLchar str[2048] = { NULL };

	readFile.open(filename);    //���� ����
	if (readFile.is_open())    //������ ���ȴ��� Ȯ��
	{
		while (!readFile.eof())    //���� ������ �о����� Ȯ��
		{
			readFile.getline(tmp, 1024);    //���پ� �о����
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
	vertexsource = filetobuf("vertex_light.glsl");

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
	fragmentsource = filetobuf("fragment_light.glsl");

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

// obj ���� �б�
void ReadObj(FILE* objFile, glm::vec3** Vout, glm::vec3** VNout, glm::ivec3** Fout, glm::ivec3** FNout, int* vertexNout, int* faceNout)
{
	//--- 1. ��ü ���ؽ� ���� �� �ﰢ�� ���� ����
	char count[100];
	int vertexNum = 0;
	int vertexNNum = 0;
	int faceNum = 0;
	while (!feof(objFile)) {
		fscanf(objFile, "%s", count);
		if (count[0] == 'v' && count[1] == '\0')
			vertexNum += 1;
		else if (count[0] == 'v' && count[1] == 'n')
			vertexNNum += 1;
		else if (count[0] == 'f' && count[1] == '\0')
			faceNum += 1;
		memset(count, '\0', sizeof(count)); // �迭 �ʱ�ȭ
	}
	//--- 2. �޸� �Ҵ�
	glm::vec3* V;
	glm::vec3* VN;
	glm::ivec3* F;
	glm::ivec3* FN;
	int VIndex = 0;
	int VNIndex = 0;
	int FIndex = 0;
	V = (glm::vec3*)malloc(sizeof(glm::vec3) * vertexNum);
	VN = (glm::vec3*)malloc(sizeof(glm::vec3) * vertexNNum);
	F = (glm::ivec3*)malloc(sizeof(glm::ivec3) * faceNum);
	FN = (glm::ivec3*)malloc(sizeof(glm::ivec3) * faceNum);
	rewind(objFile);

	int T[3];

	//--- 3. �Ҵ�� �޸𸮿� �� ���ؽ�, ���̽� ���� �Է�
	while (!feof(objFile)) {
		fscanf(objFile, "%s", count);
		if (count[0] == 'v' && count[1] == '\0') {
			fscanf(objFile, "%f %f %f",
				&V[VIndex].x, &V[VIndex].y,
				&V[VIndex].z);
			VIndex++;

		}
		else if (count[0] == 'v' && count[1] == 'n') {
			fscanf(objFile, "%f %f %f",
				&VN[VNIndex].x, &VN[VNIndex].y,
				&VN[VNIndex].z);
			VNIndex++;
		}
		else if (count[0] == 'f' && count[1] == '\0') {
			fscanf(objFile, "%d/%d/%d %d/%d/%d %d/%d/%d",
				&F[FIndex].x, &T[0], &FN[FIndex].x,
				&F[FIndex].y, &T[1], &FN[FIndex].y,
				&F[FIndex].z, &T[2], &FN[FIndex].z);
			FIndex++;
		}
	}

	/**Vout = V;
	*VNout = VN;
	*Fout = F;
	*FNout = FN;
	*vertexNout = vertexNum;
	*faceNout = faceNum;*/
}


GLfloat circle[120][3] = {
	 {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0}
};

GLuint shaderID;

//--- ���� ����
GLuint VAO[6], VBO[2], EBO;

void InitBuffer()
{
	glm::vec3* V = NULL;
	glm::vec3* VN = NULL;
	glm::ivec3* F = NULL;
	glm::ivec3* FN = NULL;
	int vertexN = 0, faceN = 0;
	ReadObj(Sphere, &V, &VN, &F, &FN, &vertexN, &faceN);

	glGenVertexArrays(1, &VAO[0]); //--- VAO �� �����ϰ� �Ҵ��ϱ�
	glGenBuffers(2, VBO); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

	glBindVertexArray(VAO[0]); //--- VAO�� ���ε��ϱ�
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(V), V, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VN), VN, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glUseProgram(shaderID);
	int lightPosLocation = glGetUniformLocation(shaderID, "lightPos"); //--- lightPos �� ����: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, 0.0, 0.0, 5.0);
	int lightColorLocation = glGetUniformLocation(shaderID, "lightColor"); //--- lightColor �� ����: (1.0, 1.0, 1.0) ���
	glUniform3f(lightColorLocation, 1.0, 1.0, 1.0);
	int objColorLocation = glGetUniformLocation(shaderID, "objectColor"); //--- object Color�� ����: (1.0, 0.5, 0.3)�� ��
	glUniform3f(objColorLocation, 1.0, 0.5, 0.3);
}

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

float timecount = 0.0f;

bool Wire = true;
bool Rdirection = false;
float Howfar = 0.0f;
float Hmove[2] = { 0.0f,0.0f };

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	//--- ����� ���� ����
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // �������� ����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	glUseProgram(shaderID);
	glEnable(GL_DEPTH_TEST);

	float num = (3.14 / 180) * 12;

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
	T1 = glm::translate(T1, glm::vec3(2 * cos(0), 0, 0));
	T2 = glm::translate(T2, glm::vec3(0.8 * cos(0), 0, 0));

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

	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 120);

	InitBuffer();

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
	case 'q':
		exit(EXIT_FAILURE);
		break;
	}
	InitBuffer();
	glutPostRedisplay(); //--- ������ �ٲ𶧸��� ��� �ݹ��Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
}

void Timer(int value)
{


	glutPostRedisplay();
	glutTimerFunc(15, Timer, 1);
}
