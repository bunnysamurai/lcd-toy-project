#include "../basic_io/screen/display.hpp"
#include <algorithm>
#include <array>
#include <limits>

#include "pico/stdlib.h"
#include "pico/printf.h"
#include "pico/multicore.h"

#include "status_utilities.hpp"
#include "TextConsole.hpp"
#include "../glyphs/letters.hpp"
#include "VideoBuf.hpp"

#include "../basic_io/keyboard/TinyUsbKeyboard.hpp"

static constexpr uint8_t BPP{1};
static constexpr size_t BUFLEN{[]
                               {
                                 // pixels per byte is a function of bpp
                                 // need to convert from number of pixels to number of bits, then to number of bytes.
                                 return mVirtWidth * mVirtHeight * BPP / 8;
                               }()};

template <class T, class U>
constexpr void init_to_all_val(T &buf, const U &val)
{
  for (auto &el : buf)
  {
    el = val;
  }
}

constexpr std::array<uint8_t, BUFLEN> init_the_buffer()
{
  std::array<uint8_t, BUFLEN> rv{};
  init_to_all_val(rv, std::numeric_limits<uint8_t>::min());

  return rv;
}

static auto buffer{init_the_buffer()};
static TileBuffer<mVirtWidth, mVirtHeight, BPP> tile_buf{buffer};
static TextConsole<decltype(tile_buf), glyphs::LetterType> wrt{tile_buf};

void run_demo_animation(auto stop_looping_callback)
{
  auto &&quit_otherwise_sleep{[&](uint interval_ms)
                              {
                                if (stop_looping_callback())
                                {
                                  return true;
                                }
                                sleep_ms(interval_ms);
                                return false;
                              }};
  clear(wrt);
  print(wrt, "+------------------+\n");
  print(wrt, "| Meven 2040 Demo! |\n");
  print(wrt, "+------------------+\n");
  if (quit_otherwise_sleep(1000))
  {
    return;
  }
  while (!stop_looping_callback())
  {
    // run an animation, by hand
    print(wrt, "C:\\> ");
    if (quit_otherwise_sleep(500))
    {
      return;
    }
    static constexpr std::string_view the_stuff{R"(This is a story all about how my life got switched turned upside down so take a minute just sit right there while I tell you howibecametheprince of a town called BelAir.)"};
    for (const auto c : the_stuff)
    {
      print(wrt, c);
      if (quit_otherwise_sleep(50))
      {
        return;
      }
    }
    if (quit_otherwise_sleep(500))
    {
      return;
    }
    print(wrt, '\n');
    if (quit_otherwise_sleep(100))
    {
      return;
    }
    print(wrt, "Bad batch file or command\n");
    if (quit_otherwise_sleep(200))
    {
      return;
    }
  }
}
template <class IOThing>
void do_some_fake_processing(IOThing &thing)
{
  print(thing, "Working");
  for (size_t ii = 0; ii < 3; ++ii)
  {
    sleep_ms(1000);
    print(thing, '.');
  }
  sleep_ms(500);
  print(thing, " Work complete.\n");
}

template <class tile_buf_interface>
void steven(tile_buf_interface &tiling_device)
{
  // animation of a robot? Then, wait until there's an enter or something...
  clear(tiling_device);
}
template <class tile_buf_interface>
void claire(tile_buf_interface &tiling_device)
{
  // animation of a flower? Then, wait until there's an enter or something...
  clear(tiling_device);
}

class StaticCommandLine
{
public:
  static void push_char(char character)
  {
    // add to the command buffer
    if (next_idx < m_buffer.size())
    {
      m_buffer[next_idx] = character;
      advance_next_idx();
    }
  }
  static void pop_char()
  {
    reverse_next_idx();
  }
  static void process_command()
  {
    auto &&check_preconditions{[&]()
                               {
                                 return next_idx < m_buffer.size();
                               }};
    if (!check_preconditions())
    {
      // print(wrt, "Error: too many characters for command processing!\n");
      do_some_fake_processing(wrt);
    }
    else
    {
      constexpr auto check{[](const auto &command)
                           { return std::equal(std::begin(m_buffer), std::next(std::begin(m_buffer), next_idx), std::begin(command), std::end(command)); }};
      // valid commands
      static constexpr auto CLEAR_COMMAND{std::array{'c', 'l', 'e', 'a', 'r'}};
      static constexpr auto STEVEN_COMMAND{std::array{'s', 't', 'e', 'v', 'e', 'n'}};
      static constexpr auto CLAIRE_COMMAND{std::array{'c', 'l', 'a', 'i', 'r', 'e'}};
      if (check(CLEAR_COMMAND))
      {
        clear(wrt);
      }
      else if (check(STEVEN_COMMAND))
      {
        steven(tile_buf);
      }
      else if (check(CLAIRE_COMMAND))
      {
        claire(tile_buf);
      }
      else
      {
        do_some_fake_processing(wrt);
      }
    }
    clear_buffer();
  }

private:
  static void advance_next_idx()
  {
    ++next_idx;
  }
  static void reverse_next_idx()
  {
    if (next_idx > 0)
    {
      --next_idx;
    }
  }
  static void clear_buffer()
  {
    next_idx = 0;
  }
  static constexpr size_t BUFLEN{16};
  static size_t next_idx;
  static std::array<char, BUFLEN> m_buffer;
};
size_t StaticCommandLine::next_idx{};
std::array<char, StaticCommandLine::BUFLEN> StaticCommandLine::m_buffer{};

void handle_putchar(uint8_t c)
{
  putc(wrt, static_cast<char>(c));
  if (check_if_printable(c))
  {
    StaticCommandLine::push_char(c);
  }
  if (check_if_backspace(c))
  {
    StaticCommandLine::pop_char();
  }
  // TODO hack this in here for now
  if (c == '\n')
  {
    StaticCommandLine::process_command();
    print(wrt, "[meven]$ ");
  }
}

// void setup_for_input(uint id)
// {
//   gpio_init(id);
//   gpio_set_dir(id, false);
// }
// void setup_for_output(uint id)
// {
//   gpio_init(id);
//   gpio_set_dir(id, true);
// }

// bool lcd_init(auto &video_buf)
// {
//   bool status{true};
//   setup_for_output(PIN_TOUCH_CS);
//   setup_for_output(PIN_LCD_RESET);
//   setup_for_output(PIN_LCD_DnC);
//   setup_for_output(PIN_LCD_CS);
//   setup_for_output(PIN_SPI_CLK);
//   setup_for_output(PIN_SPI_MOSI);
//   setup_for_input(PIN_SPI_MISO);
//   setup_for_output(PIN_LCD_BL);

//   status &= dispInit(std::data(video_buf), 1);
//   dispSetDepth(BPP);

//   printf("INFO: Turning on backlight\n");

//   gpio_put(PIN_LCD_BL, true);

//   return status;
// }

bool stop_demo{false}; // TODO globals are everywhere, by definition ;)
void set_stop_demo()
{
  // FIXME race conditions
  stop_demo = true;
}

int main()
{
  static_assert(BPP == 1, "init_letter_list(): Haven't handled more that 1 bit per pixel yet. Sorry.");

  if (!screen::init(std::data(buffer), BPP))
  {
    BlinkStatus{BlinkStatus::Milliseconds{250}}.blink_forever();
  }

  /* Needs a rethink.  Some thoughts:
   *  I'd like to maintain running a demo on startup that gets cleared once there's keyboard input.
   *  I feel I need a way to register new callbacks with the keyboard processing loop, so each new application that wants
   *  to run can get access (subscribe?) to the keyboard inputs.
   *  Need a way to add commands easily, to run different programs.  What does a program need?  Input from the keyboard and access to the video buffer.
   *  If we do this like an OS, then a program can opt into receiving keyboard input in the usual fashion: std::cin, cv::waitKey(), etc. which are all blocking and that's a good thing.
   *  As for interfacing with the video buffer, that's pretty easy, as we already have a method for doing this :)
   *
   *  I think, then, the next step is handling keyboard inputs in a sane way, that other applications can get access to easily.
   *  Right now, there's hardcoded callbacks that only interacts with the one app "console".
   *
   *  Design: If a process wants keyboard input, then:
   *    #include "keyboard_input.h"
   *
   *    int main()
   *    {
   *      char key_pressed;
   *      wait_key( key_pressed, 1000ms ); // don't forget to handle errors
   *    }
   *
   *    result_t wait_key(char& out, uint timeout)
   *    {
   *      // precondition: a keyboard is connected
   *      timer obj{timeout};
   *      while(obj.not_timed_out())
   *      {
   *        tuh_task();
   *        if( !tuh_kbuffer_is_emtpy() )
   *        {
   *          out = tuh_pop_kbuffer();
   *          return result_t::SUCCESS;
   *        }
   *      }
   *      return result_t::ERROR_TIMEOUT;
   *    }
   *
   *  Design: if a process wants to write to a screen... hmm.
   *    The video buffer is allocated exactly once, and by the "OS" as it has visibility into what hardware is capable of.
   *    So initializing the raw video buffer will be done in the top-level main(), as it already is.
   *    The question, then, is how to provide higher level functionality?  Maybe an "OS" method that just gives a pointer to the raw video buffer and it's limitations?
   *    Sounds like we're punting a bit to the user.  Which I like!
   *
   *    #include "vidbuf.h"
   *
   *    int main()
   *    {
   *      vidbuf_type p_vid;
   *      vidbuf_get_buffer(p_vid); // as always, remember to handle your errors!
   *    }
   *
   *    result_t vidbuf_get(vidbuf_type &out_struct)
   *    {
   *      // do all the HW specific things to get the video buffer system up and running.  For example:
   *      static constexpr M{ bsp::screen_width() };
   *      static constexpr N{ bsp::screen_height() };
   *      static constexpr BPP{ bsp::screen_bpp() };
   *      static std::array<uint8_t, M*N*BPP / 8> buf{};
   *      if( !bsp::screen_init(buf) ) // which can delegate to lcd_init() above, for example
   *      {
   *        return result_t::ERROR_LCD_INIT_FAILURE;
   *      }
   *
   *      // now, let our caller know the details:
   *      out_struct.p_buf = &buf;
   *      out_struct.bpp = BPP;
   *      out_struct.width = M;
   *      out_struct.height = N;
   *      // if we want to get fancy, we could hand callbacks to the user, or let the user register callbacks, that signal/get called on "VSYNC"
   *
   *      return result_t::SUCCESS;
   *    }
   *
   *    This still allows for the "OS" to provide higher level features, like TextConsole and TileBuffer. All of these, including user-defined, all share the same video buffer resource.
   *
   *    #include "TileBuffer.h"
   *    #include "TextConsole.h"
   *
   *    int main()
   *    {
   *      result_t err;
   *      auto console{ get_text_out_device(err) };
   *      auto tiler{ get_tile_buffer_device(err) }; // please, friends don't let friends skip error handling
   *
   *      print(console, "This is a thing!");
   *      draw(tiler, custom_tile, 15, 22);
   *      clear(console);
   *    }
   *
   *    auto& get_tile_buffer_device(result_t& err)
   *    {
   *      vidbuf_type vid;
   *      vidbuf_get_buffer(vid);
   *      static TileBuffer<bsp::screen_width(), bsp::screen_height(), bsp::screen_bpp()> tile_buf{*(vid.p_buf)};
   *      return tile_buf;
   *    }
   *
   *    auto& get_text_out_device(result_t& err)
   *    {
   *      result_t err;
   *      static TextConsole wrt{get_tile_buffer_device(err)};
   *      return wrt;
   *    }
   */

  // start by running the demo
  // FIXME race conditions
  multicore_launch_core1([]
                         { run_demo_animation([&]()
                                              { return stop_demo; });
                                              while (true){
/* spin forever */
                                              } });
  while (!stop_demo)
  {
    keyboard::result_t err;
    const char c{keyboard::wait_key(std::chrono::milliseconds{1}, err)};
    set_stop_demo();
  }
  sleep_ms(1000); // TODO hack sleep to wait for the animation to stop running.  There's probably a way to check if the other core is halted...
  clear(wrt);
  print(wrt, "[meven]$ ");
  for (;;)
  {
    keyboard::result_t err;
    const char c{keyboard::wait_key(std::chrono::milliseconds{1}, err)};
    handle_putchar(c);
  }
}