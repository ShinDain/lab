#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <gl/glew.h> // �ʿ��� ������� include
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
	vertexsource = filetobuf("vertex_texture.glsl");

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
	fragmentsource = filetobuf("fragment_texture.glsl");

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
GLuint make_shaderProgram2()
{
	GLint result;
	GLchar errorLog[512];

	GLchar* vertexsource;
	vertexsource = filetobuf("vertex2.glsl");

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
	fragmentsource = filetobuf("fragment2.glsl");

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
	// x��
	{
		{-1.0f,0.0f,0.0f},
		{1.0f,0.0f,0.0f}
	},
	// y��
	{
		{0.0f,1.0f,0.0f},
		{0.0f,-1.0f,0.0f}
	},
	// z��
	{
		{0.0f,0.0f,1.0f},
		{0.0f,0.0f,-1.0f}
	}
};

GLfloat Pyramid_Normal[6][24] = {//--- ���ؽ� �Ӽ�: ��ǥ��(FragPos), �븻�� (Normal)
	// ������� �ݽð�
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

	// �غ�
{0.5f,-0.5f,0.5f,0.0f,-1.0f,0.0f,1.0f,1.0f,
	-0.5f,-0.5f,0.5f,0.0f,-1.0f,0.0f,0.0f,1.0f,
	-0.5f,-0.5f,-0.5f,0.0f,-1.0f,0.0f,0.0f,0.0f,
	},

{-0.5f,-0.5f,-0.5f,0.0f,-1.0f,0.0f,0.0f,0.0f,
	0.5f,-0.5f,-0.5f,0.0f,-1.0f,0.0f,0.0f,1.0f,
	0.5f,-0.5f,0.5f,0.0f,-1.0f,0.0f,1.0f,1.0f
	}
};

float vertices[6][48] = { //--- ���ؽ� �Ӽ�: ��ǥ��(FragPos), �븻�� (Normal)
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

//--- ���� ����
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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); //--- ��ġ �Ӽ�
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); //--- �ؽ�ó �Ӽ�
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); //--- �ؽ�ó �Ӽ�
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

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //--- ��ġ �Ӽ�
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

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); //--- ��ġ �Ӽ�
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); //--- �븻 �Ӽ�
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); //--- �ؽ�ó �Ӽ�
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

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); //--- ��ġ �Ӽ�
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); //--- �븻 �Ӽ�
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); //--- �ؽ�ó �Ӽ�
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- ���� ���ε��� �ؽ�ó�� �Ķ���� �����ϱ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// �ؽ�ó �ε� �� ����
	unsigned char* data1 = stbi_load("A.bmp", &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1); //---�ؽ�ó �̹��� ����

	glGenTextures(1, &texture[1]);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- ���� ���ε��� �ؽ�ó�� �Ķ���� �����ϱ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// �ؽ�ó �ε� �� ����
	unsigned char* data2 = stbi_load("B.bmp", &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2); //---�ؽ�ó �̹��� ����

	glGenTextures(1, &texture[2]);
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- ���� ���ε��� �ؽ�ó�� �Ķ���� �����ϱ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// �ؽ�ó �ε� �� ����
	unsigned char* data3 = stbi_load("C.bmp", &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data3); //---�ؽ�ó �̹��� ����

	glGenTextures(1, &texture[3]);
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- ���� ���ε��� �ؽ�ó�� �Ķ���� �����ϱ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// �ؽ�ó �ε� �� ����
	unsigned char* data4 = stbi_load("D.bmp", &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data4); //---�ؽ�ó �̹��� ����

	glGenTextures(1, &texture[4]);
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- ���� ���ε��� �ؽ�ó�� �Ķ���� �����ϱ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// �ؽ�ó �ε� �� ����
	unsigned char* data5 = stbi_load("E.bmp", &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data5); //---�ؽ�ó �̹��� ����

	glGenTextures(1, &texture[5]);
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- ���� ���ε��� �ؽ�ó�� �Ķ���� �����ϱ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// �ؽ�ó �ε� �� ����
	unsigned char* data6 = stbi_load("F.bmp", &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data6); //---�ؽ�ó �̹��� ����

	glGenTextures(1, &bgtexture);
	glBindTexture(GL_TEXTURE_2D, bgtexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- ���� ���ε��� �ؽ�ó�� �Ķ���� �����ϱ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// �ؽ�ó �ε� �� ����
	unsigned char* bgdata = stbi_load("bg.bmp", &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, bgdata); //---�ؽ�ó �̹��� ����
}

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	//--- ������ �����ϱ�
	glutInit(&argc, argv); // glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // ���÷��� ��� ����
	glutInitWindowPosition(100, 100); // �������� ��ġ ����
	glutInitWindowSize(WINDOW_WITDH, WINDOW_HEIGHT); // �������� ũ�� ����
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
	shaderID2 = make_shaderProgram2();
	InitBuffer_lines();
	InitBuffer_cube();
	InitBuffer_pyramid();
	InitTexture();
	InitBuffer_bg();

	glutDisplayFunc(drawScene); // ��� �ݹ��Լ��� ����
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �ݹ��Լ� ����
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(10, Timer, 1);

	glutMainLoop(); // �̺�Ʈ ó�� ����
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

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	//--- ����� ���� ����
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // �������� ����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	glEnable(GL_DEPTH_TEST);
	glUseProgram(shaderID2);

	glBindVertexArray(bg);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bgtexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glUseProgram(shaderID);
	
	int modelLoc = glGetUniformLocation(shaderID, "Transform"); //--- ���ؽ� ���̴����� �𵨸� ��ȯ ��� �������� �޾ƿ´�.
	int viewLoc = glGetUniformLocation(shaderID, "viewTransform"); //--- ���ؽ� ���̴����� ���� ��ȯ ��� �������� �޾ƿ´�.
	int projLoc = glGetUniformLocation(shaderID, "projectionTransform"); //--- ���ؽ� ���̴����� ���� ��ȯ ��� �������� �޾ƿ´�.
	//--- �𵨸� ��ȯ
	glm::mat4 modelTransform = glm::mat4(1.0f);
	modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
	//--- ���� ��ȯ
	glm::vec4 cameraPos = glm::rotate(glm::mat4(1.0f), glm::radians(degree_cameraRotate), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(camera_pos[0], camera_pos[1], camera_pos[2], 1.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(cameraPos), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//--- ���� ��ȯ ����� ������ ��, ���ؽ� ���̴��� �����Ѵ�.
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
	
	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
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

	glutPostRedisplay(); //--- ������ �ٲ𶧸��� ��� �ݹ��Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
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
