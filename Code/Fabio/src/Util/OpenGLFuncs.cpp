
#include "OpenGLFuncs.hpp"
#include <QDebug>
#include <QVector>

void getBuffer(int size, GLenum format, GLuint bo, void* data)
{
    glBindBuffer(format, bo);
    glGetBufferSubData(format, 0, size*sizeof(format), data);
    glBindBuffer(format, 0);

    GLenum err = glGetError();
    if( err > 0 ){
        QString strError;
        strError.sprintf("%s", glewGetErrorString(err));
        qDebug() << "getBuffer error: " << strError;
    }
}

void createTextureBuffer(int size, GLenum format, GLuint *tex, GLuint *bo, void* data)
{
    GLenum err;

    if( (*bo) > 0 )
        glDeleteBuffers( 1, bo );  //delete previously created tbo

    glGenBuffers( 1, bo );

    glBindBuffer( GL_TEXTURE_BUFFER, *bo );
    glBufferData( GL_TEXTURE_BUFFER, size, data, GL_STATIC_DRAW );

    err = glGetError();
    if( err > 0 ){
        QString strError;
        strError.sprintf("%s", glewGetErrorString(err));
        qDebug() << "createTextureBuffer error 1: " << strError;
    }

    if( (*tex) > 0 )
        glDeleteTextures( 1, tex ); //delete previously created texture

    glGenTextures( 1, tex );
    glBindTexture( GL_TEXTURE_BUFFER, *tex );
    glTexBuffer( GL_TEXTURE_BUFFER, format,  *bo );
    glBindBuffer( GL_TEXTURE_BUFFER, 0 );

    err = glGetError();
    if( err > 0 ){
        QString strError;
        strError.sprintf("%s", glewGetErrorString(err));
        qDebug() << "createTextureBuffer error 2: " << strError;
    }

}

void createUniformBuffer(int size, GLuint *bo, void* data)
{
    GLenum err;

    if( (*bo) > 0 )
        glDeleteBuffers( 1, bo );  //delete previously created tbo

    glGenBuffers( 1, bo );

    glBindBuffer( GL_UNIFORM_BUFFER, *bo );
    glBufferData( GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_DRAW );
    err = glGetError();

    if( err > 0 ){
        QString strError;
        strError.sprintf("%s", glewGetErrorString(err));
        qDebug() << "createUniformBuffer error 1: " << strError;
    }
}

QString readShaderBasedOnArchitecture(QString filePath)
{
    QFile f(filePath);
    if (!f.open(QFile::ReadOnly | QFile::Text)) return "";
    QTextStream in(&f);
//    qDebug() << f.size() << in.readAll();

    // Add version header to file according to architecture
    QString version;
#ifdef __APPLE__
    version = "#version 410\n";
#else
    version = "#version 430\n";
    version+= "#define VERSION_430\n";
#endif


    return version+in.readAll();
}
