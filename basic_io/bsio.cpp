#include "bsio.hpp"

#include "platform/platform.hpp"

namespace bsio
{

bool init() noexcept
{
    return basic_io::platform::init();
}

} // namespace bsio
