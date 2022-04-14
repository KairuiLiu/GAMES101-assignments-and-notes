#include<cmath>
#include<eigen3/Eigen/Core>
#include<eigen3/Eigen/Dense>
#include<iostream>

int main(){

    // Basic Example of cpp
    std::cout << "Example of cpp \n";
    float a = 1.0, b = 2.0;
    std::cout << a << std::endl;
    std::cout << a/b << std::endl;
    std::cout << std::sqrt(b) << std::endl;
    std::cout << std::acos(-1) << std::endl;                    // pi
    std::cout << std::sin(30.0/180.0*acos(-1)) << std::endl;    // 注意转为弧度制

    // Example of vector
    std::cout << "Example of vector \n";
    // vector definition
    Eigen::Vector3f v(1.0f,2.0f,3.0f);
    Eigen::Vector3f w(1.0f,0.0f,0.0f);
    // vector output
    std::cout << "Example of output \n";
    std::cout << v << std::endl;
    // vector add
    std::cout << "Example of add \n";
    std::cout << v + w << std::endl;
    // vector scalar multiply
    std::cout << "Example of scalar multiply \n";
    std::cout << v * 3.0f << std::endl;
    std::cout << 2.0f * v << std::endl;

    // 📖-1 vector cross product
    // |  0 -z  y |   | x |
    // |  z  0 -x | X | y |
    // | -y  x  0 |   | z |
    {
        std::cout << "Example of cross product(v x w) \n";
        const float x = v[0],y = v[1],z = v[2]; 
        Eigen::Matrix3f leftMatrix;
        leftMatrix << 0.0, -z, y, z, 0.0, -x, -y, x, 0.0;
        std::cout << leftMatrix * w << std::endl;
    }

    // Example of matrix
    std::cout << "Example of matrix \n";
    // matrix definition
    Eigen::Matrix3f i,j;
    i << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0;
    j << 2.0, 3.0, 1.0, 4.0, 6.0, 5.0, 9.0, 7.0, 8.0;
    // matrix output
    std::cout << "Example of output \n";
    std::cout << i << std::endl;
    // 📖-2 matrix add i + j
    {
        std::cout << "Example matrix add i + j \n ";
        std::cout << i + j << std::endl;
    }
    // 📖-3 matrix scalar multiply i * 2.0
    {
        std::cout << "Example matrix scalar multiply i * 2.0 \n ";
        std::cout << i * 2.0 << std::endl;
    }
    // 📖-4 matrix multiply i * j
    {
        std::cout << "Example matrix multiply i * j \n ";
        std::cout << i * j << std::endl;
    }
    // 📖-5 matrix multiply vector i * v
    {
        std::cout << "Example matrix multiply vector i * v \n ";
        std::cout << i * v << std::endl;
    }

    /* 
    * 📖-6 PA 0 
    */
    // TO DO: Define point P
    Eigen::Vector3f P(2.0f,1.0f,1.0f);
    // TO DO: Define rotation matrix M
    // |  cosx -sinx  tx |
    // |  sinx  cosx  ty | X P = P'
    // |     0     0   1 |
    Eigen::Matrix3f M;
    const float sin45 = std::sin(45.0/180.0*acos(-1));
    const float cos45 = std::cos(45.0/180.0*acos(-1));
    M << cos45, -sin45, 1.0, sin45, cos45, 2.0, 0.0, 0.0, 1.0;
    // TO DO: M * P
    Eigen::Vector3f PP = M * P;
    std::cout << PP << std::endl;
    return 0;
}