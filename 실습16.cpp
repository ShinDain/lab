#include <iostream>
#include <gl/glew.h> // 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
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
	vertexsource = filetobuf("vertex_view.glsl");

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
	fragmentsource = filetobuf("fragment_view.glsl");

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
	{
		{-100.0f,0.0f,0.0f},
		{100.0f,0.0f,0.0f}
	},
	{
		{0.0f,100.0f,0.0f},
		{0.0f,-100.0f,0.0f}
	},
	{
		{0.0f,0.0f,100.0f},
		{0.0f,0.0f,-100.0f}
	},
};
GLfloat LineC[3] =
{
	0.0f,0.0f,0.0f
};

GLfloat Pyramid[6][3] = {
	// 밑면
	{-0.5f,-0.5f,0.0f},
	{0.5f,-0.5f,0.0f},
	{0.0f,-0.5f,0.5f},
	{-0.5f,-0.5f,0.0f},
	{0.0f,-0.5f,-0.5f},
	{0.5f,-0.5f,0.0f}
};
GLfloat PyramidS[4][3][3] = {
	{
		{0.0f,0.5f,0.0f},
		{-0.5f,-0.5f,0.0f},
		{0.0f,-0.5f,0.5f}
	},

	{
		{0.0f,0.5f,0.0f},
		{0.0f,-0.5f,0.5f},
		{0.5f,-0.5f,0.0f}
	},

	{
		{0.0f,0.5f,0.0f},
		{0.5f,-0.5f,0.0f},
		{0.0f,-0.5f,-0.5f}	
	},

	{
		{0.0f,0.5f,0.0f},
		{0.0f,-0.5f,-0.5f},
		{-0.5f,-0.5f,0.0f}
	}
};
GLfloat PyramidC[6][3] = {
	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f}
};
GLfloat PyramidCS[4][3][3] = {
	{
		{1.0f,0.5f,0.2f},
		{1.0f,0.5f,0.2f},
		{1.0f,0.5f,0.2f}
	},
	{
		{1.0f,1.0f,0.0f},
		{1.0f,1.0f,0.0f},
		{1.0f,1.0f,0.0f}
	},
	{
		{1.0f,0.0f,1.0f},
		{1.0f,0.0f,1.0f},
		{1.0f,0.0f,1.0f}
	},
	{
		{0.0f,1.0f,1.0f},
		{0.0f,1.0f,1.0f},
		{0.0f,1.0f,1.0f}
	}
};

GLfloat Square[24][3] = {
	{-0.5f,0.5f,0.0f},
	{-0.5f,-0.5f,0.0f},
	{0.0f,-0.5f,0.5f},

	{-0.5f,0.5f,0.0f},
	{0.0f,-0.5f,0.5f},
	{0.0f,0.5f,0.5f},

	{0.5f,0.5f,0.0f},
	{0.5f,-0.5f,0.0f},
	{0.0f,-0.5f,-0.5f},

	{0.5f,0.5f,0.0f},
	{0.0f,-0.5f,-0.5f},
	{0.0f,0.5f,-0.5f},

	{0.0f,0.5f,-0.5f},
	{0.0f,-0.5f,-0.5f},
	{-0.5f,-0.5f,0.0f},

	{0.0f,0.5f,-0.5f},
	{-0.5f,-0.5f,0.0f},
	{-0.5f,0.5f,0.0f},
	//아랫면
	{0.0f,-0.5f,-0.5f},
	{-0.5f,-0.5f,0.0f},
	{0.0f,-0.5f,0.5f},

	{0.0f,-0.5f,-0.5f},
	{0.0f,-0.5f,0.5f},
	{0.5f,-0.5f,0.0f}
};
GLfloat SquareTF[2][6][3] = {
	//윗면
	{
		{0.0f,0.5f,-0.5f},
		{0.0f,0.5f,0.5f},
		{0.5f,0.5f,0.0f},
		{0.0f,0.5f,-0.5f},
		{-0.5f,0.5f,0.0f},
		{0.0f,0.5f,0.5f}
	}
	,
	// 앞면
	{
		{0.0f, 0.5f, 0.5f},
		{ 0.0f,-0.5f,0.5f },
		{ 0.5f,-0.5f,0.0f },
		{ 0.0f,0.5f,0.5f },
		{ 0.5f,-0.5f,0.0f },
		{ 0.5f,0.5f,0.0f }
	}
};

GLfloat SquareC[24][3] = {
	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},

	{0.0f,0.0f,1.0f},
	{0.0f,0.0f,1.0f},
	{0.0f,0.0f,1.0f},

	{0.5f,0.0f,0.5f},
	{0.5f,0.0f,0.5f},
	{0.5f,0.0f,0.5f},

	{0.0f,0.5f,0.5f},
	{0.0f,0.5f,0.5f},
	{0.0f,0.5f,0.5f},

	{0.0f,1.0f,0.0f},
	{0.0f,1.0f,0.0f},
	{0.0f,1.0f,0.0f},

	{1.0f,0.8f,0.0f},
	{1.0f,0.8f,0.0f},
	{1.0f,0.8f,0.0f},

	{0.5f,1.0f,0.5f},
	{0.5f,1.0f,0.5f},
	{0.5f,1.0f,0.5f},

	{1.0f,1.0f,0.0f},
	{1.0f,1.0f,0.0f},
	{1.0f,1.0f,0.0f},
};
GLfloat SquareCTF[2][6][3] = {
	{
		{1.0f,0.5f,0.2f},
		{1.0f,0.5f,0.2f},
		{1.0f,0.5f,0.2f},
		{1.0f,0.5f,0.2f},
		{1.0f,0.5f,0.2f},
		{1.0f,0.5f,0.2f}
	},
	{
		{1.0f,1.0f,0.0f},
		{1.0f,1.0f,0.0f},
		{1.0f,1.0f,0.0f},
		{1.0f,1.0f,0.0f},
		{1.0f,1.0f,0.0f},
		{1.0f,1.0f,0.0f}
	}
};

//--- 변수 선언
GLuint VAO[11], VBO[2];

void InitBuffer()
{
	for (int i = 0; i < 3; i++)
	{
		glGenVertexArrays(1, &VAO[i]); //--- VAO 를 지정하고 할당하기
		glBindVertexArray(VAO[i]); //--- VAO를 바인드하기
		glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(DrawLine[i]), DrawLine[i], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(LineC), LineC, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
	}

	// 피라미드 밑면
	glGenVertexArrays(1, &VAO[3]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[3]); //--- VAO를 바인드하기
	glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Pyramid), Pyramid, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(PyramidC), PyramidC, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	// 피라미드 사이드
	for (int i = 4; i < 8; i++)
	{
		glGenVertexArrays(1, &VAO[i]); //--- VAO 를 지정하고 할당하기
		glBindVertexArray(VAO[i]); //--- VAO를 바인드하기
		glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(PyramidS[i - 4]), PyramidS[i - 4], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(PyramidCS[i - 4]), PyramidCS[i - 4], GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
	}

	// 직육면체 나머지
	glGenVertexArrays(1, &VAO[8]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[8]); //--- VAO를 바인드하기
	glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Square), Square, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SquareC), SquareC, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	// 윗면과 앞면
	for (int i = 9; i < 11; i++)
	{
		glGenVertexArrays(1, &VAO[i]); //--- VAO 를 지정하고 할당하기
		glBindVertexArray(VAO[i]); //--- VAO를 바인드하기
		glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(SquareTF[i - 9]), SquareTF[i - 9], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(SquareCTF[i - 9]), SquareCTF[i - 9], GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
	}

}

GLuint shaderID;
GLint width = 1200;
GLint height = 1200;

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

float Timecount = 0.0f;
float Tricount = 0.0f;
float STcount = 0.0f;
float SFcount = 0.0f;

bool drawP = false;
bool RotateSelf = false;
bool Profar = true;
bool RST = false;
bool RSF = false;
bool Openp = false;

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	//--- 변경된 배경색 설정
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 바탕색을 변경
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 설정된 색으로 전체를 칠하기

	glUseProgram(shaderID);

	glEnable(GL_DEPTH_TEST);

	float xAngle = 0.0f;
	float yAngle = 0.0f;

	int modelLoc = glGetUniformLocation(shaderID, "Transform"); //--- 버텍스 세이더에서 모델링 변환 행렬 변수값을 받아온다.
	int viewLoc = glGetUniformLocation(shaderID, "viewTransform"); //--- 버텍스 세이더에서 뷰잉 변환 행렬 변수값을 받아온다.
	int projLoc = glGetUniformLocation(shaderID, "projectionTransform"); //--- 버텍스 세이더에서 투영 변환 행렬 변수값을 받아온다.
	//--- 모델링 변환
	glm::mat4 mTransform = glm::mat4(1.0f);
	glm::mat4 mTransform1 = glm::mat4(1.0f);
	glm::mat4 Rtime = glm::mat4(1.0f);
	mTransform = glm::rotate(mTransform, glm::radians(yAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	Rtime = glm::rotate(Rtime, glm::radians(Timecount), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform));
	//--- 뷰잉 변환
	glm::vec3 cameraPos = glm::vec3(2.0f, 3.0f, 5.0f);
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 vTransform = glm::mat4(1.0f);
	vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(vTransform));
	//--- 투영 변환 행렬을 설정한 후, 버텍스 세이더에 저장한다.
	glm::mat4 pTransform = glm::mat4(1.0f);
	glm::mat4 pTransform1 = glm::mat4(1.0f);
	pTransform = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.1f, 50.0f); 
	pTransform1 = glm::ortho((float)-3, (float)3, (float)-3, (float)3, 0.1f, 50.0f);
	if (Profar == true)
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pTransform));
	else
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pTransform1));

	glm::mat4 TriR1_1 = glm::mat4(1.0f);
	glm::mat4 TriR1_2 = glm::mat4(1.0f);
	glm::mat4 TriR1_3 = glm::mat4(1.0f);
	glm::mat4 TriT1 = glm::mat4(1.0f);
	glm::mat4 TriTR1 = glm::mat4(1.0f);

	TriR1_1 = glm::rotate(TriR1_1, glm::radians(Tricount), glm::vec3(1.0f, 0.0f, 0.0f));
	TriR1_2 = glm::rotate(TriR1_2, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	TriR1_3 = glm::rotate(TriR1_3, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	TriT1 = glm::translate(TriT1, glm::vec3(0.25f,0.5f,-0.25f));
	TriTR1 = glm::translate(TriTR1, glm::vec3(-0.25f, -0.5f, 0.25f));

	glm::mat4 TriR2_1 = glm::mat4(1.0f);
	glm::mat4 TriR2_2 = glm::mat4(1.0f);
	glm::mat4 TriR2_3 = glm::mat4(1.0f);
	glm::mat4 TriT2 = glm::mat4(1.0f);
	glm::mat4 TriTR2 = glm::mat4(1.0f);

	TriR2_1 = glm::rotate(TriR2_1, glm::radians(Tricount), glm::vec3(1.0f, 0.0f, 0.0f));
	TriR2_2 = glm::rotate(TriR2_2, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	TriR2_3 = glm::rotate(TriR2_3, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	TriT2 = glm::translate(TriT2, glm::vec3(-0.25f, 0.5f, -0.25f));
	TriTR2 = glm::translate(TriTR2, glm::vec3(0.25f, -0.5f, 0.25f));

	glm::mat4 TriR3_1 = glm::mat4(1.0f);
	glm::mat4 TriR3_2 = glm::mat4(1.0f);
	glm::mat4 TriR3_3 = glm::mat4(1.0f);
	glm::mat4 TriT3 = glm::mat4(1.0f);
	glm::mat4 TriTR3 = glm::mat4(1.0f);

	TriR3_1 = glm::rotate(TriR3_1, glm::radians(-Tricount), glm::vec3(1.0f, 0.0f, 0.0f));
	TriR3_2 = glm::rotate(TriR3_2, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	TriR3_3 = glm::rotate(TriR3_3, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	TriT3 = glm::translate(TriT3, glm::vec3(-0.25f, 0.5f, 0.25f));
	TriTR3 = glm::translate(TriTR3, glm::vec3(0.25f, -0.5f, -0.25f));

	glm::mat4 TriR4_1 = glm::mat4(1.0f);
	glm::mat4 TriR4_2 = glm::mat4(1.0f);
	glm::mat4 TriR4_3 = glm::mat4(1.0f);
	glm::mat4 TriT4 = glm::mat4(1.0f);
	glm::mat4 TriTR4 = glm::mat4(1.0f);

	TriR4_1 = glm::rotate(TriR4_1, glm::radians(-Tricount), glm::vec3(1.0f, 0.0f, 0.0f));
	TriR4_2 = glm::rotate(TriR4_2, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	TriR4_3 = glm::rotate(TriR4_3, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	TriT4 = glm::translate(TriT4, glm::vec3(0.25f, 0.5f, 0.25f));
	TriTR4 = glm::translate(TriTR4, glm::vec3(-0.25f, -0.5f, -0.25f));

	glm::mat4 SFR1 = glm::mat4(1.0f);
	glm::mat4 SFR2 = glm::mat4(1.0f);
	glm::mat4 SFR3 = glm::mat4(1.0f);
	glm::mat4 SFT = glm::mat4(1.0f);
	glm::mat4 SFTR = glm::mat4(1.0f);

	SFR1 = glm::rotate(SFR1, glm::radians(SFcount), glm::vec3(1.0f, 0.0f, 0.0f));
	SFR2 = glm::rotate(SFR2, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	SFR3 = glm::rotate(SFR3, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	SFT = glm::translate(SFT, glm::vec3(-0.25f, 0.5f, -0.25f));
	SFTR = glm::translate(SFTR, glm::vec3(0.25f, -0.5f, 0.25f));

	glm::mat4 STR1 = glm::mat4(1.0f);
	glm::mat4 STR2 = glm::mat4(1.0f);
	glm::mat4 STR3 = glm::mat4(1.0f);
	glm::mat4 STT = glm::mat4(1.0f);
	glm::mat4 STTR = glm::mat4(1.0f);

	STR1 = glm::rotate(STR1, glm::radians(STcount), glm::vec3(1.0f, 0.0f, 0.0f));
	STR2 = glm::rotate(STR2, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	STR3 = glm::rotate(STR3, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	STT = glm::translate(STT, glm::vec3(0.0f, -0.5f, 0.0f));
	STTR = glm::translate(STTR, glm::vec3(0.0f, 0.5f, 0.0f));

	for (int i = 0; i < 3; i++)
	{
		glBindVertexArray(VAO[i]);
		glDrawArrays(GL_LINE_STRIP, 0, 2);
	}

	if (drawP == true)
	{
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * Rtime));
		glBindVertexArray(VAO[3]);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * Rtime * TriTR1 * TriR1_3 * TriR1_1 * TriR1_2 * TriT1));
		glBindVertexArray(VAO[4]);								
		glDrawArrays(GL_TRIANGLES, 0, 3);						
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * Rtime * TriTR2 * TriR2_3 * TriR2_1 * TriR2_2 * TriT2));
		glBindVertexArray(VAO[5]);												    
		glDrawArrays(GL_TRIANGLES, 0, 3);										    
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * Rtime * TriTR3 * TriR3_3 * TriR3_1 * TriR3_2 * TriT3));
		glBindVertexArray(VAO[6]);												    
		glDrawArrays(GL_TRIANGLES, 0, 3);										    
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * Rtime * TriTR4 * TriR4_3 * TriR4_1 * TriR4_2 * TriT4));
		glBindVertexArray(VAO[7]);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	else
	{
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * Rtime));
		glBindVertexArray(VAO[8]);
		glDrawArrays(GL_TRIANGLES, 0, 24);

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * Rtime * STTR * STR3 * STR1 * STR2 * STT));
		glBindVertexArray(VAO[9]);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * Rtime * SFTR * SFR3 * SFR1 * SFR2 * SFT));
		glBindVertexArray(VAO[10]);
		glDrawArrays(GL_TRIANGLES, 0, 6);
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
	case 'c':
		if (drawP == true)
			drawP = false;
		else
			drawP = true;
		break;
	case 'y':
		if(RotateSelf == false)
			RotateSelf = true;
		else
			RotateSelf = false;
		break;
	case 'p':
		if (Profar == true)
			Profar = false;
		else
			Profar = true;
		break;
	case 't':
		if (RST == true)
			RST = false;
		else
			RST = true;
		break;
	case 'f':
		if (RSF == true)
			RSF = false;
		else
			RSF = true;
		break;
	case 'o':
		if (Openp == true)
			Openp = false;
		else
			Openp = true;
		break;
	case 'q':
		exit(EXIT_FAILURE);
		break;
	}
	glutPostRedisplay(); //--- 배경색이 바뀔때마다 출력 콜백함수를 호출하여 화면을 refresh 한다
}

void Timer(int value)
{
	if(RotateSelf == true)
		Timecount++;
	if (Openp == true)
	{
		if (Tricount < 90)
			Tricount++;
	}
	else
	{
		if (Tricount > 0)
			Tricount--;
	}
	if (RST == true)
		STcount++;
	if (RSF == true)
	{
		if (SFcount < 90)
			SFcount++;
	}
	else
	{
		if (SFcount > 0)
			SFcount--;
	}

	
	InitBuffer();
	glutPostRedisplay();
	glutTimerFunc(1, Timer, 1);
}
