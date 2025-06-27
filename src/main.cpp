#include "../basic_io/screen/screen.hpp"
#include <algorithm>
#include <array>
#include <limits>

#include "pico/multicore.h"
#include "pico/printf.h"
#include "pico/stdio/driver.h"
#include "pico/stdlib.h"

#include "TextOut.hpp"
#include "status_utilities.hpp"
// #include "TextConsole.hpp"
#include "../glyphs/letters.hpp"
#include "VideoBuf.hpp"

#include "../basic_io/keyboard/TinyUsbKeyboard.hpp"

#include "Shell.h"

static constexpr uint8_t BPP{1};
static constexpr uint32_t DISPLAY_WIDTH{screen::PHYSICAL_SIZE.width};
static constexpr uint32_t DISPLAY_HEIGHT{screen::PHYSICAL_SIZE.height};
static constexpr size_t BUFLEN{[] {
  // pixels per byte is a function of bpp
  // need to convert from number of pixels to number of bits, then to number of
  // bytes.
  return DISPLAY_WIDTH * DISPLAY_HEIGHT * BPP / 8;
}()};

template <class T, class U>
constexpr void init_to_all_val(T &buf, const U &val) {
  for (auto &el : buf) {
    el = val;
  }
}

constexpr std::array<uint8_t, BUFLEN> init_the_buffer() {
  std::array<uint8_t, BUFLEN> rv{};
  init_to_all_val(rv, std::numeric_limits<uint8_t>::min());

  return rv;
}

static auto buffer{init_the_buffer()};
static TileBuffer<DISPLAY_WIDTH, DISPLAY_HEIGHT, BPP> tile_buf{buffer};
static TextOut<decltype(tile_buf)> wrt{tile_buf};

void run_demo_animation(auto stop_looping_callback) {
  auto &&quit_otherwise_sleep{[&](uint interval_ms) {
    if (stop_looping_callback()) {
      return true;
    }
    sleep_ms(interval_ms);
    return false;
  }};
  clear(wrt);
  print(wrt, "+------------------+\n");
  print(wrt, "| Meven 2040 Demo! |\n");
  print(wrt, "+------------------+\n");
  if (quit_otherwise_sleep(1000)) {
    return;
  }
  while (!stop_looping_callback()) {
    // run an animation, by hand
    print(wrt, "C:\\> ");
    if (quit_otherwise_sleep(500)) {
      return;
    }
    static constexpr std::string_view the_stuff{
        R"(This is a story all about how my life got switched turned upside down so take a minute just sit right there while I tell you howibecametheprince of a town called BelAir.)"};
    for (const auto c : the_stuff) {
      print(wrt, c);
      if (quit_otherwise_sleep(50)) {
        return;
      }
    }
    if (quit_otherwise_sleep(500)) {
      return;
    }
    print(wrt, '\n');
    if (quit_otherwise_sleep(100)) {
      return;
    }
    print(wrt, "Bad batch file or command\n");
    if (quit_otherwise_sleep(200)) {
      return;
    }
  }
}
template <class IOThing> void do_some_fake_processing(IOThing &thing) {
  print(thing, "Working");
  for (size_t ii = 0; ii < 3; ++ii) {
    sleep_ms(1000);
    print(thing, '.');
  }
  sleep_ms(500);
  print(thing, " Work complete.\n");
}

template <class tile_buf_interface>
void steven(tile_buf_interface &tiling_device) {
  // animation of a robot? Then, wait until there's an enter or something...
  clear(tiling_device);
}
template <class tile_buf_interface>
void claire(tile_buf_interface &tiling_device) {
  // animation of a flower? Then, wait until there's an enter or something...
  clear(tiling_device);
}

class StaticCommandLine {
public:
  static void push_char(char character) {
    // add to the command buffer
    if (next_idx < m_buffer.size()) {
      m_buffer[next_idx] = character;
      advance_next_idx();
    }
  }
  static void pop_char() { reverse_next_idx(); }
  static void process_command() {
    auto &&check_preconditions{[&]() { return next_idx < m_buffer.size(); }};
    if (!check_preconditions()) {
      // print(wrt, "Error: too many characters for command processing!\n");
      do_some_fake_processing(wrt);
    } else {
      constexpr auto check{[](const auto &command) {
        return std::equal(std::begin(m_buffer),
                          std::next(std::begin(m_buffer), next_idx),
                          std::begin(command), std::end(command));
      }};
      // valid commands
      static constexpr auto CLEAR_COMMAND{std::array{'c', 'l', 'e', 'a', 'r'}};
      static constexpr auto STEVEN_COMMAND{
          std::array{'s', 't', 'e', 'v', 'e', 'n'}};
      static constexpr auto CLAIRE_COMMAND{
          std::array{'c', 'l', 'a', 'i', 'r', 'e'}};
      if (check(CLEAR_COMMAND)) {
        clear(wrt);
      } else if (check(STEVEN_COMMAND)) {
        steven(tile_buf);
      } else if (check(CLAIRE_COMMAND)) {
        claire(tile_buf);
      } else {
        do_some_fake_processing(wrt);
      }
    }
    clear_buffer();
  }

private:
  static void advance_next_idx() { ++next_idx; }
  static void reverse_next_idx() {
    if (next_idx > 0) {
      --next_idx;
    }
  }
  static void clear_buffer() { next_idx = 0; }
  static constexpr size_t BUFLEN{16};
  static size_t next_idx;
  static std::array<char, BUFLEN> m_buffer;
};
size_t StaticCommandLine::next_idx{};
std::array<char, StaticCommandLine::BUFLEN> StaticCommandLine::m_buffer{};

void handle_putchar(uint8_t c) {
  putc(wrt, static_cast<char>(c));
  if (check_if_printable(c)) {
    StaticCommandLine::push_char(c);
  }
  if (check_if_backspace(c)) {
    StaticCommandLine::pop_char();
  }
  // TODO hack this in here for now
  if (c == '\n' || c == '\r') {
    StaticCommandLine::process_command();
    print(wrt, "[meven]$ ");
  }
}

bool stop_demo{false}; // TODO globals are everywhere, by definition ;)
void set_stop_demo() {
  // FIXME race conditions
  stop_demo = true;
}

static bool screen_init() {
  return screen::init(std::data(buffer), {.row = 0, .column = 0},
                      {.width = DISPLAY_WIDTH, .height = DISPLAY_HEIGHT},
                      {.bpp = BPP});
}

/* callbacks for stdio_driver_t */
void my_out_chars(const char *buf, int len) {
  for (int ii = 0; ii < len; ++ii) {
    putc(wrt, buf[ii]);
  }
}
void my_out_flush() { /* do nothing, we don't buffer... right meow */ }
int my_in_chars(char *buf, int len) {
  /* we block until we get all the keys! */
  keyboard::result_t err;
  for (int ii = 0; ii < len; ++ii) {
    const char c{keyboard::wait_key(std::chrono::milliseconds{1}, err)};
    if (err != keyboard::result_t::SUCCESS) {
      return ii;
    }
    buf[ii] = c;
  }
  return len;
}

void stdio_init_mine() {
  static stdio_driver_t my_driver = {.out_chars = my_out_chars,
                                     .out_flush = my_out_flush,
                                     .in_chars = my_in_chars};
  stdio_set_driver_enabled(&my_driver, true);
}

int mywrap_putchar(int c, FILE *) { return stdio_putchar(c); }
int mywrap_flush(FILE *) {
  stdio_flush();
  return 0;
}
int mywrap_getchar(FILE *) { return stdio_getchar(); }

int ShellCmd_Info(int argc, const char *argv[]) {
  (void)argc;
  (void)argv;

  printf("Hello, here's some info...\n");
  return EXIT_SUCCESS;
}

int main() {
  static_assert(BPP == 1, "init_letter_list(): Haven't handled more that 1 bit "
                          "per pixel yet. Sorry.");

  stdio_init_mine();

  if (!screen_init()) {
    BlinkStatus{BlinkStatus::Milliseconds{250}}.blink_forever();
  }

  // start by running the demo
  // FIXME race conditions
  multicore_launch_core1([] {
    run_demo_animation([&]() { return stop_demo; });
    while (true) {
      /* spin forever */
    }
  });
  while (!stop_demo) {
    keyboard::result_t err;
    const char c{keyboard::wait_key(std::chrono::milliseconds{1}, err)};
    set_stop_demo();
  }
  sleep_ms(
      1000); // TODO hack sleep to wait for the animation to stop running.
             // There's probably a way to check if the other core is halted...

  clear(wrt);
  static constexpr uint SHELL_BUFFER_LEN{64U};
  static constexpr uint ARGV_LEN{32U};
  static char shell_buffer[SHELL_BUFFER_LEN]; /* characters input by the user */
  static char
      *argument_values[ARGV_LEN]; /* list of pointers into shell_buffer */

  ShellFunction_t additional_cmds[] = {
      {.id = "info", .callback = ShellCmd_Info},
  };
  const int ADDITIONAL_CMDS_LENGTH =
      sizeof(additional_cmds) / sizeof(ShellFunction_t);
  for (int ii = 0; ii < ADDITIONAL_CMDS_LENGTH; ++ii) {
    Shell_RegisterCommand(additional_cmds[ii]);
  }

  const ShellInterface_t my_interface{.printf = stdio_printf,
                                      .putc = mywrap_putchar,
                                      .flush = mywrap_flush,
                                      .getc = mywrap_getchar};
  Shell_RegisterInterface(my_interface);

  /* launch the shell... does not return */
  ShellTask(&shell_buffer[0], SHELL_BUFFER_LEN, &argument_values[0], ARGV_LEN);
}