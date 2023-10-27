#ifndef WINGS_GRAPHICS_WINDOWS_OPENGL_C_
#define WINGS_GRAPHICS_WINDOWS_OPENGL_C_

#include "wings/base/types.c"
#include "wings/base/macros.c"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <wingdi.h>
#include "GL/gl.h"
#include "wings/extern/glext.h"
#include <assert.h>
#include <stdio.h>
#include <malloc.h>

typedef HGLRC WINAPI wgl_create_context_attribs_arb(HDC hDC, HGLRC hShareContext,
                                                    const int *attribList);
typedef BOOL WINAPI  wgl_swap_interval_ext(int interval);

#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_FLAGS_ARB 0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_DEBUG_BIT_ARB 0x00000001

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
wgl_swap_interval_ext           *wglSwapIntervalEXT;

HDC _opengl_device_context  = 0;
b32 _graphics_context_ready = 0;

void
init_opengl(void)
{
    glPushDebugGroup          = (PFNGLPUSHDEBUGGROUPPROC)wglGetProcAddress("glPushDebugGroup");
    glPopDebugGroup           = (PFNGLPOPDEBUGGROUPPROC)wglGetProcAddress("glPopDebugGroup");
    glCreateProgram           = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
    glCompileShader           = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
    glCreateShader            = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
    glDeleteShader            = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
    glGenBuffers              = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
    glBindBuffer              = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
    glGenFramebuffers         = (PFNGLGENFRAMEBUFFERSPROC)wglGetProcAddress("glGenFramebuffers");
    glDeleteFramebuffers      = (PFNGLDELETEFRAMEBUFFERSPROC)wglGetProcAddress("glDeleteFramebuffers");
    glBindFramebuffer         = (PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebuffer");
    glFramebufferTexture2D    = (PFNGLFRAMEBUFFERTEXTURE2DPROC)wglGetProcAddress("glFramebufferTexture2D");
    glMapBuffer               = (PFNGLMAPBUFFERPROC)wglGetProcAddress("glMapBuffer");
    glUnmapBuffer             = (PFNGLUNMAPBUFFERPROC)wglGetProcAddress("glUnmapBuffer");
    glGenVertexArrays         = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
    glBindVertexArray         = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
    glLinkProgram             = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
    glUseProgram              = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
    glAttachShader            = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
    glShaderSource            = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
    glBufferData              = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
    glVertexAttribPointer     = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
    glVertexAttribIPointer    = (PFNGLVERTEXATTRIBIPOINTERPROC)wglGetProcAddress("glVertexAttribIPointer");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
    glGetUniformLocation      = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
    glUniform1f               = (PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f");
    glUniform2f               = (PFNGLUNIFORM2FPROC)wglGetProcAddress("glUniform2f");
    glUniform1i               = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
    glUniform1ui              = (PFNGLUNIFORM1UIPROC)wglGetProcAddress("glUniform1ui");
    glUniformMatrix4fv        = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
    glUniformMatrix3fv        = (PFNGLUNIFORMMATRIX3FVPROC)wglGetProcAddress("glUniformMatrix3fv");
    glUniformMatrix2fv        = (PFNGLUNIFORMMATRIX2FVPROC)wglGetProcAddress("glUniformMatrix2fv");
    glActiveTexture           = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
    glGenerateMipmap          = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
    glBufferSubData           = (PFNGLBUFFERSUBDATAPROC)wglGetProcAddress("glBufferSubData");
    glTexImage2DMultisample   = (PFNGLTEXIMAGE2DMULTISAMPLEPROC)wglGetProcAddress("glTexImage2DMultisample");
    glBlitFramebuffer         = (PFNGLBLITFRAMEBUFFERPROC)wglGetProcAddress("glBlitFramebuffer");
    glGetShaderiv             = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
    glGetShaderInfoLog        = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
    glGetAttribLocation       = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");
    glGetActiveAttrib         = (PFNGLGETACTIVEATTRIBPROC)wglGetProcAddress("glGetActiveAttrib");
    glGetActiveUniform        = (PFNGLGETACTIVEATTRIBPROC)wglGetProcAddress("glGetActiveUniform");
    glUniform2fv              = (PFNGLUNIFORM2FVPROC)wglGetProcAddress("glUniform2fv");
    glUniform2iv              = (PFNGLUNIFORM2IVPROC)wglGetProcAddress("glUniform2iv");
    glUniform3f               = (PFNGLUNIFORM3FPROC)wglGetProcAddress("glUniform3f");
    glUniform3fv              = (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
    glUniform4f               = (PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f");
    glUniform4fv              = (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");
    glGetProgramiv            = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
    glGetProgramInfoLog       = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
    glBindSampler             = (PFNGLBINDSAMPLERPROC)wglGetProcAddress("glBindSampler");
    glTexImage3D              = (PFNGLTEXIMAGE3DPROC)wglGetProcAddress("glTexImage3D");
    glTexSubImage3D           = (PFNGLTEXSUBIMAGE3DPROC)wglGetProcAddress("glTexSubImage3D");
    glBlendEquation           = (PFNGLBLENDEQUATIONPROC)wglGetProcAddress("glBlendEquation");
    glBlendEquationSeparate   = (PFNGLBLENDEQUATIONSEPARATEPROC)wglGetProcAddress("glBlendEquationSeparate");
    glDeleteVertexArrays      = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
    glDeleteBuffers           = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
    glDetachShader            = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
    glDeleteProgram           = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
    glBlendFuncSeparate       = (PFNGLBLENDFUNCSEPARATEPROC)wglGetProcAddress("glBlendFuncSeparate");
    wglSwapIntervalEXT        = (wgl_swap_interval_ext *)wglGetProcAddress("wglSwapIntervalEXT");
    glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)wglGetProcAddress("glFramebufferRenderbuffer");
    glRenderbufferStorage     = (PFNGLRENDERBUFFERSTORAGEPROC)wglGetProcAddress("glRenderbufferStorage");
    glBindRenderbuffer        = (PFNGLBINDRENDERBUFFERPROC)wglGetProcAddress("glBindRenderbuffer");
    glGenRenderbuffers        = (PFNGLGENRENDERBUFFERSPROC)wglGetProcAddress("glGenRenderbuffers");
    glCheckFramebufferStatus  = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)wglGetProcAddress("glCheckFramebufferStatus");
    glDrawElementsBaseVertex  = (PFNGLDRAWELEMENTSBASEVERTEXPROC)wglGetProcAddress("glDrawElementsBaseVertex");
    glDebugMessageCallback    = (PFNGLDEBUGMESSAGECALLBACKPROC)wglGetProcAddress("glDebugMessageCallback");
    glDebugMessageControl     = (PFNGLDEBUGMESSAGECONTROLPROC)wglGetProcAddress("glDebugMessageControl");
    glClipControl             = (PFNGLCLIPCONTROLPROC)wglGetProcAddress("glClipControl");
}

b32
initialise_graphics_context(HDC device_context)
{
    _opengl_device_context   = device_context;
    s32   result             = 0;
    HGLRC tmp_opengl_context = wglCreateContext(_opengl_device_context);
    result                   = wglMakeCurrent(_opengl_device_context, tmp_opengl_context);
    if (result == 0)
        return 1;

    wgl_create_context_attribs_arb *wglCreateContextAttribsARB;
    wglCreateContextAttribsARB = (wgl_create_context_attribs_arb *)wglGetProcAddress("wglCreateContextAttribsARB");

    int attributes[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
        0
    };

    HGLRC opengl_context;
    opengl_context = wglCreateContextAttribsARB(_opengl_device_context, 0, attributes);
    result         = wglMakeCurrent(0, 0); // release current context
    if (result == 0)
        return 2;
    wglDeleteContext(tmp_opengl_context);
    result = wglMakeCurrent(device_context, opengl_context);
    if (result == 0)
        return 3;

    init_opengl();
    wglSwapIntervalEXT(0);
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
