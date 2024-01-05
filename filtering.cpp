#include "filtering.h"

namespace PointFilter {
Filter AlwaysTrue()
{
    return [](Point) { return true; };
}

Filter And(Filter a, Filter b)
{
    return [a, b](Point p) { return a(p) && b(p); };
}

Filter Or(Filter a, Filter b)
{
    return [a, b](Point p) { return a(p) || b(p); };
}

Filter Not(Filter a)
{
    return [a](Point p) { return !a(p); };
}

Filter ComponentLessThan(Component c, float val)
{
    if (c == Component::X) {
        return [=](Point p) { return p.pos.x < val; };
    } else if (c == Component::Y) {
        return [=](Point p) { return p.pos.y < val; };
    }
    return [=](Point p) { return p.pos.z < val; };
}

Filter ComponentGreaterThan(Component c, float val)
{
    if (c == Component::X) {
        return [=](Point p) { return p.pos.x > val; };
    } else if (c == Component::Y) {
        return [=](Point p) { return p.pos.y > val; };
    }
    return [=](Point p) { return p.pos.z > val; };
}
} // namespace PointFilter
