#include "demo.hpp"

#include <string_view>

#include "pico/printf.h"
#include "pico/stdio.h"
#include "pico/time.h"

#include "../basic_io/bsio.hpp"

namespace demo {

void run_animation(callback_t stop_looping_callback) {
  auto &&quit_otherwise_sleep{[&](uint interval_ms) {
    if (stop_looping_callback()) {
      return true;
    }
    sleep_ms(interval_ms);
    return false;
  }};
  bsio::clear_screen();
  printf("+------------------+\n");
  printf("| Meven 2040 Demo! |\n");
  printf("+------------------+\n");
  if (quit_otherwise_sleep(1000)) {
    return;
  }
  while (!stop_looping_callback()) {
    // run an animation, by hand
    printf("C:\\> ");
    if (quit_otherwise_sleep(500)) {
      return;
    }
    static constexpr std::string_view the_stuff{
        R"(This is a story all about how my life got switched turned upside down so take a minute just sit right there while I tell you howibecametheprince of a town called BelAir.)"};
    for (const auto c : the_stuff) {
      printf("%c", c);
      if (quit_otherwise_sleep(50)) {
        return;
      }
    }
    if (quit_otherwise_sleep(500)) {
      return;
    }
    printf("\n");
    if (quit_otherwise_sleep(100)) {
      return;
    }
    printf("Bad batch file or command\n");
    if (quit_otherwise_sleep(200)) {
      return;
    }
  }
}

} // namespace demo