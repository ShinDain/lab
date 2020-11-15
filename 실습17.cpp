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

GLfloat Square1[36][3] = {
	// 정면부터 반시계
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
GLfloat Square1C[36][3] = {
	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},

	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},

	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},
	
	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},

	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},
	
	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},
	
	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},
	
	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},
	
	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},

	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},

	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},

	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},
	{1.0f,0.0f,0.0f},
};

GLfloat Square2[36][3] = {
	// 정면부터 반시계
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
GLfloat Square2C[36][3] = {
	{1.0f,1.0f,0.0f},
	{1.0f,1.0f,0.0f},
	{1.0f,1.0f,0.0f},
	
	{1.0f,1.0f,0.0f},
	{1.0f,1.0f,0.0f},
	{1.0f,1.0f,0.0f},

	{1.0f,1.0f,0.0f},
	{1.0f,1.0f,0.0f},
	{1.0f,1.0f,0.0f},
	
	{1.0f,1.0f,0.0f},
	{1.0f,1.0f,0.0f},
	{1.0f,1.0f,0.0f},

	{1.0f,1.0f,0.0f},
	{1.0f,1.0f,0.0f},
	{1.0f,1.0f,0.0f},
	
	{1.0f,1.0f,0.0f},
	{1.0f,1.0f,0.0f},
	{1.0f,1.0f,0.0f},

	{1.0f,1.0f,0.0f},
	{1.0f,1.0f,0.0f},
	{1.0f,1.0f,0.0f},

	{1.0f,1.0f,0.0f},
	{1.0f,1.0f,0.0f},
	{1.0f,1.0f,0.0f},

	{1.0f,1.0f,0.0f},
	{1.0f,1.0f,0.0f},
	{1.0f,1.0f,0.0f},

	{1.0f,1.0f,0.0f},
	{1.0f,1.0f,0.0f},
	{1.0f,1.0f,0.0f},

	{1.0f,1.0f,0.0f},
	{1.0f,1.0f,0.0f},
	{1.0f,1.0f,0.0f},

	{1.0f,1.0f,0.0f},
	{1.0f,1.0f,0.0f},
	{1.0f,1.0f,0.0f},
};

GLfloat Square3_1[36][3] = {
	// 정면부터 반시계
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
GLfloat Square3_1C[36][3] = {
	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},

	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},

	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},

	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},

	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},

	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},

	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},

	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},

	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},

	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},

	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},

	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},
};
GLfloat Square3_2[36][3] = {
	// 정면부터 반시계
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
GLfloat Square3_2C[36][3] = {
	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},

	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},

	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},

	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},

	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},

	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},

	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},

	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},

	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},

	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},

	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},

	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},
	{0.0f,1.0f,1.0f},
};

//--- 변수 선언
GLuint VAO[5], VBO[2];

void InitBuffer()
{
	glGenVertexArrays(1, &VAO[0]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[0]); //--- VAO를 바인드하기
	glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Square1), Square1, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Square1C), Square1C, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	
	glGenVertexArrays(1, &VAO[1]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[1]); //--- VAO를 바인드하기
	glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Square2), Square2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Square2C), Square2C, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &VAO[2]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[2]); //--- VAO를 바인드하기
	glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Square3_1), Square3_1, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Square3_1C), Square3_1C, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &VAO[3]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[3]); //--- VAO를 바인드하기
	glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Square3_2), Square3_2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Square3_2C), Square3_2C, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &VAO[4]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[4]); //--- VAO를 바인드하기
	glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Stage), Stage, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(StageC), StageC, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	
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

float CP[3] = {0.0f , 3.0f,5.0f };
float look_pos[3] = { 0.0f,0.0f ,0.0f };

float S1_pos[3] = { 0.0f, 0.2f, 0.0f };
float S2_Angle = 45.0f;
float S3_Angle = 0.0f;

float vAngle = 0;

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	//CP[0] = glm::cos(angle) * 10;
	//CP[2] = glm::sin(angle) * 10;

	//--- 변경된 배경색 설정
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 바탕색을 변경
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

	SS = glm::scale(SS, glm::vec3(3.0f, 1.0f, 3.0f));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * S1T * S1R * S1S));
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * S2T * S2R * S2S));
	glBindVertexArray(VAO[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform  * S3_1T * S3_1R  * S3_1S * S3R_2));
	glBindVertexArray(VAO[2]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * S3_2T * S3_2R  * S3_2S * S3R_2));
	glBindVertexArray(VAO[3]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mTransform * SS));
	glBindVertexArray(VAO[4]);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}

int S2_Move = 0;
int S3Move = 0;

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'b':
		S1_pos[0] += 0.1f;
		break;
	case 'B':
		S1_pos[0] -= 0.1f;
		break;

	case 'm':
		S2_Move = 1;
		break;
	case 'M':
		S2_Move = 2;
		break;

	case 't':
		S3Move = 1;
		break;
	case 'T':
		S3Move = 2;
		break;

	case 'z':
		CP[2] += 0.1f;
		break;
	case 'Z':
		CP[2] -= 0.1f;
		break;
	case 'r':
		vAngle += 1.0f;
		break;
	case 'R':
		vAngle -= 1.0f;
		break;
	case 'x':
		CP[0] += 0.1f;
		break;
	case 'X':
		CP[0] -= 0.1f;
		break;
	case 's':
		S2_Move = 0;
		S3Move = 0;
		break;
	case 'c':
		CP[0] = 0.0f;
		CP[1] = 3.0f;
		CP[2] = 5.0f;

		look_pos[0] =0.0f;
		look_pos[1] =0.0f;
		look_pos[2] =0.0f;

		S1_pos[0] = 0.0f;
		S1_pos[1] = 0.2f;
		S1_pos[2] = 0.0f;
		S2_Angle = 45.0f; 
		S3_Angle = 0.0f;

		vAngle = 0;
		S2_Move = 0;
		S3Move = 0;
		break;
	case 'q':
		exit(EXIT_FAILURE);
		break;
	}
	glutPostRedisplay(); //--- 배경색이 바뀔때마다 출력 콜백함수를 호출하여 화면을 refresh 한다
}

void Timer(int value)
{
	if (S2_Move != 0)
	{
		if (S2_Move == 1)
		{
			S2_Angle += 1.0f;
			if (S2_Angle > 90)
				S2_Move = 2;
		}
		if (S2_Move == 2)
		{
			S2_Angle -= 1.0f;
			if (S2_Angle < -90)
				S2_Move = 1;
		}
	}

	if (S3Move != 0)
	{
		if (S3Move == 1)
		{
			S3_Angle += 1.0f;
			if (S3_Angle > 60)
				S3Move = 2;
		}
		if (S3Move == 2)
		{
			S3_Angle -= 1.0f;
			if (S3_Angle < -60)
				S3Move = 1;
		}
	}


	InitBuffer();
	glutPostRedisplay();
	glutTimerFunc(1, Timer, 1);
}
