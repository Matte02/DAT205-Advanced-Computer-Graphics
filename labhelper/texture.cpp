#include <iostream>
#include <GL/glew.h>
// STB_IMAGE for loading images of many filetypes
#include <stb_image.h>
#include <stb_image_write.h>"
#include "texture.h"


Texture::Texture(GLenum TextureTarget)
{
	m_textureTarget = TextureTarget;
}

Texture::~Texture()
{
	if (m_textureObj != 0)
	{
		glDeleteTextures(1, &m_textureObj);
		m_textureObj = 0;
	}
}

void Texture::CreateEmptyTexture(int width, int height, GLenum format)
{

    m_width = width;
    m_height = height;
    m_format = format;

    glCreateTextures(m_textureTarget, 1, &m_textureObj);
    glTextureStorage2D(m_textureObj, 1, format, width, height);

    glTextureParameteri(m_textureObj, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_textureObj, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameterf(m_textureObj, GL_TEXTURE_BASE_LEVEL, 0);
    glTextureParameteri(m_textureObj, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_textureObj, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Texture::Load(const std::string& filename, int colorComponents)
{
    stbi_set_flip_vertically_on_load(1);

    unsigned char* pImageData = stbi_load(filename.c_str(), &m_width, &m_height, &m_bpp, colorComponents);

    if (!pImageData) {
        printf("Can't load texture from '%s' - %s\n", filename.c_str(), stbi_failure_reason());
        exit(0);
    }

    printf("Width %d, height %d, bpp %d\n", m_width, m_height, m_bpp);
    glCreateTextures(m_textureTarget, 1, &m_textureObj);

    int Levels = CalculateMipMapLevel();

    if (m_textureTarget == GL_TEXTURE_2D) {
        switch (m_bpp) {
        case 1:
            glTextureStorage2D(m_textureObj, Levels, GL_R8, m_width, m_height);
            glTextureSubImage2D(m_textureObj, 0, 0, 0, m_width, m_height, GL_RED, GL_UNSIGNED_BYTE, pImageData);
            break;

        case 2:
            glTextureStorage2D(m_textureObj, Levels, GL_RG8, m_width, m_height);
            glTextureSubImage2D(m_textureObj, 0, 0, 0, m_width, m_height, GL_RG, GL_UNSIGNED_BYTE, pImageData);
            break;

        case 3:
            glTextureStorage2D(m_textureObj, Levels, GL_RGB8, m_width, m_height);
            glTextureSubImage2D(m_textureObj, 0, 0, 0, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, pImageData);
            break;

        case 4:
            glTextureStorage2D(m_textureObj, Levels, GL_RGBA8, m_width, m_height);
            glTextureSubImage2D(m_textureObj, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, pImageData);
            break;

        default:
            break;
        }
    }
    else {
        printf("Support for texture target %x is not implemented\n", m_textureTarget);
        exit(1);
    }

    glTextureParameteri(m_textureObj, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(m_textureObj, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameterf(m_textureObj, GL_TEXTURE_BASE_LEVEL, 0);
    glTextureParameteri(m_textureObj, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_textureObj, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glGenerateTextureMipmap(m_textureObj);
}

void Texture::Load(const std::vector<std::string>& filePaths, int colorComponents)
{
    if (m_textureTarget != GL_TEXTURE_2D_ARRAY) {
        std::cerr << " Load with multiple files is only supported for GL_TEXTURE_2D_ARRAY and not: " << m_textureTarget << std::endl;
        exit(0);
    }

    std::vector<uint8_t*> imageData;

    for (int i = 0; i < filePaths.size(); i++) {
        imageData.push_back(FetchImageData(filePaths[i], colorComponents));
    }

    int levels = CalculateMipMapLevel();
    glCreateTextures(m_textureTarget, 1, &m_textureObj);
    glTextureStorage3D(m_textureObj, levels, GL_RGBA8, m_width, m_height, imageData.size());


    for (int i = 0; i < imageData.size(); i++) {
        glTextureSubImage3D(m_textureObj, 0, 0, 0, i, m_width, m_height, 1, GL_RGBA, GL_UNSIGNED_BYTE, imageData[i]);
    }

    for (auto& data : imageData) {
        stbi_image_free(data);
    }

    glTextureParameteri(m_textureObj, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(m_textureObj, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(m_textureObj, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_textureObj, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(m_textureObj, GL_TEXTURE_MAX_ANISOTROPY, 16);

    glGenerateTextureMipmap(m_textureObj);
}

unsigned char* Texture::FetchImageData(std::string filePath, int colorComponents)
{
    unsigned char* pImageData = stbi_load(filePath.c_str(), &m_width, &m_height, &m_bpp, colorComponents);
    if (!pImageData) {
        std::cerr << "Can't load texture from '" << filePath << "' - " << stbi_failure_reason() << std::endl;
    }
    return pImageData;
}

int Texture::CalculateMipMapLevel() const
{
    return std::min(5, (int)log2f((float)std::max(m_width, m_height)));
}


void Texture::Bind(GLenum TextureUnit)
{
	glBindTextureUnit(TextureUnit - GL_TEXTURE0, m_textureObj);
}

void Texture::BindImage(GLenum ImageUnit, GLenum access)
{
	glBindImageTexture(ImageUnit - GL_TEXTURE0, m_textureObj, 0, GL_FALSE, 0, access, m_format);
}