/** Core functionalities for all rendering techniques.
 *
 *  @defgroup PkgRenderCore Render Core
 *  @ingroup PkgRender
 */
#pragma once

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
    using Vec3 = Eigen::Vector3f;

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
     *  @param tnear The near clipping plane.
     *  @param tfar The far clipping plane.
     */
    Camera(const Vec3& position,
           const Vec3& focus,
           const Vec3& up,
           float tnear,
           float tfar)
    {
        pos = position;
        dir = (position - focus).normalized();
        u = up.cross(dir).normalized();
        v = dir.cross(u);
        this->tnear = tnear;
        this->tfar = tfar;
    }

    virtual ~Camera();

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

    /** Set the range of the ray.
     *
     *  @param tnear The near range of ray.
     *  @param tfar The far range of ray.
     */
    void set_range(float tnear, float tfar)
    {
        this->tnear = tnear;
        this->tfar = tfar;
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

    /** The near plane.
     *
     */
    float tnear = 0.0f;

    /** The far plane.
     *
     */
    float tfar = std::numeric_limits<float>::max();
};

inline Camera::~Camera() {}

struct Transform
{
    /**The model matrix.
     *
     */
    alignas(32) Eigen::Matrix4f model_matrix;

    /**The model-view-projection matrix.
     *
     */
    alignas(32) Eigen::Matrix4f mvp;

    /**The normal matrix.
     *
     * Inverse transpose of model matrix.
     */
    alignas(32) Eigen::Matrix4f normal_matrix;

    /** Initialize to identity matrices.
     *
     */
    Transform()
    {
        model_matrix.setIdentity();
        mvp.setIdentity();
        normal_matrix.setIdentity();
    }

    /**Initialize from the mvp matrices.
     *
     */
    Transform(const Eigen::Matrix4f& model,
              const Eigen::Matrix4f& view,
              const Eigen::Matrix4f& projection)
    {
        model_matrix = model;
        mvp = projection * view * model;
        normal_matrix = model.inverse().transpose();
    }
};

/** A simple Lambertian material model.
 *
 */
struct Material
{
    /** Ambient color, in range [0, 1].
     *
     */
    alignas(16) Eigen::Array3f ambient;

    /** Diffuse color, in range [0, 1].
     *
     */
    alignas(16) Eigen::Array3f diffuse;
};

/**A simple point light model.
 *
 */
struct Light
{
    /**Light position in world space.
     *
     */
    alignas(16) Eigen::Array3f position;

    /**Light color, in range [0, 1].
     *
     */
    alignas(16) Eigen::Array3f color;

    /**Light intensity, in range [0, 1].
     *
     */
    alignas(4) float intensity;
};

/** @}*/
} // namespace Euclid
