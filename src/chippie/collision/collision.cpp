#include "collision.hpp"

#include "embp/variable_array.hpp"
#include "entities/basic_entity.hpp"

namespace chippie
{

namespace
{
/* this is definitely apart of the generic game state */
embp::variable_array<entity *, 128> collision_list;

} // namespace
} // namespace chippie