#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

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

    Eigen::Matrix4f Mortho1;
    Eigen::Matrix4f Mortho2;
    Eigen::Matrix4f Mpersptoortho;

    // 📖-2 Create the projection matrix for the given parameters.
    // Then return it.
    // | n  0    0    0 |
    // | 0  n    0    0 |
    // | 0  0  n+f  -nf |
    // | 0  0    0    1 |
    float top = -std::abs(zNear) * std::tan(eye_fov / 2 / 180 * MY_PI);
    const float right = top * aspect_ratio;
    Mortho1 << 1/right, 0, 0, 0,
                0, 1/top, 0, 0,
                0, 0, 1/(zNear-zFar), 0,
                0, 0, 0, 1;
    Mortho2 << 1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, -(zNear+zFar)/2,
                0, 0, 0, 1;
    Mpersptoortho << zNear, 0.0, 0.0, 0.0,
                    0.0, zNear, 0.0, 0.0, 
                    0.0 ,0.0 , zNear+zFar, -zNear*zFar, 
                    0.0, 0.0, 1.0, 0.0; 
    return Mortho1 * Mortho2 * Mpersptoortho;
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
    float angle = 0;
    bool command_line = false;
    std::string filename = "output.png";
    // 📖-5 the axis we want to rotate
    Eigen::Vector3f axis(0.0, 0.0, -1.0);

    if (argc >= 3) {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc == 4) {
            filename = std::string(argv[3]);
        }
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = {0, 0, 5};

    std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

    std::vector<Eigen::Vector3i> ind{{0, 1, 2}};

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;

    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);

        return 0;
    }

    while (key != 27) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        // 📖-6 replace the model matrix with rotation matrix
        // r.set_model(get_model_matrix(angle));
        r.set_model(get_rotation(axis, angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';

        if (key == 'a') {
            angle += 10;
        }
        else if (key == 'd') {
            angle -= 10;
        }
        // 📖-7 use 's' to reset the axis we want to rotate with 
        else if (key == 's') {
            float x, y, z;
            std::cout<< "input the new axis: \n > ";
            std::cin >> x >> y >> z;
            axis = Eigen::Vector3f(x,y,z);
            angle = 0;
        }
    }

    return 0;
}
