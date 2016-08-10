#ifndef OPENGLFUNCS_HPP
#define OPENGLFUNCS_HPP

#include <GL/glew.h>
#include <QString>
#include <QFile>

void getBuffer(int size, GLenum format, GLuint bo, void* data);
void createTextureBuffer(int size, GLenum format, GLuint *tex, GLuint *bo, void *data);
void createUniformBuffer(int size, GLuint *bo, void* data);
QString readShaderBasedOnArchitecture(QString filePath);

#endif // OPENGLFUNCS_HPP

