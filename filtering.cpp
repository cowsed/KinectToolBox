#include "filtering.h"

namespace PointFilter {
Filter AlwaysTrue()
{
    return [](Point) { return true; };
}

Filter And(const Filter &filta, const Filter &filtb)
{
    return [filta, filtb](Point point) { return filta(point) && filtb(point); };
}

Filter Or(const Filter &filta, const Filter &filtb)
{
    return [filta, filtb](Point point) { return filta(point) || filtb(point); };
}

Filter Not(const Filter &filta)
{
    return [filta](Point point) { return !filta(point); };
}

Filter ComponentLessThan(Component comp, float val)
{
    if (comp == Component::X) {
        return [=](Point p) { return p.x < val; };
    } else if (comp == Component::Y) {
        return [=](Point p) { return p.y < val; };
    }
    return [=](Point p) { return p.z < val; };
}

Filter ComponentGreaterThan(Component c, float val)
{
    if (c == Component::X) {
        return [=](Point p) { return p.x > val; };
    } else if (c == Component::Y) {
        return [=](Point p) { return p.y > val; };
    }
    return [=](Point p) { return p.z > val; };
}
} // namespace PointFilter
