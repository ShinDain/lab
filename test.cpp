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
GLuint make_shaderProgram();

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
	vertexsource = filetobuf("vertex_light.glsl");

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
	fragmentsource = filetobuf("fragment_light.glsl");

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
	0.5f,-0.5f,0.5f,0.0f,0.0f,1.0f,
	0.0f,0.5f,0.0f,0.0f,0.0f,1.0f,
	-0.5f,-0.5f,0.5f,0.0f,0.0f,1.0f,

	0.5f,-0.5f,0.5f,1.0f,0.0f,0.0f,
	0.5f,-0.5f,-0.5f,1.0f,0.0f,0.0f,
	0.0f,0.5f,0.0f,1.0f,0.0f,0.0f,

	0.0f,0.5f,0.0f,0.0f,0.0f,-1.0f,
	-0.5f,-0.5f,-0.5f,0.0f,0.0f,-1.0f,
	0.5f,-0.5f,-0.5f,0.0f,0.0f,-1.0f,

	0.0f,0.5f,0.0f,-1.0f,0.0f,0.0f,
	-0.5f,-0.5f,-0.5f,-1.0f,0.0f,0.0f,
	-0.5f,-0.5f,0.5f,-1.0f,0.0f,0.0f,

	// 밑변
	0.5f,-0.5f,0.5f,0.0f,-1.0f,0.0f,
	-0.5f,-0.5f,0.5f,0.0f,-1.0f,0.0f,
	-0.5f,-0.5f,-0.5f,0.0f,-1.0f,0.0f,

	-0.5f,-0.5f,-0.5f,0.0f,-1.0f,0.0f,
	0.5f,-0.5f,-0.5f,0.0f,-1.0f,0.0f,
	0.5f,-0.5f,0.5f,0.0f,-1.0f,0.0f,
};

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

float vertices[] = { //--- 버텍스 속성: 좌표값(FragPos), 노말값 (Normal)
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

//--- 변수 선언
GLuint VAO[5], VBO;

GLuint shaderID;

bool lighton = true;

void InitBufferA()
{
	glUseProgram(shaderID);
	int lightPosLocation = glGetUniformLocation(shaderID, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, LPos[0], LPos[1], LPos[2]);
	int lightColorLocation = glGetUniformLocation(shaderID, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
	glUniform3f(lightColorLocation, LColor[0], LColor[1], LColor[2]);
	int red = glGetUniformLocation(shaderID, "objectColor");
	glUniform3f(red, 1.0f, 1.0f, 1.0f);
}

void InitBuffer()
{
	glGenVertexArrays(1, &VAO[0]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[0]); //--- VAO를 바인드하기
	glGenBuffers(1, &VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //--- 위치 속성
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //--- 노말 속성
	glEnableVertexAttribArray(1);

	glUseProgram(shaderID);
	int lightPosLocation = glGetUniformLocation(shaderID, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, LPos[0], LPos[1], LPos[2]);
	int lightColorLocation = glGetUniformLocation(shaderID, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
	glUniform3f(lightColorLocation, LColor[0], LColor[1], LColor[2]);
	int red = glGetUniformLocation(shaderID, "objectColor");
	glUniform3f(red, 1.0f, 0.0f, 0.0f);
}

void InitBuffer1()
{
	glGenVertexArrays(1, &VAO[1]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[1]); //--- VAO를 바인드하기
	glGenBuffers(1, &VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //--- 위치 속성
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //--- 노말 속성
	glEnableVertexAttribArray(1);

	glUseProgram(shaderID);
	int lightPosLocation = glGetUniformLocation(shaderID, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, LPos[0], LPos[1], LPos[2]);
	int lightColorLocation = glGetUniformLocation(shaderID, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
	glUniform3f(lightColorLocation, LColor[0], LColor[1], LColor[2]);
	int yellow = glGetUniformLocation(shaderID, "objectColor");
	glUniform3f(yellow, 1.0f, 1.0f, 0.0f);
}
void InitBuffer2()
{
	glGenVertexArrays(1, &VAO[2]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[2]); //--- VAO를 바인드하기
	glGenBuffers(1, &VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //--- 위치 속성
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //--- 노말 속성
	glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &VAO[3]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[3]); //--- VAO를 바인드하기
	glGenBuffers(1, &VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //--- 위치 속성
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //--- 노말 속성
	glEnableVertexAttribArray(1);

	glUseProgram(shaderID);
	int lightPosLocation = glGetUniformLocation(shaderID, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, LPos[0], LPos[1], LPos[2]);
	int lightColorLocation = glGetUniformLocation(shaderID, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
	glUniform3f(lightColorLocation, LColor[0], LColor[1], LColor[2]);
	int cyan = glGetUniformLocation(shaderID, "objectColor");
	glUniform3f(cyan, 0.0f, 1.0f, 1.0f);
}
void InitBuffer3()
{
	glGenVertexArrays(1, &VAO[4]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[4]); //--- VAO를 바인드하기
	glGenBuffers(1, &VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //--- 위치 속성
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //--- 노말 속성
	glEnableVertexAttribArray(1);

	glUseProgram(shaderID);
	int lightPosLocation = glGetUniformLocation(shaderID, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, LPos[0], LPos[1], LPos[2]);
	int lightColorLocation = glGetUniformLocation(shaderID, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
	glUniform3f(lightColorLocation, LColor[0], LColor[1], LColor[2]);
	int copper = glGetUniformLocation(shaderID, "objectColor");
	glUniform3f(copper, 0.5f, 0.3f, 0.0f);
}

GLfloat OColor[3] = { 1.0f, 1.0f, 1.0f };

//--- 변수 선언
GLuint platform,pyramid, snow[20] ;

void InitBuffer_snow()
{
	for (int i = 0; i < 20; i++)
	{
		glGenVertexArrays(1, &snow[i]); //--- VAO 를 지정하고 할당하기
		glBindVertexArray(snow[i]); //--- VAO를 바인드하기
		glGenBuffers(1, &VBO); //--- 2개의 VBO를 지정하고 할당하기

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //--- 위치 속성
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //--- 노말 속성
		glEnableVertexAttribArray(1);
	}

	glUseProgram(shaderID);
	int lightPosLocation = glGetUniformLocation(shaderID, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, LPos[0], LPos[1], LPos[2]);
	int lightColorLocation = glGetUniformLocation(shaderID, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
	glUniform3f(lightColorLocation, LColor[0], LColor[1], LColor[2]);
	int objColorLocation = glGetUniformLocation(shaderID, "objectColor"); //--- object Color값 전달: (1.0, 0.5, 0.3)의 색
	glUniform3f(objColorLocation, OColor[0], OColor[1], OColor[2]);
}

void InitBuffer_platform()
{
	glGenVertexArrays(1, &platform); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(platform); //--- VAO를 바인드하기
	glGenBuffers(1, &VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //--- 위치 속성
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //--- 노말 속성
	glEnableVertexAttribArray(1);
	
	glUseProgram(shaderID);
	int lightPosLocation = glGetUniformLocation(shaderID, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, LPos[0], LPos[1], LPos[2]);
	int lightColorLocation = glGetUniformLocation(shaderID, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
	glUniform3f(lightColorLocation, LColor[0], LColor[1], LColor[2]);
	int objColorLocation = glGetUniformLocation(shaderID, "objectColor"); //--- object Color값 전달: (1.0, 0.5, 0.3)의 색
	glUniform3f(objColorLocation, 1.0f,1.0f, 0.5f);
}

void InitBuffer_pyramid()
{
	glGenVertexArrays(1, &pyramid); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(pyramid); //--- VAO를 바인드하기
	glGenBuffers(1, &VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Pyramid_Normal), Pyramid_Normal, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //--- 위치 속성
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //--- 노말 속성
	glEnableVertexAttribArray(1);

	glUseProgram(shaderID);
	int lightPosLocation = glGetUniformLocation(shaderID, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, LPos[0], LPos[1], LPos[2]);
	int lightColorLocation = glGetUniformLocation(shaderID, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
	glUniform3f(lightColorLocation, LColor[0], LColor[1], LColor[2]);
	int objColorLocation = glGetUniformLocation(shaderID, "objectColor"); //--- object Color값 전달: (1.0, 0.5, 0.3)의 색
	glUniform3f(objColorLocation, 1.0f, 1.0f, 0.0f);
}


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

	glutDisplayFunc(drawScene); // 출력 콜백함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 콜백함수 지정
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(10, Timer, 1);

	glutMainLoop(); // 이벤트 처리 시작
}

float CP[3] = { 0.0f , 5.0f,10.0f };
float look_pos[3] = { 0.0f,0.0f ,0.0f };

float S1_pos[3] = { 0.0f, 0.8f, 0.0f };
float S2_Angle = 45.0f;
float S3_Angle = 0.0f;


GLUquadricObj* qobj[4];

float timecount = 180.0f;

bool Wire = true;
bool Rdirection = false;
float Howfar = 0.0f;
float Hmove[2] = { 0.0f,0.0f };

float snow_height[20];

float vAngle = 0;

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	//--- 변경된 배경색 설정
		//--- 변경된 배경색 설정
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // 바탕색을 변경
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
	glm::mat4 mTransform = glm::mat4(1.0f);
	glm::mat4 mT = glm::mat4(1.0f);
	mTransform = glm::rotate(mTransform, glm::radians(xAngle), glm::vec3(1.0f, 0.0f, 0.0f));
	mT = glm::translate(mT, glm::vec3(Hmove[0], Hmove[1], Howfar));
	mTransform = mT * mTransform;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * Rlarge * Rself * T1 * T2 * T1));
	//--- 뷰잉 변환
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 20.0f);
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 vTransform = glm::mat4(1.0f);
	vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(vTransform));
	//--- 투영 변환 행렬을 설정한 후, 버텍스 세이더에 저장한다.
	glm::mat4 pTransform = glm::mat4(1.0f);
	pTransform = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.1f, 200.0f);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pTransform));

	qobj[0] = gluNewQuadric();
	if (Wire == false)
		gluQuadricDrawStyle(qobj[0], GLU_FILL);
	else
		gluQuadricDrawStyle(qobj[0], GLU_LINE);
	gluQuadricNormals(qobj[0], GLU_SMOOTH);
	gluQuadricOrientation(qobj[0], GLU_OUTSIDE);
	gluSphere(qobj[0], 0.5, 50, 50);


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * Rlarge * Rself * T1 * T2));
	qobj[1] = gluNewQuadric();
	if (Wire == false)
		gluQuadricDrawStyle(qobj[1], GLU_FILL);
	else
		gluQuadricDrawStyle(qobj[1], GLU_LINE);
	gluQuadricNormals(qobj[1], GLU_SMOOTH);
	gluQuadricOrientation(qobj[1], GLU_OUTSIDE);
	gluSphere(qobj[1], 0.2, 50, 50);

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * Rlarge * Rself * T1));
	qobj[2] = gluNewQuadric();
	if (Wire == false)
		gluQuadricDrawStyle(qobj[2], GLU_FILL);
	else
		gluQuadricDrawStyle(qobj[2], GLU_LINE);
	gluQuadricNormals(qobj[2], GLU_SMOOTH);
	gluQuadricOrientation(qobj[2], GLU_OUTSIDE);
	gluSphere(qobj[2], 0.1, 50, 50);

	glm::mat4 LT = glm::mat4(1.0f);

	LT = glm::translate(LT, glm::vec3(LPos[0], LPos[1], LPos[2]));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * LT));
	qobj[3] = gluNewQuadric();
	gluQuadricDrawStyle(qobj[3], GLU_FILL);
	gluQuadricNormals(qobj[3], GLU_SMOOTH);
	gluQuadricOrientation(qobj[3], GLU_OUTSIDE);
	gluSphere(qobj[3], 0.1, 50, 50);

	glutSwapBuffers(); // 화면에 출력하기1

	glm::mat4 PT = glm::mat4(1.0f);
	glm::mat4 PS = glm::mat4(1.0f);

	PT = glm::translate(PT, glm::vec3(0.0f, -2.0f, 0.0f));
	PS = glm::scale(PS, glm::vec3(10.0f, 0.2f, 10.0f));

	InitBuffer_platform();
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(PT * PS));
	glBindVertexArray(platform);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glm::mat4 PyT = glm::mat4(1.0f);
	glm::mat4 PyS = glm::mat4(1.0f);

	PyT = glm::translate(PyT, glm::vec3(0.0f, -1.0f, 0.0f));
	PyS = glm::scale(PyS, glm::vec3(2.0f, 2.0f, 2.0f));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(PyT * PyS));
	InitBuffer_pyramid();
	glBindVertexArray(pyramid);
	glDrawArrays(GL_TRIANGLES, 0, 18);

	srand(time(NULL));

	glm::mat4 ST = glm::mat4(1.0f);
	glm::mat4 SS = glm::mat4(1.0f);

	InitBuffer_snow();
	for (int i = 0; i < 20; i++)
	{
		ST = glm::translate(ST, glm::vec3((float)(rand() % 10) - 10, snow_height[i], (float)(rand() % 10) - 10));
		SS = glm::scale(SS, glm::vec3(0.2f, 0.2f, 0.2f));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(ST * SS));

		glBindVertexArray(snow[i]);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
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
		lighton = !lighton;
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
	glutPostRedisplay(); //--- 배경색이 바뀔때마다 출력 콜백함수를 호출하여 화면을 refresh 한다
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
