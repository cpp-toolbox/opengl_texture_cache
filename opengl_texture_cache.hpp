#ifndef OPENGL_TEXTURE_CACHE_HPP
#define OPENGL_TEXTURE_CACHE_HPP

#include <GL/gl.h>
#include <unordered_map>
#include <string>
#include <vector>

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