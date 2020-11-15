#include <iostream>
#include <gl/glew.h> // 필요한 헤더파일 include
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
	vertexsource = filetobuf("vertex_light2.glsl");

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
	fragmentsource = filetobuf("fragment_light2.glsl");

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

float CP[3] = {0.0f , 5.0f,10.0f };
float look_pos[3] = { 0.0f,0.0f ,0.0f };

float S1_pos[3] = { 0.0f, 0.8f, 0.0f };
float S2_Angle = 45.0f;
float S3_Angle = 0.0f;

GLUquadricObj* qobj;

float vAngle = 0;

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	//--- 변경된 배경색 설정
	glClearColor(0.0f,0.0f,0.0f,0.0f); // 바탕색을 변경
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 설정된 색으로 전체를 칠하기

	glUseProgram(shaderID);

	glEnable(GL_DEPTH_TEST);

	int modelLoc = glGetUniformLocation(shaderID, "Transform"); //--- 버텍스 세이더에서 모델링 변환 행렬 변수값을 받아온다.
	int viewLoc = glGetUniformLocation(shaderID, "viewTransform"); //--- 버텍스 세이더에서 뷰잉 변환 행렬 변수값을 받아온다.
	int projLoc = glGetUniformLocation(shaderID, "projectionTransform"); //--- 버텍스 세이더에서 투영 변환 행렬 변수값을 받아온다.
	//--- 모델링 변환
	glm::mat4 mTransform = glm::mat4(1.0f);
	mTransform = glm::translate(mTransform, glm::vec3(0.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform));
	//--- 뷰잉 변환
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
	//--- 투영 변환 행렬을 설정한 후, 버텍스 세이더에 저장한다.
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
