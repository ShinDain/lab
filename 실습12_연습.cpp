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
void SpecialKeyboard(int key, int x, int y);
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

GLfloat DrawLine[2][2][3] =
{
	{
		{-1.0f,0.0f,0.0f},
		{1.0f,0.0f,0.0f}
	},
	{
		{0.0f,1.0f,0.0f},
		{0.0f,-1.0f,0.0f}
	},
};

GLfloat LineC[2][3] =
{
	{0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f}
};

//--- 변수 선언
GLuint VAO[2], VBO[2];

void InitBuffer()
{
	for (int i = 0; i < 2; i++)
	{
		glGenVertexArrays(1, &VAO[i]); //--- VAO 를 지정하고 할당하기
		glBindVertexArray(VAO[i]); //--- VAO를 바인드하기
		glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), DrawLine[i], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), LineC, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
	}
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
	glutSpecialFunc(SpecialKeyboard);
	glutTimerFunc(10, Timer, 1);

	glutMainLoop(); // 이벤트 처리 시작
}

GLUquadric* gobj;

bool pyramid = false;
bool Erase = true;
bool Line = true;

float dx = 0.0f;
float dy = 0.0f;
float dz = 0.0f;

float Rx = -45.0f;
float Ry = 0.0f;
float Rz = 0.0f;

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	//--- 변경된 배경색 설정
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 바탕색을 변경
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 설정된 색으로 전체를 칠하기

	if(Erase == true)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	glUseProgram(shaderID);

	for (int i = 0; i < 2; i++)
	{
		glBindVertexArray(VAO[i]);
		glDrawArrays(GL_LINE_LOOP, 0, 2);
	}

	glm::mat4 T = glm::mat4(1.0f);
	glm::mat4 R = glm::mat4(1.0f);
	glm::mat4 TR = glm::mat4(1.0f);

	T = glm::translate(T, glm::vec3(dx, dy, dz));
	R = glm::rotate(R, glm::radians(Rx), glm::vec3(1.0f, 0.0f, 0.0f));

	TR = T * R;

	unsigned int modelLocation = glGetUniformLocation(shaderID, "transform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

	gobj = gluNewQuadric();
	if (Line == true)
	{
		gluQuadricDrawStyle(gobj, GLU_LINE);
	}
	else
	{
		gluQuadricDrawStyle(gobj, GLU_FILL);
	}
	gluQuadricNormals(gobj, GLU_SMOOTH);
	gluQuadricOrientation(gobj, GLU_OUTSIDE);

	if (pyramid == true)
		gluCylinder(gobj, 0.3, 0.0, 0.6, 4, 1); 
	
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
		pyramid = false;
		break;
	case 'p':
		pyramid = true;
		break;
	case 'h':
		if (Erase == false)
			Erase = true;
		else
			Erase = false;
		break;
	case 'w':
		Line = true;
		break;
	case 'W':
		Line = false;
		break;
	case 'x':
		break;
	case 'X':
		break;
	case 'y':
		break;
	case 'Y':
		break;
	case 's':
		break;
	case 'q':
		exit(EXIT_FAILURE);
		break;
	}
	glutPostRedisplay(); //--- 배경색이 바뀔때마다 출력 콜백함수를 호출하여 화면을 refresh 한다
}

void SpecialKeyboard(int key, int x, int y)
{
	if (key == GLUT_KEY_LEFT)
	{

	}
	else if (key == GLUT_KEY_RIGHT)
	{

	}
	else if (key == GLUT_KEY_UP)
	{

	}
	else if (key == GLUT_KEY_DOWN)
	{

	}
}

void Timer(int value)
{


	//InitBuffer();
	glutPostRedisplay();
	glutTimerFunc(1, Timer, 1);
}
