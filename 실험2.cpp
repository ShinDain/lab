#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <gl/glew.h> // 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <stdio.h>

using namespace std;

FILE* Sphere = fopen("sphere.obj", "r");


GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void Timer(int value);
GLchar* filetobuf(const GLchar* filename);
void ReadObj(FILE* obhFile);

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

// obj 파일 읽기
void ReadObj(FILE* objFile, glm::vec3** Vout, glm::vec3** VNout, glm::ivec3** Fout, glm::ivec3** FNout, int* vertexNout, int* faceNout)
{
	//--- 1. 전체 버텍스 개수 및 삼각형 개수 세기
	char count[100];
	int vertexNum = 0;
	int vertexNNum = 0;
	int faceNum = 0;
	while (!feof(objFile)) {
		fscanf(objFile, "%s", count);
		if (count[0] == 'v' && count[1] == '\0')
			vertexNum += 1;
		else if (count[0] == 'v' && count[1] == 'n')
			vertexNNum += 1;
		else if (count[0] == 'f' && count[1] == '\0')
			faceNum += 1;
		memset(count, '\0', sizeof(count)); // 배열 초기화
	}
	//--- 2. 메모리 할당
	glm::vec3* V;
	glm::vec3* VN;
	glm::ivec3* F;
	glm::ivec3* FN;
	int VIndex = 0;
	int VNIndex = 0;
	int FIndex = 0;
	V = (glm::vec3*)malloc(sizeof(glm::vec3) * vertexNum);
	VN = (glm::vec3*)malloc(sizeof(glm::vec3) * vertexNNum);
	F = (glm::ivec3*)malloc(sizeof(glm::ivec3) * faceNum);
	FN = (glm::ivec3*)malloc(sizeof(glm::ivec3) * faceNum);
	rewind(objFile);

	int T[3];

	//--- 3. 할당된 메모리에 각 버텍스, 페이스 정보 입력
	while (!feof(objFile)) {
		fscanf(objFile, "%s", count);
		if (count[0] == 'v' && count[1] == '\0') {
			fscanf(objFile, "%f %f %f",
				&V[VIndex].x, &V[VIndex].y,
				&V[VIndex].z);
			VIndex++;

		}
		else if (count[0] == 'v' && count[1] == 'n') {
			fscanf(objFile, "%f %f %f",
				&VN[VNIndex].x, &VN[VNIndex].y,
				&VN[VNIndex].z);
			VNIndex++;
		}
		else if (count[0] == 'f' && count[1] == '\0') {
			fscanf(objFile, "%d/%d/%d %d/%d/%d %d/%d/%d",
				&F[FIndex].x, &T[0], &FN[FIndex].x,
				&F[FIndex].y, &T[1], &FN[FIndex].y,
				&F[FIndex].z, &T[2], &FN[FIndex].z);
			FIndex++;
		}
	}

	/**Vout = V;
	*VNout = VN;
	*Fout = F;
	*FNout = FN;
	*vertexNout = vertexNum;
	*faceNout = faceNum;*/
}


GLfloat circle[120][3] = {
	 {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0},{0.0, 1.0, 0.0}
};

GLuint shaderID;

//--- 변수 선언
GLuint VAO[6], VBO[2], EBO;

void InitBuffer()
{
	glm::vec3* V = NULL;
	glm::vec3* VN = NULL;
	glm::ivec3* F = NULL;
	glm::ivec3* FN = NULL;
	int vertexN = 0, faceN = 0;
	ReadObj(Sphere, &V, &VN, &F, &FN, &vertexN, &faceN);

	glGenVertexArrays(1, &VAO[0]); //--- VAO 를 지정하고 할당하기
	glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindVertexArray(VAO[0]); //--- VAO를 바인드하기
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(V), V, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VN), VN, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glUseProgram(shaderID);
	int lightPosLocation = glGetUniformLocation(shaderID, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, 0.0, 0.0, 5.0);
	int lightColorLocation = glGetUniformLocation(shaderID, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
	glUniform3f(lightColorLocation, 1.0, 1.0, 1.0);
	int objColorLocation = glGetUniformLocation(shaderID, "objectColor"); //--- object Color값 전달: (1.0, 0.5, 0.3)의 색
	glUniform3f(objColorLocation, 1.0, 0.5, 0.3);
}

float width = 1200;
float height = 1200;

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // 디스플레이 모드 설정
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

float timecount = 0.0f;

bool Wire = true;
bool Rdirection = false;
float Howfar = 0.0f;
float Hmove[2] = { 0.0f,0.0f };

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	//--- 변경된 배경색 설정
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // 바탕색을 변경
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	glUseProgram(shaderID);
	glEnable(GL_DEPTH_TEST);

	float num = (3.14 / 180) * 12;

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
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * Rlarge * Rself));
	//--- 뷰잉 변환
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 vTransform = glm::mat4(1.0f);
	vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(vTransform));
	//--- 투영 변환 행렬을 설정한 후, 버텍스 세이더에 저장한다.
	glm::mat4 pTransform = glm::mat4(1.0f);
	pTransform = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.1f, 200.0f);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pTransform));

	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 120);

	InitBuffer();

	glutSwapBuffers(); // 화면에 출력하기1
}
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'm':
		if (Wire == false)
			Wire = true;
		else
			Wire = false;
		break;
	case 'q':
		exit(EXIT_FAILURE);
		break;
	}
	InitBuffer();
	glutPostRedisplay(); //--- 배경색이 바뀔때마다 출력 콜백함수를 호출하여 화면을 refresh 한다
}

void Timer(int value)
{


	glutPostRedisplay();
	glutTimerFunc(15, Timer, 1);
}
