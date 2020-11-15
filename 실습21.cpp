#include <iostream>
#include <gl/glew.h> // �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <fstream>

using namespace std;

#define LIGHT_AMBIENT	glm::vec3(0.1, 0.1, 0.1)

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Mouse(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y);
void Timer(int value);
GLchar* filetobuf(const GLchar* filename);
GLuint make_shaderProgram();

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
	vertexsource = filetobuf("vertex_light2.glsl");

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
	fragmentsource = filetobuf("fragment_light2.glsl");

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

GLfloat Stage[6][3] = {
	{1.0f,0.0f,1.0f},
	{1.0f,0.0f,-1.0f},
	{-1.0f,0.0f,-1.0f},
	{1.0f,0.0f,1.0f},
	{-1.0f,0.0f,-1.0f},
	{-1.0f,0.0f,1.0f}
};
GLfloat StageC[4][3] = {
	{0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f}
};

float vertices[] = { //--- ���ؽ� �Ӽ�: ��ǥ��(FragPos), �븻�� (Normal)
-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f
};

GLfloat LPos[3] = { 0.0f,3.0f,3.0f };
GLfloat LColor[3] = { 1.0f, 1.0f, 1.0f };

//--- ���� ����
GLuint VAO[5], VBO;

GLuint shaderID;

bool lighton = true;

void InitBufferA()
{
	glUseProgram(shaderID);
	int lightPosLocation = glGetUniformLocation(shaderID, "lightPos"); //--- lightPos �� ����: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, LPos[0], LPos[1], LPos[2]);
	int lightColorLocation = glGetUniformLocation(shaderID, "lightColor"); //--- lightColor �� ����: (1.0, 1.0, 1.0) ���
	glUniform3f(lightColorLocation, LColor[0], LColor[1], LColor[2]);
	int red = glGetUniformLocation(shaderID, "objectColor");
	glUniform3f(red, 1.0f, 1.0f, 1.0f);
}

void InitBuffer()
{
	glGenVertexArrays(1, &VAO[0]); //--- VAO �� �����ϰ� �Ҵ��ϱ�
	glBindVertexArray(VAO[0]); //--- VAO�� ���ε��ϱ�
	glGenBuffers(1, &VBO); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //--- ��ġ �Ӽ�
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //--- �븻 �Ӽ�
	glEnableVertexAttribArray(1);

	glUseProgram(shaderID);
	int lightPosLocation = glGetUniformLocation(shaderID, "lightPos"); //--- lightPos �� ����: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, LPos[0], LPos[1], LPos[2]);
	int lightColorLocation = glGetUniformLocation(shaderID, "lightColor"); //--- lightColor �� ����: (1.0, 1.0, 1.0) ���
	glUniform3f(lightColorLocation, LColor[0], LColor[1], LColor[2]);
	int red = glGetUniformLocation(shaderID, "objectColor");
	glUniform3f(red, 1.0f, 0.0f, 0.0f);
}

void InitBuffer1()
{
	glGenVertexArrays(1, &VAO[1]); //--- VAO �� �����ϰ� �Ҵ��ϱ�
	glBindVertexArray(VAO[1]); //--- VAO�� ���ε��ϱ�
	glGenBuffers(1, &VBO); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //--- ��ġ �Ӽ�
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //--- �븻 �Ӽ�
	glEnableVertexAttribArray(1);

	glUseProgram(shaderID);
	int lightPosLocation = glGetUniformLocation(shaderID, "lightPos"); //--- lightPos �� ����: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, LPos[0], LPos[1], LPos[2]);
	int lightColorLocation = glGetUniformLocation(shaderID, "lightColor"); //--- lightColor �� ����: (1.0, 1.0, 1.0) ���
	glUniform3f(lightColorLocation, LColor[0], LColor[1], LColor[2]);
	int yellow = glGetUniformLocation(shaderID, "objectColor");
	glUniform3f(yellow, 1.0f, 1.0f, 0.0f);
}
void InitBuffer2()
{
	glGenVertexArrays(1, &VAO[2]); //--- VAO �� �����ϰ� �Ҵ��ϱ�
	glBindVertexArray(VAO[2]); //--- VAO�� ���ε��ϱ�
	glGenBuffers(1, &VBO); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //--- ��ġ �Ӽ�
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //--- �븻 �Ӽ�
	glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &VAO[3]); //--- VAO �� �����ϰ� �Ҵ��ϱ�
	glBindVertexArray(VAO[3]); //--- VAO�� ���ε��ϱ�
	glGenBuffers(1, &VBO); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //--- ��ġ �Ӽ�
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //--- �븻 �Ӽ�
	glEnableVertexAttribArray(1);

	glUseProgram(shaderID);
	int lightPosLocation = glGetUniformLocation(shaderID, "lightPos"); //--- lightPos �� ����: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, LPos[0], LPos[1], LPos[2]);
	int lightColorLocation = glGetUniformLocation(shaderID, "lightColor"); //--- lightColor �� ����: (1.0, 1.0, 1.0) ���
	glUniform3f(lightColorLocation, LColor[0], LColor[1], LColor[2]);
	int cyan = glGetUniformLocation(shaderID, "objectColor");
	glUniform3f(cyan, 0.0f, 1.0f, 1.0f);
}
void InitBuffer3()
{
	glGenVertexArrays(1, &VAO[4]); //--- VAO �� �����ϰ� �Ҵ��ϱ�
	glBindVertexArray(VAO[4]); //--- VAO�� ���ε��ϱ�
	glGenBuffers(1, &VBO); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //--- ��ġ �Ӽ�
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //--- �븻 �Ӽ�
	glEnableVertexAttribArray(1);

	glUseProgram(shaderID);
	int lightPosLocation = glGetUniformLocation(shaderID, "lightPos"); //--- lightPos �� ����: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, LPos[0], LPos[1], LPos[2]);
	int lightColorLocation = glGetUniformLocation(shaderID, "lightColor"); //--- lightColor �� ����: (1.0, 1.0, 1.0) ���
	glUniform3f(lightColorLocation, LColor[0], LColor[1], LColor[2]);
	int copper = glGetUniformLocation(shaderID, "objectColor");
	glUniform3f(copper, 0.5f, 0.3f, 0.0f);
}

GLint width = 1200;
GLint height = 1200;

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

	glutDisplayFunc(drawScene); // ��� �ݹ��Լ��� ����
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �ݹ��Լ� ����
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(10, Timer, 1);

	glutMainLoop(); // �̺�Ʈ ó�� ����
}

float CP[3] = {0.0f , 5.0f,10.0f };
float look_pos[3] = { 0.0f,0.0f ,0.0f };

float S1_pos[3] = { 0.0f, 0.8f, 0.0f };
float S2_Angle = 45.0f;
float S3_Angle = 0.0f;

GLUquadricObj* qobj;

float vAngle = 0;

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	//--- ����� ���� ����
	glClearColor(0.0f,0.0f,0.0f,0.0f); // �������� ����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�

	glUseProgram(shaderID);

	glEnable(GL_DEPTH_TEST);

	int modelLoc = glGetUniformLocation(shaderID, "Transform"); //--- ���ؽ� ���̴����� �𵨸� ��ȯ ��� �������� �޾ƿ´�.
	int viewLoc = glGetUniformLocation(shaderID, "viewTransform"); //--- ���ؽ� ���̴����� ���� ��ȯ ��� �������� �޾ƿ´�.
	int projLoc = glGetUniformLocation(shaderID, "projectionTransform"); //--- ���ؽ� ���̴����� ���� ��ȯ ��� �������� �޾ƿ´�.
	//--- �𵨸� ��ȯ
	glm::mat4 mTransform = glm::mat4(1.0f);
	mTransform = glm::translate(mTransform, glm::vec3(0.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform));
	//--- ���� ��ȯ
	glm::vec3 cameraPos = glm::vec3(CP[0], CP[1], CP[2]);
	glm::vec3 cameraDirection = glm::vec3(look_pos[0], look_pos[1], look_pos[2]);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 vTransform = glm::mat4(1.0f);
	glm::mat4 vTrans = glm::mat4(1.0f);
	glm::mat4 vrotate = glm::mat4(1.0f);
	glm::mat4 vTR = glm::mat4(1.0f);
	vTrans = glm::translate(vTrans, glm::vec3(-CP[0], -CP[1], -CP[2]));
	vTR = glm::translate(vTR, glm::vec3(CP[0], CP[1], CP[2]));
	vrotate = glm::rotate(vrotate, glm::radians(vAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	vTransform = vTransform * vTR * vrotate * vTrans;
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(vTransform));
	//--- ���� ��ȯ ����� ������ ��, ���ؽ� ���̴��� �����Ѵ�.
	glm::mat4 pTransform = glm::mat4(1.0f);
	glm::mat4 pTransform1 = glm::mat4(1.0f);
	pTransform = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.1f, 50.0f);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pTransform));


	glm::vec3 lightAmbient = LIGHT_AMBIENT;
	GLuint lightAmbientLocation = glGetUniformLocation(shaderID, "lightAmbient");
	glUniform3fv(lightAmbientLocation, 1, (float*)&lightAmbient);


	glm::mat4 S1T = glm::mat4(1.0f);
	glm::mat4 S2T = glm::mat4(1.0f);
	glm::mat4 S3_1T = glm::mat4(1.0f);
	glm::mat4 S3_2T = glm::mat4(1.0f);
	glm::mat4 S3_1TR = glm::mat4(1.0f);
	glm::mat4 S3_2TR = glm::mat4(1.0f);
	glm::mat4 S1R = glm::mat4(1.0f);
	glm::mat4 S2R = glm::mat4(1.0f);
	glm::mat4 S3R_2 = glm::mat4(1.0f);
	glm::mat4 S3_1R = glm::mat4(1.0f);
	glm::mat4 S3_2R = glm::mat4(1.0f);
	glm::mat4 S1S = glm::mat4(1.0f);
	glm::mat4 S2S = glm::mat4(1.0f);
	glm::mat4 S3_1S = glm::mat4(1.0f);
	glm::mat4 S3_2S = glm::mat4(1.0f);
	glm::mat4 SS = glm::mat4(1.0f);

	S1T = glm::translate(S1T, glm::vec3(S1_pos[0], S1_pos[1], S1_pos[2]));
	S1S = glm::scale(S1S, glm::vec3(2.0f, 0.5f, 2.0f));

	S2T = glm::translate(S2T, glm::vec3(S1_pos[0], S1_pos[1] + 0.5f, S1_pos[2]));
	S2R = glm::rotate(S2R, glm::radians(S2_Angle), glm::vec3(0.0f, 1.0f, 0.0f));
	S2S = glm::scale(S2S, glm::vec3(1.0f, 0.5f, 1.0f));

	S3R_2 = glm::rotate(S3R_2, glm::radians(S2_Angle), glm::vec3(0.0f, 1.0f, 0.0f));

	S3_1T = glm::translate(S3_1T, glm::vec3(S1_pos[0] -0.2f, S1_pos[1] + 0.8f, S1_pos[2]));
	S3_1R = glm::rotate(S3_1R, glm::radians(S3_Angle), glm::vec3(1.0f, 0.0f, 0.0f));
	S3_1S = glm::scale(S3_1S, glm::vec3(0.1f, 0.5f, 0.1f));

	S3_2T = glm::translate(S3_2T, glm::vec3(S1_pos[0] + 0.2f, S1_pos[1] + 0.8f, S1_pos[2]));
	S3_2R = glm::rotate(S3_2R, glm::radians(-S3_Angle), glm::vec3(1.0f, 0.0f, 0.0f));
	S3_2S = glm::scale(S3_2S, glm::vec3(0.1f, 0.5f, 0.1f));

	S3_1TR = glm::translate(S3_1TR, glm::vec3(-S1_pos[0] + 0.2f, -S1_pos[1] - 1.0f, -S1_pos[2]));
	S3_2TR = glm::translate(S3_2TR, glm::vec3(-S1_pos[0] - 0.2f, -S1_pos[1] - 1.0f, -S1_pos[2]));

	SS = glm::scale(SS, glm::vec3(10.0f, 1.0f, 10.0f));

	InitBuffer();
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * S1T * S1R * S1S));
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	InitBuffer1();
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * S2T * S2R * S2S));
	glBindVertexArray(VAO[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	InitBuffer2();
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform  * S3_1T * S3_1R  * S3_1S * S3R_2));
	glBindVertexArray(VAO[2]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * S3_2T * S3_2R  * S3_2S * S3R_2));
	glBindVertexArray(VAO[3]);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	InitBuffer3();
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * SS));
	glBindVertexArray(VAO[4]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	

	InitBufferA();
	glm::mat4 LT = glm::mat4(1.0f);

	LT = glm::translate(LT, glm::vec3(LPos[0], LPos[1], LPos[2]));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(LT));
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GLU_FILL);
	gluQuadricNormals(qobj, GLU_SMOOTH);
	gluQuadricOrientation(qobj, GLU_OUTSIDE);
	gluSphere(qobj, 0.05, 50, 50);

	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
	glViewport(0, 0, w, h);
}

int Cnum = 2;

int Rmove = 0;


void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'b':
		S1_pos[0] += 0.1f;
		break;
	case 'B':
		S1_pos[0] -= 0.1f;
		break;
	case 'c':
		if (Cnum == 1)
		{
			LColor[0] = 1.0f;
			LColor[1] = 1.0f;
			LColor[2] = 1.0f;
			Cnum = 2;
		}
		else if (Cnum == 2)
		{
			LColor[0] = 0.5f;
			LColor[1] = 0.3f;
			LColor[2] = 1.0f;
			Cnum = 3;
		}
		else
		{
			LColor[0] = 1.0f;
			LColor[1] = 0.5f;
			LColor[2] = 0.3f;
			Cnum = 1;
		}
		break;
	case 'm':
		if (lighton == true)
		{
			LColor[0] = 0.1f;
			LColor[1] = 0.1f;
			LColor[2] = 0.1f;
			lighton = false;
		}
		else
		{
			LColor[0] = 1.0f;
			LColor[1] = 1.0f;
			LColor[2] = 1.0f;
			lighton = true;
		}
		break;
	case 'r':
		Rmove = 1;
		break;
	case 'R':
		Rmove = 2;
		break;
	case 's':
		Rmove = 0;
		break;
	case 'q':
		exit(EXIT_FAILURE);
		break;
	}
	glutPostRedisplay(); //--- ������ �ٲ𶧸��� ��� �ݹ��Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
}

float rad = 0.0f;

void Timer(int value)
{
	if (Rmove == 1)
	{
		rad += 0.2f;
		LPos[0] = glm::cos(rad) * 5;
		LPos[2] = glm::sin(rad) * 5;
	}
	else if (Rmove == 2)
	{
		rad -= 0.2f;
		LPos[0] = glm::cos(rad) * 5;
		LPos[2] = glm::sin(rad) * 5;
	}

	glutPostRedisplay();
	glutTimerFunc(50, Timer, 1);
}
