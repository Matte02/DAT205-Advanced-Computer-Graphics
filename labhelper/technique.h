#ifndef TECHNIQUE_H
#define TECHNIQUE_H

#include <list>
#include <GL/glew.h>
#include <string>

#define INVALID_UNIFORM_LOCATION 0xffffffff

class Technique
{
public:

    Technique();

    virtual ~Technique();

    virtual bool Init();

    virtual void Enable();

    GLuint GetProgram() const { return m_shaderProg; }

protected:

    bool AddShader(GLenum ShaderType, const char* pFilename);

    bool Finalize();

    GLint GetUniformLocation(const char* pUniformName);

    GLuint m_shaderProg = 0;

private:

    typedef std::list<GLuint> ShaderObjList;
    ShaderObjList m_shaderObjList;
};

#endif  /* TECHNIQUE_H */