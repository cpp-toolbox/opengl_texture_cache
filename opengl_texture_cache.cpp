#include "opengl_texture_cache.hpp"
#include <stdexcept>
#include <spdlog/spdlog.h>

#include "sbpt_generated_includes.hpp"

/**
 * \brief Load a texture from a file and return its OpenGL texture ID
 */
GLuint OpenGLTextureCache::load_texture(const std::string &path) {

    // Check if the texture is already loaded
    if (texture_map.find(path) != texture_map.end()) {
        spdlog::get(Systems::graphics)->info("just used a cached texture for {}", path);
        texture_map[path].reference_count++;
        return texture_map[path].texture_id;
    }

    TextureData texture_info = load_texture_from_file(path);

    // Generate and bind a new OpenGL texture
    GLuint texture_id;
    glGenTextures(1, &texture_id);

    glBindTexture(GL_TEXTURE_2D, texture_id);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    GLenum format = 0;
    if (texture_info.num_components == 1) {
        format = GL_RED;
    } else if (texture_info.num_components == 3) {
        format = GL_RGB;
    } else if (texture_info.num_components == 4) {
        format = GL_RGBA;
    }

    // this binds the image data to our texture_id so that we can load in textures later on
    // Upload the texture data to the GPU,
    glTexImage2D(GL_TEXTURE_2D, 0, format, texture_info.width, texture_info.height, 0, format, GL_UNSIGNED_BYTE,
                 texture_info.image_data.data());
    spdlog::get(Systems::graphics)->info("generated a new image for {}", path);

    // Store the texture information with a reference count of 1
    texture_map[path] = {texture_id, 1};

    // Print the size of the map
    spdlog::get(Systems::graphics)->info("Texture map size: {}", texture_map.size());

    // Print the contents of the map
    for (const auto &[key, value] : texture_map) {
        spdlog::get(Systems::graphics)
            ->info("Key: {}, Texture ID: {}, Reference Count: {}", key, value.texture_id, value.reference_count);
    }
    return texture_id;
}

/**
 * \brief decrease the reference count of a texture and delete it if the count reaches zero
 * \param path for the texture to be deleted
 */
void OpenGLTextureCache::delete_texture(const std::string &path) {
    if (texture_map.find(path) == texture_map.end()) {
        throw std::runtime_error("Texture not found: " + path);
    }

    texture_map[path].reference_count--;
    if (texture_map[path].reference_count <= 0) {
        glDeleteTextures(1, &texture_map[path].texture_id);
        texture_map.erase(path);
    }
}

/**
 * \pre this must be done with an active OpenGL context!
 * \brief destructor to clean up loaded textures
 */
OpenGLTextureCache::~OpenGLTextureCache() {
    for (auto &[path, texture_info] : texture_map) {
        glDeleteTextures(1, &texture_info.texture_id);
    }
}