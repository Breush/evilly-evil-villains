#include "scml/transform.hpp"

#include "tools/math.hpp" // lerp

using namespace SCML;

// This is for rotating untranslated points and offsetting them to a new origin.
static void rotate_point(float& x, float& y, float angle, float origin_x, float origin_y, bool flipped)
{
    float s = sinf(angle*M_PI/180);
    float c = cosf(angle*M_PI/180);
    float xnew = (x * c) - (y * s);
    float ynew = (x * s) + (y * c);
    xnew += origin_x;
    ynew += origin_y;

    x = xnew;
    y = ynew;
}

Transform::Transform()
    : x(0.0f), y(0.0f), angle(0.0f), scale_x(1.0f), scale_y(1.0f)
{}

Transform::Transform(float x, float y, float angle, float scale_x, float scale_y)
    : x(x), y(y), angle(angle), scale_x(scale_x), scale_y(scale_y)
{}

bool Transform::operator==(const Transform& t) const
{
    return (x == t.x && y == t.y && angle == t.angle && scale_x == t.scale_x && scale_y == t.scale_y);
}

bool Transform::operator!=(const Transform& t) const
{
    return !(*this == t);
}

void Transform::lerp(const Transform& transform, float t, int spin)
{
    x = interpolate(x, transform.x, t);
    y = interpolate(y, transform.y, t);

    // 'spin' is based on what you are coming from (key1)
    if(spin != 0)
    {
        if(spin > 0 && angle > transform.angle)
            angle = interpolate(angle, transform.angle + 360, t);
        else if(spin < 0 && angle < transform.angle)
            angle = interpolate(angle, transform.angle - 360, t);
        else
            angle = interpolate(angle, transform.angle, t);
    }

    scale_x = interpolate(scale_x, transform.scale_x, t);
    scale_y = interpolate(scale_y, transform.scale_y, t);
}

void Transform::apply_parent_transform(const Transform& parent)
{
    x *= parent.scale_x;
    y *= parent.scale_y;

    bool flipped = ((parent.scale_x < 0) != (parent.scale_y < 0));
    rotate_point(x, y, parent.angle, parent.x, parent.y, flipped);

    angle += parent.angle;
    scale_x *= parent.scale_x;
    scale_y *= parent.scale_y;
}
