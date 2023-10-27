#ifndef WINGS_GRAPHICS_LINUX_OPENGL_C_
#define WINGS_GRAPHICS_LINUX_OPENGL_C_

#include "wings/base/types.c"
#include "wings/base/macros.c"

#include "GL/gl.h"
#include "GL/glx.h"
#include "wings/extern/glext.h"
#include <assert.h>
#include <stdio.h>
#include <malloc.h>

typedef GLXContext glXCreateContextAttribsARBProc(Display *display, GLXFBConfig fb_config, GLXContext context, Bool, const int *attribList);

void APIENTRY
debug_message_callback(GLenum        source,
                       GLenum        type,
                       GLuint        id,
                       GLenum        severity,
                       GLsizei       length,
                       const GLchar *message,
                       const void   *userParam)
{
    UNUSED(type);
    UNUSED(id);
    UNUSED(severity);
    UNUSED(length);
    UNUSED(userParam);
    printf("gl debug message: (%d) %s\n", source, message);
}

#define IF_GL_ERROR_RETURN(err_no)   \
    do                               \
    {                                \
        u32 gl_error = glGetError(); \
        if (gl_error != GL_NO_ERROR) \
            return (err_no);         \
    }                                \
    while (0)

PFNGLPUSHDEBUGGROUPPROC          glPushDebugGroup;
PFNGLPOPDEBUGGROUPPROC           glPopDebugGroup;
PFNGLDETACHSHADERPROC            glDetachShader;
PFNGLDELETEPROGRAMPROC           glDeleteProgram;
PFNGLCREATEPROGRAMPROC           glCreateProgram;
PFNGLCOMPILESHADERPROC           glCompileShader;
PFNGLCREATESHADERPROC            glCreateShader;
PFNGLDELETESHADERPROC            glDeleteShader;
PFNGLGENBUFFERSPROC              glGenBuffers;
PFNGLDELETEBUFFERSPROC           glDeleteBuffers;
PFNGLBINDBUFFERPROC              glBindBuffer;
PFNGLBINDSAMPLERPROC             glBindSampler;
PFNGLGENFRAMEBUFFERSPROC         glGenFramebuffers;
PFNGLDELETEFRAMEBUFFERSPROC      glDeleteFramebuffers;
PFNGLBINDFRAMEBUFFERPROC         glBindFramebuffer;
PFNGLBLENDEQUATIONPROC           glBlendEquation;
PFNGLBLENDFUNCSEPARATEPROC       glBlendFuncSeparate;
PFNGLBLENDEQUATIONSEPARATEPROC   glBlendEquationSeparate;
PFNGLFRAMEBUFFERTEXTURE2DPROC    glFramebufferTexture2D;
PFNGLMAPBUFFERPROC               glMapBuffer;
PFNGLUNMAPBUFFERPROC             glUnmapBuffer;
PFNGLGENVERTEXARRAYSPROC         glGenVertexArrays;
PFNGLBINDVERTEXARRAYPROC         glBindVertexArray;
PFNGLLINKPROGRAMPROC             glLinkProgram;
PFNGLUSEPROGRAMPROC              glUseProgram;
PFNGLATTACHSHADERPROC            glAttachShader;
PFNGLSHADERSOURCEPROC            glShaderSource;
PFNGLBUFFERDATAPROC              glBufferData;
PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer;
PFNGLVERTEXATTRIBIPOINTERPROC    glVertexAttribIPointer;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLGETUNIFORMLOCATIONPROC      glGetUniformLocation;
PFNGLUNIFORM1FPROC               glUniform1f;
PFNGLUNIFORM2FPROC               glUniform2f;
PFNGLUNIFORM2FVPROC              glUniform2fv;
PFNGLUNIFORM2IVPROC              glUniform2iv;
PFNGLUNIFORM3FPROC               glUniform3f;
PFNGLUNIFORM3FVPROC              glUniform3fv;
PFNGLUNIFORM4FPROC               glUniform4f;
PFNGLUNIFORM4FVPROC              glUniform4fv;
PFNGLUNIFORM1IPROC               glUniform1i;
PFNGLUNIFORM1UIPROC              glUniform1ui;
PFNGLUNIFORMMATRIX4FVPROC        glUniformMatrix4fv;
PFNGLUNIFORMMATRIX2FVPROC        glUniformMatrix2fv;
PFNGLUNIFORMMATRIX3FVPROC        glUniformMatrix3fv;
PFNGLACTIVETEXTUREPROC           glActiveTexture;
PFNGLGENERATEMIPMAPPROC          glGenerateMipmap;
PFNGLBUFFERSUBDATAPROC           glBufferSubData;
PFNGLTEXIMAGE2DMULTISAMPLEPROC   glTexImage2DMultisample;
PFNGLTEXIMAGE3DPROC              glTexImage3D;
PFNGLTEXSUBIMAGE3DPROC           glTexSubImage3D;
PFNGLBLITFRAMEBUFFERPROC         glBlitFramebuffer;
PFNGLGETSHADERIVPROC             glGetShaderiv;
PFNGLGETPROGRAMIVPROC            glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC       glGetProgramInfoLog;
PFNGLGETSHADERINFOLOGPROC        glGetShaderInfoLog;
PFNGLGETATTRIBLOCATIONPROC       glGetAttribLocation;
PFNGLGETACTIVEATTRIBPROC         glGetActiveAttrib;
PFNGLGETACTIVEUNIFORMPROC        glGetActiveUniform;
PFNGLDELETEVERTEXARRAYSPROC      glDeleteVertexArrays;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
PFNGLRENDERBUFFERSTORAGEPROC     glRenderbufferStorage;
PFNGLBINDRENDERBUFFERPROC        glBindRenderbuffer;
PFNGLGENRENDERBUFFERSPROC        glGenRenderbuffers;
PFNGLCHECKFRAMEBUFFERSTATUSPROC  glCheckFramebufferStatus;
PFNGLDRAWELEMENTSBASEVERTEXPROC  glDrawElementsBaseVertex;
PFNGLDEBUGMESSAGECALLBACKPROC    glDebugMessageCallback;
PFNGLDEBUGMESSAGECONTROLPROC     glDebugMessageControl;
PFNGLCLIPCONTROLPROC             glClipControl;

GLXContext _opengl_device_context  = 0;
b32        _graphics_context_ready = 0;

void
init_opengl(void)
{
    glPushDebugGroup          = (PFNGLPUSHDEBUGGROUPPROC)glXGetProcAddress((const GLubyte *)"glPushDebugGroup");
    glPopDebugGroup           = (PFNGLPOPDEBUGGROUPPROC)glXGetProcAddress((const GLubyte *)"glPopDebugGroup");
    glCreateProgram           = (PFNGLCREATEPROGRAMPROC)glXGetProcAddress((const GLubyte *)"glCreateProgram");
    glCompileShader           = (PFNGLCOMPILESHADERPROC)glXGetProcAddress((const GLubyte *)"glCompileShader");
    glCreateShader            = (PFNGLCREATESHADERPROC)glXGetProcAddress((const GLubyte *)"glCreateShader");
    glDeleteShader            = (PFNGLDELETESHADERPROC)glXGetProcAddress((const GLubyte *)"glDeleteShader");
    glGenBuffers              = (PFNGLGENBUFFERSPROC)glXGetProcAddress((const GLubyte *)"glGenBuffers");
    glBindBuffer              = (PFNGLBINDBUFFERPROC)glXGetProcAddress((const GLubyte *)"glBindBuffer");
    glGenFramebuffers         = (PFNGLGENFRAMEBUFFERSPROC)glXGetProcAddress((const GLubyte *)"glGenFramebuffers");
    glDeleteFramebuffers      = (PFNGLDELETEFRAMEBUFFERSPROC)glXGetProcAddress((const GLubyte *)"glDeleteFramebuffers");
    glBindFramebuffer         = (PFNGLBINDFRAMEBUFFERPROC)glXGetProcAddress((const GLubyte *)"glBindFramebuffer");
    glFramebufferTexture2D    = (PFNGLFRAMEBUFFERTEXTURE2DPROC)glXGetProcAddress((const GLubyte *)"glFramebufferTexture2D");
    glMapBuffer               = (PFNGLMAPBUFFERPROC)glXGetProcAddress((const GLubyte *)"glMapBuffer");
    glUnmapBuffer             = (PFNGLUNMAPBUFFERPROC)glXGetProcAddress((const GLubyte *)"glUnmapBuffer");
    glGenVertexArrays         = (PFNGLGENVERTEXARRAYSPROC)glXGetProcAddress((const GLubyte *)"glGenVertexArrays");
    glBindVertexArray         = (PFNGLBINDVERTEXARRAYPROC)glXGetProcAddress((const GLubyte *)"glBindVertexArray");
    glLinkProgram             = (PFNGLLINKPROGRAMPROC)glXGetProcAddress((const GLubyte *)"glLinkProgram");
    glUseProgram              = (PFNGLUSEPROGRAMPROC)glXGetProcAddress((const GLubyte *)"glUseProgram");
    glAttachShader            = (PFNGLATTACHSHADERPROC)glXGetProcAddress((const GLubyte *)"glAttachShader");
    glShaderSource            = (PFNGLSHADERSOURCEPROC)glXGetProcAddress((const GLubyte *)"glShaderSource");
    glBufferData              = (PFNGLBUFFERDATAPROC)glXGetProcAddress((const GLubyte *)"glBufferData");
    glVertexAttribPointer     = (PFNGLVERTEXATTRIBPOINTERPROC)glXGetProcAddress((const GLubyte *)"glVertexAttribPointer");
    glVertexAttribIPointer    = (PFNGLVERTEXATTRIBIPOINTERPROC)glXGetProcAddress((const GLubyte *)"glVertexAttribIPointer");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)glXGetProcAddress((const GLubyte *)"glEnableVertexAttribArray");
    glGetUniformLocation      = (PFNGLGETUNIFORMLOCATIONPROC)glXGetProcAddress((const GLubyte *)"glGetUniformLocation");
    glUniform1f               = (PFNGLUNIFORM1FPROC)glXGetProcAddress((const GLubyte *)"glUniform1f");
    glUniform2f               = (PFNGLUNIFORM2FPROC)glXGetProcAddress((const GLubyte *)"glUniform2f");
    glUniform1i               = (PFNGLUNIFORM1IPROC)glXGetProcAddress((const GLubyte *)"glUniform1i");
    glUniform1ui              = (PFNGLUNIFORM1UIPROC)glXGetProcAddress((const GLubyte *)"glUniform1ui");
    glUniformMatrix4fv        = (PFNGLUNIFORMMATRIX4FVPROC)glXGetProcAddress((const GLubyte *)"glUniformMatrix4fv");
    glUniformMatrix3fv        = (PFNGLUNIFORMMATRIX3FVPROC)glXGetProcAddress((const GLubyte *)"glUniformMatrix3fv");
    glUniformMatrix2fv        = (PFNGLUNIFORMMATRIX2FVPROC)glXGetProcAddress((const GLubyte *)"glUniformMatrix2fv");
    glActiveTexture           = (PFNGLACTIVETEXTUREPROC)glXGetProcAddress((const GLubyte *)"glActiveTexture");
    glGenerateMipmap          = (PFNGLGENERATEMIPMAPPROC)glXGetProcAddress((const GLubyte *)"glGenerateMipmap");
    glBufferSubData           = (PFNGLBUFFERSUBDATAPROC)glXGetProcAddress((const GLubyte *)"glBufferSubData");
    glTexImage2DMultisample   = (PFNGLTEXIMAGE2DMULTISAMPLEPROC)glXGetProcAddress((const GLubyte *)"glTexImage2DMultisample");
    glBlitFramebuffer         = (PFNGLBLITFRAMEBUFFERPROC)glXGetProcAddress((const GLubyte *)"glBlitFramebuffer");
    glGetShaderiv             = (PFNGLGETSHADERIVPROC)glXGetProcAddress((const GLubyte *)"glGetShaderiv");
    glGetShaderInfoLog        = (PFNGLGETSHADERINFOLOGPROC)glXGetProcAddress((const GLubyte *)"glGetShaderInfoLog");
    glGetAttribLocation       = (PFNGLGETATTRIBLOCATIONPROC)glXGetProcAddress((const GLubyte *)"glGetAttribLocation");
    glGetActiveAttrib         = (PFNGLGETACTIVEATTRIBPROC)glXGetProcAddress((const GLubyte *)"glGetActiveAttrib");
    glGetActiveUniform        = (PFNGLGETACTIVEATTRIBPROC)glXGetProcAddress((const GLubyte *)"glGetActiveUniform");
    glUniform2fv              = (PFNGLUNIFORM2FVPROC)glXGetProcAddress((const GLubyte *)"glUniform2fv");
    glUniform2iv              = (PFNGLUNIFORM2IVPROC)glXGetProcAddress((const GLubyte *)"glUniform2iv");
    glUniform3f               = (PFNGLUNIFORM3FPROC)glXGetProcAddress((const GLubyte *)"glUniform3f");
    glUniform3fv              = (PFNGLUNIFORM3FVPROC)glXGetProcAddress((const GLubyte *)"glUniform3fv");
    glUniform4f               = (PFNGLUNIFORM4FPROC)glXGetProcAddress((const GLubyte *)"glUniform4f");
    glUniform4fv              = (PFNGLUNIFORM4FVPROC)glXGetProcAddress((const GLubyte *)"glUniform4fv");
    glGetProgramiv            = (PFNGLGETPROGRAMIVPROC)glXGetProcAddress((const GLubyte *)"glGetProgramiv");
    glGetProgramInfoLog       = (PFNGLGETPROGRAMINFOLOGPROC)glXGetProcAddress((const GLubyte *)"glGetProgramInfoLog");
    glBindSampler             = (PFNGLBINDSAMPLERPROC)glXGetProcAddress((const GLubyte *)"glBindSampler");
    glTexImage3D              = (PFNGLTEXIMAGE3DPROC)glXGetProcAddress((const GLubyte *)"glTexImage3D");
    glTexSubImage3D           = (PFNGLTEXSUBIMAGE3DPROC)glXGetProcAddress((const GLubyte *)"glTexSubImage3D");
    glBlendEquation           = (PFNGLBLENDEQUATIONPROC)glXGetProcAddress((const GLubyte *)"glBlendEquation");
    glBlendEquationSeparate   = (PFNGLBLENDEQUATIONSEPARATEPROC)glXGetProcAddress((const GLubyte *)"glBlendEquationSeparate");
    glDeleteVertexArrays      = (PFNGLDELETEVERTEXARRAYSPROC)glXGetProcAddress((const GLubyte *)"glDeleteVertexArrays");
    glDeleteBuffers           = (PFNGLDELETEBUFFERSPROC)glXGetProcAddress((const GLubyte *)"glDeleteBuffers");
    glDetachShader            = (PFNGLDETACHSHADERPROC)glXGetProcAddress((const GLubyte *)"glDetachShader");
    glDeleteProgram           = (PFNGLDELETEPROGRAMPROC)glXGetProcAddress((const GLubyte *)"glDeleteProgram");
    glBlendFuncSeparate       = (PFNGLBLENDFUNCSEPARATEPROC)glXGetProcAddress((const GLubyte *)"glBlendFuncSeparate");
    // wglSwapIntervalEXT        = (wgl_swap_interval_ext *)glXGetProcAddress((const GLubyte *)"wglSwapIntervalEXT");
    glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)glXGetProcAddress((const GLubyte *)"glFramebufferRenderbuffer");
    glRenderbufferStorage     = (PFNGLRENDERBUFFERSTORAGEPROC)glXGetProcAddress((const GLubyte *)"glRenderbufferStorage");
    glBindRenderbuffer        = (PFNGLBINDRENDERBUFFERPROC)glXGetProcAddress((const GLubyte *)"glBindRenderbuffer");
    glGenRenderbuffers        = (PFNGLGENRENDERBUFFERSPROC)glXGetProcAddress((const GLubyte *)"glGenRenderbuffers");
    glCheckFramebufferStatus  = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)glXGetProcAddress((const GLubyte *)"glCheckFramebufferStatus");
    glDrawElementsBaseVertex  = (PFNGLDRAWELEMENTSBASEVERTEXPROC)glXGetProcAddress((const GLubyte *)"glDrawElementsBaseVertex");
    glDebugMessageCallback    = (PFNGLDEBUGMESSAGECALLBACKPROC)glXGetProcAddress((const GLubyte *)"glDebugMessageCallback");
    glDebugMessageControl     = (PFNGLDEBUGMESSAGECONTROLPROC)glXGetProcAddress((const GLubyte *)"glDebugMessageControl");
    glClipControl             = (PFNGLCLIPCONTROLPROC)glXGetProcAddress((const GLubyte *)"glClipControl");
}

b32
initialise_graphics_context(Display *display, Window window)
{
    s32 result = 0;

    glXCreateContextAttribsARBProc *glXCreateContextAttribsARB;
    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc *)glXGetProcAddress((const GLubyte *)"glXCreateContextAttribsARB");

    int attributes[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 3,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_DEBUG_BIT_ARB,
        0
    };

    printf("Getting matching framebuffer configs\n");

    int visual_attribs[] = {
        GLX_X_RENDERABLE,
        True,
        GLX_DRAWABLE_TYPE,
        GLX_WINDOW_BIT,
        GLX_RENDER_TYPE,
        GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE,
        GLX_TRUE_COLOR,
        GLX_RED_SIZE,
        8,
        GLX_GREEN_SIZE,
        8,
        GLX_BLUE_SIZE,
        8,
        GLX_ALPHA_SIZE,
        8,
        GLX_DEPTH_SIZE,
        24,
        GLX_STENCIL_SIZE,
        8,
        GLX_DOUBLEBUFFER,
        True,
    };

    int          fbcount;
    GLXFBConfig *fbc = glXChooseFBConfig(display, DefaultScreen(display), visual_attribs, &fbcount);
    if (!fbc)
    {
        printf("Failed to retrieve a framebuffer config\n");
        return (1);
    }
    printf("Found %d matching FB configs.\n", fbcount);

    // Pick the FB config/visual with the most samples per pixel
    printf("Getting XVisualInfos\n");
    int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

    int i;
    for (i = 0; i < fbcount; ++i)
    {
        XVisualInfo *vi = glXGetVisualFromFBConfig(display, fbc[i]);
        if (vi)
        {
            int samp_buf, samples;
            glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
            glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLES, &samples);

            printf("  Matching fbconfig %d, visual ID 0x%2x: SAMPLE_BUFFERS = %d,"
                   " SAMPLES = %d\n",
                   i, (unsigned int)vi->visualid, samp_buf, samples);

            if (best_fbc < 0 || samp_buf && samples > best_num_samp)
                best_fbc = i, best_num_samp = samples;
            if (worst_fbc < 0 || !samp_buf || samples < worst_num_samp)
                worst_fbc = i, worst_num_samp = samples;
        }
        XFree(vi);
    }

    GLXFBConfig  bestFbc = fbc[best_fbc];
    XVisualInfo *vi      = glXGetVisualFromFBConfig(display, bestFbc);
    // Be sure to free the FBConfig list allocated by glXChooseFBConfig()
    XFree(fbc);

    _opengl_device_context = glXCreateContextAttribsARB(display, bestFbc, 0, True, attributes);
    glXMakeCurrent(display, window, _opengl_device_context); // release current context

    init_opengl();
    // wglSwapIntervalEXT(0);
    glDebugMessageCallback(debug_message_callback, 0);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_FALSE);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, 0, GL_TRUE);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_MEDIUM, 0, 0, GL_TRUE);

    _graphics_context_ready = 1;
    return (0);
}

b32
_link_program(u32 program)
{
    assert(program > 0);

    glLinkProgram(program);
    IF_GL_ERROR_RETURN(1);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    IF_GL_ERROR_RETURN(2);
    if (success != GL_TRUE)
    {
        GLsizei log_length    = 0;
        char    message[1024] = { 0 };
        glGetProgramInfoLog(program, 1024, &log_length, message);
        printf("link status %d failed with(length:%d): %s\n", success, log_length, message);
        return (3);
    }
    return (0);
}

error
_compile_and_attach_shader(u32 program, const char *text, u32 shader_type, u32 *shader)
{
    *shader = glCreateShader(shader_type);
    glShaderSource(*shader, 1, &text, 0);
    IF_GL_ERROR_RETURN(1);
    glCompileShader(*shader);
    IF_GL_ERROR_RETURN(2);
    GLint compiled = 0;
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &compiled);
    if (compiled != GL_TRUE)
    {
        GLsizei log_length = 0;
        char   *message    = (char *)calloc(1024, sizeof(char));
        glGetShaderInfoLog(*shader, 1024, &log_length, message);
        printf("%s\n%.30s\n", message, text);
        return (3);
    }

    glAttachShader(program, *shader);
    IF_GL_ERROR_RETURN(4);

    glDeleteShader(*shader);
    IF_GL_ERROR_RETURN(5);

    return (0);
}

error
compile_shader_program(u32 *program, const char *vertex_shader, const char *fragment_shader)
{
    error error = 0;
    *program    = glCreateProgram();

    u32 vs = 0;
    error  = _compile_and_attach_shader(*program, vertex_shader, GL_VERTEX_SHADER, &vs);
    if (error)
    {
        return 1;
    }

    u32 fs = 0;
    error  = _compile_and_attach_shader(*program, fragment_shader, GL_FRAGMENT_SHADER, &fs);
    if (error)
        return 2;

    error = _link_program(*program);
    if (error)
        return 3;

    //@Todo: according to documentation we could detach here and get them deleted. But it doesn't seem to work on all drivers
    // glDetachShader(*program, vs);
    // glDetachShader(*program, fs);

    return (0);
}

void
print_attributes(u32 shader_program)
{
    s32 i     = 0;
    s32 count = 0;

    GLint  size = 0;
    GLenum type = 0;

    GLchar  name[256]; // variable name in GLSL
    GLsizei length; // name length

    glGetProgramiv(shader_program, GL_ACTIVE_ATTRIBUTES, &count);
    printf("Active Attributes: %d\n", count);

    for (i = 0; i < count; i++)
    {
        glGetActiveAttrib(shader_program, (GLuint)i, 256, &length, &size, &type, name);
        s32 loc = glGetAttribLocation(shader_program, name);
        printf("Attribute #%d At: %d Type: %u Name: %s\n", i, loc, type, name);
    }

    glGetProgramiv(shader_program, GL_ACTIVE_UNIFORMS, &count);
    printf("Active Uniforms: %d\n", count);

    for (i = 0; i < count; i++)
    {
        glGetActiveUniform(shader_program, (GLuint)i, 256, &length, &size, &type, name);
        printf("Uniform #%d Type: %u Name: %s\n", i, type, name);
    }
}

#endif
