#include <atomic>
#include <iostream>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

// Must come first.
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glog/logging.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#undef FTERRORS_H_
#define FT_ERRORDEF(e, v, s) {e, s},
#define FT_ERROR_START_LIST {
#define FT_ERROR_END_LIST \
  { 0, NULL }             \
  }                       \
  ;

const struct {
  int err_code;
  const char *err_msg;
} ft_errors[] =

#include FT_ERRORS_H

// GL includes
#include "Shader.h"

#include "epoll.h"
#include "pts.h"
#include "zero_copy.h"

    // Properties
    const GLuint WIDTH = 800,
  HEIGHT = 600;

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
  GLuint TextureID;    // ID handle of the glyph texture
  glm::ivec2 Size;     // Size of glyph
  glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
  GLuint Advance;      // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;
GLuint VAO, VBO;

const char *FT_Error_String(int err_code) {
  for (const auto &ft_error : ft_errors) {
    if (ft_error.err_code == err_code) {
      return ft_error.err_msg;
    }
  }
  return "<unknown>";
}

#define CheckGLError()                            \
  {                                               \
    GLenum err;                                   \
    while ((err = glGetError()) != GL_NO_ERROR) { \
      LOG(ERROR) << "Got GL error: " << err;      \
    }                                             \
  }

void RenderCharacter(Shader &shader, int character, GLfloat x, GLfloat y,
                     GLfloat scale, glm::vec3 color);
void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y,
                GLfloat scale, glm::vec3 color);

int key_write = -1;

// The MAIN function, from here we start our application and run the Game loop
int main() {
  // Init GLFW
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr,
                                        nullptr);  // Windowed
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize OpenGL context\n";
    return -1;
  }

  // Define the viewport dimensions
  glViewport(0, 0, WIDTH, HEIGHT);
  CheckGLError();

  // Set OpenGL options
  glEnable(GL_CULL_FACE);
  CheckGLError();
  glEnable(GL_BLEND);
  CheckGLError();
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  CheckGLError();

  // Compile and setup the shader
  Shader shader("shaders/text.vs", "shaders/text.frag");
#if 1
  glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(WIDTH), 0.0f,
                                    static_cast<GLfloat>(HEIGHT));
  shader.use();
  glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE,
                     glm::value_ptr(projection));
#endif
  CheckGLError();

  // FreeType
  FT_Library ft;
  // All functions return a value different than 0 whenever an error occurred
  if (FT_Init_FreeType(&ft))
    std::cout << "ERROR::FREETYPE: Could not init FreeType Library"
              << std::endl;

  // Load font as face
  FT_Face face;
  auto error = FT_New_Face(ft, "7x13.pcf", 0, &face);
  if (error) {
    std::cout << "ERROR::FREETYPE: Failed to load font: "
              << FT_Error_String(error) << std::endl;
    return -1;
  }

  for (int i = 0; i < face->num_fixed_sizes; ++i) {
    std::cout << "Available size " << i << "\n";
    std::cout << "  -> height = " << face->available_sizes[i].height << "\n";
    std::cout << "  -> width = " << face->available_sizes[i].width << "\n";
    std::cout << "  -> size = " << face->available_sizes[i].size << "\n";
    std::cout << "  -> x_ppem = " << face->available_sizes[i].x_ppem << "\n";
    std::cout << "  -> y_ppem = " << face->available_sizes[i].y_ppem << "\n";
  }

  // Set size to load glyphs as
  if (FT_Set_Pixel_Sizes(face, face->available_sizes[0].width,
                         face->available_sizes[0].height)) {
    std::cout << "ERROR::FREETYPE: Failed to set pixel size" << std::endl;
    if (FT_Select_Size(face, 0)) {
      std::cout << "ERROR::FREETYPE: Failed to select size" << std::endl;
    }
  }

  // Disable byte-alignment restriction
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  CheckGLError();

  // Load first 128 characters of ASCII set
  for (GLubyte c = 0; c < 128; c++) {
    // Load character glyph
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
      continue;
    }
    // Generate texture
    GLuint texture;
    glGenTextures(1, &texture);
    CheckGLError();
    glBindTexture(GL_TEXTURE_2D, texture);
    CheckGLError();

    std::vector<GLubyte> expanded_data;
    GLvoid *data = face->glyph->bitmap.buffer;
    if (face->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_MONO) {
      const auto &bitmap = face->glyph->bitmap;
      CHECK_GT(bitmap.pitch, 0);
      for (int row = 0; row < static_cast<int>(bitmap.rows); ++row) {
        for (int pixel = 0; pixel < static_cast<int>(bitmap.width); ++pixel) {
          uint8_t buffer_byte =
              bitmap.buffer[row * bitmap.pitch + (pixel >> 3)];
          if (buffer_byte & (1 << ((8 - pixel) & 0x7))) {
            expanded_data.push_back(255);
          } else {
            expanded_data.push_back(0);
          }
        }
      }
      data = expanded_data.data();
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
                 face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, data);
    CheckGLError();

    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    CheckGLError();
    // Now store character for later use
    Character character = {
        texture,
        glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
        glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
        static_cast<GLuint>(face->glyph->advance.x),
    };
    Characters.insert(std::pair<GLchar, Character>(c, character));
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  CheckGLError();
  // Destroy FreeType once we're finished
  FT_Done_Face(face);
  FT_Done_FreeType(ft);

  // Configure VAO/VBO for texture quads
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  CheckGLError();

  ttyrex::pts::PseudoTerminal terminal;
  terminal.Fork("/bin/bash", {"bash"});

  constexpr int kTerminalWidth = 80;
  constexpr int kTerminalHeight = 26;

  constexpr int kCharacterHeight = HEIGHT / kTerminalHeight;
  constexpr int kCharacterWidth = WIDTH / kTerminalWidth;

  ttyrex::zero_copy::Buffer raw_buffer(kTerminalWidth * kTerminalHeight);
  auto reader = raw_buffer.MakeReader();
  auto writer = raw_buffer.MakeWriter();

  std::mutex thread_mutex;
  ttyrex::zero_copy::CircularBuffer<uint8_t> thread_buffer(kTerminalWidth *
                                                           kTerminalHeight);

  ttyrex::epoll::EPoll epoll;

  epoll.Add(terminal.master(), EPOLLIN, [&] {
    std::span<uint8_t> buffer = writer->Get(100);
    const ssize_t result =
        read(terminal.master(), buffer.data(), buffer.size());
    if (result == -1) {
      if (errno == EINTR || errno == EAGAIN) {
        writer->Rewind(buffer.size());
        return;
      }
      PLOG(ERROR) << "Failed to read from pty.";
      writer->Rewind(buffer.size());
      return;
    }
    writer->Rewind(buffer.size() - result);

    {
      const std::lock_guard<std::mutex> lock(thread_mutex);
      while (true) {
        std::span<const uint8_t> new_data = reader->Get(1024);
        if (new_data.size() == 0) {
          break;
        }
        thread_buffer.Add(new_data);
        glfwPostEmptyEvent();
      }
    }
    LOG(INFO) << "Read out " << result << " bytes";
  });

  std::atomic_bool should_quit{false};
  std::thread poller_thread([&] {
    while (!should_quit) {
      epoll.Wait(std::chrono::milliseconds(250));
    }
  });

  key_write = terminal.master();
  glfwSetKeyCallback(window, [](GLFWwindow*, int key, int, int action, int) {
      if (action != GLFW_PRESS) {
      return;
      }
      CHECK_NE(key_write, -1);
      if (isascii(key)) {
      char buffer = key;
      write(key_write, &buffer, sizeof(buffer));
      }
      
      });

  // Game loop
  while (true) {
    // Check and call events
    glfwWaitEvents();

    if (glfwWindowShouldClose(window)) {
      break;
    }

    if (thread_buffer.NewDataIsAvailable()) {
      {
        const std::lock_guard<std::mutex> lock(thread_mutex);
        int index = 0;

        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        {
          for (int row = 0; row < kTerminalHeight; ++row) {
            for (int column = 0; column < kTerminalWidth; ++column) {
              if (index >= thread_buffer.size()) {
                row = kTerminalHeight;
                column = kTerminalWidth;
                break;
              }
              RenderCharacter(shader, thread_buffer[index],
                              column * kCharacterWidth,
                              (kTerminalHeight - row - 1) * kCharacterHeight,
                              1.0f, glm::vec3(0.5, 0.8f, 0.2f));
              ++index;
            }
          }
        }
      }

      // Swap the buffers
      glfwSwapBuffers(window);
    }
  }

  should_quit = true;
  poller_thread.join();

  epoll.Delete(terminal.master());

  terminal.KillAndWait();

  glfwTerminate();
  return 0;
}

void RenderCharacter(Shader &shader, int character, GLfloat x, GLfloat y,
                     GLfloat scale, glm::vec3 color) {
  // Activate corresponding render state
  shader.use();
  glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y,
              color.z);
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(VAO);

  Character ch = Characters[character];

  GLfloat xpos = x + ch.Bearing.x * scale;
  GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

  GLfloat w = ch.Size.x * scale;
  GLfloat h = ch.Size.y * scale;
  // Update VBO for each character
  GLfloat vertices[6][4] = {
      {xpos, ypos + h, 0.0, 0.0},    {xpos, ypos, 0.0, 1.0},
      {xpos + w, ypos, 1.0, 1.0},

      {xpos, ypos + h, 0.0, 0.0},    {xpos + w, ypos, 1.0, 1.0},
      {xpos + w, ypos + h, 1.0, 0.0}};
  // Render glyph texture over quad
  glBindTexture(GL_TEXTURE_2D, ch.TextureID);
  // Update content of VBO memory
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferSubData(
      GL_ARRAY_BUFFER, 0, sizeof(vertices),
      vertices);  // Be sure to use glBufferSubData and not glBufferData

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // Render quad
  glDrawArrays(GL_TRIANGLES, 0, 6);
  // Now advance cursors for next glyph (note that advance is number of 1/64
  // pixels)
  x += (ch.Advance >> 6) *
       scale;  // Bitshift by 6 to get value in pixels (2^6 = 64 (divide
               // amount of 1/64th pixels by 64 to get amount of pixels))

  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}
void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y,
                GLfloat scale, glm::vec3 color) {
  // Activate corresponding render state
  shader.use();
  glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y,
              color.z);
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(VAO);

  // Iterate through all characters
  std::string::const_iterator c;
  for (c = text.begin(); c != text.end(); c++) {
    Character ch = Characters[*c];

    GLfloat xpos = x + ch.Bearing.x * scale;
    GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

    GLfloat w = ch.Size.x * scale;
    GLfloat h = ch.Size.y * scale;
    // Update VBO for each character
    GLfloat vertices[6][4] = {
        {xpos, ypos + h, 0.0, 0.0},    {xpos, ypos, 0.0, 1.0},
        {xpos + w, ypos, 1.0, 1.0},

        {xpos, ypos + h, 0.0, 0.0},    {xpos + w, ypos, 1.0, 1.0},
        {xpos + w, ypos + h, 1.0, 0.0}};
    // Render glyph texture over quad
    glBindTexture(GL_TEXTURE_2D, ch.TextureID);
    // Update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(
        GL_ARRAY_BUFFER, 0, sizeof(vertices),
        vertices);  // Be sure to use glBufferSubData and not glBufferData

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // Now advance cursors for next glyph (note that advance is number of 1/64
    // pixels)
    x += (ch.Advance >> 6) *
         scale;  // Bitshift by 6 to get value in pixels (2^6 = 64 (divide
                 // amount of 1/64th pixels by 64 to get amount of pixels))
  }
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}
