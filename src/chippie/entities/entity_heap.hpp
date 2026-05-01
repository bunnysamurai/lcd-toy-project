/**
    The enitity heap is to help get around some Issues I have with polymorphism, namely, iterating over collections of
    base types but those collections can't own the objects themselves.

    Which means I need somewhere else to keep ownership.  The other niggle is levels will likely be loaded off of disk,
    so I need to allocate space for the dizzying array of different entities any level could present at runtime.  Which
    means I need a heap.

    This module will be a specialized heap to control the number of entities, but can be used generally to allocate
    space at runtime of C++ objects.

 */
#if !defined(ENTITY_HEAP_HPP)
#define ENTITY_HEAP_HPP

namespace chippie
{

}

#endif