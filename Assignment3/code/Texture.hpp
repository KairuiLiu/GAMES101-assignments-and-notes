//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_TEXTURE_H
#define RASTERIZER_TEXTURE_H
#include "global.hpp"
#include <eigen3/Eigen/Eigen>
#include <opencv2/opencv.hpp>
class Texture{
private:
    cv::Mat image_data;

public:
    Texture(const std::string& name)
    {
        image_data = cv::imread(name);
        cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
        width = image_data.cols;
        height = image_data.rows;
    }

    int width, height;

    Eigen::Vector3f getColor(float u, float v)
    {
        auto u_img = u * width;
        auto v_img = (1 - v) * height;
        auto color = image_data.at<cv::Vec3b>(v_img, u_img);
        return Eigen::Vector3f(color[0], color[1], color[2]);
    }

    // 📖-10 实现双线性插值
    Eigen::Vector3f getColorBilinear(float u, float v)
    {
        auto u_img = u * width;
        auto v_img = (1 - v) * height;

        auto ul = floor(u_img - 0.5)+0.5;
        auto vl = floor(v_img - 0.5)+0.5;

        auto color_lb = image_data.at<cv::Vec3b>(vl, ul);
        auto color_lt = image_data.at<cv::Vec3b>(vl, ul+1.0);
        auto color_rb = image_data.at<cv::Vec3b>(vl+1.0, ul);
        auto color_rt = image_data.at<cv::Vec3b>(vl+1.0, ul+1.0);

        auto color_b = color_lb + (v_img-vl) * (color_rb-color_lb);
        auto color_t = color_lt + (v_img-vl) * (color_rt-color_lt);
        auto color = color_b + (u_img-ul) * (color_t-color_b);

        return Eigen::Vector3f(color[0], color[1], color[2]);
    }

};
#endif //RASTERIZER_TEXTURE_H
