#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#define GLM_SWIZZLE
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

GLuint vertex_array_object; // VAO句柄
GLuint vertex_buffer_object; // VBO句柄
GLuint texture_buffer_object1; // 纹理对象句柄1
GLuint texture_buffer_object2; // 纹理对象句柄2
GLuint lightCubeVAO;//光源顶点VAO句柄

//指定摄像机
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// 指定时间参数
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// 指定光照
//glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
//绕任意轴的旋转
glm::mat4 RotateArbitraryLine(glm::vec3 v1, glm::vec3 v2, float theta)
{
	glm::mat4 rmatrix;
	float a = v1.x;
	float b = v1.y;
	float c = v1.z;

	glm::vec3 p1 = v2 - v1;
	glm::vec3 p = glm::normalize(p1);

	float u = p.x;
	float v = p.y;
	float w = p.z;

	float uu = u * u;
	float uv = u * v;
	float uw = u * w;
	float vv = v * v;
	float vw = v * w;
	float ww = w * w;
	float au = a * u;
	float av = a * v;
	float aw = a * w;
	float bu = b * u;
	float bv = b * v;
	float bw = b * w;
	float cu = c * u;
	float cv = c * v;
	float cw = c * w;

	float costheta = glm::cos(theta);
	float sintheta = glm::sin(theta);

	rmatrix[0][0] = uu + (vv + ww) * costheta;
	rmatrix[0][1] = uv * (1 - costheta) + w * sintheta;
	rmatrix[0][2] = uw * (1 - costheta) - v * sintheta;
	rmatrix[0][3] = 0;

	rmatrix[1][0] = uv * (1 - costheta) - w * sintheta;
	rmatrix[1][1] = vv + (uu + ww) * costheta;
	rmatrix[1][2] = vw * (1 - costheta) + u * sintheta;
	rmatrix[1][3] = 0;

	rmatrix[2][0] = uw * (1 - costheta) + v * sintheta;
	rmatrix[2][1] = vw * (1 - costheta) - u * sintheta;
	rmatrix[2][2] = ww + (uu + vv) * costheta;
	rmatrix[2][3] = 0;

	rmatrix[3][0] = (a * (vv + ww) - u * (bv + cw)) * (1 - costheta) + (bw - cv) * sintheta;
	rmatrix[3][1] = (b * (uu + ww) - v * (au + cw)) * (1 - costheta) + (cu - aw) * sintheta;
	rmatrix[3][2] = (c * (uu + vv) - w * (au + bv)) * (1 - costheta) + (av - bu) * sintheta;
	rmatrix[3][3] = 1;

	return rmatrix;
}



void initial(void)
{
	// 定义图形对象的顶点数据和纹理坐标
	float vertices[] = {
		//位置                    // 法线               // 纹理
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};



	// 生成并绑定VAO和VBO
	glGenVertexArrays(1, &vertex_array_object);
	glBindVertexArray(vertex_array_object);

	glGenBuffers(1, &vertex_buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);

	// 将顶点数据绑定至当前默认的缓冲中
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 设置顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//指定光源顶点数据
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	int width, height, nrchannels;//纹理长宽，通道	
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("res/Earth.png", &width, &height, &nrchannels, 0);
	if (data)
	{


		glGenTextures(1, &texture_buffer_object1);
		glBindTexture(GL_TEXTURE_2D, texture_buffer_object1);
		//指定纹理的参数
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		//生成Mipmap纹理
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load Sun texture" << std::endl;
	}
	stbi_image_free(data);
	//加载纹理图片wall.jpg或者htmdb.png
	data = stbi_load("res/Earth.png", &width, &height, &nrchannels, 0);

	if (data)
	{

		glGenTextures(1, &texture_buffer_object2);
		glBindTexture(GL_TEXTURE_2D, texture_buffer_object2);
		//指定纹理的参数
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		//生成Mipmap纹理
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load Earth texture" << std::endl;
	}

	stbi_image_free(data);//释放资源

	glEnable(GL_DEPTH_TEST);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}


void reshaper(GLFWwindow* window, int width, int height)
{
	// 指定当前视口尺寸(前两个参数为左下角位置，后两个参数是渲染窗口宽、高)
	glViewport(0, 0, width, height);

}

//鼠标滚轮回调函数
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//对摄像机进行操作
	camera.ProcessMouseScroll((float)yoffset*10);
}

// 鼠标移动回调函数
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos; // y坐标系进行反转

	lastX = (float)xpos;
	lastY = (float)ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

int main()
{
	glfwInit(); // 初始化GLFW

	// OpenGL版本为3.3，主次版本号均设为3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// 使用核心模式(无需向后兼容性)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// 创建窗口(宽、高、窗口名称)
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Base Lighting", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to Create OpenGL Context" << std::endl;
		glfwTerminate();
		return -1;
	}

	// 将窗口的上下文设置为当前线程的主上下文
	glfwMakeContextCurrent(window);

	// 初始化GLAD，加载OpenGL函数指针地址的函数
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	initial();

	//指定窗口大小改变时调用的函数reshaper
	glfwSetFramebufferSizeCallback(window, reshaper);

	//窗口键盘事件调用函数key_callback
	//glfwSetKeyCallback(window, key_callback);

	//鼠标回调函数
	glfwSetCursorPosCallback(window, mouse_callback);
	//当鼠标滚轮滚动时调用函数scroll_callback
	glfwSetScrollCallback(window, scroll_callback);

	//让GLFW捕捉鼠标事件
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//指定顶点和片段着色器
	Shader lightingShader = Shader("shader/colors.vs", "shader/colors.fs");
	Shader lightCubeShader = Shader("shader/light_cube.vs", "shader/light_cube.fs");

	while (!glfwWindowShouldClose(window))
	{
		//处理时间
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// 输入处理
		processInput(window);

		// 清空颜色缓冲
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		//定义箱子的位置
		glm::vec3 cubePositions[] = {
			//glm::vec3(-1.0f,  0.0f,  0.0f),
			glm::vec3(0.0f,  0.0f,  -11.0f),
			glm::vec3(0.0f,  1.0f,  -11.0f),

		};
		//定义点光源的位置
		glm::vec3 pointLightPositions[] = {
			glm::vec3(0.0f,  0.0f,  -10.0f),
			//glm::vec3(2.3f, -3.3f, -4.0f),
			//glm::vec3(-4.0f,  2.0f, -12.0f),
			//glm::vec3(0.0f,  0.0f, -3.0f)
		};

		// 激活光照
		lightingShader.use();
		lightingShader.setInt("material.diffuse", 1);
		lightingShader.setInt("material.specular", 1);
		lightingShader.setVec3("viewPos", camera.Position);
		lightingShader.setFloat("material.shininess", 32.0f);

		// point light 1
		lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[0].constant", 1.0f);
		lightingShader.setFloat("pointLights[0].linear", 0.09);
		lightingShader.setFloat("pointLights[0].quadratic", 0.032);
		// point light 2
		lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[1].constant", 1.0f);
		lightingShader.setFloat("pointLights[1].linear", 0.09);
		lightingShader.setFloat("pointLights[1].quadratic", 0.032);
		// point light 3
		lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[2].constant", 1.0f);
		lightingShader.setFloat("pointLights[2].linear", 0.09);
		lightingShader.setFloat("pointLights[2].quadratic", 0.032);
		// point light 4
		lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[3].constant", 1.0f);
		lightingShader.setFloat("pointLights[3].linear", 0.09);
		lightingShader.setFloat("pointLights[3].quadratic", 0.032);
		// spotLight
		lightingShader.setVec3("spotLight.position", camera.Position);
		lightingShader.setVec3("spotLight.direction", camera.Front);
		lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("spotLight.constant", 1.0f);
		lightingShader.setFloat("spotLight.linear", 0.09);
		lightingShader.setFloat("spotLight.quadratic", 0.032);
		lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));


		// 指定变换
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(6.0f);
		lightingShader.setMat4("projection", projection);//定义投影变换矩阵
		lightingShader.setMat4("view", view);		          //定义视图变换矩阵
		lightingShader.setMat4("model", model);            //定义模型变换矩阵

		//绑定漫反射贴图
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_buffer_object1);
		//绑定镜面反射贴图
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture_buffer_object2);



		glBindVertexArray(vertex_array_object);
		//地球旋转
			model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[0]);
			glm::mat4 TransMatrix = RotateArbitraryLine(
				glm::vec3(0.0f, 3.0f, -10.0f),
				glm::vec3(0.0f, -3.0f, -10.0f),
				lastFrame);
			model = model*TransMatrix;
			//model = glm::scale(model, glm::vec3(0.5f)); // 缩放
			lightingShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			//和地球一起转
			model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[1]);
			model = model * TransMatrix ;//和地球同行的相对位置
			glm::vec4 earthpos(cubePositions[0][0], cubePositions[0][1], cubePositions[0][2], 1);
			earthpos = earthpos * TransMatrix;//求得地球现在的位置
			earthpos = glm::vec4(earthpos[0] / earthpos[3], earthpos[1] / earthpos[3], earthpos[2] / earthpos[3], 1);
			glm::mat4 TransMatrix2 = RotateArbitraryLine(
				glm::vec3(earthpos[0], earthpos[1] - 1, earthpos[2] + 3),
				glm::vec3(earthpos[0], earthpos[1] - 1, earthpos[2] - 3),
				lastFrame);
			model = model *TransMatrix2;
			model= glm::scale(model, glm::vec3(0.5f));
			lightingShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);

		

		// also draw the lamp object(s)
		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);

		// we now draw as many light bulbs as we have point lights.
		glBindVertexArray(lightCubeVAO);
		for (unsigned int i = 0; i < 1; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(1.0f)); // Make it a smaller cube
			lightCubeShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}


		glBindVertexArray(0);                                                      // 解除绑定


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// 解绑VAO和VBO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// 删除VAO和VBO
	glDeleteVertexArrays(1, &vertex_array_object);
	glDeleteBuffers(1, &vertex_buffer_object);

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}





