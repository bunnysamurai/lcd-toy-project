#include "demo.hpp"

#include <string_view>

#include "pico/printf.h"
#include "pico/stdio.h"
#include "pico/time.h"

#include "../basic_io/bsio.hpp"

namespace demo {

void run_animation() {
  bsio::clear_screen();
  printf("+------------------+\n");
  printf("| Meven 2040 Demo! |\n");
  printf("+------------------+\n");
  sleep_ms(1000);
  while (true) {
    // run an animation, by hand
    printf("C:\\> ");
    sleep_ms(500);
    static constexpr std::string_view the_stuff{
        R"(This is a story all about how my life got switched turned upside down so take a minute just sit right there while I tell you howibecametheprince of a town called BelAir.)"};
    for (const auto c : the_stuff) {
      printf("%c", c);
      sleep_ms(50);
    }
    sleep_ms(500);
    printf("\n");
    sleep_ms(100);
    printf("Bad batch file or command\n");
    sleep_ms(200);
  }
}

} // namespace demo