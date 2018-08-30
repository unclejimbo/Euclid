/** Core functionalities for all rendering techniques.
 *
 *  @defgroup PkgRenderCore Render Core
 *  @ingroup PkgRender
 */
#pragma once

// FIXME:
// #include <Eigen/Core> will generate link error when using Eigen::Ref
#include <Eigen/Dense>

namespace Euclid
{
/** @{*/

/** A basic positionable camera model.
 *
 */
class Camera
{
public:
    using Vec3 = Eigen::Ref<const Eigen::Vector3f>;

public:
    /** Create a Camera with default paramters.
     *
     */
    Camera() = default;

    /** Create a Camera.
     *
     *  Pose the camera using position, focus and up.
     *
     *  @param position Position.
     *  @param focus Focus.
     *  @param up Rough up direction.
     */
    Camera(const Vec3& position,
           const Vec3& focus = Eigen::Vector3f::Zero(),
           const Vec3& up = Eigen::Vector3f(0.0f, 1.0f, 0.0f))
    {
        pos = position;
        dir = (position - focus).normalized();
        u = up.cross(dir).normalized();
        v = dir.cross(u);
    }

    virtual ~Camera() = default;

    /** Pose the camera according to the parameteres.
     *
     */
    void lookat(const Vec3& position, const Vec3& focus, const Vec3& up)
    {
        pos = position;
        dir = (position - focus).normalized();
        u = up.cross(dir).normalized();
        v = dir.cross(u);
    }

public:
    /** Position.
     *
     */
    Eigen::Vector3f pos{ 0.0f, 0.0f, 0.0f };

    /** Right vector.
     *
     */
    Eigen::Vector3f u{ 1.0f, 0.0f, 0.0f };

    /** Up vector.
     *
     */
    Eigen::Vector3f v{ 0.0f, 1.0f, 0.0f };

    /** Negative view vector.
     *
     */
    Eigen::Vector3f dir{ 0.0f, 0.0f, 1.0f };
};

/** A simple Phong material model.
 *
 */
struct Material
{
    /** Ambient color, in range [0, 1].
     *
     */
    Eigen::Array3f ambient;

    /** Diffuse color, in range [0, 1]
     *
     */
    Eigen::Array3f diffuse;
};

/** @}*/
} // namespace Euclid
