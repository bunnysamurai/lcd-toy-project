#include "bsio.hpp"

#include "platform/platform.hpp"

namespace bsio
{

bool init() noexcept
{
    return bsio::platform::init();
}

} // namespace bsio
