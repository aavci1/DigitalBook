#include "Page.h"

#include <GL/glew.h>
#include <GL/gl.h>

class PagePrivate {
public:
    PagePrivate() : left(0), right(0) {
        glGenTextures(1, &left);
        glGenTextures(1, &right);
    }

    ~PagePrivate() {
        glDeleteTextures(1, &left);
        glDeleteTextures(1, &right);
    }

    GLuint left;
    GLuint right;
};

Page::Page() : d(new PagePrivate()) {
}

Page::~Page() {
    delete d;
}

void Page::setData(Side side, int width, int height, unsigned char *data) {
    // bind texture
    if (side == Left)
        glBindTexture(GL_TEXTURE_2D, d->left);
    else if (side == Right)
        glBindTexture(GL_TEXTURE_2D, d->right);
    // set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // set texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
    // generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Page::render(Side side, int width, int height) {
    if (side == Left) {
        // bind texture
        glBindTexture(GL_TEXTURE_2D, d->left);
        // draw quad
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f * width, -0.5f * height, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(+0.5f * width, -0.5f * height, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(+0.5f * width, +0.5f * height, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f * width, +0.5f * height, 0.0f);
        glEnd();
    } else if (side == Right) {
        // bind texture
        glBindTexture(GL_TEXTURE_2D, d->right);
        // draw quad
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f * width, -0.5f * height, 0.0f);
        glTexCoord2f(-1.0f, 1.0f); glVertex3f(+0.5f * width, -0.5f * height, 0.0f);
        glTexCoord2f(-1.0f, 0.0f); glVertex3f(+0.5f * width, +0.5f * height, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f * width, +0.5f * height, 0.0f);
        glEnd();
    }
}
