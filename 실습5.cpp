#include <iostream>
#include <gl/glew.h> // 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <fstream>

using namespace std;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Mouse(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y);\
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

GLfloat triShape1[3][3] = {// 삼각형 꼭짓점 좌표값
{ 0.6f, -0.8f, 0.0f},
{ 0.8f, -0.8f, 0.0f },
{ 0.7f, -0.7f, 0.0f} };
GLfloat triShape2[3][3] = {// 삼각형 꼭짓점 좌표값
{ -0.8f, -0.8f, 0.0f},
{ -0.6f, -0.8f, 0.0f },
{ -0.7f, -0.7f, 0.0f} };
GLfloat triShape3[3][3] = {// 삼각형 꼭짓점 좌표값
{ 0.6f, 0.7f, 0.0f},
{ 0.8f, 0.7f, 0.0f },
{ 0.7f, 0.8f, 0.0f } };
GLfloat triShape4[3][3] = {// 삼각형 꼭짓점 좌표값
{ -0.8f, +0.7f, 0.0f},
{ -0.6f, +0.7f, 0.0f },
{ -0.7f, +0.8f, 0.0f} };
GLfloat colors1[3][3] = { // 삼각형 꼭짓점 색상
{1.0f, 0.0f, 0.0f},
{1.0f, 0.0f, 0.0f},
{1.0f, 0.0f, 0.0f} };
GLfloat colors2[3][3] = { // 삼각형 꼭짓점 색상
{0.0f, 1.0f, 0.0f},
{0.0f, 1.0f, 0.0f},
{0.0f, 1.0f, 0.0f} };
GLfloat colors3[3][3] = { // 삼각형 꼭짓점 색상
{0.0f, 0.0f, 1.0f},
{0.0f, 0.0f, 1.0f},
{0.0f, 0.0f, 1.0f} };
GLfloat colors4[3][3] = { // 삼각형 꼭짓점 색상
{1.0f, 1.0f, 0.0f},
{1.0f, 1.0f, 0.0f},
{1.0f, 1.0f, 0.0f} };

//--- 변수 선언
GLuint VAO[4], VBO1[2], VBO2[2], VBO3[2], VBO4[2];

void InitBuffer()
{
	glGenVertexArrays(1, &VAO[0]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[0]); //--- VAO를 바인드하기
	glGenBuffers(2, VBO1); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO1[0]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), triShape1, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1[1]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colors1, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &VAO[1]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[1]); //--- VAO를 바인드하기
	glGenBuffers(2, VBO2); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO2[0]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), triShape2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2[1]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colors2, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);


	glGenVertexArrays(1, &VAO[2]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[2]); //--- VAO를 바인드하기
	glGenBuffers(2, VBO3); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO3[0]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), triShape3, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO3[1]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colors3, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &VAO[3]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[3]); //--- VAO를 바인드하기
	glGenBuffers(2, VBO4); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO4[0]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), triShape4, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO4[1]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colors4, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

}

GLuint shaderID;

GLint width = 800;
GLint height = 800;
GLint Number = 0;

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
	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(1, Timer, 1);

	glutMainLoop(); // 이벤트 처리 시작
}

bool DrawL = false;
bool Rotate = false;

int direction[4] = { rand() % 4 ,rand() % 4 ,rand() % 4 ,rand() % 4 };

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	//--- 변경된 배경색 설정
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 바탕색을 변경
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 설정된 색으로 전체를 칠하기

	glUseProgram(shaderID);

	for (int i = 0; i < 4; i++)
	{
		glBindVertexArray(VAO[i]);
		if(DrawL == true)
			glDrawArrays(GL_LINE_LOOP, 0, 3);
		else
			glDrawArrays(GL_TRIANGLES, 0, 3);
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
	case 'f':
		if(DrawL == false)
			DrawL = true;
		else
			DrawL = false;
		break;
	case 'm':
		Rotate = true;
		break;
	case 's':
		Rotate = false;
		break;
	case 'c':
		int a;
		a = rand() % 3;
		int b;
		b = rand() % 3;
		int c;
		c = rand() % 3;
		int d;
		d = rand() % 3;
		colors1[a][0] = (float)(rand() % 100) / 100;
		colors1[a][1] = (float)(rand() % 100) / 100;
		colors1[a][2] = (float)(rand() % 100) / 100;
		colors2[b][0] = (float)(rand() % 100) / 100;
		colors2[b][1] = (float)(rand() % 100) / 100;
		colors2[b][2] = (float)(rand() % 100) / 100;
		colors3[c][0] = (float)(rand() % 100) / 100;
		colors3[c][1] = (float)(rand() % 100) / 100;
		colors3[c][2] = (float)(rand() % 100) / 100;
		colors4[d][0] = (float)(rand() % 100) / 100;
		colors4[d][1] = (float)(rand() % 100) / 100;
		colors4[d][2] = (float)(rand() % 100) / 100;
		break;
	case 'q':
		exit(EXIT_FAILURE);
		break;
	}

	InitBuffer();

	glutPostRedisplay(); //--- 배경색이 바뀔때마다 출력 콜백함수를 호출하여 화면을 refresh 한다
}

GLvoid Mouse(int button, int state, int x, int y)
{
	cout << x << y << endl;

	// 왼쪽 클릭
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		float Mx = x;
		float My = y;

		if (Number == 0)
		{
			triShape1[0][0] = ((Mx - width / 2) / (width / 2)) - 0.1;
			triShape1[0][1] = -((My - height / 2) / (height / 2)) - 0.1;
			triShape1[1][0] = ((Mx - width / 2) / (width / 2)) + 0.1;
			triShape1[1][1] = -((My - height / 2) / (height / 2)) - 0.1;
			triShape1[2][0] = ((Mx - width / 2) / (width / 2));
			triShape1[2][1] = -((My - height / 2) / (height / 2)) + 0.1;
		}
		else if (Number == 1)
		{
			triShape2[0][0] = ((Mx - width / 2) / (width / 2)) - 0.1;
			triShape2[0][1] = -((My - height / 2) / (height / 2)) - 0.1;
			triShape2[1][0] = ((Mx - width / 2) / (width / 2)) + 0.1;
			triShape2[1][1] = -((My - height / 2) / (height / 2)) - 0.1;
			triShape2[2][0] = ((Mx - width / 2) / (width / 2));
			triShape2[2][1] = -((My - height / 2) / (height / 2)) + 0.1;
		}
		else if (Number == 2)
		{
			triShape3[0][0] = ((Mx - width / 2) / (width / 2)) - 0.1;
			triShape3[0][1] = -((My - height / 2) / (height / 2)) - 0.1;
			triShape3[1][0] = ((Mx - width / 2) / (width / 2)) + 0.1;
			triShape3[1][1] = -((My - height / 2) / (height / 2)) - 0.1;
			triShape3[2][0] = ((Mx - width / 2) / (width / 2));
			triShape3[2][1] = -((My - height / 2) / (height / 2)) + 0.1;
		}
		else if (Number == 3)
		{
			triShape4[0][0] = ((Mx - width / 2) / (width / 2)) - 0.1;
			triShape4[0][1] = -((My - height / 2) / (height / 2)) - 0.1;
			triShape4[1][0] = ((Mx - width / 2) / (width / 2)) + 0.1;
			triShape4[1][1] = -((My - height / 2) / (height / 2)) - 0.1;
			triShape4[2][0] = ((Mx - width / 2) / (width / 2));
			triShape4[2][1] = -((My - height / 2) / (height / 2)) + 0.1;
		}
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		Number = (Number + 1) % 4;

	InitBuffer();

	glutPostRedisplay();
}

void Timer(int value)
{
	if (Rotate == true)
	{
		if (direction[0] == 0) // 좌
		{
			triShape1[0][0] -= 0.01;
			triShape1[1][0] -= 0.01;
			triShape1[2][0] -= 0.01;
			if (triShape1[0][0] <= -0.9)
				direction[0] = 3;
		}
		else if (direction[0] == 1) // 우
		{
			triShape1[0][0] += 0.01;
			triShape1[1][0] += 0.01;
			triShape1[2][0] += 0.01;
			if (triShape1[1][0] >= 0.9)
				direction[0] = 2;
		}
		else if (direction[0] == 2) // 상
		{
			triShape1[0][1] += 0.01;
			triShape1[1][1] += 0.01;
			triShape1[2][1] += 0.01;
			if (triShape1[2][1] >= 0.9)
				direction[0] = 0;
		}
		else // 하
		{
			triShape1[0][1] -= 0.01;
			triShape1[1][1] -= 0.01;
			triShape1[2][1] -= 0.01;
			if (triShape1[0][1] <= -0.9)
				direction[0] = 1;
		}

		if (direction[1] == 0) // 좌
		{
			triShape2[0][0] -= 0.01;
			triShape2[1][0] -= 0.01;
			triShape2[2][0] -= 0.01;
			if (triShape2[0][0] <= -0.9)
				direction[1] = 3;
		}
		else if (direction[1] == 1) // 우
		{
			triShape2[0][0] += 0.01;
			triShape2[1][0] += 0.01;
			triShape2[2][0] += 0.01;
			if (triShape2[1][0] >= 0.9)
				direction[1] = 2;
		}
		else if (direction[1] == 2) // 상
		{
			triShape2[0][1] += 0.01;
			triShape2[1][1] += 0.01;
			triShape2[2][1] += 0.01;
			if (triShape2[2][1] >= 0.9)
				direction[1] = 0;
		}
		else // 하
		{
			triShape2[0][1] -= 0.01;
			triShape2[1][1] -= 0.01;
			triShape2[2][1] -= 0.01;
			if (triShape2[0][1] <= -0.9)
				direction[1] = 1;
		}

		if (direction[2] == 0) // 좌
		{
			triShape3[0][0] -= 0.01;
			triShape3[1][0] -= 0.01;
			triShape3[2][0] -= 0.01;
			if (triShape3[0][0] < -0.9)
				direction[2] = 3;
		}
		else if (direction[2] == 1) // 우
		{
			triShape3[0][0] += 0.01;
			triShape3[1][0] += 0.01;
			triShape3[2][0] += 0.01;
			if (triShape3[1][0] > 0.9)
				direction[2] = 2;
		}
		else if (direction[2] == 2) // 상
		{
			triShape3[0][1] += 0.01;
			triShape3[1][1] += 0.01;
			triShape3[2][1] += 0.01;
			if (triShape3[2][1] > 0.9)
				direction[2] = 0;
		}
		else // 하
		{
			triShape3[0][1] -= 0.01;
			triShape3[1][1] -= 0.01;
			triShape3[2][1] -= 0.01;
			if (triShape3[0][1] < -0.9)
				direction[2] = 1;
		}

		if (direction[3] == 0) // 좌
		{
			triShape4[0][0] -= 0.01;
			triShape4[1][0] -= 0.01;
			triShape4[2][0] -= 0.01;
			if (triShape4[0][0] <= -0.9)
				direction[3] = 3;
		}
		else if (direction[3] == 1) // 우
		{
			triShape4[0][0] += 0.01;
			triShape4[1][0] += 0.01;
			triShape4[2][0] += 0.01;
			if (triShape4[1][0] >= 0.9)
				direction[3] = 2;
		}
		else if (direction[3] == 2) // 상
		{
			triShape4[0][1] += 0.01;
			triShape4[1][1] += 0.01;
			triShape4[2][1] += 0.01;
			if (triShape4[2][1] >= 0.9)
				direction[3] = 0;
		}
		else // 하
		{
			triShape4[0][1] -= 0.01;
			triShape4[1][1] -= 0.01;
			triShape4[2][1] -= 0.01;
			if (triShape4[0][1] <= -0.9)
				direction[3] = 1;
		}

	}
	InitBuffer();

	glutPostRedisplay();
	glutTimerFunc(1, Timer, 1);
}