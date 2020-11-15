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

GLfloat circle[6][30][3];
GLfloat circleC[30];

//--- 변수 선언
GLuint VAO[6], VBO[2];

void InitBuffer()
{
	for (int i = 0; i < 6; i++)
	{
		glGenVertexArrays(1, &VAO[i]); //--- VAO 를 지정하고 할당하기
		glBindVertexArray(VAO[i]); //--- VAO를 바인드하기
		glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(circle[i]), circle[i], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(circleC), circleC, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
	}
}

GLuint shaderID;

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

GLUquadricObj* qobj[7];

float timecount = 0.0f;

bool Wire = true;
bool Rdirection = false;
float Howfar = 0.0f;
float Hmove[2] = { 0.0f,0.0f};

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	//--- 변경된 배경색 설정
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 바탕색을 변경
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	glUseProgram(shaderID);
	glEnable(GL_DEPTH_TEST);

	float num = (3.14 / 180) * 12;

	// 궤도 그리기
	for (int i = 0; i < 3; i++)
	{
		for (int k = 0; k < 30; k++)
		{
			circle[i][k][0] = 2 * cos(num * k);
			circle[i][k][1] = 0.0;
			circle[i][k][2] = 2 * sin(num * k);
		}
	}
	for (int i = 3; i < 6; i++)
	{
		for (int k = 0; k < 30; k++)
		{
			circle[i][k][0] = 0.8 * cos(num * k);
			circle[i][k][1] = 0.0;
			circle[i][k][2] = 0.8 * sin(num * k);
		}
	}

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
	T1 = glm::translate(T1, glm::vec3(2 * cos(0),0,0));
	T2 = glm::translate(T2, glm::vec3(0.8 * cos(0),0,0));

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

	qobj[0] = gluNewQuadric();
	if(Wire == false)
		gluQuadricDrawStyle(qobj[0], GLU_FILL);
	else
		gluQuadricDrawStyle(qobj[0], GLU_LINE);
	gluQuadricNormals(qobj[0], GLU_SMOOTH);
	gluQuadricOrientation(qobj[0], GLU_OUTSIDE);
	gluSphere(qobj[0], 0.5, 50, 50);

	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_LINE_LOOP, 0, 30);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * Rlarge * Rself * T1));
	qobj[1] = gluNewQuadric();
	if (Wire == false)
		gluQuadricDrawStyle(qobj[1], GLU_FILL);
	else
	gluQuadricDrawStyle(qobj[1], GLU_LINE);
	gluQuadricNormals(qobj[1], GLU_SMOOTH);
	gluQuadricOrientation(qobj[1], GLU_OUTSIDE);
	gluSphere(qobj[1], 0.2, 50, 50);
	glBindVertexArray(VAO[3]);
	glDrawArrays(GL_LINE_LOOP, 0, 30);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * Rlarge * Rself * T1 * T2));
	qobj[4] = gluNewQuadric();
	if (Wire == false)
		gluQuadricDrawStyle(qobj[4], GLU_FILL);
	else
	gluQuadricDrawStyle(qobj[4], GLU_LINE);
	gluQuadricNormals(qobj[4], GLU_SMOOTH);
	gluQuadricOrientation(qobj[4], GLU_OUTSIDE);
	gluSphere(qobj[4], 0.1, 50, 50);

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * S1));
	glBindVertexArray(VAO[1]);
	glDrawArrays(GL_LINE_LOOP, 0, 30);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform  * S1 * T1));
	qobj[2] = gluNewQuadric();
	if (Wire == false)
		gluQuadricDrawStyle(qobj[2], GLU_FILL);
	else
	gluQuadricDrawStyle(qobj[2], GLU_LINE);
	gluQuadricNormals(qobj[2], GLU_SMOOTH);
	gluQuadricOrientation(qobj[2], GLU_OUTSIDE);
	gluSphere(qobj[2], 0.2, 50, 50);
	glBindVertexArray(VAO[4]);
	glDrawArrays(GL_LINE_LOOP, 0, 30);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * S1 * T1 * T2));
	qobj[5] = gluNewQuadric();
	if (Wire == false)
		gluQuadricDrawStyle(qobj[5], GLU_FILL);
	else
	gluQuadricDrawStyle(qobj[5], GLU_LINE);
	gluQuadricNormals(qobj[5], GLU_SMOOTH);
	gluQuadricOrientation(qobj[5], GLU_OUTSIDE);
	gluSphere(qobj[5], 0.1, 50, 50);

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * S2));
	glBindVertexArray(VAO[2]);
	glDrawArrays(GL_LINE_LOOP, 0, 30);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * S2 * T1));
	qobj[3] = gluNewQuadric();
	if (Wire == false)
		gluQuadricDrawStyle(qobj[3], GLU_FILL);
	else
	gluQuadricDrawStyle(qobj[3], GLU_LINE);
	gluQuadricNormals(qobj[3], GLU_SMOOTH);
	gluQuadricOrientation(qobj[3], GLU_OUTSIDE);
	gluSphere(qobj[3], 0.2, 50, 50);
	glBindVertexArray(VAO[5]);
	glDrawArrays(GL_LINE_LOOP, 0, 30);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * S2 * T1 * T2));
	qobj[6] = gluNewQuadric();
	if (Wire == false)
		gluQuadricDrawStyle(qobj[6], GLU_FILL);
	else
	gluQuadricDrawStyle(qobj[6], GLU_LINE);
	gluQuadricNormals(qobj[6], GLU_SMOOTH);
	gluQuadricOrientation(qobj[6], GLU_OUTSIDE);
	gluSphere(qobj[6], 0.1, 50, 50);
	
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
	case 'w':
		Hmove[1] = Hmove[1] + 0.1;
		break;
	case 'a':
		Hmove[0] = Hmove[0] - 0.1;
		break;
	case 's':
		Hmove[1] = Hmove[1] - 0.1;
		break;
	case 'd':
		Hmove[0] = Hmove[0] + 0.1;
		break;
	case 'z':
		Howfar = Howfar + 0.1;
		break;
	case 'x':
		Howfar = Howfar - 0.1;
		break;
	case 'y':
		if (Rdirection == false)
			Rdirection = true;
		else
			Rdirection = false;
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
	if (Rdirection == true)
		timecount = timecount + 1;
	else
		timecount = timecount - 1;

	InitBuffer();
	glutPostRedisplay();
	glutTimerFunc(15, Timer, 1);
}
