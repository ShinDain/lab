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
	vertexsource = filetobuf("vertex_glm.glsl");

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
	fragmentsource = filetobuf("fragment_glm.glsl");

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
		{1.0f,0.0f,0.0f},
		{-1.0f,0.0f,0.0f}
	},
	{
		{0.0f,1.0f,0.0f},
		{0.0f,-1.0f,0.0f}
	},
	{
		{0.0f,0.0f,1.0f},
		{0.0f,0.0f,-1.0f}
	},
};

GLfloat LineC[2][3] = {
	{0.3f,0.5f,0.3f},
	{0.8f,0.6f,0.4f}
};

GLfloat Square[36][3] = {
	{-0.5f,0.5f,0.0f},
	{-0.5f,-0.5f,0.0f},
	{0.0f,-0.5f,0.5f},

	{-0.5f,0.5f,0.0f},
	{0.0f,-0.5f,0.5f},
	{0.0f,0.5f,0.5f},

	{0.0f,0.5f,0.5f},
	{0.0f,-0.5f,0.5f},
	{0.5f,-0.5f,0.0f},

	{0.0f,0.5f,0.5f},
	{0.5f,-0.5f,0.0f},
	{0.5f,0.5f,0.0f},

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

	{0.0f,0.5f,-0.5f},
	{-0.5f,0.5f,0.0f},
	{0.0f,0.5f,0.5f},

	{0.0f,0.5f,-0.5f},
	{0.0f,0.5f,0.5f},
	{0.5f,0.5f,0.0f},

	{-0.5f,-0.5f,0.0f},
	{0.0f,-0.5f,-0.5f},
	{0.0f,-0.5f,0.5f},

	{0.0f,-0.5f,-0.5f},
	{0.5f,-0.5f,0.0f},
	{0.0f,-0.5f,0.5f}
};
GLfloat SquareC[36][3];

//--- 변수 선언
GLuint VAO[4], VBO[2];

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
	glGenVertexArrays(1, &VAO[3]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[3]); //--- VAO를 바인드하기
	glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Square), Square, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SquareC), SquareC, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}

GLuint shaderID;

GLint width = 1000;
GLint height = 1000;

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

GLUquadricObj* qobj;

bool change = false;
bool animation = false;
int aniSphere = 0;
int aniSquare = 0;
int Allani = 0;

float TransSquare[3] = { -0.5f,0.0f,0.0f };
float TransSphere[3] = { 0.5f,0.0f,0.0f };
float RotSquare = 0.0f;
float RotSphere = 0.0f;

float Alldegree = 0.0f;

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	//--- 변경된 배경색 설정
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 바탕색을 변경
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 설정된 색으로 전체를 칠하기

	glUseProgram(shaderID);

	glEnable(GL_DEPTH_TEST);

	glm::mat4 TSphere = glm::mat4(1.0f);
	glm::mat4 TSquare = glm::mat4(1.0f);
	glm::mat4 RSphere = glm::mat4(1.0f);
	glm::mat4 RSquare = glm::mat4(1.0f);
	glm::mat4 RAll = glm::mat4(1.0f);
	glm::mat4 R = glm::mat4(1.0f);
	glm::mat4 R2 = glm::mat4(1.0f);
	glm::mat4 AR = glm::mat4(1.0f);
	glm::mat4 SSquare = glm::mat4(1.0f);
	glm::mat4 Sphere = glm::mat4(1.0f);
	glm::mat4 Square = glm::mat4(1.0f);

	TSphere = glm::translate(TSphere, glm::vec3(TransSquare[0], TransSquare[1], TransSquare[2]));
	TSquare = glm::translate(TSquare, glm::vec3(TransSphere[0], TransSphere[1], TransSphere[2]));
	SSquare = glm::scale(SSquare, glm::vec3(0.5f, 0.5f, 0.5f));
	RSphere = glm::rotate(RSphere, glm::radians(RotSphere), glm::vec3(0.0f, 1.0f, 0.0f));
	RSquare = glm::rotate(RSquare, glm::radians(RotSquare), glm::vec3(1.0f, 0.0f, 0.0f));

	R = glm::rotate(R, glm::radians(15.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	R2 = glm::rotate(R2, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	RAll = glm::rotate(RAll, glm::radians(Alldegree), glm::vec3(0.0f, 1.0f, 0.0f));

	AR = R2 * R * RAll;

	Square = AR * RAll * TSquare * RSquare * SSquare;
	Sphere = AR * RAll * TSphere * RSphere * SSquare;

	unsigned int modelLocation = glGetUniformLocation(shaderID, "transform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(AR));
	for (int i = 0; i < 3; i++)
	{
		glBindVertexArray(VAO[i]);
		glDrawArrays(GL_LINE_STRIP, 0, 2);
	}

	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GLU_LINE);
	gluQuadricNormals(qobj, GLU_SMOOTH);
	gluQuadricOrientation(qobj, GLU_OUTSIDE);
	if(change == false)
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Sphere));
	else
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Square));
	gluSphere(qobj, 0.1, 50, 50);

	if (change == false)
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Square));
	else
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Sphere));
	glBindVertexArray(VAO[3]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glutSwapBuffers(); // 화면에 출력하기1
}
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'x':
		animation = true;
		aniSquare++;
		aniSquare = aniSquare % 3;
		break;
	case 'y':
		animation = true;
		aniSphere++;
		aniSphere = aniSphere % 3;
		break;
		break;
	case 'b':
		animation = true;
		Allani++;
		Allani = Allani % 3;
		break;
	case 'c':
		if (change == false)
			change = true;
		else
			change = false;
		break;
	case 's':
		animation = false;
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
	if (animation == true)
	{
		if (aniSquare == 1)
			RotSquare += 1.0f;
		else if (aniSquare == 2)
			RotSquare -= 1.0f;
		if (aniSphere == 1)
			RotSphere += 1.0f;
		else if (aniSphere == 2)
			RotSphere -= 1.0f;
		if (Allani == 1)
			Alldegree += 1.0f;
		else if (Allani == 2)
			Alldegree -= 1.0f;
	}
	InitBuffer();
	glutPostRedisplay();
	glutTimerFunc(10, Timer, 1);
}
