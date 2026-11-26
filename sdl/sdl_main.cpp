#include <SDL2/SDL_pixels.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <stdexcept>

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>

#include <fmt/core.h>
#include <fmt/format.h>

enum struct PrimaryColor
{
    RED = 0,
    GREEN = 1,
    BLUE = 2,
};

enum struct Format
{
    RGB565_LUT4,
    RGB565,
    RGB888_LUT4,
    RGB888,
};

size_t to_bitdepth(Format fmt)
{
    switch (fmt)
    {
    case Format::RGB565:
        return 16;
    case Format::RGB888:
        return 24;
    case Format::RGB565_LUT4:
    case Format::RGB888_LUT4:
        return 4;
    }
    return 8;
}

size_t to_pitch(Format fmt, size_t width)
{
    const auto bits{to_bitdepth(fmt)};

    if (bits == 4)
    {
        return width / 2;
    }

    return width * (bits / 8);
}

size_t get_mask(Format fmt, PrimaryColor prime)
{
    /* 888 */
    static constexpr std::array<uint32_t, 3> fmt_888{
        0x0000FF, /* red */
        0x00FF00, /* green */
        0xFF0000, /* blue */
    };
    /* 565 */
    static constexpr std::array<uint32_t, 3> fmt_565{
        0x00001F, /* red */
        0x0007E0, /* green */
        0x00F800, /* blue */
    };

    const auto &fmt_lut{[=]() {
        switch (fmt)
        {
        case Format::RGB565:
        case Format::RGB565_LUT4:
            return fmt_565;
        case Format::RGB888:
        case Format::RGB888_LUT4:
            return fmt_888;
        }
        return fmt_888;
    }()};

    return fmt_lut[static_cast<size_t>(prime)];
}

struct FrameBuffer
{
    std::span<uint8_t> buffer;
    size_t width;
    size_t height;
    Format format;
};

/* this class is given a reference to a framebuffer of a particular format and shape
   The class provides one method, which when called causes this frame buffer to appear
   on a screen... whatever that might mean.

   For my case, it means the SDL framework gets invoked.

*/
class MySDLScreenThing
{
  private:
    class SurfaceDeleter
    {
      public:
        void operator()(SDL_Surface *surface)
        {
            SDL_FreeSurface(surface);
        }
    };

  public:
    struct Config
    {
        FrameBuffer buffer;
        SDL_Window *window;
    };

    MySDLScreenThing(Config cfg)
        : m_cfg{cfg}, m_screen_surface{SDL_GetWindowSurface(cfg.window)}, m_scratch_surface{create_surface()}
    {
    }

    void render() const
    {
        SDL_BlitSurface(m_scratch_surface.get(), nullptr, m_screen_surface, nullptr);
        SDL_UpdateWindowSurface(m_cfg.window);
    }

    [[nodiscard]] SDL_Surface *get_surface() const
    {
        return m_scratch_surface.get();
    }

  private:
    Config m_cfg;
    SDL_Surface *m_screen_surface;
    std::unique_ptr<SDL_Surface, SurfaceDeleter> m_scratch_surface;

    [[nodiscard]] std::unique_ptr<SDL_Surface, SurfaceDeleter> create_surface() const
    {
        /* clang-format off */
        std::unique_ptr<SDL_Surface, SurfaceDeleter> result { std::unique_ptr<SDL_Surface, SurfaceDeleter>{
            SDL_CreateRGBSurfaceFrom(
                std::data(m_cfg.buffer.buffer), 
                m_cfg.buffer.width, 
                m_cfg.buffer.height, 
                to_bitdepth(m_cfg.buffer.format),
                to_pitch(m_cfg.buffer.format, m_cfg.buffer.width), 
                get_mask(m_cfg.buffer.format, PrimaryColor::RED),
                get_mask(m_cfg.buffer.format, PrimaryColor::GREEN), 
                get_mask(m_cfg.buffer.format, PrimaryColor::BLUE), 
                0  /* TODO there might be an alpha channel at some point, 
                      but the mevenboy renderers don't support it at the 
                      moment so we are leaving it unhandled
                    */
            )}};
        /* clang-format on */

        if (result == nullptr)
        {
            throw std::runtime_error(fmt::format("Failed to create scratch surface: {}", SDL_GetError()));
        }

        return std::move(result);
    }
};

/* Just pushes pixels onto a FrameBuffer, also keeps track of where it is on said FrameBuffer */
class SquareDemo
{
  public:
    SquareDemo(FrameBuffer frame) : frame{frame}
    {
        if (frame.format != Format::RGB888)
        {
            throw std::runtime_error("Only RGB888 surfaces surpported at the moment.");
        }

        draw_square(colstart, rowstart, sqsz, 0xFF, 0x00, 0x00);
    }

    void move_square_left()
    {
        draw_square(colstart, rowstart, sqsz, 0x00, 0x00, 0x00);
        colstart = colstart == 0 ? frame.width - sqsz - 1 : colstart - 1;
        draw_square(colstart, rowstart, sqsz, 0xFF, 0x00, 0x00);
    }

    void move_square_right()
    {
        draw_square(colstart, rowstart, sqsz, 0x00, 0x00, 0x00);
        colstart = colstart + 1 < (frame.width - sqsz) ? colstart + 1 : 0;
        draw_square(colstart, rowstart, sqsz, 0xFF, 0x00, 0x00);
    }

    void move_square_up()
    {
        draw_square(colstart, rowstart, sqsz, 0x00, 0x00, 0x00);
        rowstart = rowstart == 0 ? frame.height - sqsz - 1 : rowstart - 1;
        draw_square(colstart, rowstart, sqsz, 0xFF, 0x00, 0x00);
    }

    void move_square_down()
    {
        draw_square(colstart, rowstart, sqsz, 0x00, 0x00, 0x00);
        rowstart = rowstart + 1 < (frame.height - sqsz) ? rowstart + 1 : 0;
        draw_square(colstart, rowstart, sqsz, 0xFF, 0x00, 0x00);
    }

  private:
    /* data for the square... could be objectified */
    FrameBuffer frame;
    int sqsz{50};
    int rowstart{static_cast<int>(frame.height) / 2 - sqsz / 2};
    int colstart{static_cast<int>(frame.width) / 2 - sqsz / 2};

    void draw_square(int left, int top, int size, uint8_t red, uint8_t green, uint8_t blue)
    {
        const auto pitch{static_cast<int>(frame.width) * 3};
        for (int yy = top; yy < top + size; ++yy)
        {
            for (int xx = left; xx < left + size; ++xx)
            {
                const int idx{yy * pitch + xx * 3};
                frame.buffer[idx] = red;
                frame.buffer[idx + 1] = green;
                frame.buffer[idx + 2] = blue;
            }
        }
    }
};

/* top class for this whole silly thing */
class MyGameEmulator
{
  private:
    class WindowDeleter
    {
      public:
        void operator()(SDL_Window *window)
        {
            SDL_DestroyWindow(window);
        }
    };

  public:
    MyGameEmulator()
    {
        init_sdl();
        init_window();
        init_my_screen();
    }

    MyGameEmulator(const MyGameEmulator &) = delete;
    MyGameEmulator(MyGameEmulator &&) = delete;
    MyGameEmulator &operator=(const MyGameEmulator &) = delete;
    MyGameEmulator &operator=(MyGameEmulator &&) = delete;

    ~MyGameEmulator()
    {
        SDL_Quit();
    }

    void run()
    {
        /* run the render loop, in a hacky way for now */
        SDL_Event event{};
        bool quit{false};
        while (!quit)
        {
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    quit = true;
                }
                /* process keypresses */
                if (event.type == SDL_KEYDOWN)
                {
                    switch (event.key.keysym.sym)
                    {
                    case SDLK_UP:
                        square.move_square_up();
                        myscreen->render();
                        break;
                    case SDLK_DOWN:
                        square.move_square_down();
                        myscreen->render();
                        break;
                    case SDLK_LEFT:
                        square.move_square_left();
                        myscreen->render();
                        break;
                    case SDLK_RIGHT:
                        square.move_square_right();
                        myscreen->render();
                        break;
                    default:
                        break;
                    }
                }
            }
        }
    }

  private:
    static constexpr size_t SCREEN_WIDTH{640};
    static constexpr size_t SCREEN_HEIGHT{320};

    std::unique_ptr<SDL_Window, WindowDeleter> window;
    SDL_Surface *screen_surface;
    std::array<uint8_t, 3 * SCREEN_WIDTH * SCREEN_HEIGHT> frame_data{};
    std::unique_ptr<MySDLScreenThing> myscreen;
    FrameBuffer surface_buffer{
        .buffer = std::span{frame_data},
        .width = SCREEN_WIDTH,
        .height = SCREEN_HEIGHT,
        .format = Format::RGB888,
    };
    SquareDemo square{surface_buffer};

    void init_sdl()
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            throw std::runtime_error(fmt::format("Could not initialize SDL: {}", SDL_GetError()));
        }
    }

    void init_window()
    {
        /* clang-format off */
        window = std::unique_ptr<SDL_Window, WindowDeleter>(
            SDL_CreateWindow(
                "MyGameEmulator", 
                SDL_WINDOWPOS_UNDEFINED, 
                SDL_WINDOWPOS_UNDEFINED,
                SCREEN_WIDTH,
                SCREEN_HEIGHT,
                SDL_WINDOW_SHOWN
            ));
        /* clang-format on */
        if (window == nullptr)
        {
            throw std::runtime_error(fmt::format("Could not create window: {}", SDL_GetError()));
        }
    }

    void init_my_screen()
    {
        myscreen = std::make_unique<MySDLScreenThing>(MySDLScreenThing::Config{
            .buffer = surface_buffer,
            .window = window.get(),
        });
        myscreen->render();
    }
};

int main()
try
{
    MyGameEmulator{}.run();
    return 0;
}
catch (const std::exception &exc)
{
    fmt::print("Error: {}\n", exc.what());
    return -1;
}
