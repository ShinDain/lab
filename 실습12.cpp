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

GLfloat Pyramid[18][3] = {
	{0.0f,0.5f,0.0f},
	{-0.5f,-0.5f,0.0f},
	{0.0f,-0.5f,0.5f},

	{0.0f,0.5f,0.0f},
	{0.0f,-0.5f,0.5f},
	{0.5f,-0.5f,0.0f},

	{0.0f,0.5f,0.0f},
	{0.5f,-0.5f,0.0f},
	{0.0f,-0.5f,-0.5f},

	{0.0f,0.5f,0.0f},
	{0.0f,-0.5f,-0.5f},
	{-0.5f,-0.5f,0.0f},

	{-0.5f,-0.5f,0.0f},
	{0.5f,-0.5f,0.0f},
	{0.0f,-0.5f,0.5f},

	{-0.5f,-0.5f,0.0f},
	{0.0f,-0.5f,-0.5f},
	{0.5f,-0.5f,0.0f}
};
GLfloat PyramidC[18][3] = {
	{1.0f,0.0f,0.0f},
	{0.0f,1.0f,0.0f},
	{0.0f,0.0f,1.0f},
	{0.5f,0.5f,0.0f},
	{0.0f,0.5f,0.5f},
	{1.0f,0.0f,0.0f},
	{0.0f,1.0f,0.0f},
	{0.0f,0.0f,1.0f},
	{0.5f,0.5f,0.0f},
	{0.0f,0.5f,0.5f},
	{1.0f,0.0f,0.0f},
	{0.0f,1.0f,0.0f},
	{0.0f,0.0f,1.0f},
	{0.5f,0.5f,0.0f},
	{0.0f,0.5f,0.5f},
	{1.0f,0.0f,0.0f},
	{0.0f,1.0f,0.0f},
	{0.0f,0.0f,1.0f},
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

	//윗면
	{0.0f,0.5f,-0.5f},
	{0.0f,0.5f,0.5f},
	{0.5f,0.5f,0.0f},

	{0.0f,0.5f,-0.5f},
	{-0.5f,0.5f,0.0f},
	{0.0f,0.5f,0.5f},

	//아랫면
	{0.0f,-0.5f,-0.5f},
	{-0.5f,-0.5f,0.0f},
	{0.0f,-0.5f,0.5f},

	{0.0f,-0.5f,-0.5f},
	{0.0f,-0.5f,0.5f},
	{0.5f,-0.5f,0.0f}
};
GLfloat SquareC[36][3] = {
	{1.0f,0.0f,0.0f},
	{0.0f,1.0f,0.0f},
	{0.0f,0.0f,1.0f},

	{1.0f,0.0f,0.0f},
	{0.0f,0.0f,1.0f},
	{0.6f,0.1f,0.4f},

	{0.6f,0.1f,0.4f},
	{0.0f,0.0f,1.0f},
	{0.6f,0.2f,0.8f},

	{0.6f,0.1f,0.4f},
	{0.6f,0.2f,0.8f},
	{0.6f,0.1f,0.4f},

	{0.6f,0.1f,0.4f},
	{0.6f,0.2f,0.8f},
	{0.5f,0.0f,0.5f},

	{0.6f,0.1f,0.4f},
	{0.5f,0.0f,0.5f},
	{0.0f,0.5f,0.5f},

	{0.0f,0.5f,0.5f},
	{0.0f,1.0f,0.0f},
	{0.5f,0.0f,0.5f},

	{0.0f,0.5f,0.5f},
	{0.5f,0.0f,0.5f},
	{1.0f,0.0f,0.0f},

	{0.0f,0.5f,0.5f},
	{1.0f,0.0f,0.0f},
	{0.6f,0.1f,0.4f},

	{0.0f,0.5f,0.5f},
	{0.6f,0.1f,0.4f},
	{0.6f,0.1f,0.4f},

	{0.5f,0.0f,0.5f},
	{0.6f,0.2f,0.8f},
	{0.0f,1.0f,0.0f},

	{0.0f,1.0f,0.0f},
	{0.6f,0.2f,0.8f},
	{0.0f,0.0f,1.0f},
};

//--- 변수 선언
GLuint VAO[5], VBO[2];

void InitBuffer()
{
	glGenVertexArrays(1, &VAO[0]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[0]); //--- VAO를 바인드하기
	glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Pyramid), Pyramid, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(PyramidC), PyramidC, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &VAO[1]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[1]); //--- VAO를 바인드하기
	glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Square), Square, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SquareC), SquareC, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	for (int i = 2; i < 5; i++)
	{
		glGenVertexArrays(1, &VAO[i]); //--- VAO 를 지정하고 할당하기
		glBindVertexArray(VAO[i]); //--- VAO를 바인드하기
		glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(DrawLine[i - 2]), DrawLine[i - 2], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(LineC), LineC, GL_STATIC_DRAW);
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
float Timecount1 = 0.0f;

bool Wire = false;
bool drawP = false;
bool RotateSelfy = false;
bool RotateSelfx = false;
bool depth = true;


GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	//--- 변경된 배경색 설정
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 바탕색을 변경
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 설정된 색으로 전체를 칠하기

	glUseProgram(shaderID);
	if (depth == true)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	float xAngle = 30.0f;
	float yAngle = -30.0f;

	int modelLoc = glGetUniformLocation(shaderID, "Transform"); //--- 버텍스 세이더에서 모델링 변환 행렬 변수값을 받아온다.
	int viewLoc = glGetUniformLocation(shaderID, "viewTransform"); //--- 버텍스 세이더에서 뷰잉 변환 행렬 변수값을 받아온다.
	int projLoc = glGetUniformLocation(shaderID, "projectionTransform"); //--- 버텍스 세이더에서 투영 변환 행렬 변수값을 받아온다.
	//--- 모델링 변환
	glm::mat4 mTransform = glm::mat4(1.0f);
	glm::mat4 mTransform1 = glm::mat4(1.0f);
	glm::mat4 Rtime = glm::mat4(1.0f);
	glm::mat4 Rtime1 = glm::mat4(1.0f);
	mTransform = glm::rotate(mTransform, glm::radians(yAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	mTransform1 = glm::rotate(mTransform1, glm::radians(xAngle), glm::vec3(1.0f, 0.0f, 0.0f));
	Rtime = glm::rotate(Rtime, glm::radians(Timecount), glm::vec3(0.0f, 1.0f, 0.0f));
	Rtime1 = glm::rotate(Rtime1, glm::radians(Timecount1), glm::vec3(1.0f, 0.0f, 0.0f));
	mTransform = mTransform1 * mTransform;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform));
	//--- 뷰잉 변환
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 vTransform = glm::mat4(1.0f);
	vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(vTransform));
	//--- 투영 변환 행렬을 설정한 후, 버텍스 세이더에 저장한다.
	glm::mat4 pTransform = glm::mat4(1.0f);
	pTransform = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.1f, 50.0f);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pTransform));

	for (int i = 2; i < 5; i++)
	{
		glBindVertexArray(VAO[i]);
		glDrawArrays(GL_LINE_STRIP, 0, 2);
	}

	if (drawP == true)
	{
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * Rtime * Rtime1));
		if (Wire == false)
		{
			glBindVertexArray(VAO[0]);
			glDrawArrays(GL_TRIANGLES, 0, 18);
		}
		else
		{
			glBindVertexArray(VAO[0]);
			glDrawArrays(GL_LINE_LOOP, 0, 18);
		}

	}
	else
	{
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * Rtime * Rtime1));
		
		if (Wire == false)
		{
			glBindVertexArray(VAO[1]);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		else
		{
			glBindVertexArray(VAO[1]);
			glDrawArrays(GL_LINE_LOOP, 0, 36);
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
	case 'c':
		if (drawP == true)
			drawP = false;
		else
			drawP = true;
		break;
	case 'x':
		if (RotateSelfx == false)
			RotateSelfx = true;
		else
			RotateSelfx = false;
		break;
	case 'y':
		if (RotateSelfy == false)
			RotateSelfy = true;
		else
			RotateSelfy = false;
		break;
	case 'h':
		if (depth == true)
			depth = false;
		else
			depth = true;
		break;
	case 'w':
		if (Wire == false)
			Wire = true;
		else
			Wire = false;
		break;
	case 's':
		Timecount = 0.0f;
		Timecount1 = 0.0f;

		drawP = false;
		RotateSelfy = false;
		RotateSelfx = false;
		depth = true;
		break;
	case 'q':
		exit(EXIT_FAILURE);
		break;
	}
	glutPostRedisplay(); //--- 배경색이 바뀔때마다 출력 콜백함수를 호출하여 화면을 refresh 한다
}

void Timer(int value)
{
	if (RotateSelfy == true)
		Timecount++;
	if (RotateSelfx == true)
		Timecount1++;
	InitBuffer();
	glutPostRedisplay();
	glutTimerFunc(1, Timer, 1);
}
