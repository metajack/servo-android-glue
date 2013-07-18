/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

//BEGIN_INCLUDE(all)
#include <jni.h>
#include <errno.h>
//#include <dlfcn.h>
#include <string.h>

#include <stdlib.h>
#include <unistd.h>

#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <GLES2/gl2.h>
#include <GLES/gl.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android-dl.h>

#define LOG(prio, tag, a, args...) __android_log_print(prio, tag, "[%s::%d]"#a"",__FUNCTION__, __LINE__, ##args);
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))

typedef void (*fty_glutMainLoopEvent)();
typedef void (*fty_glutInit)(int*, char**);
typedef void (*fty_glutInitDisplayMode)(unsigned int);
typedef int (*fty_glutCreateWindow)(const char*);
typedef void (*fty_glutDestroyWindow)(int);
typedef void (*fty_glutPostRedisplay)();
typedef void (*fty_glutSwapBuffers)();
typedef int (*fty_glutGetWindow)();
typedef void (*fty_glutSetWindow)(int);
typedef void (*fty_glutReshapeWindow)(int ,int);
typedef void (*fty_glutDisplayFunc)(void (*)());
typedef void (*fty_glutReshapeFunc)(void (*)(int, int));
typedef void (*fty_glutTimerFunc)(unsigned int, void (*)(int), int);
typedef int (*fty_glutGet)(unsigned int);
typedef void (*fty_glutKeyboardFunc)(void (*)(unsigned char, int, int));
typedef void (*fty_glutMouseFunc)(void (*)(int, int, int, int));
typedef void (*fty_glutMouseWheelFunc)(void (*)(int, int, int, int));
typedef void (*fty_glutSetWindowTitle)(char const*);
typedef void (*fty_glutIdleFunc)(void(*)());
typedef void (*fty_glutInitWindowSize)(int, int);
typedef int (*fty_glutGetModifiers)();


#define REGISTER_FUNCTION(lib, function)\
    void (*reg_fn_##function)(fty_##function);\
    *(void**)(&reg_fn_##function) = dlsym(lib, "reg_fn_" #function);\
    if (function == NULL) {\
        LOGW("could not find reg_fn_" #function " from " #lib);\
        return;\
    } else {\
        LOGI("loaded reg_fn_" #function " from " #lib);\
        reg_fn_##function(function);\
        LOGI("registerd "#function);\
    }\

static void init_servo()
{
    LOGI("init_servo");

    setenv("RUST_LOG", "servo,servo-gfx,layers,js,glut", 1);
    
    char* size_stack = getenv("RUST_MIN_STACK");
    char* rust_log = getenv("RUST_LOG");

    LOGI("Stack Size is : %s", size_stack);
    LOGI("RUST_LOG flag is : %s", rust_log);
    
    LOGI("load servo library");
    void* libservo = android_dlopen("/data/data/com.example.ServoAndroid/lib/libservo-4dc624e940ae8193-0.1.so");
    if (libservo == NULL) {
    	LOGW("failed to load servo lib: %s", dlerror());
    	return;
    }

    LOGI("load rust-glut library");
    void* libglut = android_dlopen("/data/data/com.example.ServoAndroid/lib/libglut-102129e09d96658-0.1.so");
    if (libglut == NULL) {
        LOGW("failed to load rust-glut lib: %s", dlerror());
        return;
    }

    REGISTER_FUNCTION(libglut, glutMainLoopEvent);
    REGISTER_FUNCTION(libglut, glutInit);
    REGISTER_FUNCTION(libglut, glutInitDisplayMode);
    REGISTER_FUNCTION(libglut, glutCreateWindow);
    REGISTER_FUNCTION(libglut, glutDestroyWindow);
    REGISTER_FUNCTION(libglut, glutPostRedisplay);
    REGISTER_FUNCTION(libglut, glutSwapBuffers);
    REGISTER_FUNCTION(libglut, glutGetWindow);
    REGISTER_FUNCTION(libglut, glutSetWindow);
    REGISTER_FUNCTION(libglut, glutReshapeWindow);
    REGISTER_FUNCTION(libglut, glutDisplayFunc);
    REGISTER_FUNCTION(libglut, glutReshapeFunc);
    REGISTER_FUNCTION(libglut, glutTimerFunc);
    REGISTER_FUNCTION(libglut, glutGet);
    REGISTER_FUNCTION(libglut, glutKeyboardFunc);
    REGISTER_FUNCTION(libglut, glutMouseFunc);
    REGISTER_FUNCTION(libglut, glutMouseWheelFunc);
    REGISTER_FUNCTION(libglut, glutSetWindowTitle);
    REGISTER_FUNCTION(libglut, glutIdleFunc);
    REGISTER_FUNCTION(libglut, glutInitWindowSize);
    REGISTER_FUNCTION(libglut, glutGetModifiers);

    void (*amain)(void);
    *(void**)(&amain) = dlsym(libservo, "amain");
    if (amain) {
        LOGI("go into amain()");
        (*amain)();
        return;
    }
    LOGW("could not find amain() from servo");
}

const int W = 800;
const int H = 600;
GLuint program;
/**
 * Initialize an EGL context for the current display.
 */
static int init_display() {
    // initialize OpenGL ES and EGL

    int argc = 1;
    char* argv[] = {"servo"};

	LOGI("initialize GLUT START");

    glutInitWindowSize(W, H);
	LOGI("initialize 1");
//    glutInitWindowPosition(40,40);
	LOGI("initialize 2");
//    glutInit(&argc, argv);
	LOGI("initialize 3");
//    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	LOGI("initialize 4");
    
//    glutCreateWindow("Servo Android");
    
	LOGI("initialize OpenGL END");
    return 0;
}


unsigned short data[800 * 600 * 2];

void test_display() {
    static int count = 0;
    ++count;
	
    LOGI("test display");
    
    glClearColor(0.5, 0.5, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glViewport(0, 0, W, H);
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glOrthof(0, W, H, 0, -1, 1);
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    
    GLuint textures[] = {0};
    glGenTextures(1, textures);

    GLuint texture = textures[0];
    glBindTexture(GL_TEXTURE_2D, texture);
    LOGI("test display - bind texture: %d", texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    int idx = 0;
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            unsigned short r = (unsigned char)((count + idx) >> 3);
            unsigned short g = (unsigned char)(((count + 2*idx) << 1) >> 2);
            unsigned short b = (unsigned char)(((count + 3*idx) << 2) >> 3);
            data[idx++] = (r << 4) | g;
            data[idx++] = (b << 4) | 0xF;
        }
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, W, H, 0, GL_RGBA,
            GL_UNSIGNED_SHORT_4_4_4_4, data);

//    glFrontFace(GL_CW);

    LOGI("texture end");
//    glEnableClientState(GL_VERTEX_ARRAY);
//    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(program, "uSampler"), 0);
    LOGI("uSampler: %d", glGetUniformLocation(program, "uSampler"));
 
    GLuint buffers[] = {0, 0};
   glGenBuffers(2, buffers);
    
    GLuint triangle_vertex_buffer = buffers[0];
    GLfloat vertices1[12] = {0, 0, 0,  0, H, 0,  W, 0, 0,  W, H, 0};
    glBindBuffer(GL_ARRAY_BUFFER, triangle_vertex_buffer);
    glVertexAttribPointer(glGetAttribLocation(program, "aVertexPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), vertices1, GL_STATIC_DRAW);
    LOGI("aVertexPosition: %d", glGetAttribLocation(program, "aVertexPosition"));

    GLuint texture_coord_buffer = buffers[1];
    GLfloat vertices2[8] = {0, 0,  0, 1,  1, 0,  1, 1};
    glBindBuffer(GL_ARRAY_BUFFER, texture_coord_buffer);
    glVertexAttribPointer(glGetAttribLocation(program, "aTextureCoord"), 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), vertices2, GL_STATIC_DRAW);
    LOGI("aTextureCoord: %d", glGetAttribLocation(program, "aTextureCoord"));

//    glVertexPointer(3, GL_FLOAT, 0, vertices1);
//    glTexCoordPointer(2, GL_FLOAT, 0, vertices2);

//    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//    glFlush();

    glBindTexture(GL_TEXTURE_2D, 0);
    
    glutSwapBuffers();
    glutPostRedisplay();
    
//    glDisableClientState(GL_VERTEX_ARRAY);
//    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
}

static int test(int argc, char* argv[]) {
    LOGI("test");
    glutInitWindowPosition(0, 0);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutCreateWindow("Servo Android");

    int vertex_shader;
    int frag_shader;
    int err;
    int res;
    
    LOGI("create vertex shader");
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    
    LOGI("vertex shader id: %d", vertex_shader);
    const GLchar* vertex_source[] = {
        "attribute vec3 aVertexPosition;\n"
        "attribute vec2 aTextureCoord;\n"
        "uniform mat4 uMVMatrix;\n"
        "uniform mat4 uPMatrix;\n"
        "varying vec2 vTextureCoord;\n"
        "void main(void) {\n"
            "gl_Position = uPMatrix * uMVMatrix * vec4(aVertexPosition, 1.0);\n"
            "vTextureCoord = aTextureCoord;\n"
        "}\n"};

    int vertex_source_len[] = { strlen(vertex_source[0]) };
    
    LOGI("glShaderSource");
    glShaderSource(vertex_shader, 1, vertex_source, vertex_source_len);
    
    LOGI("glCompileShader");
    glCompileShader(vertex_shader);
    err = glGetError();
    LOGI("shader erro: %d", err);

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &res);
    LOGI("compile status: %d\n", res);

    
    LOGI("create fragment shader");
    frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    
    LOGI("shader id: %d", frag_shader);
    const GLchar* frag_source[] = {
        "precision mediump float;\n"
        "varying vec2 vTextureCoord;\n"
        "uniform sampler2D uSampler;\n"
        "void main(void) {\n"
            "gl_FragColor = texture2D(uSampler, vec2(vTextureCoord.s, vTextureCoord.t));\n"
        "}\n"};

    int frag_source_len[] = { strlen(frag_source[0]) };
    
    LOGI("glShaderSource");
    glShaderSource(frag_shader, 1, frag_source, frag_source_len);
    
    LOGI("glCompileShader");
    glCompileShader(frag_shader);
    err = glGetError();
    LOGI("shader erro: %d", err);

    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &res);
    LOGI("compile status: %d\n", res);
   
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, frag_shader);
    glLinkProgram(program);
    GLint result;
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if (result == 0) {
        LOGI("failed to initialize program");
    }
    glUseProgram(program);
    
   
    glutDisplayFunc(test_display);

    LOGI("glutMainLoop");
    glutMainLoop();
    return 0;
}

int main(int argc, char* argv[])
{
    init_display();
    init_servo();

    //test(argc, argv);

    return 0;
}

//END_INCLUDE(all)
