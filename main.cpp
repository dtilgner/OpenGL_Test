/******************************************************************************\
| OpenGL 4 Example Code: Texture as a 2D drawing buffer.                       |
| Based originally on "Anton's OpenGL 4 Tutorials"                             |
| EKP 2016                                                                     |
|******************************************************************************/

#include "maths_funcs.h"
#include "maths_funcs.cpp"
#include "gl_utils.h"
#include "gl_utils.cpp"
#include <GL/glew.h> 
#include <GLFW/glfw3.h> 
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define _USE_MATH_DEFINES
#include <math.h>

#define GL_LOG_FILE "gl.log"

int g_gl_width = 512;
int g_gl_height = 512;

GLFWwindow* g_window = NULL;

// --- File reading partial example --- :
GLuint loadSIF_custom(const char * imagepath){
	FILE *ptr_myfile = fopen(imagepath, "rb");
	unsigned char header[6];
	fread(header, 1, 6, ptr_myfile);
	unsigned int dataPos = *(int*)&(header[0x06]);
	unsigned int width = *(int*)&(header[0x02]);
	unsigned int height = *(int*)&(header[0x04]);
	unsigned char * data;

	unsigned int imageSize = (g_gl_width * g_gl_height * 3);
	data = new unsigned char [imageSize];
	fread(data, 1, imageSize, ptr_myfile);
	fclose(ptr_myfile);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_gl_width, g_gl_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	delete[] data;
	return textureID;
}


/* function example with malloc to reserve buffer, ptrs to access buffer bytes */
GLubyte * drawBuffer (int x, int y) {
	int bpp = 3 * sizeof(GLubyte);
	GLubyte *buffer =(unsigned char*) malloc(x * y * bpp);

	for (int n=0; n<x*y*3; n+=3)
	{
	    *(buffer+n) = 0;
	    *(buffer+n+1) = n%255;
	    *(buffer+n+2) = 0;
	    *(buffer+n+3) = 255;
        }  

	return buffer;
}


int main () {
	assert (restart_gl_log ());  // Keeping a log file for useful debug messages, etc. 
	assert (start_gl ());

	/* Two triangles and texture coordinates to make a textured square. */
        GLfloat points[] = {
                -1.0f, -1.0f,  0.0f,
                 1.0f, -1.0f,  0.0f,
                 1.0f,  1.0f,  0.0f,
                 1.0f,  1.0f,  0.0f,
                -1.0f,  1.0f,  0.0f,
                -1.0f, -1.0f,  0.0f
        };

        GLfloat texcoords[] = {
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f
        };

	GLuint points_vbo;
	glGenBuffers (1, &points_vbo);
	glBindBuffer (GL_ARRAY_BUFFER, points_vbo);
	glBufferData (GL_ARRAY_BUFFER, sizeof (points), points, GL_STATIC_DRAW);
	
	GLuint texcoords_vbo;
	glGenBuffers (1, &texcoords_vbo);
	glBindBuffer (GL_ARRAY_BUFFER, texcoords_vbo);
	glBufferData (GL_ARRAY_BUFFER, sizeof (texcoords), texcoords, GL_STATIC_DRAW);
	
	GLuint vao;
	glGenVertexArrays (1, &vao);
	glBindVertexArray (vao);
	glBindBuffer (GL_ARRAY_BUFFER, points_vbo);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer (GL_ARRAY_BUFFER, texcoords_vbo);
	glVertexAttribPointer (1, 2, GL_FLOAT, GL_TRUE, 0, NULL); // normalise!
	glEnableVertexAttribArray (0);
	glEnableVertexAttribArray (1);

	GLuint Texture = loadSIF_custom("ybrr.sif");

	GLuint shader_program = create_programme_from_files (
		"test_vs.glsl", "test_fs.glsl");
	
	GLuint TextureID = glGetUniformLocation(shader_program, "myTextureSampler");
	#define ONE_DEG_IN_RAD (2.0 * M_PI) / 360.0 // 0.017444444

	GLfloat proj_mat[] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, -1.0f,
		0.0f, 0.0f, 1.0f, 0.0f
	};
	
		
	float cam_pos[] = {0.0f, 0.0f, 1.0f};   // look down Z-axis 
	float cam_yaw = 0.0f; 			// y-rotation in degrees
	mat4 T = translate (identity_mat4 (), vec3 (-cam_pos[0], -cam_pos[1], -cam_pos[2]));
	mat4 R = rotate_y_deg (identity_mat4 (), -cam_yaw);
	mat4 view_mat = R * T; 

	int view_mat_location = glGetUniformLocation (shader_program, "view");
	glUseProgram (shader_program);
	glUniformMatrix4fv (view_mat_location, 1, GL_FALSE, view_mat.m);
	int proj_mat_location = glGetUniformLocation (shader_program, "proj");
	glUseProgram (shader_program);
	glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, proj_mat);
	
	// load buffer as texture 
	GLuint tex;
        GLubyte *bufferBytes;
	bufferBytes = drawBuffer(g_gl_width, g_gl_height);  // Call earlier function to get image data
	glGenTextures (1, &Texture);
	glActiveTexture (GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_2D, Texture);
	glUniform1i(TextureID, 0);
	glTexImage2D (
		GL_TEXTURE_2D,
		0,
		GL_RGB,
		g_gl_width,
		g_gl_height,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
	        bufferBytes	
	);
	glGenerateMipmap (GL_TEXTURE_2D);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	GLfloat max_aniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);
	
	
	while (!glfwWindowShouldClose (g_window)) {
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport (0, 0, g_gl_width, g_gl_height);
		
		glUseProgram (shader_program);
		glBindVertexArray (vao);
		glDrawArrays (GL_TRIANGLES, 0, 6);

		glfwPollEvents ();
		
		// control keys
		if (glfwGetKey (g_window, GLFW_KEY_C)) {
			printf("C-key pressed.\n");
		}
		
		if (GLFW_PRESS == glfwGetKey (g_window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose (g_window, 1);
		}

		// put the stuff we've been drawing onto the display
		glfwSwapBuffers (g_window);
	}
	
	// close GL context and any other GLFW resources
	glfwTerminate();
	return 0;
}
