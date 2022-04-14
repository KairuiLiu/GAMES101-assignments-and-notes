#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
#include <iostream>

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1,
        -eye_pos[2], 0, 0, 0, 1;

    view = translate * view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

    // 📖-1 Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.
    // | cosx -sinx  0  0 |
    // | sinx  cosx  0  0 |
    // |    0     0  1  0 |
    // |    0     0  0  1 |
    const float angle = rotation_angle / 180 * MY_PI;
    const float cosx = std::cos(angle);
    const float sinx = std::sin(angle);
    model << cosx, -sinx, 0.0, 0.0, sinx, cosx, 0.0, 0.0, 0.0 ,0.0 ,1.0, 0.0, 0.0, 0.0, 0.0, 1.0; 
    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)
{
    // Students will implement this function

    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

    // 📖-2 Create the projection matrix for the given parameters.
    // Then return it.
    // | n  0    0    0 |
    // | 0  n    0    0 |
    // | 0  0  n+f  -nf |
    // | 0  0    0    1 |
    projection << zNear, 0.0, 0.0, 0.0, 0.0, zNear, 0.0, 0.0, 0.0 ,0.0 , zNear+zFar, -zNear*zFar, 0.0, 0.0, 0.0, 1.0; 
    return projection;
}

Eigen::Matrix4f getCrossProductMatrix4(Vector4f v){
    // 📖-3 create a function to get cross product matrix
    Eigen::Matrix4f crossMatrix;
    crossMatrix <<  0.0,   -v[2],  v[1],  0.0, 
                    v[2],  0.0,    -v[0], 0.0, 
                    -v[1], v[0],   0.0,   0.0,
                    0.0,   0.0,    0.0,   0.0;
    return crossMatrix;
}

Eigen::Matrix4f get_rotation(Vector3f axis = Eigen::Vector3f(0.0,0.0,1.0), float angle = 0){
    // 📖-4 create a the rotation transformation matrix around any axis passing through the origin
    // R(n,a) = cos(a) I + (1-cos(a))nnT + sin(a)[cross]
    const Eigen::Vector4f axis4 = Eigen::Vector4f(axis[0],axis[1],axis[2],0.0);
    const float radianAngle = angle / 180.0 * MY_PI;
    const float cosa = std::cos(radianAngle);
    const float sina = std::sin(radianAngle);
    Eigen::Matrix4f rotation = cosa * Eigen::Matrix4f::Identity() + (1-cosa) * axis4 * axis4.transpose() + sina * getCrossProductMatrix4(axis4);
    rotation(3,3) = 1.0;
    return rotation;
}

int main(int argc, const char** argv)
{
    float t = std::max(1.0,2.0);
    std::cout<< t;
    return 0;
}
