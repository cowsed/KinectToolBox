#ifndef FILTERING_H
#define FILTERING_H

#include "kinect_types.h"
#include <functional>

namespace PointFilter {
using Filter = std::function<bool(Point)>;

Filter AlwaysTrue();

Filter And(Filter a, Filter b);

Filter Or(Filter a, Filter b);

Filter Not(Filter a);
enum class Component { X, Y, Z };

Filter ComponentLessThan(Component c, float val);

Filter ComponentGreaterThan(Component c, float val);
} // namespace PointFilter

#endif // FILTERING_H
