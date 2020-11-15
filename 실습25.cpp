#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <gl/glew.h> // 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <fstream>

#define WINDOW_WITDH	800
#define WINDOW_HEIGHT	600

#define CAMERA_POS		glm::vec3(0.0, 1.0, 10.0)
#define OBJECT_COLOR	glm::vec3(1.0, 1.0, 1.0)
#define LIGHT_AMBIENT	glm::vec3(0.1, 0.1, 0.1)
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
	vertexsource = filetobuf("vertex_texture.glsl");

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
	fragmentsource = filetobuf("fragment_texture.glsl");

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
GLuint make_shaderProgram2()
{
	GLint result;
	GLchar errorLog[512];

	GLchar* vertexsource;
	vertexsource = filetobuf("vertex2.glsl");

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
	fragmentsource = filetobuf("fragment2.glsl");

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

GLfloat DrawLine[3][2][3] =
{
	// x축
	{
		{-1.0f,0.0f,0.0f},
		{1.0f,0.0f,0.0f}
	},
	// y축
	{
		{0.0f,1.0f,0.0f},
		{0.0f,-1.0f,0.0f}
	},
	// z축
	{
		{0.0f,0.0f,1.0f},
		{0.0f,0.0f,-1.0f}
	}
};

GLfloat Pyramid_Normal[6][24] = {//--- 버텍스 속성: 좌표값(FragPos), 노말값 (Normal)
	// 정면부터 반시계
	{0.5f,-0.5f,0.5f,0.0f,0.0f,1.0f,1.0f,1.0f,
	0.0f,0.5f,0.0f,0.0f,0.0f,1.0f,0.5f,0.0f,
	-0.5f,-0.5f,0.5f,0.0f,0.0f,1.0f,0.0f,1.0f,
	},

{0.5f,-0.5f,0.5f,1.0f,0.0f,0.0f,0.0f,1.0f,
	0.5f,-0.5f,-0.5f,1.0f,0.0f,0.0f,1.0f,1.0f,
	0.0f,0.5f,0.0f,1.0f,0.0f,0.0f,0.5f,0.0f,
	},

{0.0f,0.5f,0.0f,0.0f,0.0f,-1.0f,0.5f,0.0f,
	0.5f,-0.5f,-0.5f,0.0f,0.0f,-1.0f,0.0f,1.0f,
	-0.5f,-0.5f,-0.5f,0.0f,0.0f,-1.0f,1.0f,1.0f,
	},

{0.0f,0.5f,0.0f,-1.0f,0.0f,0.0f,0.5f,0.0f,
	-0.5f,-0.5f,-0.5f,-1.0f,0.0f,0.0f,0.0f,1.0f,
	-0.5f,-0.5f,0.5f,-1.0f,0.0f,0.0f,1.0f,1.0f,
	},

	// 밑변
{0.5f,-0.5f,0.5f,0.0f,-1.0f,0.0f,1.0f,1.0f,
	-0.5f,-0.5f,0.5f,0.0f,-1.0f,0.0f,0.0f,1.0f,
	-0.5f,-0.5f,-0.5f,0.0f,-1.0f,0.0f,0.0f,0.0f,
	},

{-0.5f,-0.5f,-0.5f,0.0f,-1.0f,0.0f,0.0f,0.0f,
	0.5f,-0.5f,-0.5f,0.0f,-1.0f,0.0f,0.0f,1.0f,
	0.5f,-0.5f,0.5f,0.0f,-1.0f,0.0f,1.0f,1.0f
	}
};

float vertices[6][48] = { //--- 버텍스 속성: 좌표값(FragPos), 노말값 (Normal)
	{-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,1.0f,1.0f,
0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,0.0f,1.0f,
0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,0.0f,0.0f,
0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,0.0f,0.0f,
-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,1.0f,0.0f,
-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,1.0f,1.0f,
},

{-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,0.0f,0.0f,
0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,0.0f,1.0f,
0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,1.0f,0.0f,
0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,1.0f,0.0f,
-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,0.0f,0.0f,
-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,0.0f,1.0f,
},

{-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,1.0f,0.0f,
-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,0.0f,0.0f,
-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,0.0f,1.0f,
-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,0.0f,1.0f,
-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,1.0f,1.0f,
-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,1.0f,0.0f,
},

{0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,0.0f,0.0f,
0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,1.0f,0.0f,
0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,1.0f,1.0f,
0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,1.0f,1.0f,
0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,0.0f,1.0f,
0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,0.0f,0.0f,
},

{-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,0.0f,0.0f,
0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,1.0f,0.0f,
0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,1.0f,1.0f,
0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,1.0f,1.0f,
-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,0.0f,1.0f,
-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,0.0f,0.0f,
},

{-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,0.0f,0.0f,
0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,1.0f,0.0f,
0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,1.0f,1.0f,
0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,1.0f,1.0f,
-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,0.0f,1.0f,
-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,0.0f,0.0f}
};

float bgvertice[] = {
	-1.0f, -1.0f, -1.0f, 1.0f,1.0f,1.0f,0.0f,1.0f,
1.0f, -1.0f, -1.0f, 1.0f,1.0f,1.0f,1.0f,1.0f,
1.0f, 1.0f, -1.0f, 1.0f,1.0f,1.0f,1.0f,0.0f,
1.0f, 1.0f, -1.0f, 1.0f,1.0f,1.0f,1.0f,0.0f,
-1.0f, 1.0f, -1.0f, 1.0f,1.0f,1.0f,0.0f,0.0f,
-1.0f, -1.0f, -1.0f, 1.0f,1.0f,1.0f,0.0f,1.0f
};

GLuint shaderID;
GLuint shaderID2;

//--- 변수 선언
GLuint cube[6], pyramid[6], lines[3], VBO, bg;

float* vPosData;
float* vNormalData;
float* vTextureCoordinateData;
int* indexData;
int vertexCount;
int indexCount;

void InitBuffer_bg()
{
	glGenVertexArrays(1, &bg);
	glBindVertexArray(bg);

	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bgvertice), bgvertice, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); //--- 위치 속성
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); //--- 텍스처 속성
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); //--- 텍스처 속성
	glEnableVertexAttribArray(2);
}

void InitBuffer_lines()
{
	for (int i = 0; i < 3; i++)
	{
		glGenVertexArrays(1, &lines[i]);
		glBindVertexArray(lines[i]);

		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(DrawLine[i]), DrawLine[i], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //--- 위치 속성
		glEnableVertexAttribArray(0);
	}
}

void InitBuffer_cube()
{
	for (int i = 0; i < 6; i++)
	{
		glGenVertexArrays(1, &cube[i]);
		glBindVertexArray(cube[i]);

		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[i]), vertices[i], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); //--- 위치 속성
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); //--- 노말 속성
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); //--- 텍스처 속성
		glEnableVertexAttribArray(2);
	}
}

void InitBuffer_pyramid()
{
	for (int i = 0; i < 6; i++)
	{
		glGenVertexArrays(1, &pyramid[i]);
		glBindVertexArray(pyramid[i]);

		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Pyramid_Normal[i]), Pyramid_Normal[i], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); //--- 위치 속성
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); //--- 노말 속성
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); //--- 텍스처 속성
		glEnableVertexAttribArray(2);
	}
}

unsigned int texture[6];
unsigned int bgtexture;

void InitTexture()
{
	int width, height, nrChannels;

	glGenTextures(1, &texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- 현재 바인딩된 텍스처의 파라미터 설정하기
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// 텍스처 로드 및 생성
	unsigned char* data1 = stbi_load("A.bmp", &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1); //---텍스처 이미지 정의

	glGenTextures(1, &texture[1]);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- 현재 바인딩된 텍스처의 파라미터 설정하기
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// 텍스처 로드 및 생성
	unsigned char* data2 = stbi_load("B.bmp", &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2); //---텍스처 이미지 정의

	glGenTextures(1, &texture[2]);
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- 현재 바인딩된 텍스처의 파라미터 설정하기
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// 텍스처 로드 및 생성
	unsigned char* data3 = stbi_load("C.bmp", &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data3); //---텍스처 이미지 정의

	glGenTextures(1, &texture[3]);
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- 현재 바인딩된 텍스처의 파라미터 설정하기
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// 텍스처 로드 및 생성
	unsigned char* data4 = stbi_load("D.bmp", &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data4); //---텍스처 이미지 정의

	glGenTextures(1, &texture[4]);
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- 현재 바인딩된 텍스처의 파라미터 설정하기
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// 텍스처 로드 및 생성
	unsigned char* data5 = stbi_load("E.bmp", &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data5); //---텍스처 이미지 정의

	glGenTextures(1, &texture[5]);
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- 현재 바인딩된 텍스처의 파라미터 설정하기
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// 텍스처 로드 및 생성
	unsigned char* data6 = stbi_load("F.bmp", &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data6); //---텍스처 이미지 정의

	glGenTextures(1, &bgtexture);
	glBindTexture(GL_TEXTURE_2D, bgtexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- 현재 바인딩된 텍스처의 파라미터 설정하기
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// 텍스처 로드 및 생성
	unsigned char* bgdata = stbi_load("bg.bmp", &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, bgdata); //---텍스처 이미지 정의
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

	shaderID = make_shaderProgram();
	shaderID2 = make_shaderProgram2();
	InitBuffer_lines();
	InitBuffer_cube();
	InitBuffer_pyramid();
	InitTexture();
	InitBuffer_bg();

	glutDisplayFunc(drawScene); // 출력 콜백함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 콜백함수 지정
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(10, Timer, 1);

	glutMainLoop(); // 이벤트 처리 시작
}

float degree_cameraRotate = 0.0f;

float degree_lightRotate = 0.0f;

float camera_pos[3] = { 2.0f,3.0f,3.0f };

bool Bpyramid = false;

bool BXrotate = false;
bool BYrotate = false;

float timecountX = 0.0f;
float timecountY = 0.0f;

float LPos[3] = { 0.0f,0.0f,3.0f };

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	//--- 변경된 배경색 설정
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 바탕색을 변경
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	glEnable(GL_DEPTH_TEST);
	glUseProgram(shaderID2);

	glBindVertexArray(bg);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bgtexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glUseProgram(shaderID);
	
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

	GLuint projectLocation = glGetUniformLocation(shaderID, "projectionTransform");
	glUniformMatrix4fv(projectLocation, 1, GL_FALSE, glm::value_ptr(proj));

	GLuint viewLocation = glGetUniformLocation(shaderID, "viewTransform");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

	glm::vec3 lightAmbient = LIGHT_AMBIENT;
	GLuint lightAmbientLocation = glGetUniformLocation(shaderID, "lightAmbient");
	glUniform3fv(lightAmbientLocation, 1, (float*)&lightAmbient);

	glm::vec3 lightPos = glm::vec3(LIGHT_POS);
	GLuint lightPosLocation = glGetUniformLocation(shaderID, "lightPos");
	glUniform3f(lightPosLocation, LPos[0], LPos[1], LPos[2]);

	glm::vec3 lightColor = LIGHT_COLOR;
	GLuint lightColorLocation = glGetUniformLocation(shaderID, "lightColor");
	glUniform3fv(lightColorLocation, 1, (float*)&lightColor);

	glm::vec3 objColor = OBJECT_COLOR;
	GLuint objColorLocation = glGetUniformLocation(shaderID, "objectColor");
	glUniform3fv(objColorLocation, 1, (float*)&objColor);

	GLuint cameraPosLocation = glGetUniformLocation(shaderID, "cameraPos");
	glUniform3fv(cameraPosLocation, 1, (float*)&cameraPos);

	glm::mat4 XRotate = glm::rotate(glm::mat4(1.0f), glm::radians(timecountX), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 YRotate = glm::rotate(glm::mat4(1.0f), glm::radians(timecountY), glm::vec3(0.0f, 1.0f, 0.0f));

	glUniformMatrix4fv(modelTransformLocation, 1, GL_FALSE, glm::value_ptr(XRotate * YRotate));
	for (int i = 0; i < 3; i++)
	{
		glBindVertexArray(lines[i]);
		glDrawArrays(GL_LINES, 0, 2);
	}

	glUniformMatrix4fv(modelTransformLocation, 1, GL_FALSE, glm::value_ptr(XRotate * YRotate));

	if (Bpyramid == true)
	{
		for (int i = 0; i < 6; i++)
		{
			glBindVertexArray(cube[i]);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture[i]);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			glBindVertexArray(pyramid[i]);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture[i]);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		glBindVertexArray(pyramid[4]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture[4]);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(pyramid[5]);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	glDisable(GL_DEPTH_TEST);
	
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
	case 'W':
		camera_pos[1] += 0.2f;
		break;
	case 'E':
		camera_pos[1] -= 0.2f;
		break;
	case 'a':
		degree_cameraRotate += 5.0f;
		break;
	case'd':
		degree_cameraRotate -= 5.0f;
		break;
	case 'w':
		degree_lightRotate += 0.2f;
		LPos[0] = glm::cos(degree_lightRotate) * 5;
		LPos[2] = glm::sin(degree_lightRotate) * 5;
		break;
	case 'e':
		degree_lightRotate -= 0.2f;
		LPos[0] = glm::cos(degree_lightRotate) * 5;
		LPos[2] = glm::sin(degree_lightRotate) * 5;
		break;
	case 'x':
		BXrotate = !BXrotate;
		break;
	case 'y':
		BYrotate = !BYrotate;
		break;
	case 's':
		BXrotate = false;
		BYrotate = false;
		degree_cameraRotate = 0.0f;

		camera_pos[0] = 2.0f;
		camera_pos[1] = 3.0f;
		camera_pos[2] = 3.0f;

		Bpyramid = false;

		LPos[0] = 0.0f;
		LPos[1] = 0.0f;
		LPos[2] = 3.0f;
		break;
	case 'c':
		Bpyramid = !Bpyramid;
		break;
	case 'q':
		exit(EXIT_FAILURE);
		break;
	}

	glutPostRedisplay(); //--- 배경색이 바뀔때마다 출력 콜백함수를 호출하여 화면을 refresh 한다
}

void Timer(int value)
{
	if (BXrotate == true)
		timecountX += 2.0f;
	if (BYrotate == true)
		timecountY += 2.0f;

	glutPostRedisplay();
	glutTimerFunc(10, Timer, 1);
}
