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

GLfloat Stage[30][3] = {
	// 정면부터 반시계
	{0.5f,-0.5f,0.0f},
	{0.0f,-0.5f,0.5f},
	{0.0f,0.5f,0.5f},
	
	{0.5f,0.5f,0.0f},
	{0.5f,-0.5f,0.0f},
	{0.0f,0.5f,0.5f},

	{0.0f,-0.5f,-0.5f},
	{0.5f,-0.5f,0.0f},
	{0.5f,0.5f,0.0f},

	{0.0f,0.5f,-0.5f},
	{0.0f,-0.5f,-0.5f},
	{0.5f,0.5f,0.0f},

	{-0.5f,-0.5f,0.0f},
	{0.0f,-0.5f,-0.5f},
	{0.0f,0.5f,-0.5f},

	{-0.5f,0.5f,0.0f},
	{-0.5f,-0.5f,0.0f},
	{0.0f,0.5f,-0.5f},

	//윗면
	{0.5f,0.5f,0.0f},
	{0.0f,0.5f,0.5f},
	{0.0f,0.5f,-0.5f},

	{0.0f,0.5f,0.5f},
	{-0.5f,0.5f,0.0f},
	{0.0f,0.5f,-0.5f},

	//아랫면
	{0.0f,-0.5f,-0.5f},
	{-0.5f,-0.5f,0.0f},
	{0.0f,-0.5f,0.5f},
	
	{0.0f,-0.5f,-0.5f},
	{0.0f,-0.5f,0.5f},
	{0.5f,-0.5f,0.0f}
};
GLfloat StageC[30][3] = {
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

GLfloat StageF[6][3] = {
	{-0.5f,0.5f,0.0f},
	{-0.5f,-0.5f,0.0f},
	{0.0f,-0.5f,0.5f},
	
	{-0.5f,0.5f,0.0f},
	{0.0f,-0.5f,0.5f},
	{0.0f,0.5f,0.5f}
};
GLfloat StageFC[6][3] = {
	{1.0f,0.0f,0.0f},
	{0.0f,1.0f,0.0f},
	{0.0f,0.0f,1.0f},

	{1.0f,0.0f,0.0f},
	{0.0f,0.0f,1.0f},
	{0.6f,0.1f,0.4f}
};

GLfloat Head[36][3] = {
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

GLfloat L_eye[36][3] = {
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
GLfloat R_eye[36][3] = {
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
GLfloat Nose[36][3] = {
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

GLfloat Body[36][3] = {
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

GLfloat Arm_left[36][3] = {
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

GLfloat Arm_right[36][3] = {
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

GLfloat Leg_left[36][3] = {
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

GLfloat Leg_right[36][3] = {
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

GLfloat Red[36][3] = {
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
GLfloat Cyan[36][3] = {
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
GLfloat Yellow[36][3] = {
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

//--- 변수 선언
GLuint VAO[11], VBO[2];

void InitBuffer()
{
	glGenVertexArrays(1, &VAO[0]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[0]); //--- VAO를 바인드하기
	glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Stage), Stage, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(StageC), StageC, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &VAO[1]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[1]); //--- VAO를 바인드하기
	glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Head), Head, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Red), Red, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &VAO[2]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[2]); //--- VAO를 바인드하기
	glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Body), Body, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Yellow), Yellow, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &VAO[3]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[3]); //--- VAO를 바인드하기
	glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Arm_left), Arm_left, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Cyan), Cyan, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &VAO[4]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[4]); //--- VAO를 바인드하기
	glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Arm_right), Arm_right, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Yellow), Yellow, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &VAO[5]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[5]); //--- VAO를 바인드하기
	glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Leg_left), Leg_left, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Cyan), Cyan, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &VAO[6]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[6]); //--- VAO를 바인드하기
	glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Leg_right), Leg_right, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Yellow), Yellow, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &VAO[7]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[7]); //--- VAO를 바인드하기
	glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(StageF), StageF, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(StageFC), StageFC, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &VAO[8]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[8]); //--- VAO를 바인드하기
	glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(L_eye), L_eye, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Cyan), Cyan, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &VAO[9]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[9]); //--- VAO를 바인드하기
	glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(R_eye), R_eye, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Cyan), Cyan, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &VAO[10]); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(VAO[10]); //--- VAO를 바인드하기
	glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Nose), Nose, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Cyan), Cyan, GL_STATIC_DRAW);
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

float timecount = 0;
float Head_Pos[3] = {0.0f,-0.3f,0.0f};

float Cam_x = 0.0f;
float Cam_y = 0.0f;
float Cam_z = 5.0f;

float lookPos[3] = { 0.0f,0.0f,0.0f };

float Angle = 0;
float Angle2 = 0;

float Body_R_count = 0.0f;
float Arm_R_count = 0.0f;

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	//--- 변경된 배경색 설정
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 바탕색을 변경
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 설정된 색으로 전체를 칠하기

	glUseProgram(shaderID);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	int modelLoc = glGetUniformLocation(shaderID, "Transform"); //--- 버텍스 세이더에서 모델링 변환 행렬 변수값을 받아온다.
	int viewLoc = glGetUniformLocation(shaderID, "viewTransform"); //--- 버텍스 세이더에서 뷰잉 변환 행렬 변수값을 받아온다.
	int projLoc = glGetUniformLocation(shaderID, "projectionTransform"); //--- 버텍스 세이더에서 투영 변환 행렬 변수값을 받아온다.
	//--- 모델링 변환
	glm::mat4 mTransform = glm::mat4(1.0f);
	mTransform = glm::rotate(mTransform, glm::radians(0.0f),glm::vec3(0.0f, 1.0f, 0.0f));
	//--- 뷰잉 변환
	glm::vec3 cameraPos = glm::vec3(Cam_x, Cam_y, Cam_z);
	glm::vec3 cameraDirection = glm::vec3(lookPos[0], lookPos[1], lookPos[2]);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 vTransform = glm::mat4(1.0f);
	glm::mat4 vTrans = glm::mat4(1.0f);
	glm::mat4 vrotate = glm::mat4(1.0f);
	glm::mat4 vTR = glm::mat4(1.0f);
	vTrans = glm::translate(vTrans, glm::vec3(-Cam_x, -Cam_y, -Cam_z));
	vTR = glm::translate(vTR, glm::vec3(Cam_x, Cam_y, Cam_z));
	vrotate = glm::rotate(vrotate, glm::radians(Angle2), glm::vec3(0.0f, 1.0f, 0.0f));
	vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	vTransform = vTransform * vTR * vrotate * vTrans;
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(vTransform));
	//--- 투영 변환 행렬을 설정한 후, 버텍스 세이더에 저장한다.
	glm::mat4 pTransform = glm::mat4(1.0f);
	glm::mat4 pTransform1 = glm::mat4(1.0f);
	pTransform = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.1f, 50.0f);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pTransform));

	glm::mat4 StageS = glm::mat4(1.0f);
	glm::mat4 StageFT = glm::mat4(1.0f);
	
	glm::mat4 Basic = glm::mat4(1.0f);
	glm::mat4 B_Tran = glm::mat4(1.0f);
	glm::mat4 B_TranR = glm::mat4(1.0f);
	glm::mat4 B_TranRR = glm::mat4(1.0f);
	glm::mat4 B_Rotate = glm::mat4(1.0f);
	glm::mat4 B_Scale = glm::mat4(1.0f);
	
	glm::mat4 Body_R = glm::mat4(1.0f);

	glm::mat4 Head_T = glm::mat4(1.0f);
	glm::mat4 L_eye_T = glm::mat4(1.0f);
	glm::mat4 R_eye_T = glm::mat4(1.0f);
	glm::mat4 Nose_T = glm::mat4(1.0f);
	glm::mat4 Head_TR = glm::mat4(1.0f);
	glm::mat4 L_eye_TR = glm::mat4(1.0f);
	glm::mat4 R_eye_TR = glm::mat4(1.0f);
	glm::mat4 Nose_TR = glm::mat4(1.0f);
	glm::mat4 eye_S = glm::mat4(1.0f);

	glm::mat4 Body_T = glm::mat4(1.0f);

	glm::mat4 L_Arm_T = glm::mat4(1.0f);
	glm::mat4 R_Arm_T = glm::mat4(1.0f);
	glm::mat4 L_Leg_T = glm::mat4(1.0f);
	glm::mat4 R_Leg_T = glm::mat4(1.0f);
	glm::mat4 Arm_S = glm::mat4(1.0f);

	glm::mat4 Arm_Rotate_cw = glm::mat4(1.0f);
	glm::mat4 Arm_Rotate_ccw = glm::mat4(1.0f);

	Arm_Rotate_cw = glm::rotate(Arm_Rotate_cw, glm::radians(Arm_R_count), glm::vec3(1.0f, 0.0f, 0.0));
	Arm_Rotate_ccw = glm::rotate(Arm_Rotate_ccw, glm::radians(-Arm_R_count), glm::vec3(1.0f, 0.0f, 0.0));

	B_Tran = glm::translate(B_Tran, glm::vec3(0.0f,-1.0f,-0.5f));
	B_TranR = glm::translate(B_TranR, glm::vec3(0.0f, -0.5f, 0.0f));
	B_TranRR = glm::translate(B_TranRR, glm::vec3(0.0f, 0.5f, 0.0f));
	B_Rotate = glm::rotate(B_Rotate,glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0));
	B_Scale = glm::scale(B_Scale, glm::vec3(0.5f, 0.3f, 0.5f));

	Basic = B_Scale * B_Tran ;		// 전체 배치

	Body_R = glm::rotate(Body_R, glm::radians(Body_R_count), glm::vec3(0.0f, 1.0f, 0.0));

	Head_T = glm::translate(Head_T, glm::vec3(Head_Pos[0], Head_Pos[1], Head_Pos[2]));
	L_eye_T = glm::translate(L_eye_T, glm::vec3(Head_Pos[0] - 0.1f, Head_Pos[1] + 0.1f, Head_Pos[2] + 0.2f));
	R_eye_T = glm::translate(R_eye_T, glm::vec3(Head_Pos[0] + 0.1f, Head_Pos[1] + 0.1f, Head_Pos[2] + 0.2f));
	Nose_T = glm::translate(Nose_T, glm::vec3(Head_Pos[0], Head_Pos[1] - 0.1f, Head_Pos[2] + 0.2f));
	Head_TR = glm::translate(Head_TR, glm::vec3(-Head_Pos[0], -Head_Pos[1], -Head_Pos[2]));
	eye_S = glm::scale(eye_S, glm::vec3(0.15f, 0.05f, 0.15f));

	Body_T = glm::translate(Body_T, glm::vec3(Head_Pos[0], Head_Pos[1] - 0.3f, Head_Pos[2]));

	L_Arm_T = glm::translate(L_Arm_T, glm::vec3(Head_Pos[0] - 0.2f, Head_Pos[1]- 0.25f, Head_Pos[2]));
	R_Arm_T = glm::translate(R_Arm_T, glm::vec3(Head_Pos[0] + 0.2f, Head_Pos[1]- 0.25f, Head_Pos[2]));
	L_Leg_T = glm::translate(L_Leg_T, glm::vec3(Head_Pos[0] - 0.1f, Head_Pos[1]- 0.5f, Head_Pos[2]));
	R_Leg_T = glm::translate(R_Leg_T, glm::vec3(Head_Pos[0] + 0.1f, Head_Pos[1]- 0.5f, Head_Pos[2]));
	Arm_S = glm::scale(Arm_S, glm::vec3(0.2f, 1.0f, 0.2f));

	StageS = glm::scale(StageS, glm::vec3(5.0f, 2.0f, 5.0f));
	StageFT = glm::translate(StageFT, glm::vec3(0.0f, timecount, 0.0f));

	// 무대
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(B_Rotate * StageS));
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 30);
	// 무대 앞면
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(B_Rotate * StageS * StageFT));
	glBindVertexArray(VAO[7]);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// 머리와 부속품
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Head_T * B_Scale * Body_R * B_Rotate));
	glBindVertexArray(VAO[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Head_T * Body_R * Head_TR * L_eye_T * B_Scale * eye_S * B_Rotate));
	glBindVertexArray(VAO[8]);								 
	glDrawArrays(GL_TRIANGLES, 0, 36);						 
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Head_T * Body_R * Head_TR * R_eye_T * B_Scale * eye_S * B_Rotate));
	glBindVertexArray(VAO[9]);								
	glDrawArrays(GL_TRIANGLES, 0, 36);						
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Head_T * Body_R * Head_TR * Nose_T * B_Scale * eye_S * B_Rotate));
	glBindVertexArray(VAO[10]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// 몸통
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Body_T * B_Scale * Body_R * B_Rotate));
	glBindVertexArray(VAO[2]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// 팔과 다리
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Head_T * Body_R * Head_TR * L_Arm_T * B_TranRR* Arm_Rotate_cw* B_TranR * B_Scale * Arm_S* B_Rotate));
	glBindVertexArray(VAO[3]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Head_T* Body_R* Head_TR* R_Arm_T* B_TranRR* Arm_Rotate_ccw* B_TranR* B_Scale* Arm_S* B_Rotate));
	glBindVertexArray(VAO[4]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Head_T* Body_R* Head_TR* L_Leg_T* B_TranRR* Arm_Rotate_ccw* B_TranR* B_Scale* Arm_S* B_Rotate));
	glBindVertexArray(VAO[5]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Head_T* Body_R* Head_TR* R_Leg_T* B_TranRR* Arm_Rotate_cw* B_TranR* B_Scale* Arm_S* B_Rotate ));
	glBindVertexArray(VAO[6]);												  		     		 
	glDrawArrays(GL_TRIANGLES, 0, 36);															 

	glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}

bool openF = false;

bool Move_b = false;
int Move = 0;
bool jump = false;

bool arm_R_b = false;

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'o':
		if (openF == true)
			openF = false;
		else
			openF = true;
		break;

	case 'w':
		Move_b = true;
		Move = 0;
		break;
	case 's':
		Move_b = true;
		Move = 1;
		break;
	case 'a':
		Move_b = true;
		Move = 2;
		break;
	case 'd':
		Move_b = true;
		Move = 3;
		break;

	case 'j':
		if(jump == false)
			jump = true;
		break;
	case 'i':
		timecount = 0;
		Head_Pos[0] = 0.0f;
		Head_Pos[1] = 0.5f;
		Head_Pos[2] = 0.0f;

		Cam_x = 0.0f;
		Cam_y = 0.0f;
		Cam_z = 5.0f;

		lookPos[0] = 0.0f;
		lookPos[1] = 0.0f;
		lookPos[2] = 0.0f;

		Angle = 0;
		Angle2 = 0;

		openF = false;

		Move_b = false;
		Move = 0;
		jump = false;
		break;

	case 'z':
		Cam_z -= 0.2f;
		break;
	case 'Z':
		Cam_z += 0.2f;
		break;
	case 'y':
		Angle2 += 0.2f;
		break;
	case 'Y':
		Angle2 -= 0.2f;
		break;
	case 'r':
		Angle += 0.2f;
		Cam_x = glm::cos(Angle) * 5;
		Cam_z = glm::sin(Angle) * 5;
		break;
	case 'R':
		Angle -= 0.2f;
		Cam_x = glm::cos(Angle) * 5;
		Cam_z = glm::sin(Angle) * 5;
		break;
	case 'm':
		Cam_y += 0.1f;
		break;
	case 'n':
		Cam_y -= 0.1f;
		break;
	case 'q':
		exit(EXIT_FAILURE);
		break;
	}
	glutPostRedisplay(); //--- 배경색이 바뀔때마다 출력 콜백함수를 호출하여 화면을 refresh 한다
}

void Timer(int value)
{
	if (openF == true)
	{
		if (timecount < 10)
			timecount += 0.01f;
	}
	else
	{
		if (timecount >= 0)
			timecount -= 0.01f;
	}

	if (Move_b == true)
	{
		if (Move == 0)
		{
			if(Body_R_count < 180.0f)
				Body_R_count += 5.0f;
			Head_Pos[2] -= 0.05f;
			if (Head_Pos[2] < -2.0f)
				Move = 1;
		}
		else if (Move == 1)
		{
			if (Body_R_count > 0.0f)
				Body_R_count -= 5.0f;
			Head_Pos[2] += 0.05f;
			if (Head_Pos[2] > 2.0f)
				Move = 0;
		}
		else if (Move == 2)
		{
			if (Body_R_count > -90.0f)
				Body_R_count -= 5.0f;
			Head_Pos[0] -= 0.05f;
			if (Head_Pos[0] < -2.0f)
				Move = 3;
		}
		else if (Move == 3)
		{
			if (Body_R_count < 90.0f)
				Body_R_count += 5.0f;
			Head_Pos[0] += 0.05f;
			if (Head_Pos[0] > 2.0f)
				Move = 2;
		}
		if (arm_R_b == false)
		{
			Arm_R_count += 2.0f;
			if (Arm_R_count >= 30.0f)
				arm_R_b = true;
		}
		else
		{
			Arm_R_count -= 2.0f;
			if (Arm_R_count <= -30.0f)
				arm_R_b = false;
		}
	}

	if (jump == true)
	{
		Head_Pos[1] += 0.05f;
		if (Head_Pos[1] > 0.5f)
			jump = false;
	}
	else if (jump == false)
	{
		if (Head_Pos[1] > -0.3f)
			Head_Pos[1] -= 0.05f;
	}

	InitBuffer();
	glutPostRedisplay();
	glutTimerFunc(30, Timer, 1);
}
