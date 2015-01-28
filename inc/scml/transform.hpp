#pragma once

namespace scml
{

// The coordinate transform for a bone or object.

class Transform
{
public:
    float x, y;
    float angle;
    float scale_x, scale_y;

    Transform();
    Transform(float x, float y, float angle, float scale_x, float scale_y);

    bool operator==(const Transform& t) const;
    bool operator!=(const Transform& t) const;

    void lerp(const Transform& transform, float t, int spin);
    void apply_parent_transform(const Transform& parent);
};

}

