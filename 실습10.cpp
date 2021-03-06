#include <iostream>
#include <gl/glew.h> // 필요한 헤더파일 include
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

// 세이더 파일 읽기
GLchar* filetobuf(const GLchar* filename)
{
	cout << "정상" << endl;

	ifstream readFile;             //읽을 목적의 파일 선언
	GLchar tmp[256];
	GLchar str[512] = { NULL };

	readFile.open(filename);    //파일 열기
	if (readFile.is_open())    //파일이 열렸는지 확인
	{
		while (!readFile.eof())    //파일 끝까지 읽었는지 확인
		{
			readFile.getline(tmp, 256);    //한줄씩 읽어오기
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
	vertexsource = filetobuf("vertex.glsl");

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
	fragmentsource = filetobuf("fragment.glsl");

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

//--- 변수 선언
GLuint VAO[12], VBO[2];

void InitBuffer()
{
	// 바깥 삼각형
	for (int i = 0; i < 12; i++)
	{
		glGenVertexArrays(1, &VAO[i]); //--- VAO 를 지정하고 할당하기
		glBindVertexArray(VAO[i]); //--- VAO를 바인드하기
		glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기

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

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(100, 100); // 윈도우의 위치 지정
	glutInitWindowSize(width, height); // 윈도우의 크기 지정
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
	InitBuffer();

	glutDisplayFunc(drawScene); // 출력 콜백함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 콜백함수 지정
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(10, Timer, 1);

	glutMainLoop(); // 이벤트 처리 시작
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	//--- 변경된 배경색 설정
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 바탕색을 변경
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 설정된 색으로 전체를 칠하기

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
	glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
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
	glutPostRedisplay(); //--- 배경색이 바뀔때마다 출력 콜백함수를 호출하여 화면을 refresh 한다
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
