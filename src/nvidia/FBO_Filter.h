// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Giacomo Spigler
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 */

#ifdef WIN32
#include <GL/glew.h>
#define GLEW_STATIC 1
#else
#include <GL/glew.h>
#endif

#include <assert.h>

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <Cg/cgGL.h>



class FBO_Filter {
protected:
    void checkProfile(const char *name, CGprofile profile, 
                      CGprofile highlight = CG_PROFILE_UNKNOWN) {
        CGbool supported = cgGLIsProfileSupported(profile);
        bool show = (highlight==CG_PROFILE_UNKNOWN)||(highlight==profile);
        if (show) {
            printf("Cg profile %s is%s supported.\n", name,
                   supported?"":" not");
        }
    } 

    void checkProfiles(CGprofile highlight = CG_PROFILE_UNKNOWN) {
        checkProfile("vp20", CG_PROFILE_VP20, highlight);
        checkProfile("fp20", CG_PROFILE_FP20, highlight);
        checkProfile("vp30", CG_PROFILE_VP30, highlight);
        checkProfile("fp30", CG_PROFILE_FP30, highlight);
        checkProfile("vp40", CG_PROFILE_VP40, highlight);
        checkProfile("fp40", CG_PROFILE_FP40, highlight);
        checkProfile("arbvp1", CG_PROFILE_ARBVP1, highlight);
        checkProfile("arbfp1", CG_PROFILE_ARBFP1, highlight); 
    }

    void checkCg(const char *act) {
        CGerror error;
        const char *str = cgGetLastErrorString(&error);
        
        if (error!=CG_NO_ERROR) {
            printf("nvidia device - %s: %s\n", act, str);
            if (error == CG_COMPILER_ERROR) {
                printf("  %s\n", cgGetLastListing(cgContext));
            }
            exit(1);
        } else {
            printf("nvidia device - %s: OK\n", act);
        }
    }



    CGprogram cgProgram;
    CGprofile cgProfile;
    CGcontext cgContext;

    GLuint fb, oTex;
    int tWidth; int tHeight;
    int previousViewportDims[4];
    float depth;

    CGprogram load_cgprogram(CGprofile prof, char *name);

    void renderBegin();
    //return the rendering state to what it was before.
    void renderEnd();

    void drawQuadTex();

    void drawQuadFBT();

public :
    CGprogram getProgram() { return cgProgram; }

    FBO_Filter(CGprofile cgp,      ///< Desired profile.  Typically 
                                    ///  CG_PROFILE_FP30 or CG_PROFILE_FP40
               char *name,         ///< filename of the Cg program.  Note that
                                    /// the entry point function should be named
                                    /// "FragmentProgram" in the Cg 
               GLuint outputTex,   ///< the open GL texture object 
                                    /// to which the results will go 
               int W,             ///< width of the output texture, 
               int H);

    ///TODO figure out readback formats from target texture information.
    GLuint apply(GLuint iTex,    ///< OpenGL texture object to use as input
                 bool FBOtex,     ///< set to true if the input texture is the result of a previous 
                                  ///  FBO_filter.  False if not.
                 GLenum rb_fmt = GL_RGBA, 
                 GLenum rb_type = GL_FLOAT);
};



CGprogram FBO_Filter::load_cgprogram(CGprofile prof, char *name) {
    return cgCreateProgramFromFile(cgContext, CG_SOURCE,                                name, prof, "FragmentProgram", 0);
}

void FBO_Filter::renderBegin() {
  //since we might be operating on downsampled images, we need 
  //to reshape our current drawing area. Record the previous
  // settings first though. 
  glGetIntegerv(GL_VIEWPORT, previousViewportDims);

  glViewport(0, 0, (GLsizei) tWidth, (GLsizei) tHeight);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  float scale = 1.0;
  glFrustum(0.0, 1.0/scale,  0.0/scale, 1.0/scale,   1.0/scale,   100.0);
  gluLookAt(0.0,0.0,0.0,  0.0, 0.0,  depth,   0.0,   1.0, 0.0);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
}

//return the rendering state to what it was before.
void FBO_Filter::renderEnd() {
    glViewport(previousViewportDims[0], previousViewportDims[1],                   previousViewportDims[2], previousViewportDims[3]);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void FBO_Filter::drawQuadTex() {
    glBegin(GL_QUADS);

    glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, (float)tHeight);
    glVertex3f(0.0, 0.0, depth);

    glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, 0);
    glVertex3f(0.0, 1.0, depth);

    glMultiTexCoord2fARB(GL_TEXTURE0_ARB, (float)tWidth , 0);
    glVertex3f(1.0, 1.0, depth);

    glMultiTexCoord2fARB(GL_TEXTURE0_ARB, (float)tWidth , (float)tHeight);
    glVertex3f(1.0, 0.0, depth);

    glEnd();
}

void FBO_Filter::drawQuadFBT() {
    glBegin(GL_QUADS);

    glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, 0);
    glVertex3f(0.0, 0.0, depth);

    glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, (float)tHeight);
    glVertex3f(0.0, 1.0, depth);

    glMultiTexCoord2fARB(GL_TEXTURE0_ARB, (float)tWidth , (float)tHeight);
    glVertex3f(1.0, 1.0, depth);

    glMultiTexCoord2fARB(GL_TEXTURE0_ARB, (float)tWidth , 0);
    glVertex3f(1.0, 0.0, depth);

    glEnd();
}

FBO_Filter::FBO_Filter(CGprofile cgp, char *name, GLuint outputTex, int W, int H) {
    if (!cgGLIsProfileSupported(cgp)) {
        cgp = cgGLGetLatestProfile(CG_GL_FRAGMENT);
        if (cgp==CG_PROFILE_UNKNOWN) {
            checkProfiles();
            printf("Cannot get a \"fragment\" CG profile for this hardware\n");
        } else {
            checkProfiles(cgp);
        }
    }

    checkCg("initialize");
    depth = -1.0;
    //Load the Cg Program.
    cgProfile = cgp;
    cgContext = cgCreateContext();
    checkCg("create context");

    if(name!=NULL) {
        cgProgram = load_cgprogram(cgProfile, name);
        checkCg("preload");
        cgGLLoadProgram(cgProgram);
        checkCg("load");
    } else {
        cgProgram = 0;
    }

    tWidth = W; tHeight = H;


    if (glewGetExtension("GL_EXT_framebuffer_object") != GL_TRUE) {
        printf("GL_EXT_framebuffer_object extension needed but not supported\n");
        exit(1);
    } 

    //create the framebuffer object.
    oTex = outputTex ;
    printf("at %s %d\n", __FILE__, __LINE__);
    glGenFramebuffersEXT(1, &fb);
    printf("at %s %d\n", __FILE__, __LINE__);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
    printf("at %s %d\n", __FILE__, __LINE__);
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, oTex);
    printf("at %s %d\n", __FILE__, __LINE__);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,               GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_NV, oTex, 0);
    printf("at %s %d\n", __FILE__, __LINE__);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    printf("at %s %d\n", __FILE__, __LINE__);
}

GLuint FBO_Filter::apply(GLuint iTex, bool FBOtex, GLenum rb_fmt, GLenum rb_type) {
    GLint activeARB;
    glGetIntegerv(GL_CLIENT_ACTIVE_TEXTURE_ARB, &activeARB);
    glActiveTextureARB(GL_TEXTURE0_ARB);

    //XXX all this frambuffer binding is suboptimal, but will work for now.
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, iTex);
    glEnable(GL_TEXTURE_RECTANGLE_NV);

    renderBegin();
    cgGLEnableProfile(cgProfile);
    checkCg("enable profile");
    cgGLBindProgram(cgProgram);
    checkCg("bind program");
    (FBOtex ? drawQuadFBT() : drawQuadTex());
    checkCg("draw quad");
    cgGLDisableProfile(cgProfile);
    checkCg("disable profile");
    renderEnd();

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    glActiveTextureARB(activeARB);
    return oTex;
}


