#ifndef OPENGL_TEXTURE_CACHE_HPP
#define OPENGL_TEXTURE_CACHE_HPP

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <unordered_map>
#include <string>
#include <vector>

/**
 * \brief minimize the footprint of creating textures with opengl
 *
 * \details when working with opengl, textures need to be created and bound into the opengl context, also if you have
 * two different objects that share textures then there is no point in uploading more image data to the cpu, instead
 * just re-use it, in order to facilitate that exact feature this class was created
 *
 * \author cuppajoeman
 */
class OpenGLTextureCache {
  public:
    // Load a texture from a file and return its OpenGL texture ID
    [[nodiscard]] GLuint load_texture(const std::string &path);

    // Decrease the reference count of a texture and delete it if the count reaches zero
    void delete_texture(const std::string &path);

    // Destructor to clean up loaded textures
    ~OpenGLTextureCache();

  private:
    struct TextureInfo {
        GLuint texture_id;
        int reference_count;
    };

    std::unordered_map<std::string, TextureInfo> texture_map;
};

#endif // OPENGL_TEXTURE_CACHE_HPP
