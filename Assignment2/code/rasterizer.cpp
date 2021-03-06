// clang-format off
//
// Created by goksu on 4/6/19.
//

#include <algorithm>
#include <vector>
#include "rasterizer.hpp"
#include <opencv2/opencv.hpp>
#include <math.h>


rst::pos_buf_id rst::rasterizer::load_positions(const std::vector<Eigen::Vector3f> &positions)
{
    auto id = get_next_id();
    pos_buf.emplace(id, positions);

    return {id};
}

rst::ind_buf_id rst::rasterizer::load_indices(const std::vector<Eigen::Vector3i> &indices)
{
    auto id = get_next_id();
    ind_buf.emplace(id, indices);

    return {id};
}

rst::col_buf_id rst::rasterizer::load_colors(const std::vector<Eigen::Vector3f> &cols)
{
    auto id = get_next_id();
    col_buf.emplace(id, cols);

    return {id};
}

auto to_vec4(const Eigen::Vector3f& v3, float w = 1.0f)
{
    return Vector4f(v3.x(), v3.y(), v3.z(), w);
}


static bool insideTriangle(float x, float y, const Vector3f* _v)
{   
    // 📖-2 use cross product to check whether point in the triangle.
    float cnt = 0;
    for(int i=0;i<3;i++){
        const float x1 = x - _v[i].x();
        const float x2 = x - _v[(i+1)%3].x();
        const float y1 = y - _v[i].y();
        const float y2 = y - _v[(i+1)%3].y();
        const float v = x1 * y2 - x2 * y1;
        if(v != 0 && v * cnt >= 0) cnt += v;
        else if(v != 0)return false;
    }
    return true;
}

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector3f* v)
{
    float c1 = (x*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*y + v[1].x()*v[2].y() - v[2].x()*v[1].y()) / (v[0].x()*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*v[0].y() + v[1].x()*v[2].y() - v[2].x()*v[1].y());
    float c2 = (x*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*y + v[2].x()*v[0].y() - v[0].x()*v[2].y()) / (v[1].x()*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*v[1].y() + v[2].x()*v[0].y() - v[0].x()*v[2].y());
    float c3 = (x*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*y + v[0].x()*v[1].y() - v[1].x()*v[0].y()) / (v[2].x()*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*v[2].y() + v[0].x()*v[1].y() - v[1].x()*v[0].y());
    return {c1,c2,c3};
}

void rst::rasterizer::draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type)
{
    auto& buf = pos_buf[pos_buffer.pos_id];
    auto& ind = ind_buf[ind_buffer.ind_id];
    auto& col = col_buf[col_buffer.col_id];

    float f1 = (50 - 0.1) / 2.0;
    float f2 = (50 + 0.1) / 2.0;

    Eigen::Matrix4f mvp = projection * view * model;
    for (auto& i : ind)
    {
        Triangle t;
        Eigen::Vector4f v[] = {
                mvp * to_vec4(buf[i[0]], 1.0f),
                mvp * to_vec4(buf[i[1]], 1.0f),
                mvp * to_vec4(buf[i[2]], 1.0f)
        };
        //Homogeneous division
        for (auto& vec : v) {
            vec /= vec.w();
        }
        //Viewport transformation
        for (auto & vert : v)
        {
            vert.x() = 0.5*width*(vert.x()+1.0);
            vert.y() = 0.5*height*(vert.y()+1.0);
            vert.z() = vert.z() * f1 + f2;
        }

        for (int i = 0; i < 3; ++i)
        {
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
        }

        auto col_x = col[i[0]];
        auto col_y = col[i[1]];
        auto col_z = col[i[2]];

        t.setColor(0, col_x[0], col_x[1], col_x[2]);
        t.setColor(1, col_y[0], col_y[1], col_y[2]);
        t.setColor(2, col_z[0], col_z[1], col_z[2]);

        rasterize_triangle(t);
    }
}

//Screen space rasterization
void rst::rasterizer::rasterize_triangle(const Triangle& t) {
    auto v = t.toVector4();
    
    // 📖-3 calc the left, top, right, buttom bound box .

    // we get v[0-2], each v[x] = (x,y,z,1)T

    const float INF = 1.0 * 0x3f3f3f3f;
    float lf = INF, rt = -INF, bt = INF, tp = -INF; 
    for(int i=0;i<3;i++){
        lf = std::min(lf, v[i].x());
        rt = std::max(rt, v[i].x());
        bt = std::min(bt, v[i].y());
        tp = std::max(tp, v[i].y());
    }

    // 📖-4 calc the deepth, maintain z buffer array .

    // for(float x = std::floor(lf); x< std::ceil(rt);x+=1.0){
    //     for(float y = std::floor(bt); y< std::ceil(tp);y+=1.0){
    //         if(insideTriangle(x,y, t.v)){
    //             const float deep = depth_buf[get_index(x,y)];
    //             auto[alpha, beta, gamma] = computeBarycentric2D(x, y, t.v);
    //             float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
    //             float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
    //             z_interpolated *= w_reciprocal;
    //             if(z_interpolated < deep) {
    //                 depth_buf[get_index(x,y)] = z_interpolated;
    //                 set_pixel(Eigen::Vector3f(x,y,1), t.getColor());
    //             }
    //         }
    //     }
    // }

    // 📖-5 SSAA 2*2.
    for(float x = std::floor(lf); x< std::ceil(rt);x+=1.0){
        for(float y = std::floor(bt); y< std::ceil(tp);y+=1.0){
            int idx = 0;
            for(float delx = 0.25; delx<=1.0; delx+=0.5){
                for(float dely = 0.25; dely<=1.0; dely+=0.5){
                    const float deep = depth_buf2[4 * get_index(x,y) + idx];
                    auto[alpha, beta, gamma] = computeBarycentric2D(x+delx, y+dely, t.v);
                    float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                    float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                    z_interpolated *= w_reciprocal;
                    if(z_interpolated < deep && insideTriangle(x+delx,y+dely, t.v)) {
                        depth_buf2[4 * get_index(x,y) + idx] = z_interpolated;
                        col_buf2[4 * get_index(x,y) + idx] = t.getColor();
                    }
                    ++idx;
                }
            }
            Eigen::Vector3f color = Eigen::Vector3f{0, 0, 0};
            for(int i=0;i<4;i++)color = color + col_buf2[4 * get_index(x,y) + i];
            set_pixel(Eigen::Vector3f(x,y,1), color/4);
        }
    }
    return;
}

void rst::rasterizer::set_model(const Eigen::Matrix4f& m)
{
    model = m;
}

void rst::rasterizer::set_view(const Eigen::Matrix4f& v)
{
    view = v;
}

void rst::rasterizer::set_projection(const Eigen::Matrix4f& p)
{
    projection = p;
}

void rst::rasterizer::clear(rst::Buffers buff)
{
    if ((buff & rst::Buffers::Color) == rst::Buffers::Color)
    {
        std::fill(frame_buf.begin(), frame_buf.end(), Eigen::Vector3f{0, 0, 0});
    }
    if ((buff & rst::Buffers::Depth) == rst::Buffers::Depth)
    {
        std::fill(depth_buf.begin(), depth_buf.end(), std::numeric_limits<float>::infinity());
        std::fill(depth_buf2.begin(), depth_buf2.end(), std::numeric_limits<float>::infinity());
    }
}

rst::rasterizer::rasterizer(int w, int h) : width(w), height(h)
{
    frame_buf.resize(w * h);
    depth_buf.resize(w * h);
    depth_buf2.resize(4 * w * h);
    col_buf2.resize(4 * w * h);
}

int rst::rasterizer::get_index(int x, int y)
{
    return (height-1-y)*width + x;
}

void rst::rasterizer::set_pixel(const Eigen::Vector3f& point, const Eigen::Vector3f& color)
{
    //old index: auto ind = point.y() + point.x() * width;
    auto ind = (height-1-point.y())*width + point.x();
    frame_buf[ind] = color;
}

// 📖-6 function to get color at a pixel.
Eigen::Vector3f rst::rasterizer::get_pixel(const Eigen::Vector3f& point)
{
    //old index: auto ind = point.y() + point.x() * width;
    auto ind = (height-1-point.y())*width + point.x();
    return frame_buf[ind];
}

// clang-format on