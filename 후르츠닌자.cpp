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
#include <random>
#include <time.h>

#define WINDOW_WITDH	800
#define WINDOW_HEIGHT	600

using namespace std;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void Timer(int value);
GLvoid Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
GLchar* filetobuf(const GLchar* filename);

// ������ ã��
bool GetIntersectPoint(float Fx, float Fy, float Lx, float Ly, float P1x, float P1y, float P2x, float P2y, float* IPx, float* IPy)
{
	float t;
	float s;
	float under = (P2y - P1y) * (Lx - Fx) - (P2x - P1x) * (Ly - Fy);

	if (under == 0)	return false;

	float _t = (P2x - P1x) * (Fy - P1y) - (P2y - P1y) * (Fx - P1x);
	float _s = (Lx - Fx) * (Fy - P1y) - (Ly - Fy) * (Fx - P1x);

	t = _t / under;
	s = _s / under;

	if (t < 0.0 || t>1.0 || s < 0.0 || s>1.0) return false;
	if (_t == 0 && _s == 0) return false;

	*IPx = Fx + t * (float)(Lx - Fx);
	*IPy = Fy + t * (float)(Ly - Fy);

	return true;
}

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
	vertexsource = filetobuf("vertex_ninja.glsl");

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
	fragmentsource = filetobuf("fragment_ninja.glsl");

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

GLfloat DrawLine[10][12] =
{
	// x��
	{
		-1.0f,-0.5f,0.0f,1.0f,1.0f,1.0f,
		1.0f,-0.5f,0.0f,1.0f,1.0f,1.0f
	},
	{
		-1.0f,-0.7f,0.0f,1.0f,1.0f,1.0f,
		1.0f,-0.7f,0.0f,1.0f,1.0f,1.0f
	},
	{
		-1.0f,-0.3f,0.0f,1.0f,1.0f,1.0f,
		1.0f,-0.3f,0.0f,1.0f,1.0f,1.0f
	},
	// y��
	{
		-0.5f,-0.3f,0.0f,1.0f,1.0f,1.0f,
		-0.5f,-1.0f,0.0f,1.0f,1.0f,1.0f
	},
	{
		-0.75f,-0.3f,0.0f,1.0f,1.0f,1.0f,
		-0.75f,-1.0f,0.0f,1.0f,1.0f,1.0f
	},
	{
		-0.25f,-0.3f,0.0f,1.0f,1.0f,1.0f,
		-0.25f,-1.0f,0.0f,1.0f,1.0f,1.0f
	},
	{
		0.0f,-0.3f,0.0f,1.0f,1.0f,1.0f,
		0.0f,-1.0f,0.0f,1.0f,1.0f,1.0f
	},
	{
		0.25f,-0.3f,0.0f,1.0f,1.0f,1.0f,
		0.25f,-1.0f,0.0f,1.0f,1.0f,1.0f
	},
	{
		0.5f,-0.3f,0.0f,1.0f,1.0f,1.0f,
		0.5f,-1.0f,0.0f,1.0f,1.0f,1.0f
	},
	{
		0.75f,-0.3f,0.0f,1.0f,1.0f,1.0f,
		0.75f,-1.0f,0.0f,1.0f,1.0f,1.0f
	}
};

GLfloat Drag[12];

GLfloat random_lines[3][12];
GLfloat random_Polygon[3][36];

GLfloat Cut_Polygon[100][36];
GLfloat Index_Polygon[24][36];

GLuint shaderID;

//--- ���� ����
GLuint lines[10], VBO, R_lines[3] , R_polygon[3], Dragline, cut_polygon[100], Index_polygon[24];

int cut_count = 0;
int make_count = 0;
int time_count = 0;

int polygon_array[24];

void InitBuffer_lines()
{
	for (int i = 0; i < 10; i++)
	{
		glGenVertexArrays(1, &lines[i]);
		glBindVertexArray(lines[i]);

		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(DrawLine[i]), DrawLine[i], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //--- ��ġ �Ӽ�
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //--- ���� �Ӽ�
		glEnableVertexAttribArray(1);
	}
}
void InitBuffer_draglines()
{
	glGenVertexArrays(1, &Dragline);
	glBindVertexArray(Dragline);

	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Drag), Drag, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //--- ��ġ �Ӽ�
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //--- ���� �Ӽ�
	glEnableVertexAttribArray(1);
}

void InitBuffer_random()
{
	for (int i = 0; i < 3; i++)
	{
		glGenVertexArrays(1, &R_lines[i]);
		glBindVertexArray(R_lines[i]);

		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(random_lines[i]), random_lines[i], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //--- ��ġ �Ӽ�
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //--- ���� �Ӽ�
		glEnableVertexAttribArray(1);

		glGenVertexArrays(1, &R_polygon[i]);
		glBindVertexArray(R_polygon[i]);

		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(random_Polygon[i]), random_Polygon[i], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //--- ��ġ �Ӽ�
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //--- ���� �Ӽ�
		glEnableVertexAttribArray(1);
	}
}

void InitBuffer_cut()
{
	for (int i = 0; i < 100; i++)
	{
		glGenVertexArrays(1, &cut_polygon[i]);
		glBindVertexArray(cut_polygon[i]);

		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO); 
		glBufferData(GL_ARRAY_BUFFER, sizeof(Cut_Polygon[i]), Cut_Polygon[i], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //--- ��ġ �Ӽ�
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //--- ���� �Ӽ�
		glEnableVertexAttribArray(1);
	}
}

void InitBuffer_index()
{
	for (int i = 0; i < 24; i++)
	{
		glGenVertexArrays(1, &Index_polygon[i]);
		glBindVertexArray(Index_polygon[i]);

		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Index_Polygon[i]), Index_Polygon[i], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //--- ��ġ �Ӽ�
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //--- ���� �Ӽ�
		glEnableVertexAttribArray(1);
	}
}

float spot[3][4][2];
float dx[3] = { -3.0f,-3.0f,-3.0f };
float gab[3];

GLfloat cutdx[100];
GLfloat cutdy[100];
int cutmove[100];

void CutCut(float FirstMx, float FirstMy, float LastMx, float LastMy)
{
	float IPx[4], IPy[4];

	for (int i = 0; i < 100; i++)
	{
		bool cross[4] = { false ,false,false ,false };

		float P1x;
		float P1y;
		float P2x;
		float P2y;

		for (int k = 0; k < 4; k++)
		{
			P1x = Cut_Polygon[i][k * 6] + cutdx[i];
			P1y = Cut_Polygon[i][k * 6 + 1] + cutdy[i];
			P2x = Cut_Polygon[i][k * 6 + 6] + cutdx[i];
			P2y = Cut_Polygon[i][k * 6 + 7] + cutdy[i];

			cross[k] = GetIntersectPoint(FirstMx, FirstMy, LastMx, LastMy, P1x, P1y, P2x, P2y, &IPx[k], &IPy[k]);

			//if (cross[k] == true)
	
		}

		for (int p = 0; p < 6; p++)
		{
			Cut_Polygon[cut_count][p * 6 + 3] = 0.8f;
			Cut_Polygon[cut_count][p * 6 + 4] = 0.8f;
			Cut_Polygon[cut_count][p * 6 + 5] = 0.8f;
			Cut_Polygon[cut_count + 1][p * 6 + 3] = 0.8f;
			Cut_Polygon[cut_count + 1][p * 6 + 4] = 0.8f;
			Cut_Polygon[cut_count + 1][p * 6 + 5] = 0.8f;

			cutdx[cut_count] = (float)(rand() % 1000) * 0.0001f - 0.04;
			cutdx[cut_count + 1] = (float)(rand() % 1000) * 0.0001f - 0.04;
		}

		if (cross[0] == true && cross[1] == true)
		{
			Cut_Polygon[cut_count][0] = IPx[0];
			Cut_Polygon[cut_count][1] = IPy[0];
			Cut_Polygon[cut_count][6] = Cut_Polygon[i][6] + cutdx[i];
			Cut_Polygon[cut_count][7] = Cut_Polygon[i][6 + 1] + cutdy[i];
			Cut_Polygon[cut_count][12] = IPx[1];
			Cut_Polygon[cut_count][13] = IPy[1];
			Cut_Polygon[cut_count][18] = IPx[0];
			Cut_Polygon[cut_count][19] = IPy[0];
			Cut_Polygon[cut_count][24] = Cut_Polygon[i][6] + cutdx[i];
			Cut_Polygon[cut_count][25] = Cut_Polygon[i][6 + 1] + cutdy[i];
			Cut_Polygon[cut_count][30] = IPx[1];
			Cut_Polygon[cut_count][31] = IPy[1];

			Cut_Polygon[cut_count + 1][0] = IPx[0];
			Cut_Polygon[cut_count + 1][1] = IPy[0];
			Cut_Polygon[cut_count + 1][6] = IPx[1];
			Cut_Polygon[cut_count + 1][7] = IPy[1];
			Cut_Polygon[cut_count + 1][12] = Cut_Polygon[i][12] + cutdx[i];
			Cut_Polygon[cut_count + 1][13] = Cut_Polygon[i][12 + 1] + cutdy[i];
			Cut_Polygon[cut_count + 1][18] = Cut_Polygon[i][18] + cutdx[i];
			Cut_Polygon[cut_count + 1][19] = Cut_Polygon[i][18 + 1] + cutdy[i];
			Cut_Polygon[cut_count + 1][24] = IPx[0];
			Cut_Polygon[cut_count + 1][25] = IPy[0];
			Cut_Polygon[cut_count + 1][30] = Cut_Polygon[i][12] + cutdx[i];
			Cut_Polygon[cut_count + 1][31] = Cut_Polygon[i][12 + 1] + cutdy[i];

			cutdy[cut_count] = 0.0f;
			cutdy[cut_count + 1] = 0.0f;
			cutmove[cut_count] = 1;
			cutmove[cut_count + 1] = 1;

			cutdy[i] = -3.0f;
			cutmove[i] = 0;

			cut_count += 2;
			if (cut_count > 99)
				cut_count = 0;

			break;
		}
		else if (cross[1] == true && cross[2] == true)
		{
			Cut_Polygon[cut_count][0] = IPx[1];
			Cut_Polygon[cut_count][1] = IPy[1];
			Cut_Polygon[cut_count][6] = Cut_Polygon[i][12] + cutdx[i];
			Cut_Polygon[cut_count][7] = Cut_Polygon[i][12 + 1] + cutdy[i];
			Cut_Polygon[cut_count][12] = IPx[2];
			Cut_Polygon[cut_count][13] = IPy[2];
			Cut_Polygon[cut_count][18] = IPx[1];
			Cut_Polygon[cut_count][19] = IPy[1];
			Cut_Polygon[cut_count][24] = Cut_Polygon[i][12] + cutdx[i];
			Cut_Polygon[cut_count][25] = Cut_Polygon[i][12 + 1] + cutdy[i];
			Cut_Polygon[cut_count][30] = IPx[2];
			Cut_Polygon[cut_count][31] = IPy[2];

			Cut_Polygon[cut_count + 1][0] = Cut_Polygon[i][0] + cutdx[i];
			Cut_Polygon[cut_count + 1][1] = Cut_Polygon[i][0 + 1] + cutdy[i];
			Cut_Polygon[cut_count + 1][6] = Cut_Polygon[i][6] + cutdx[i];
			Cut_Polygon[cut_count + 1][7] = Cut_Polygon[i][6 + 1] + cutdy[i];
			Cut_Polygon[cut_count + 1][12] = IPx[1];
			Cut_Polygon[cut_count + 1][13] = IPy[1];
			Cut_Polygon[cut_count + 1][18] = IPx[2];
			Cut_Polygon[cut_count + 1][19] = IPy[2];
			Cut_Polygon[cut_count + 1][24] = Cut_Polygon[i][0] + cutdx[i];
			Cut_Polygon[cut_count + 1][25] = Cut_Polygon[i][0 + 1] + cutdy[i];
			Cut_Polygon[cut_count + 1][30] = IPx[1];
			Cut_Polygon[cut_count + 1][31] = IPy[1];

			cutdy[cut_count] = 0.0f;
			cutdy[cut_count + 1] = 0.0f;
			cutmove[cut_count] = 1;
			cutmove[cut_count + 1] = 1;

			cutdy[i] = -3.0f;
			cutmove[i] = 0;

			cut_count += 2;
			if (cut_count > 99)
				cut_count = 0;

			break;
		}
		else if (cross[2] == true && cross[3] == true)
		{
			Cut_Polygon[cut_count][0] = IPx[2];
			Cut_Polygon[cut_count][1] = IPy[2];
			Cut_Polygon[cut_count][6] = Cut_Polygon[i][18] + cutdx[i];
			Cut_Polygon[cut_count][7] = Cut_Polygon[i][18 + 1] + cutdy[i];
			Cut_Polygon[cut_count][12] = IPx[3];
			Cut_Polygon[cut_count][13] = IPy[3];
			Cut_Polygon[cut_count][18] = IPx[2];
			Cut_Polygon[cut_count][19] = IPy[2];
			Cut_Polygon[cut_count][24] = Cut_Polygon[i][18] + cutdx[i];
			Cut_Polygon[cut_count][25] = Cut_Polygon[i][18 + 1] + cutdy[i];
			Cut_Polygon[cut_count][30] = IPx[3];
			Cut_Polygon[cut_count][31] = IPy[3];

			Cut_Polygon[cut_count + 1][0] = Cut_Polygon[i][0] + cutdx[i];
			Cut_Polygon[cut_count + 1][1] = Cut_Polygon[i][0 + 1] + cutdy[i];
			Cut_Polygon[cut_count + 1][6] = Cut_Polygon[i][6] + cutdx[i];
			Cut_Polygon[cut_count + 1][7] = Cut_Polygon[i][6 + 1] + cutdy[i];
			Cut_Polygon[cut_count + 1][12] = IPx[2];
			Cut_Polygon[cut_count + 1][13] = IPy[2];
			Cut_Polygon[cut_count + 1][18] = IPx[3];
			Cut_Polygon[cut_count + 1][19] = IPy[3];
			Cut_Polygon[cut_count + 1][24] = Cut_Polygon[i][0] + cutdx[i];
			Cut_Polygon[cut_count + 1][25] = Cut_Polygon[i][0 + 1] + cutdy[i];
			Cut_Polygon[cut_count + 1][30] = IPx[2];
			Cut_Polygon[cut_count + 1][31] = IPy[2];

			cutdy[cut_count] = 0.0f;
			cutdy[cut_count + 1] = 0.0f;
			cutmove[cut_count] = 1;
			cutmove[cut_count + 1] = 1;

			cutdy[i] = -3.0f;
			cutmove[i] = 0;

			cut_count += 2;
			if (cut_count > 99)
				cut_count = 0;

			break;
		}
		else if (cross[0] == true && cross[3] == true)
		{
			Cut_Polygon[cut_count][0] = IPx[3];
			Cut_Polygon[cut_count][1] = IPy[3];
			Cut_Polygon[cut_count][6] = Cut_Polygon[i][0] + cutdx[i];
			Cut_Polygon[cut_count][7] = Cut_Polygon[i][0 + 1] + cutdy[i];
			Cut_Polygon[cut_count][12] = IPx[0];
			Cut_Polygon[cut_count][13] = IPy[0];
			Cut_Polygon[cut_count][18] = IPx[3];
			Cut_Polygon[cut_count][19] = IPy[3];
			Cut_Polygon[cut_count][24] = Cut_Polygon[i][0] + cutdx[i];
			Cut_Polygon[cut_count][25] = Cut_Polygon[i][0 + 1] + cutdy[i];
			Cut_Polygon[cut_count][30] = IPx[0];
			Cut_Polygon[cut_count][31] = IPy[0];

			Cut_Polygon[cut_count + 1][0] = IPx[0];
			Cut_Polygon[cut_count + 1][1] = IPy[0];
			Cut_Polygon[cut_count + 1][6] = Cut_Polygon[i][6] + cutdx[i];
			Cut_Polygon[cut_count + 1][7] = Cut_Polygon[i][6 + 1] + cutdy[i];
			Cut_Polygon[cut_count + 1][12] = Cut_Polygon[i][12] + cutdx[i];
			Cut_Polygon[cut_count + 1][13] = Cut_Polygon[i][12 + 1] + cutdy[i];
			Cut_Polygon[cut_count + 1][18] = IPx[3];
			Cut_Polygon[cut_count + 1][19] = IPy[3];
			Cut_Polygon[cut_count + 1][24] = IPx[0];
			Cut_Polygon[cut_count + 1][25] = IPy[0];
			Cut_Polygon[cut_count + 1][30] = Cut_Polygon[i][12] + cutdx[i];
			Cut_Polygon[cut_count + 1][31] = Cut_Polygon[i][12 + 1] + cutdy[i];

			cutdy[cut_count] = 0.0f;
			cutdy[cut_count + 1] = 0.0f;
			cutmove[cut_count] = 1;
			cutmove[cut_count + 1] = 1;

			cutdy[i] = -3.0f;
			cutmove[i] = 0;

			cut_count += 2;
			if (cut_count > 99)
				cut_count = 0;

			break;
		}
		else if (cross[0] == true && cross[2] == true)
		{
			Cut_Polygon[cut_count][0] = Cut_Polygon[i][0] + cutdx[i];
			Cut_Polygon[cut_count][1] = Cut_Polygon[i][0 + 1] + cutdy[i];
			Cut_Polygon[cut_count][6] = IPx[0];
			Cut_Polygon[cut_count][7] = IPy[0];
			Cut_Polygon[cut_count][12] = IPx[2];
			Cut_Polygon[cut_count][13] = IPy[2];
			Cut_Polygon[cut_count][18] = Cut_Polygon[i][18] + cutdx[i];
			Cut_Polygon[cut_count][19] = Cut_Polygon[i][18 + 1] + cutdy[i];
			Cut_Polygon[cut_count][24] = Cut_Polygon[i][0] + cutdx[i];
			Cut_Polygon[cut_count][25] = Cut_Polygon[i][0 + 1] + cutdy[i];
			Cut_Polygon[cut_count][30] = IPx[2];
			Cut_Polygon[cut_count][31] = IPy[2];

			Cut_Polygon[cut_count + 1][0] = IPx[0];
			Cut_Polygon[cut_count + 1][1] = IPy[0];
			Cut_Polygon[cut_count + 1][6] = Cut_Polygon[i][6] + cutdx[i];
			Cut_Polygon[cut_count + 1][7] = Cut_Polygon[i][6 + 1] + cutdy[i];
			Cut_Polygon[cut_count + 1][12] = Cut_Polygon[i][12] + cutdx[i];
			Cut_Polygon[cut_count + 1][13] = Cut_Polygon[i][12 + 1] + cutdy[i];
			Cut_Polygon[cut_count + 1][18] = IPx[2];
			Cut_Polygon[cut_count + 1][19] = IPy[2];
			Cut_Polygon[cut_count + 1][24] = IPx[0];
			Cut_Polygon[cut_count + 1][25] = IPy[0];
			Cut_Polygon[cut_count + 1][30] = Cut_Polygon[i][12] + cutdx[i];
			Cut_Polygon[cut_count + 1][31] = Cut_Polygon[i][12 + 1] + cutdy[i];

			cutdy[cut_count] = 0.0f;
			cutdy[cut_count + 1] = 0.0f;
			cutmove[cut_count] = 1;
			cutmove[cut_count + 1] = 1;

			cutdy[i] = -3.0f;
			cutmove[i] = 0;

			cut_count += 2;
			if (cut_count > 99)
				cut_count = 0;

			break;
		}
		else if (cross[1] == true && cross[3] == true)
		{
			Cut_Polygon[cut_count][0] = Cut_Polygon[i][0] + cutdx[i];
			Cut_Polygon[cut_count][1] = Cut_Polygon[i][0 + 1] + cutdy[i];
			Cut_Polygon[cut_count][6] = Cut_Polygon[i][6] + cutdx[i];
			Cut_Polygon[cut_count][7] = Cut_Polygon[i][6 + 1] + cutdy[i];
			Cut_Polygon[cut_count][12] = IPx[1];
			Cut_Polygon[cut_count][13] = IPy[1];
			Cut_Polygon[cut_count][18] = IPx[3];
			Cut_Polygon[cut_count][19] = IPy[3];
			Cut_Polygon[cut_count][24] = Cut_Polygon[i][0] + cutdx[i];
			Cut_Polygon[cut_count][25] = Cut_Polygon[i][0 + 1] + cutdy[i];
			Cut_Polygon[cut_count][30] = IPx[1];
			Cut_Polygon[cut_count][31] = IPy[1];

			Cut_Polygon[cut_count + 1][0] = IPx[3];
			Cut_Polygon[cut_count + 1][1] = IPy[3];
			Cut_Polygon[cut_count + 1][6] = IPx[1];
			Cut_Polygon[cut_count + 1][7] = IPy[1];
			Cut_Polygon[cut_count + 1][12] = Cut_Polygon[i][12] + cutdx[i];
			Cut_Polygon[cut_count + 1][13] = Cut_Polygon[i][12 + 1] + cutdy[i];
			Cut_Polygon[cut_count + 1][18] = Cut_Polygon[i][18] + cutdx[i];
			Cut_Polygon[cut_count + 1][19] = Cut_Polygon[i][18 + 1] + cutdy[i];
			Cut_Polygon[cut_count + 1][24] = IPx[3];
			Cut_Polygon[cut_count + 1][25] = IPy[3];
			Cut_Polygon[cut_count + 1][30] = Cut_Polygon[i][12] + cutdx[i];
			Cut_Polygon[cut_count + 1][31] = Cut_Polygon[i][12 + 1] + cutdy[i];

			cutdy[cut_count] = 0.0f;
			cutdy[cut_count + 1] = 0.0f;
			cutmove[cut_count] = 1;
			cutmove[cut_count + 1] = 1;

			cutdy[i] = -3.0f;
			cutmove[i] = 0;

			cut_count += 2;
			if (cut_count > 99)
				cut_count = 0;

			break;
		}
	}

	InitBuffer_cut();
}

void Cut(float FirstMx, float FirstMy, float LastMx, float LastMy)
{
	float IPx[4], IPy[4];
	
	for (int i = 0; i < make_count; i++)
	{
		bool cross[4] = { false ,false,false ,false };

		float P1x;
		float P1y;
		float P2x;
		float P2y;

		for (int k = 0; k < 4; k++)
		{
			if (k == 3)
			{
				P1x = spot[i][k][0] + dx[i] + 1.0f;
				P1y = spot[i][k][1] + gab[i] * dx[i] + random_lines[i][1];
				P2x = spot[i][0][0] + dx[i] + 1.0f;
				P2y = spot[i][0][1] + gab[i] * dx[i] + random_lines[i][1];
			}
			else
			{
				P1x = spot[i][k][0] + dx[i] + 1.0f;
				P1y = spot[i][k][1] + gab[i] * dx[i] + random_lines[i][1];
				P2x = spot[i][k + 1][0] + dx[i] + 1.0f;
				P2y = spot[i][k + 1][1] + gab[i] * dx[i] + random_lines[i][1];

				//cout << P1x << ", " << P1y << ", " << P2x << ", " << P2y << endl << endl;
			}
			
			cross[k] = GetIntersectPoint(FirstMx, FirstMy, LastMx, LastMy, P1x, P1y, P2x, P2y, &IPx[k], &IPy[k]); 

			//if (cross[k] == true)
				
		}

		for (int p = 0; p < 6; p++)
		{
			Cut_Polygon[cut_count][p * 6 + 3] = 0.8f;
			Cut_Polygon[cut_count][p * 6 + 4] = 0.8f;
			Cut_Polygon[cut_count][p * 6 + 5] = 0.8f;
			Cut_Polygon[cut_count + 1][p * 6 + 3] = 0.8f;
			Cut_Polygon[cut_count + 1][p * 6 + 4] = 0.8f;
			Cut_Polygon[cut_count + 1][p * 6 + 5] = 0.8f;

			cutdx[cut_count] = (float)(rand() % 1000) * 0.0001f - 0.04;
			cutdx[cut_count + 1] = (float)(rand() % 1000) * 0.0001f - 0.04;
		} 

		if (cross[0] == true && cross[1] == true)
		{
			Cut_Polygon[cut_count][0] = IPx[0];
			Cut_Polygon[cut_count][1] = IPy[0];
			Cut_Polygon[cut_count][6] = spot[i][1][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count][7] = spot[i][1][1] + gab[i] * dx[i] + random_lines[i][1];
			Cut_Polygon[cut_count][12] = IPx[1];
			Cut_Polygon[cut_count][13] = IPy[1];
			Cut_Polygon[cut_count][18] = IPx[0];
			Cut_Polygon[cut_count][19] = IPy[0];
			Cut_Polygon[cut_count][24] = spot[i][1][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count][25] = spot[i][1][1] + gab[i] * dx[i] + random_lines[i][1];
			Cut_Polygon[cut_count][30] = IPx[1];
			Cut_Polygon[cut_count][31] = IPy[1];
			
			Cut_Polygon[cut_count + 1][0] = IPx[0];
			Cut_Polygon[cut_count + 1][1] = IPy[0];
			Cut_Polygon[cut_count + 1][6] = IPx[1];
			Cut_Polygon[cut_count + 1][7] = IPy[1];
			Cut_Polygon[cut_count + 1][12] = spot[i][2][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count + 1][13] = spot[i][2][1] + gab[i] * dx[i] + random_lines[i][1];
			Cut_Polygon[cut_count + 1][18] = spot[i][3][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count + 1][19] = spot[i][3][1] + gab[i] * dx[i] + random_lines[i][1];
			Cut_Polygon[cut_count + 1][24] = IPx[0];
			Cut_Polygon[cut_count + 1][25] = IPy[0];
			Cut_Polygon[cut_count + 1][30] = spot[i][2][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count + 1][31] = spot[i][2][1] + gab[i] * dx[i] + random_lines[i][1];

			dx[i] = -3.0f;

			cutdy[cut_count] = 0.0f;
			cutdy[cut_count + 1] = 0.0f;
			cutmove[cut_count] = 1;
			cutmove[cut_count + 1] = 1;

			cut_count += 2;
			if (cut_count > 99)
				cut_count = 0;

			break;
		}
		else if (cross[1] == true && cross[2] == true)
		{
			Cut_Polygon[cut_count][0] = IPx[1];
			Cut_Polygon[cut_count][1] = IPy[1];
			Cut_Polygon[cut_count][6] = spot[i][2][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count][7] = spot[i][2][1] + gab[i] * dx[i] + random_lines[i][1];
			Cut_Polygon[cut_count][12] = IPx[2];
			Cut_Polygon[cut_count][13] = IPy[2];
			Cut_Polygon[cut_count][18] = IPx[1];
			Cut_Polygon[cut_count][19] = IPy[1];
			Cut_Polygon[cut_count][24] = spot[i][2][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count][25] = spot[i][2][1] + gab[i] * dx[i] + random_lines[i][1];
			Cut_Polygon[cut_count][30] = IPx[2];
			Cut_Polygon[cut_count][31] = IPy[2];

			Cut_Polygon[cut_count + 1][0] = spot[i][0][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count + 1][1] = spot[i][0][1] + gab[i] * dx[i] + random_lines[i][1];
			Cut_Polygon[cut_count + 1][6] = spot[i][1][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count + 1][7] = spot[i][1][1] + gab[i] * dx[i] + random_lines[i][1];
			Cut_Polygon[cut_count + 1][12] = IPx[1];
			Cut_Polygon[cut_count + 1][13] = IPy[1];
			Cut_Polygon[cut_count + 1][18] = IPx[2];
			Cut_Polygon[cut_count + 1][19] = IPy[2];
			Cut_Polygon[cut_count + 1][24] = spot[i][0][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count + 1][25] = spot[i][0][1] + gab[i] * dx[i] + random_lines[i][1];
			Cut_Polygon[cut_count + 1][30] = IPx[1];
			Cut_Polygon[cut_count + 1][31] = IPy[1];

			dx[i] = -3.0f;

			cutdy[cut_count] = 0.0f;
			cutdy[cut_count + 1] = 0.0f;
			cutmove[cut_count] = 1;
			cutmove[cut_count + 1] = 1;

			cut_count += 2;
			if (cut_count > 99)
				cut_count = 0;

			break;
		}
		else if (cross[2] == true && cross[3] == true)
		{

			Cut_Polygon[cut_count][0] = IPx[2];
			Cut_Polygon[cut_count][1] = IPy[2];
			Cut_Polygon[cut_count][6] = spot[i][3][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count][7] = spot[i][3][1] + gab[i] * dx[i] + random_lines[i][1];
			Cut_Polygon[cut_count][12] = IPx[3];
			Cut_Polygon[cut_count][13] = IPy[3];
			Cut_Polygon[cut_count][18] = IPx[2];
			Cut_Polygon[cut_count][19] = IPy[2];
			Cut_Polygon[cut_count][24] = spot[i][3][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count][25] = spot[i][3][1] + gab[i] * dx[i] + random_lines[i][1];
			Cut_Polygon[cut_count][30] = IPx[3];
			Cut_Polygon[cut_count][31] = IPy[3];

			Cut_Polygon[cut_count + 1][0] = spot[i][0][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count + 1][1] = spot[i][0][1] + gab[i] * dx[i] + random_lines[i][1];
			Cut_Polygon[cut_count + 1][6] = spot[i][1][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count + 1][7] = spot[i][1][1] + gab[i] * dx[i] + random_lines[i][1];
			Cut_Polygon[cut_count + 1][12] = IPx[2];
			Cut_Polygon[cut_count + 1][13] = IPy[2];
			Cut_Polygon[cut_count + 1][18] = IPx[3];
			Cut_Polygon[cut_count + 1][19] = IPy[3];
			Cut_Polygon[cut_count + 1][24] = spot[i][0][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count + 1][25] = spot[i][0][1] + gab[i] * dx[i] + random_lines[i][1];
			Cut_Polygon[cut_count + 1][30] = IPx[2];
			Cut_Polygon[cut_count + 1][31] = IPy[2];

			dx[i] = -3.0f;

			cutdy[cut_count] = 0.0f;
			cutdy[cut_count + 1] = 0.0f;
			cutmove[cut_count] = 1;
			cutmove[cut_count + 1] = 1;

			cut_count += 2;
			if (cut_count > 99)
				cut_count = 0;

			break;
		}
		else if (cross[0] == true && cross[3] == true)
		{
			Cut_Polygon[cut_count][0] = IPx[3];
			Cut_Polygon[cut_count][1] = IPy[3];
			Cut_Polygon[cut_count][6] = spot[i][0][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count][7] = spot[i][0][1] + gab[i] * dx[i] + random_lines[i][1];
			Cut_Polygon[cut_count][12] = IPx[0];
			Cut_Polygon[cut_count][13] = IPy[0];
			Cut_Polygon[cut_count][18] = IPx[3];
			Cut_Polygon[cut_count][19] = IPy[3];
			Cut_Polygon[cut_count][24] = spot[i][0][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count][25] = spot[i][0][1] + gab[i] * dx[i] + random_lines[i][1];
			Cut_Polygon[cut_count][30] = IPx[0];
			Cut_Polygon[cut_count][31] = IPy[0];

			Cut_Polygon[cut_count + 1][0] = IPx[0];
			Cut_Polygon[cut_count + 1][1] = IPy[0];
			Cut_Polygon[cut_count + 1][6] = spot[i][1][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count + 1][7] = spot[i][1][1] + gab[i] * dx[i] + random_lines[i][1];
			Cut_Polygon[cut_count + 1][12] = spot[i][2][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count + 1][13] = spot[i][2][1] + gab[i] * dx[i] + random_lines[i][1];
			Cut_Polygon[cut_count + 1][18] = IPx[3];
			Cut_Polygon[cut_count + 1][19] = IPy[3];
			Cut_Polygon[cut_count + 1][24] = IPx[0];
			Cut_Polygon[cut_count + 1][25] = IPy[0];
			Cut_Polygon[cut_count + 1][30] = spot[i][2][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count + 1][31] = spot[i][2][1] + gab[i] * dx[i] + random_lines[i][1];

			dx[i] = -3.0f;

			cutdy[cut_count] = 0.0f;
			cutdy[cut_count + 1] = 0.0f;
			cutmove[cut_count] = 1;
			cutmove[cut_count + 1] = 1;

			cut_count += 2;
			if (cut_count > 99)
				cut_count = 0;

			break;
		}
		else if (cross[0] == true && cross[2] == true)
		{
			Cut_Polygon[cut_count][0] = spot[i][0][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count][1] = spot[i][0][1] + gab[i] * dx[i] + random_lines[i][1];
			Cut_Polygon[cut_count][6] = IPx[0];
			Cut_Polygon[cut_count][7] = IPy[0];
			Cut_Polygon[cut_count][12] = IPx[2];
			Cut_Polygon[cut_count][13] = IPy[2];
			Cut_Polygon[cut_count][18] = spot[i][3][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count][19] = spot[i][3][1] + gab[i] * dx[i] + random_lines[i][1];
			Cut_Polygon[cut_count][24] = spot[i][0][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count][25] = spot[i][0][1] + gab[i] * dx[i] + random_lines[i][1];
			Cut_Polygon[cut_count][30] = IPx[2];
			Cut_Polygon[cut_count][31] = IPy[2];

			Cut_Polygon[cut_count + 1][0] = IPx[0];
			Cut_Polygon[cut_count + 1][1] = IPy[0];
			Cut_Polygon[cut_count + 1][6] = spot[i][1][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count + 1][7] = spot[i][1][1] + gab[i] * dx[i] + random_lines[i][1];
			Cut_Polygon[cut_count + 1][12] = spot[i][2][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count + 1][13] = spot[i][2][1] + gab[i] * dx[i] + random_lines[i][1];
			Cut_Polygon[cut_count + 1][18] = IPx[2];
			Cut_Polygon[cut_count + 1][19] = IPy[2];
			Cut_Polygon[cut_count + 1][24] = IPx[0];
			Cut_Polygon[cut_count + 1][25] = IPy[0];
			Cut_Polygon[cut_count + 1][30] = spot[i][2][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count + 1][31] = spot[i][2][1] + gab[i] * dx[i] + random_lines[i][1];

			dx[i] = -3.0f;

			cutdy[cut_count] = 0.0f;
			cutdy[cut_count + 1] = 0.0f;
			cutmove[cut_count] = 1;
			cutmove[cut_count + 1] = 1;

			cut_count += 2;
			if (cut_count > 99)
				cut_count = 0;

			break;
		}
		else if (cross[1] == true && cross[3] == true)
		{
			Cut_Polygon[cut_count][0] = spot[i][0][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count][1] = spot[i][0][1] + gab[i] * dx[i] + random_lines[i][1];
			Cut_Polygon[cut_count][6] = spot[i][1][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count][7] = spot[i][1][1] + gab[i] * dx[i] + random_lines[i][1];
			Cut_Polygon[cut_count][12] = IPx[1];
			Cut_Polygon[cut_count][13] = IPy[1];
			Cut_Polygon[cut_count][18] = IPx[3];
			Cut_Polygon[cut_count][19] = IPy[3];
			Cut_Polygon[cut_count][24] = spot[i][0][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count][25] = spot[i][0][1] + gab[i] * dx[i] + random_lines[i][1];
			Cut_Polygon[cut_count][30] = IPx[1];
			Cut_Polygon[cut_count][31] = IPy[1];

			Cut_Polygon[cut_count + 1][0] = IPx[3];
			Cut_Polygon[cut_count + 1][1] = IPy[3];
			Cut_Polygon[cut_count + 1][6] = IPx[1];
			Cut_Polygon[cut_count + 1][7] = IPy[1];
			Cut_Polygon[cut_count + 1][12] = spot[i][2][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count + 1][13] = spot[i][2][1] + gab[i] * dx[i] + random_lines[i][1];
			Cut_Polygon[cut_count + 1][18] = spot[i][3][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count + 1][19] = spot[i][3][1] + gab[i] * dx[i] + random_lines[i][1];
			Cut_Polygon[cut_count + 1][24] = IPx[3];
			Cut_Polygon[cut_count + 1][25] = IPy[3];
			Cut_Polygon[cut_count + 1][30] = spot[i][2][0] + dx[i] + 1.0f;
			Cut_Polygon[cut_count + 1][31] = spot[i][2][1] + gab[i] * dx[i] + random_lines[i][1];

			dx[i] = -3.0f;

			cutdy[cut_count] = 0.0f;
			cutdy[cut_count + 1] = 0.0f;
			cutmove[cut_count] = 1;
			cutmove[cut_count + 1] = 1;

			cut_count += 2;
			if (cut_count > 99)
				cut_count = 0;

			break;
		}
	}

	InitBuffer_cut();
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
	InitBuffer_lines();

	glutDisplayFunc(drawScene); // ��� �ݹ��Լ��� ����
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �ݹ��Լ� ����
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutTimerFunc(10, Timer, 1);

	glutMainLoop(); // �̺�Ʈ ó�� ����
}

int time_speed = 10;
bool draw_lines = true;
bool draw_polygon = true;

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	for (int k = 0; k < 3; k++)
	{
		for (int i = 0; i < 6; i++) // ���� �簢�� ����
		{
			random_Polygon[k][i * 6 + 3] = 0.8f;
			random_Polygon[k][i * 6 + 4] = 0.8f;
			random_Polygon[k][i * 6 + 5] = 0.8f;
		}
	}
	for (int i = 0; i < 2; i++)
	{
		Drag[i * 6 + 3] = 0.8f;
		Drag[i * 6 + 4] = 0.8f;
		Drag[i * 6 + 5] = 0.8f;
	}

	//--- ����� ���� ����
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // �������� ����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�

	glUseProgram(shaderID);

	glm::mat4 modelTransform = glm::mat4(1.0f);
	modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
	GLuint modelTransformLocation = glGetUniformLocation(shaderID, "Transform");
	glUniformMatrix4fv(modelTransformLocation, 1, GL_FALSE, glm::value_ptr(modelTransform));
	
	for (int i = 0; i < 10; i++)
	{
		glBindVertexArray(lines[i]);
		glDrawArrays(GL_LINES, 0, 2);
	}

	glm::mat4 R_PolyT[3] = { glm::mat4(1.0f),glm::mat4(1.0f),glm::mat4(1.0f) };

	for (int i = 0; i < make_count; i++)
	{
		R_PolyT[i] = glm::translate(glm::mat4(1.0f), glm::vec3(dx[i], gab[i] * dx[i], 0.0f));
	}
	
	for (int i = 0; i < make_count; i++)
	{
		if (draw_polygon == true)
		{
			glUniformMatrix4fv(modelTransformLocation, 1, GL_FALSE, glm::value_ptr(R_PolyT[i]));
			glBindVertexArray(R_polygon[i]);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		else
		{
			glUniformMatrix4fv(modelTransformLocation, 1, GL_FALSE, glm::value_ptr(R_PolyT[i]));
			glBindVertexArray(R_polygon[i]);
			glDrawArrays(GL_LINE_LOOP, 0, 36);
		}
		if (draw_lines == true)
		{
			glUniformMatrix4fv(modelTransformLocation, 1, GL_FALSE, glm::value_ptr(modelTransform));
			glBindVertexArray(R_lines[i]);
			glDrawArrays(GL_LINE_LOOP, 0, 2);
		}
	}

	glBindVertexArray(Dragline);
	glDrawArrays(GL_LINES, 0, 2);

	glm::mat4 cut_PolyT[100];

	for (int i = 0; i < 100; i++)
	{
		cut_PolyT[i] = glm::translate(glm::mat4(1.0f), glm::vec3(cutdx[i], cutdy[i], 0.0f));
	}

	for (int i = 0; i < 100; i++)
	{
		if (draw_polygon == true)
		{
			glUniformMatrix4fv(modelTransformLocation, 1, GL_FALSE, glm::value_ptr(cut_PolyT[i]));
			glBindVertexArray(cut_polygon[i]);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		else
		{
			glUniformMatrix4fv(modelTransformLocation, 1, GL_FALSE, glm::value_ptr(cut_PolyT[i]));
			glBindVertexArray(cut_polygon[i]);
			glDrawArrays(GL_LINE_LOOP, 0, 36);
		}
	}

	glUniformMatrix4fv(modelTransformLocation, 1, GL_FALSE, glm::value_ptr(modelTransform));
	for (int i = 0; i < 24; i++)
	{
		if (draw_polygon == true)
		{
			glBindVertexArray(Index_polygon[i]);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		else
		{
			glBindVertexArray(Index_polygon[i]);
			glDrawArrays(GL_LINE_LOOP, 0, 36);
		}
	}

	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
	glViewport(0, 0, w, h);
}

float FirstMx;
float FirstMy;
float LastMx;
float LastMy;

GLvoid Mouse(int button, int state, int x, int y)
{
	float Mx = (((float)x - WINDOW_WITDH / 2) / (WINDOW_WITDH / 2));
	float My = -(((float)y - WINDOW_HEIGHT / 2) / (WINDOW_HEIGHT / 2));

	// ���� Ŭ��
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		FirstMx = Mx;
		FirstMy = My;

		Drag[0] = Mx;
		Drag[1] = My;
		Drag[6] = Mx;
		Drag[7] = My;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		LastMx = Mx;
		LastMy = My;

		Drag[0] = -2.0f;
		Drag[1] = -2.0f;
		Drag[6] = -2.0f;
		Drag[7] = -2.0f;

		Cut(FirstMx, FirstMy, LastMx, LastMy);
		CutCut(FirstMx, FirstMy, LastMx, LastMy);
		InitBuffer_draglines();
	}
	glutPostRedisplay();
}

void Motion(int x, int y)
{
	float Mx = (((float)x - WINDOW_WITDH / 2) / (WINDOW_WITDH / 2));
	float My = -(((float)y - WINDOW_HEIGHT / 2) / (WINDOW_HEIGHT / 2));

	Drag[6] = Mx;
	Drag[7] = My;

	InitBuffer_draglines();
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {

	case '+':
		if (time_speed > 1)
			time_speed--;
		break;

	case '-':
		time_speed++;
		break;
	case 'n':
		draw_polygon = !draw_polygon;
		break;
	case 'm':
		draw_lines = !draw_lines;
		break;

	case 'q':
		exit(EXIT_FAILURE);
		break;
	}

	glutPostRedisplay(); //--- ������ �ٲ𶧸��� ��� �ݹ��Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
}

int index_array[24];

void Timer(int value)
{
	srand(time(NULL));

	// �簢�� ����
	if (time_count > 100)
	{
		if(make_count < 3)
			make_count++;
		time_count = 0;
	}
	// �簢�� ����
	for (int i = 0; i < make_count; i++)
	{
		if (dx[i] < -2.0f)
		{
			dx[i] = 0.0f;
			spot[i][0][0] = (float)(rand() % 10) * -0.02f;
			spot[i][0][1] = (float)(rand() % 10) * 0.02f;
			spot[i][1][0] = (float)(rand() % 10) * -0.02f;
			spot[i][1][1] = (float)(rand() % 10) * -0.02f;
			spot[i][2][0] = (float)(rand() % 10) * 0.02f;
			spot[i][2][1] = (float)(rand() % 10) * -0.02f;
			spot[i][3][0] = (float)(rand() % 10) * 0.02f;
			spot[i][3][1] = (float)(rand() % 10) * 0.02f;

			for (int k = 0; k < 2; k++) // �� �߱�
			{
				random_lines[i][k * 6 + 3] = 1.0f;
				random_lines[i][k * 6 + 4] = 1.0f;
				random_lines[i][k * 6 + 5] = 1.0f;
				random_lines[i][k * 6 + 1] = (float)(rand() % 100) * 0.01f - 0.1f;
			}
			random_lines[i][0] = 1.0f;
			random_lines[i][6] = -1.0f;

			for (int k = 0; k < 3; k++) // ���� �簢�� ����
			{
				random_Polygon[i][k * 6] = 1.0f + spot[i][k][0];
				random_Polygon[i][k * 6 + 1] = random_lines[i][1] + spot[i][k][1];
			}
			for (int k = 3; k < 6; k++) // ���� �簢�� ����
			{
				random_Polygon[i][k * 6] = 1.0f + spot[i][k - 1][0];
				random_Polygon[i][k * 6 + 1] = random_lines[i][1] + spot[i][k - 1][1];
				if (k == 5)
				{
					random_Polygon[i][k * 6] = 1.0f + spot[i][0][0];
					random_Polygon[i][k * 6 + 1] = random_lines[i][1] + spot[i][0][1];
				}
			}
		}
		InitBuffer_random();
	}
	time_count += 1;

	// �ٵ��� �̵� 
	for (int i = 0; i < make_count; i++)
	{
		float dy = random_lines[i][1] - random_lines[i][7];
		gab[i] = dy / 2.0f;
		dx[i] -= 0.01f;
	}

	// �߸� �ٰ��� �̵�
	for (int i = 0; i < 100; i++)
	{
		if(cutdx[i] < 0.1 || cutdx[i] > -0.1)
			cutdx[i] = cutdx[i] * 1.01;

		cutdy[i] -= 0.005f * cutmove[i];
	}

	// �߸� �ٰ��� ����
	for (int i = 0; i < 100; i++)
	{
		int tmpx = 0;

		for (int k = 0; k < 8; k++)
		{
			if ((Cut_Polygon[i][6] + cutdx[i]) > (0.25 * (float)k - 1.0f) && (Cut_Polygon[i][6] + cutdx[i]) < (0.25 * (float)k - 0.75f))
			{
				tmpx = k;
				break;
			}
		}

		if ((Cut_Polygon[i][7] + cutdy[i]) < -0.3f && (Cut_Polygon[i][7] + cutdy[i]) > -0.5f)
		{
			if (index_array[tmpx + 16] == 1 && index_array[tmpx + 8] == 1 && index_array[tmpx] != 1)
			{
				index_array[tmpx] = 1;

				for (int k = 0; k < 6; k++)
				{
					Index_Polygon[tmpx][k * 6] = Cut_Polygon[i][k * 6] - Cut_Polygon[i][6] + (0.25 * (float)tmpx -1.0f) + 0.05f;;
					Index_Polygon[tmpx][k * 6 + 1] = Cut_Polygon[i][k * 6 + 1] - Cut_Polygon[i][7] - 0.5f + 0.05f;;

					Index_Polygon[tmpx][k * 6 + 3] = 0.8f;
					Index_Polygon[tmpx][k * 6 + 4] = 0.8f;
					Index_Polygon[tmpx][k * 6 + 5] = 0.8f;
				}

				cutdx[i] = -3.0f;
				cutdy[i] = -3.0f;
				InitBuffer_index();
			}
		}
		if ((Cut_Polygon[i][7] + cutdy[i]) < -0.5f && (Cut_Polygon[i][7] + cutdy[i]) > -0.7f)
		{
			if (index_array[tmpx + 16] == 1 && index_array[tmpx + 8] != 1)
			{
				index_array[tmpx + 8] = 1;

				for (int k = 0; k < 6; k++)
				{
					Index_Polygon[tmpx + 8][k * 6] = Cut_Polygon[i][k * 6] - Cut_Polygon[i][6] + (0.25 * (float)tmpx - 1.0f) + 0.05f;;
					Index_Polygon[tmpx + 8][k * 6 + 1] = Cut_Polygon[i][k * 6 + 1] - Cut_Polygon[i][7] - 0.7f + 0.05f;;

					Index_Polygon[tmpx + 8][k * 6 + 3] = 0.8f;
					Index_Polygon[tmpx + 8][k * 6 + 4] = 0.8f;
					Index_Polygon[tmpx + 8][k * 6 + 5] = 0.8f;
				}

				cutdx[i] = -3.0f;
				cutdy[i] = -3.0f;
				InitBuffer_index();
			}
		}
		if ((Cut_Polygon[i][7] + cutdy[i]) < -0.7f && (Cut_Polygon[i][7] + cutdy[i]) > -1.0f)
		{
			if (index_array[tmpx + 16] != 1)
			{
				index_array[tmpx + 16] = 1;

				for (int k = 0; k < 6; k++)
				{
					Index_Polygon[tmpx + 16][k * 6] = Cut_Polygon[i][k * 6] - Cut_Polygon[i][6] + (0.25 * (float)tmpx - 1.0f) + 0.05f;
					Index_Polygon[tmpx + 16][k * 6 + 1] = Cut_Polygon[i][k * 6 + 1] - Cut_Polygon[i][7] - 1.0f + 0.05f;

					Index_Polygon[tmpx + 16][k * 6 + 3] = 0.8f;
					Index_Polygon[tmpx + 16][k * 6 + 4] = 0.8f;
					Index_Polygon[tmpx + 16][k * 6 + 5] = 0.8f;
				}

				cutdx[i] = -3.0f;
				cutdy[i] = -3.0f;
				InitBuffer_index();
			}
		}
	}
	
	int tmpcount = 0;

	for (int i = 0; i < 24; i++)
	{
		if (index_array[i] == true)
		{
			tmpcount++;
		}
	}
	if (tmpcount >= 24)
	{
		exit(EXIT_FAILURE);
	}

	glutPostRedisplay();
	glutTimerFunc(time_speed, Timer, 1);
}
