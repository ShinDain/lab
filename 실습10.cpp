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

GLfloat triShape[12][3][3] = {
	{
		{-0.8f,0.5f,0.0f},
		{-0.2f,0.5f,0.0f},
		{-0.5f,0.5f,0.0f}
	},
	{
		{-0.8f,0.5f,0.0f},
		{-0.2f,0.5f,0.0f},
		{-0.5f,0.5f,0.0f}
	},
	{
		{-0.8f,0.5f,0.0f},
		{-0.2f,0.5f,0.0f},
		{-0.5f,0.5f,0.0f}
	},

	{
		{0.2f,0.2f,0.0f},
		{0.8f,0.2f,0.0f},
		{0.5f,0.8f,0.0f}
	},
	{
		{0.2f,0.2f,0.0f},
		{0.8f,0.2f,0.0f},
		{0.5f,0.8f,0.0f}
	},
	{
		{0.2f,0.2f,0.0f},
		{0.8f,0.2f,0.0f},
		{0.5f,0.8f,0.0f}
	},

	{
		{-0.8f,-0.8f,0.0f },
		{-0.2f,-0.2f,0.0f},
		{-0.8f,-0.2f,0.0f}
	},
	{
		{-0.8f,-0.8f,0.0f },
		{-0.2f,-0.8f,0.0f},
		{-0.2f,-0.2f,0.0f}
	},
	{
		{-0.8f,-0.8f,0.0f },
		{-0.2f,-0.8f,0.0f},
		{-0.2f,-0.2f,0.0f}
	},

	{
		{0.4f,-0.8f,0.0f },
		{0.6f,-0.2f,0.0f},
		{0.2f,-0.4f,0.0f}
	},
	{
		{0.4f,-0.8f,0.0f },
		{0.8f,-0.8f,0.0f},
		{0.6f,-0.2f,0.0f}
	},
	{
		{0.6f,-0.2f,0.0f },
		{0.8f,-0.8f,0.0f},
		{1.0f,-0.4f,0.0f}
	}
};
GLfloat colors[12][3][3] = {
	{
		{0.8f,0.0f,0.0f},
		{0.8f,0.0f,0.0f},
		{0.8f,0.0f,0.0f}
	},
	{
		{0.8f,0.0f,0.0f},
		{0.8f,0.0f,0.0f},
		{0.8f,0.0f,0.0f}
	},
	{
		{0.8f,0.0f,0.0f},
		{0.8f,0.0f,0.0f},
		{0.8f,0.0f,0.0f}
	},
	{
		{0.0f,0.8f,0.1f},
		{0.0f,0.8f,0.1f},
		{0.0f,0.8f,0.1f}
	},				
	{				
		{0.0f,0.8f,0.1f},
		{0.0f,0.8f,0.1f},
		{0.0f,0.8f,0.1f}
	},			
	{			
		{0.0f,0.8f,0.1f},
		{0.0f,0.8f,0.1f},
		{0.0f,0.8f,0.1f}
	},
	{
		{0.8f,0.4f,0.0f},
		{0.8f,0.4f,0.0f},
		{0.8f,0.4f,0.0f}
	},			
	{			
		{0.8f,0.4f,0.0f},
		{0.8f,0.4f,0.0f},
		{0.8f,0.4f,0.0f}
	},			
	{			
		{0.8f,0.4f,0.0f},
		{0.8f,0.4f,0.0f},
		{0.8f,0.4f,0.0f}
	},
	{
		{0.8f,0.0f,0.6f},
		{0.8f,0.0f,0.6f},
		{0.8f,0.0f,0.6f}
	},				 
	{				 
		{0.8f,0.0f,0.6f},
		{0.8f,0.0f,0.6f},
		{0.8f,0.0f,0.6f}
	},				
	{				
		{0.8f,0.0f,0.6f},
		{0.8f,0.0f,0.6f},
		{0.8f,0.0f,0.6f}
	}
};

//--- ���� ����
GLuint VAO[12], VBO[2];

void InitBuffer()
{
	// �ٱ� �ﰢ��
	for (int i = 0; i < 12; i++)
	{
		glGenVertexArrays(1, &VAO[i]); //--- VAO �� �����ϰ� �Ҵ��ϱ�
		glBindVertexArray(VAO[i]); //--- VAO�� ���ε��ϱ�
		glGenBuffers(2, VBO); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), triShape[i], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colors[i], GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
	}
}

GLuint shaderID;

GLint width = 800;
GLint height = 600;

int DrawLine[4] = {0,1,2,3};

bool animation = false;

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
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(10, Timer, 1);

	glutMainLoop(); // �̺�Ʈ ó�� ����
}

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	//--- ����� ���� ����
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // �������� ����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�

	glUseProgram(shaderID);

	for (int i = 0; i < 3; i++)
	{
		if (DrawLine[0] == 0)
		{
			glBindVertexArray(VAO[i]);
			glDrawArrays(GL_LINE_LOOP, 0, 3);
		}
		else
		{
			glBindVertexArray(VAO[i]);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
	}
	for (int i = 3; i < 6; i++)
	{
		if (DrawLine[1] == 0)
		{
			glBindVertexArray(VAO[i]);
			glDrawArrays(GL_LINE_LOOP, 0, 3);
		}
		else
		{
			glBindVertexArray(VAO[i]);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
	}
	for (int i = 6; i < 9; i++)
	{
		if (DrawLine[2] == 0)
		{
			glBindVertexArray(VAO[i]);
			glDrawArrays(GL_LINE_LOOP, 0, 3);
		}
		else
		{
			glBindVertexArray(VAO[i]);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
	}
	for (int i = 9; i < 12; i++)
	{
		if (DrawLine[3] == 0)
		{
			glBindVertexArray(VAO[i]);
			glDrawArrays(GL_LINE_LOOP, 0, 3);
		}
		else
		{
			glBindVertexArray(VAO[i]);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
	}
	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
	glViewport(0, 0, w, h);
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 's' :
		if (animation == false)
			animation = true;
		else
			animation = false;
		break;
	case 'q':
		exit(EXIT_FAILURE);
		break;
	}
	glutPostRedisplay(); //--- ������ �ٲ𶧸��� ��� �ݹ��Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
}

void Timer(int value)
{
	if (animation == true)
	{
		for (int i = 0; i < 4; i++)
		{
			if (DrawLine[i] == 0)
			{
				for (int k = 0; k < 3; k++)
				{
					triShape[(i * 3) + k][0][1] -= 0.002;
					triShape[(i * 3) + k][1][1] -= 0.002;
					triShape[(i * 3) + k][2][1] += 0.002;
				}		
				if (triShape[(i * 3)][1][1] <= 0.2f && i < 2)
					DrawLine[i] = 1;
				else if(triShape[(i * 3)][1][1] <= -0.8f && i >= 2)
					DrawLine[i] = 1;
			}
			else if (DrawLine[i] == 1)
			{
				triShape[(i * 3)][1][1] += 0.004;
				triShape[(i * 3)][2][0] -= 0.002;
				for (int k = 1; k < 3; k++)
				{
					triShape[(i * 3) + k][2][0] += 0.002;
				}
				if (triShape[(i * 3)][1][1] >= 0.8f && i < 2)
					DrawLine[i] = 2;
				else if(triShape[(i * 3)][1][1] >= -0.2f && i >= 2)
					DrawLine[i] = 2;
			}
			else if (DrawLine[i] == 2)
			{
				if(i == 2)
				{
					triShape[(i * 3)][1][0] -= 0.004;
					triShape[(i * 3)][2][0] -= 0.002;
					triShape[(i * 3)][2][1] -= 0.002;
					triShape[(i * 3) + 1][1][0] -= 0.002;
					triShape[(i * 3) + 1][2][0] -= 0.004;
					triShape[(i * 3) + 2][0][0] += 0.002;
					triShape[(i * 3) + 2][0][1] += 0.006;
					triShape[(i * 3) + 2][1][0] -= 0.002;
					triShape[(i * 3) + 2][2][1] -= 0.002;
				}
				else
				{
					triShape[(i * 3)][0][0] += 0.002;
					triShape[(i * 3)][1][0] -= 0.002;
					triShape[(i * 3)][2][1] -= 0.002;
					triShape[(i * 3) + 1][0][0] += 0.002;
					triShape[(i * 3) + 1][2][0] -= 0.002;
					triShape[(i * 3) + 2][0][0] += 0.004;
					triShape[(i * 3) + 2][0][1] += 0.006;
					triShape[(i * 3) + 2][2][0] += 0.002;
					triShape[(i * 3) + 2][2][1] -= 0.002;
				}
				
				if (triShape[(i * 3)][2][1] <= 0.6f && i < 2)
					DrawLine[i] = 3;
				else if (triShape[(i * 3)][2][1] <= -0.4f && i >= 2)
					DrawLine[i] = 3;
			}
			else if (DrawLine[i] == 3)
			{
				if (i == 2)
				{
					triShape[(i * 3)][0][0] += 0.003;
					triShape[(i * 3)][1][0] += 0.001;
					triShape[(i * 3)][2][0] += 0.005;
					triShape[(i * 3) + 1][0][0] += 0.003;
					triShape[(i * 3) + 1][1][0] -= 0.001;
					triShape[(i * 3) + 1][2][0] += 0.001;
					triShape[(i * 3) + 2][0][0] += 0.001;
					triShape[(i * 3) + 2][1][0] -= 0.001;
					triShape[(i * 3) + 2][2][0] -= 0.003;
				}
				else
				{
					triShape[(i * 3)][0][0] += 0.001;
					triShape[(i * 3)][1][0] -= 0.001;
					triShape[(i * 3)][2][0] += 0.003;
					triShape[(i * 3) + 1][0][0] += 0.001;
					triShape[(i * 3) + 1][1][0] -= 0.003;
					triShape[(i * 3) + 1][2][0] -= 0.001;
					triShape[(i * 3) + 2][0][0] -= 0.001;
					triShape[(i * 3) + 2][1][0] -= 0.003;
					triShape[(i * 3) + 2][2][0] -= 0.005;
				}
				triShape[(i * 3)][0][1] += 0.003;
				triShape[(i * 3)][1][1] -= 0.003;
				triShape[(i * 3)][2][1] -= 0.001;
				triShape[(i * 3) + 1][0][1] += 0.003;
				triShape[(i * 3) + 1][1][1] += 0.003;
				triShape[(i * 3) + 1][2][1] -= 0.003;
				triShape[(i * 3) + 2][0][1] -= 0.003;
				triShape[(i * 3) + 2][1][1] += 0.003;
				triShape[(i * 3) + 2][2][1] -= 0.001;
				if (triShape[(i * 3)][2][1] <= 0.5f && i < 2)
					DrawLine[i] = 4;
				else if (triShape[(i * 3)][2][1] <= -0.5f && i >= 2)
					DrawLine[i] = 4;
			}
			else if (DrawLine[i] == 4)
			{
				for (int k = 0; k < 3; k++)
				{
					triShape[(i * 3) + k][0][0] -= 0.002;
					triShape[(i * 3) + k][1][0] += 0.002;
				}
				if (i == 0 || i == 2)
				{
					if (triShape[(i * 3)][0][0] <= -0.8f)
						DrawLine[i] = 0;
				}
				else
				{
					if (triShape[(i * 3)][0][0] <= 0.2f)
						DrawLine[i] = 0;
				}
			}
		}
	}

	InitBuffer();

	glutPostRedisplay();
	glutTimerFunc(1, Timer, 1);
}
