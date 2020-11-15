#include <iostream>
#include <gl/glew.h> // 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <time.h>
#include <random>

#include "ReadObj.h"

#define FILE_NAME		"sphere.obj"

#define WINDOW_WITDH	800
#define WINDOW_HEIGHT	600

#define CAMERA_POS		glm::vec3(0.0, 5.0, 30.0)
#define OBJECT_COLOR	glm::vec3(1.0, 1.0, 1.0)
#define LIGHT_AMBIENT	glm::vec3(0.03, 0.03, 0.03)
#define LIGHT_POS		glm::vec3(1000.0, 1000.0, 0.0)
#define LIGHT_COLOR		glm::vec3(1.0, 1.0, 1.0)

using namespace std;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void Timer(int value);
GLchar* filetobuf(const GLchar* filename);

// 세이더 파일 읽기
GLchar* filetobuf(const GLchar* filename)
{
	cout << "정상" << endl;

	ifstream readFile;             //읽을 목적의 파일 선언
	GLchar tmp[1024];
	GLchar str[2048] = { NULL };

	readFile.open(filename);    //파일 열기
	if (readFile.is_open())    //파일이 열렸는지 확인
	{
		while (!readFile.eof())    //파일 끝까지 읽었는지 확인
		{
			readFile.getline(tmp, 1024);    //한줄씩 읽어오기
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
		cout << "정상2" << endl;
	}
	readFile.close();    //파일 닫기

	cout << str << endl;

	return str;
}

// 세이더 생성
GLuint make_shaderProgram()
{
	GLint result;
	GLchar errorLog[512];

	GLchar* vertexsource;
	vertexsource = filetobuf("vertex_blend.glsl");

	//--- 버텍스 세이더 읽어 저장하고 컴파일 하기
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexsource, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << endl;
		return false;
	}

	GLchar* fragmentsource;
	fragmentsource = filetobuf("fragment_blend.glsl");

	//--- 프래그먼트 세이더 읽어 저장하고 컴파일하기
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentsource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << endl;
		return false;
	}

	GLuint ShaderProgramID = glCreateProgram(); //--- 세이더 프로그램 만들기
	glAttachShader(ShaderProgramID, vertexShader); //--- 세이더 프로그램에 버텍스 세이더 붙이기
	glAttachShader(ShaderProgramID, fragmentShader); //--- 세이더 프로그램에 프래그먼트 세이더 붙이기
	glLinkProgram(ShaderProgramID); //--- 세이더 프로그램 링크하기
	glDeleteShader(vertexShader); //--- 세이더 프로그램에 링크하여 세이더 객체 자체는 삭제 가능
	glDeleteShader(fragmentShader);

	glGetProgramiv(ShaderProgramID, GL_LINK_STATUS, &result); // ---세이더가 잘 연결되었는지 체크하기
	if (!result)
	{
		glGetProgramInfoLog(ShaderProgramID, 512, NULL, errorLog);
		cerr << "ERROR: shader program 연결 실패\n" << errorLog << endl;
		return false;
	}
	glUseProgram(ShaderProgramID); //--- 만들어진 세이더 프로그램 사용하기
	//--- 여러 개의 세이더프로그램 만들 수 있고, 특정 프로그램을 사용하려면
	//--- glUseProgram 함수를 호출하여 사용 할 특정 프로그램을 지정한다.
	//--- 사용하기 직전에 호출할 수 있다.
	return ShaderProgramID;
}

GLfloat Pyramid_Normal[] = {//--- 버텍스 속성: 좌표값(FragPos), 노말값 (Normal)
	// 정면부터 반시계
	0.5f,-0.5f,0.5f,1.0f,1.0f,1.0f,0.0f,0.0f,0.0f,1.0f,
	0.0f,0.5f,0.0f,1.0f,1.0f,1.0f,0.0f,0.0f,0.0f,1.0f,
	-0.5f,-0.5f,0.5f,1.0f,1.0f,1.0f,0.0f,0.0f,0.0f,1.0f,

	0.5f,-0.5f,0.5f,1.0f,1.0f,1.0f,0.0f,1.0f,0.0f,0.0f,
	0.5f,-0.5f,-0.5f,1.0f,1.0f,1.0f,0.0f,1.0f,0.0f,0.0f,
	0.0f,0.5f,0.0f,1.0f,1.0f,1.0f,0.0f,1.0f,0.0f,0.0f,

	0.0f,0.5f,0.0f,1.0f,1.0f,1.0f,0.0f,0.0f,0.0f,-1.0f,
	0.5f,-0.5f,-0.5f,1.0f,1.0f,1.0f,0.0f,0.0f,0.0f,-1.0f,
	-0.5f,-0.5f,-0.5f,1.0f,1.0f,1.0f,0.0f,0.0f,0.0f,-1.0f,

	0.0f,0.5f,0.0f,1.0f,1.0f,1.0f,0.0f,-1.0f,0.0f,0.0f,
	-0.5f,-0.5f,-0.5f,1.0f,1.0f,1.0f,0.0f,-1.0f,0.0f,0.0f,
	-0.5f,-0.5f,0.5f,1.0f,1.0f,1.0f,0.0f,-1.0f,0.0f,0.0f,

	// 밑변
	0.5f,-0.5f,0.5f,1.0f,1.0f,1.0f,0.0f,0.0f,-1.0f,0.0f,
	-0.5f,-0.5f,0.5f,1.0f,1.0f,1.0f,0.0f,0.0f,-1.0f,0.0f,
	-0.5f,-0.5f,-0.5f,1.0f,1.0f,1.0f,0.0f,0.0f,-1.0f,0.0f,

	-0.5f,-0.5f,-0.5f,1.0f,1.0f,1.0f,0.0f,0.0f,-1.0f,0.0f,
	0.5f,-0.5f,-0.5f,1.0f,1.0f,1.0f,0.0f,0.0f,-1.0f,0.0f,
	0.5f,-0.5f,0.5f,1.0f,1.0f,1.0f,0.0f,0.0f,-1.0f,0.0f,
};

float vertices[] = { //--- 버텍스 속성: 좌표값(FragPos), 노말값 (Normal)
-0.5f, -0.5f, -0.5f, 1.0f,1.0f,1.0f,0.0f,0.0f, 0.0f, -1.0f,
0.5f, -0.5f, -0.5f,1.0f,1.0f,1.0f,0.0f, 0.0f, 0.0f, -1.0f,
0.5f, 0.5f, -0.5f,1.0f,1.0f,1.0f,0.0f, 0.0f, 0.0f, -1.0f,
0.5f, 0.5f, -0.5f,1.0f,1.0f,1.0f,0.0f, 0.0f, 0.0f, -1.0f,
-0.5f, 0.5f, -0.5f,1.0f,1.0f,1.0f,0.0f, 0.0f, 0.0f, -1.0f,
-0.5f, -0.5f, -0.5f, 1.0f,1.0f,1.0f,0.0f,0.0f, 0.0f, -1.0f,
-0.5f, -0.5f, 0.5f,1.0f,1.0f,1.0f,0.0f, 0.0f, 0.0f, 1.0f,
0.5f, -0.5f, 0.5f,1.0f,1.0f,1.0f,0.0f, 0.0f, 0.0f, 1.0f,
0.5f, 0.5f, 0.5f,1.0f,1.0f,1.0f,0.0f, 0.0f, 0.0f, 1.0f,
0.5f, 0.5f, 0.5f,1.0f,1.0f,1.0f,0.0f, 0.0f, 0.0f, 1.0f,
-0.5f, 0.5f, 0.5f,1.0f,1.0f,1.0f,0.0f, 0.0f, 0.0f, 1.0f,
-0.5f, -0.5f, 0.5f, 1.0f,1.0f,1.0f,0.0f,0.0f, 0.0f, 1.0f,
-0.5f, 0.5f, 0.5f, 1.0f,1.0f,1.0f,0.0f,-1.0f, 0.0f, 0.0f,
-0.5f, 0.5f, -0.5f, 1.0f,1.0f,1.0f,0.0f,-1.0f, 0.0f, 0.0f,
-0.5f, -0.5f, -0.5f, 1.0f,1.0f,1.0f,0.0f,-1.0f, 0.0f, 0.0f,
-0.5f, -0.5f, -0.5f,1.0f,1.0f,1.0f,0.0f, -1.0f, 0.0f, 0.0f,
-0.5f, -0.5f, 0.5f,1.0f,1.0f,1.0f,0.0f, -1.0f, 0.0f, 0.0f,
-0.5f, 0.5f, 0.5f, 1.0f,1.0f,1.0f,0.0f,-1.0f, 0.0f, 0.0f,
0.5f, 0.5f, 0.5f,1.0f,1.0f,1.0f,0.0f, 1.0f, 0.0f, 0.0f,
0.5f, 0.5f, -0.5f, 1.0f,1.0f,1.0f,0.0f,1.0f, 0.0f, 0.0f,
0.5f, -0.5f, -0.5f,1.0f,1.0f,1.0f,0.0f, 1.0f, 0.0f, 0.0f,
0.5f, -0.5f, -0.5f, 1.0f,1.0f,1.0f,0.0f,1.0f, 0.0f, 0.0f,
0.5f, -0.5f, 0.5f, 1.0f,1.0f,1.0f,0.0f,1.0f, 0.0f, 0.0f,
0.5f, 0.5f, 0.5f,1.0f,1.0f,1.0f,0.0f, 1.0f, 0.0f, 0.0f,
-0.5f, -0.5f, -0.5f,1.0f,1.0f,1.0f,0.0f, 0.0f, -1.0f, 0.0f,
0.5f, -0.5f, -0.5f, 1.0f,1.0f,1.0f,0.0f,0.0f, -1.0f, 0.0f,
0.5f, -0.5f, 0.5f,1.0f,1.0f,1.0f,0.0f, 0.0f, -1.0f, 0.0f,
0.5f, -0.5f, 0.5f, 1.0f,1.0f,1.0f,0.0f,0.0f, -1.0f, 0.0f,
-0.5f, -0.5f, 0.5f, 1.0f,1.0f,1.0f,0.0f,0.0f, -1.0f, 0.0f,
-0.5f, -0.5f, -0.5f,1.0f,1.0f,1.0f,0.0f, 0.0f, -1.0f, 0.0f,
-0.5f, 0.5f, -0.5f,1.0f,1.0f,1.0f,0.0f, 0.0f, 1.0f, 0.0f,
0.5f, 0.5f, -0.5f,1.0f,1.0f,1.0f,0.0f, 0.0f, 1.0f, 0.0f,
0.5f, 0.5f, 0.5f,1.0f,1.0f,1.0f,0.0f, 0.0f, 1.0f, 0.0f,
0.5f, 0.5f, 0.5f, 1.0f,1.0f,1.0f,0.0f,0.0f, 1.0f, 0.0f,
-0.5f, 0.5f, 0.5f, 1.0f,1.0f,1.0f,0.0f,0.0f, 1.0f, 0.0f,
-0.5f, 0.5f, -0.5f, 1.0f,1.0f,1.0f,0.0f,0.0f, 1.0f, 0.0f
};

float invis_vertices[] = { //--- 버텍스 속성: 좌표값(FragPos), 노말값 (Normal)
	-0.5f, -0.5f, -0.5f,0.0f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f, -1.0f,
0.5f, -0.5f, -0.5f,0.0, 1.0, 0.0, 0.5, 0.0f, 0.0f, -1.0f,
0.5f, 0.5f, -0.5f,0.0, 1.0, 0.0, 0.5, 0.0f, 0.0f, -1.0f,
0.5f, 0.5f, -0.5f,0.0, 1.0, 0.0, 0.5, 0.0f, 0.0f, -1.0f,
-0.5f, 0.5f, -0.5f,0.0, 1.0, 0.0, 0.5, 0.0f, 0.0f, -1.0f,
-0.5f, -0.5f, -0.5f,0.0, 1.0, 0.0, 0.5, 0.0f, 0.0f, -1.0f,


-0.5f, -0.5f, 0.5f, 0.0, 1.0, 0.0, 0.5,0.0f, 0.0f, 1.0f,
0.5f, -0.5f, 0.5f,0.0, 1.0, 0.0, 0.5, 0.0f, 0.0f, 1.0f,
0.5f, 0.5f, 0.5f, 0.0, 1.0, 0.0, 0.5,0.0f, 0.0f, 1.0f,
0.5f, 0.5f, 0.5f,0.0, 1.0, 0.0, 0.5, 0.0f, 0.0f, 1.0f,
-0.5f, 0.5f, 0.5f,0.0, 1.0, 0.0, 0.5, 0.0f, 0.0f, 1.0f,
-0.5f, -0.5f, 0.5f, 0.0, 1.0, 0.0, 0.5,0.0f, 0.0f, 1.0f,


-0.5f, 0.5f, 0.5f,0.0, 1.0, 0.0, 0.5, -1.0f, 0.0f, 0.0f,
-0.5f, 0.5f, -0.5f, 0.0, 1.0, 0.0, 0.5,-1.0f, 0.0f, 0.0f,
-0.5f, -0.5f, -0.5f, 0.0, 1.0, 0.0, 0.5,-1.0f, 0.0f, 0.0f,
-0.5f, -0.5f, -0.5f, 0.0, 1.0, 0.0, 0.5,-1.0f, 0.0f, 0.0f,
-0.5f, -0.5f, 0.5f,0.0, 1.0, 0.0, 0.5, -1.0f, 0.0f, 0.0f,
-0.5f, 0.5f, 0.5f,0.0, 1.0, 0.0, 0.5, -1.0f, 0.0f, 0.0f,


0.5f, 0.5f, 0.5f,0.0, 1.0, 0.0, 0.5, 1.0f, 0.0f, 0.0f,
0.5f, 0.5f, -0.5f,0.0, 1.0, 0.0, 0.5, 1.0f, 0.0f, 0.0f,
0.5f, -0.5f, -0.5f,0.0, 1.0, 0.0, 0.5, 1.0f, 0.0f, 0.0f,
0.5f, -0.5f, -0.5f,0.0, 1.0, 0.0, 0.5, 1.0f, 0.0f, 0.0f,
0.5f, -0.5f, 0.5f,0.0, 1.0, 0.0, 0.5, 1.0f, 0.0f, 0.0f,
0.5f, 0.5f, 0.5f, 0.0, 1.0, 0.0, 0.5,1.0f, 0.0f, 0.0f,


-0.5f, -0.5f, -0.5f, 0.0, 1.0, 0.0, 0.5,0.0f, -1.0f, 0.0f,
0.5f, -0.5f, -0.5f, 0.0, 1.0, 0.0, 0.5,0.0f, -1.0f, 0.0f,
0.5f, -0.5f, 0.5f, 0.0, 1.0, 0.0, 0.5,0.0f, -1.0f, 0.0f,
0.5f, -0.5f, 0.5f, 0.0, 1.0, 0.0, 0.5,0.0f, -1.0f, 0.0f,
-0.5f, -0.5f, 0.5f, 0.0, 1.0, 0.0, 0.5,0.0f, -1.0f, 0.0f,
-0.5f, -0.5f, -0.5f, 0.0, 1.0, 0.0, 0.5,0.0f, -1.0f, 0.0f,


-0.5f, 0.5f, -0.5f, 0.0, 1.0, 0.0, 0.5,0.0f, 1.0f, 0.0f,
0.5f, 0.5f, -0.5f, 0.0, 1.0, 0.0, 0.5,0.0f, 1.0f, 0.0f,
0.5f, 0.5f, 0.5f, 0.0, 1.0, 0.0, 0.5,0.0f, 1.0f, 0.0f,
0.5f, 0.5f, 0.5f, 0.0, 1.0, 0.0, 0.5,0.0f, 1.0f, 0.0f,
-0.5f, 0.5f, 0.5f,0.0, 1.0, 0.0, 0.5, 0.0f, 1.0f, 0.0f,
-0.5f, 0.5f, -0.5f,0.0, 1.0, 0.0, 0.5, 0.0f, 1.0f, 0.0f,
};

GLuint shaderID;

GLfloat LPos[3] = { 0.0f,0.0f,3.0f };
GLfloat LColor[3] = { 1.0f, 1.0f, 1.0f };
GLfloat OColor[3] = { 1.0f, 1.0f, 1.0f };

//--- 변수 선언
GLuint platform, pyramid, VBO, snow[20], invis_box;

float* vPosData;
float* vNormalData;
float* vTextureCoordinateData;
int* indexData;
int vertexCount;
int indexCount;

void InitBuffer_snow()
{
	for (int i = 0; i < 20; i++)
	{
		glGenVertexArrays(1, &snow[i]); //--- VAO 를 지정하고 할당하기
		glBindVertexArray(snow[i]); //--- VAO를 바인드하기
		glGenBuffers(1, &VBO); //--- 2개의 VBO를 지정하고 할당하기

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0); //--- 위치 속성
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3 * sizeof(float))); //--- 색상 속성
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(7 * sizeof(float))); //--- 노말 속성
		glEnableVertexAttribArray(2);
	}
}

void InitBuffer_platform()
{
	glGenVertexArrays(1, &platform); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(platform); //--- VAO를 바인드하기
	glGenBuffers(1, &VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0); //--- 위치 속성
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3 * sizeof(float))); //--- 색상 속성
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(7 * sizeof(float))); //--- 노말 속성
	glEnableVertexAttribArray(2);
}

void InitBuffer_pyramid()
{
	glGenVertexArrays(1, &pyramid); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(pyramid); //--- VAO를 바인드하기
	glGenBuffers(1, &VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Pyramid_Normal), Pyramid_Normal, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0); //--- 위치 속성
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3 * sizeof(float))); //--- 색상 속성
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(7 * sizeof(float))); //--- 노말 속성
	glEnableVertexAttribArray(2);
}

void InitBuffer_invis_box()
{
	glGenVertexArrays(1, &invis_box); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(invis_box); //--- VAO를 바인드하기
	glGenBuffers(1, &VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(invis_vertices), invis_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0); //--- 위치 속성
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3 * sizeof(float))); //--- 색상 속성
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(7 * sizeof(float))); //--- 노말 속성
	glEnableVertexAttribArray(2);
}

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // 디스플레이 모드 설정
	glutInitWindowPosition(100, 100); // 윈도우의 위치 지정
	glutInitWindowSize(WINDOW_WITDH, WINDOW_HEIGHT); // 윈도우의 크기 지정
	glutCreateWindow("Example1"); // 윈도우 생성(윈도우 이름)
		//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew 초기화
	{
		cerr << "Unable to initialize GLEW" << endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";


	ReadObj(FILE_NAME, vPosData, vNormalData, vTextureCoordinateData, indexData, vertexCount, indexCount);
	shaderID = make_shaderProgram();

	srand(time(NULL));

	glutDisplayFunc(drawScene); // 출력 콜백함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 콜백함수 지정
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(10, Timer, 1);

	glutMainLoop(); // 이벤트 처리 시작
}

GLUquadricObj* qobj;

float timecount = 180.0f;

bool Wire = true;
bool Rdirection = false;
float Howfar = 0.0f;
float Hmove[2] = { 0.0f,0.0f };

float snow_pos[20][3];

float degree_cameraRotate = 0.0f;

float camera_pos[3] = { 0.0f,5.0f,20.0f };

bool snow_fall = false;

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	//--- 변경된 배경색 설정
	glClearColor(1.0f,1.0f,1.0f, 1.0f); // 바탕색을 변경
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	glUseProgram(shaderID);
	glEnable(GL_DEPTH_TEST);

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

	int modelLoc = glGetUniformLocation(shaderID, "Transform"); //--- 버텍스 세이더에서 모델링 변환 행렬 변수값을 받아온다.
	int viewLoc = glGetUniformLocation(shaderID, "viewTransform"); //--- 버텍스 세이더에서 뷰잉 변환 행렬 변수값을 받아온다.
	int projLoc = glGetUniformLocation(shaderID, "projectionTransform"); //--- 버텍스 세이더에서 투영 변환 행렬 변수값을 받아온다.
	//--- 모델링 변환
	glm::mat4 modelTransform = glm::mat4(1.0f);
	modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
	//--- 뷰잉 변환
	glm::vec4 cameraPos = glm::rotate(glm::mat4(1.0f), glm::radians(degree_cameraRotate), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(camera_pos[0], camera_pos[1], camera_pos[2], 1.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(cameraPos), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//--- 투영 변환 행렬을 설정한 후, 버텍스 세이더에 저장한다.
	glm::mat4 proj = glm::perspective(glm::radians(60.0f), WINDOW_WITDH / (float)WINDOW_HEIGHT, 0.001f, 1000.f);

	GLuint modelTransformLocation = glGetUniformLocation(shaderID, "Transform");
	glUniformMatrix4fv(modelTransformLocation, 1, GL_FALSE, glm::value_ptr(modelTransform));

	GLuint viewLocation = glGetUniformLocation(shaderID, "viewTransform");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

	GLuint projectLocation = glGetUniformLocation(shaderID, "projectionTransform");
	glUniformMatrix4fv(projectLocation, 1, GL_FALSE, glm::value_ptr(proj));

	glm::vec3 lightAmbient = LIGHT_AMBIENT;
	GLuint lightAmbientLocation = glGetUniformLocation(shaderID, "lightAmbient");
	glUniform3fv(lightAmbientLocation, 1, (float*)&lightAmbient);

	glm::vec3 lightPos = glm::vec3(LPos[0], LPos[1], LPos[2]);
	GLuint lightPosLocation = glGetUniformLocation(shaderID, "lightPos");
	glUniform3fv(lightPosLocation, 1, (float*)&lightPos);

	glm::vec3 lightColor = LIGHT_COLOR;
	GLuint lightColorLocation = glGetUniformLocation(shaderID, "lightColor");
	glUniform3fv(lightColorLocation, 1, (float*)&lightColor);

	glm::vec3 objColor = OBJECT_COLOR;
	GLuint objColorLocation = glGetUniformLocation(shaderID, "objectColor");
	glUniform3fv(objColorLocation, 1, (float*)&objColor);

	GLuint cameraPosLocation = glGetUniformLocation(shaderID, "cameraPos");
	glUniform3fv(cameraPosLocation, 1, (float*)&cameraPos);

	glm::mat4 LT = glm::mat4(1.0f);

	glUniform3f(objColorLocation, 1.0f, 1.0f, 1.0f);
	LT = glm::translate(LT, glm::vec3(LPos[0], LPos[1], LPos[2]));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(LT));
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GLU_FILL);
	gluQuadricNormals(qobj, GLU_SMOOTH);
	gluQuadricOrientation(qobj, GLU_OUTSIDE);
	gluSphere(qobj, 0.1, 50, 50);

	glm::mat4 PT = glm::mat4(1.0f);
	glm::mat4 PS = glm::mat4(1.0f);

	PT = glm::translate(PT, glm::vec3(0.0f, -2.0f, 0.0f));
	PS = glm::scale(PS, glm::vec3(20.0f, 0.1f, 20.0f));

	InitBuffer_platform();
	glUniform3f(objColorLocation, 1.0f, 0.5f, 0.3f);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(PT * PS));
	glBindVertexArray(platform);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glm::mat4 PyT = glm::mat4(1.0f);
	glm::mat4 PyS = glm::mat4(1.0f);

	PyT = glm::translate(PyT, glm::vec3(0.0f, -1.0f, 0.0f));
	PyS = glm::scale(PyS, glm::vec3(2.0f, 2.0f, 2.0f));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(PyT * PyS));
	InitBuffer_pyramid();
	glUniform3f(objColorLocation, 1.0f, 0.0f, 1.0f);
	glBindVertexArray(pyramid);
	glDrawArrays(GL_TRIANGLES, 0, 18);
	
	glm::mat4 ST = glm::mat4(1.0f);
	glm::mat4 SS = glm::mat4(1.0f);
	
	InitBuffer_snow();
	glUniform3f(objColorLocation, 1.0f, 1.0f, 1.0f);
	for (int i = 0; i < 20; i++)
	{
		ST = glm::translate(ST, glm::vec3(snow_pos[i][0], snow_pos[i][1], snow_pos[i][2]));
		SS = glm::scale(SS, glm::vec3(0.2f, 0.2f, 0.2f));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(ST));

		glBindVertexArray(snow[i]);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	glEnable(GL_BLEND);

	InitBuffer_invis_box();
	for (int i = 0; i < 8; i++)
	{
		int tmpx = i;
		int tmpy = 0.5f;
		int tmpz = 5.0f;
		if (i > 3)
		{
			tmpx = -tmpx;
			tmpy = -tmpy;
			tmpz = -tmpz;
		}
		glm::mat4 Imodel = glm::mat4(1.0f);
		glm::mat4 Imodel2 = glm::mat4(1.0f);
		Imodel = glm::translate(Imodel, glm::vec3(tmpx * 2.0f, tmpy, tmpz));
		Imodel2 = glm::scale(Imodel2, glm::vec3(1.0f,5.0f,1.0f));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Imodel * Imodel2));
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	
	glDisable(GL_BLEND);

	glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}

int Cnum = 2;
float rad = 0.0f;

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'y':
		degree_cameraRotate += 2.0f;
		break;

	case 'm':
		if (Wire == false)
			Wire = true;
		else
			Wire = false;
		break;
	case 'r':
		rad += 0.2f;
		LPos[0] = glm::cos(rad) * 10;
		LPos[1] = glm::sin(rad) * 10;
		break;
	case 's':
		snow_fall = !snow_fall;
		break;
	case 'z':
		camera_pos[2] -= 0.1f;
		break;
	case 'Z':
		camera_pos[2] -= 0.1f;
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
	case 'q':
		exit(EXIT_FAILURE);
		break;
	}

	glutPostRedisplay(); //--- 배경색이 바뀔때마다 출력 콜백함수를 호출하여 화면을 refresh 한다
}

void Timer(int value)
{
	if (snow_fall == true)
	{
		for (int i = 0; i < 20; i++)
		{
			if (snow_pos[i][1] == NULL || snow_pos[i][1] < -0.5f)
			{
				snow_pos[i][0] = ((0.1f) * (float)(rand() % 100) - 5.0f);
				snow_pos[i][1] = 10.0f;
				snow_pos[i][2] = ((0.1f) * (float)(rand() % 100) - 5.0f);
			}
			snow_pos[i][1] -= 0.03f;
		}
	}

	timecount++;

	glutPostRedisplay();
	glutTimerFunc(10, Timer, 1);
}
