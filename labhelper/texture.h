#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <vector>
#include <GL/glew.h>

class Texture
{
public:
    Texture(GLenum TextureTarget);

    ~Texture();

	void CreateEmptyTexture(int width, int height, GLenum format);

	void Load(const std::string& filename, int colorComponents);

	void Load(const std::vector<std::string>& filePaths, int colorComponents);

	// Bind the texture array to a texture unit
	void Bind(GLenum TextureUnit);

	void BindImage(GLenum ImageUnit, GLenum access);
	// Get the texture array object
	GLuint GetTexture() const { return m_textureObj; }

private:
	unsigned char* FetchImageData(std::string filePath, int colorComponents);
	int CalculateMipMapLevel() const;

	int m_width;
	int m_height;
	int m_bpp;
	GLenum m_format;

	GLenum m_textureTarget;
	GLuint m_textureObj;
};


#endif  /* TEXTURE_H */