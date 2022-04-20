#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>
#define TAS(X,Y,M) (X = X+Y>M?M:X+Y)

std::vector<cv::Point2f> control_points;

void mouse_handler(int event, int x, int y, int flags, void *userdata) 
{
    if (event == cv::EVENT_LBUTTONDOWN && control_points.size() < 4) 
    {
        std::cout << "Left button of the mouse is clicked - position (" << x << ", "
        << y << ")" << '\n';
        control_points.emplace_back(x, y);
    }     
}

void naive_bezier(const std::vector<cv::Point2f> &points, cv::Mat &window) 
{
    auto &p_0 = points[0];
    auto &p_1 = points[1];
    auto &p_2 = points[2];
    auto &p_3 = points[3];

    for (double t = 0.0; t <= 1.0; t += 0.001) 
    {
        auto point = std::pow(1 - t, 3) * p_0 + 3 * t * std::pow(1 - t, 2) * p_1 +
                 3 * std::pow(t, 2) * (1 - t) * p_2 + std::pow(t, 3) * p_3;

        window.at<cv::Vec3b>(point.y, point.x)[2] = 255;
    }
}

cv::Point2f recursive_bezier(const std::vector<cv::Point2f> &control_points, float t) 
{
    // TODO: Implement de Casteljau's algorithm
    if(control_points.size()==1)return control_points[0];
    std::vector<cv::Point2f> new_points;
    for(int i=0;i<control_points.size()-1;i++){
        const auto st = control_points[i];
        const auto ed = control_points[i+1];
        new_points.push_back(st+(ed-st)*t);
    }
    return recursive_bezier(new_points, t);
}

float pert(cv::Point2f x, cv::Point2f st, float len, float vst, float ved){
    auto lenM = std::sqrt((x.x-st.x)*(x.x-st.x)+(x.y-st.y)*(x.y-st.y));
    std::cout<<lenM/len<<"  -  "<<vst + (ved-vst)*lenM/len<<std::endl;
    if(lenM>len)return ved;
    return vst + (ved-vst)*lenM/len;
}

void bezier(const std::vector<cv::Point2f> &control_points, cv::Mat &window) 
{
    // TODO: Iterate through all t = 0 to t = 1 with small steps, and call de Casteljau's 
    // recursive Bezier algorithm.
    
    for (double t = 0.0; t <= 1.0; t += 0.0001) 
    {
        auto point = recursive_bezier(control_points, t);
        window.at<cv::Vec3b>(point.y, point.x)[1] = 255;
    }
}

void bezier_anti(const std::vector<cv::Point2f> &control_points, cv::Mat &window) 
{
    // TODO: Iterate through all t = 0 to t = 1 with small steps, and call de Casteljau's 
    // recursive Bezier algorithm.
    
    for (double t = 0.0; t <= 1.0; t += 0.0001) 
    {
        auto point = recursive_bezier(control_points, t);
        window.at<cv::Vec3b>(point.y, point.x)[1] = 255;
    }

    for(int i=0; i<window.rows;i++){
        for(int j=0; j<window.cols;j++){
            int add = (
                window.at<cv::Vec3b>(j, std::max(i-1,0))[1]+
                window.at<cv::Vec3b>(j, std::min(i+1,window.rows-1))[1]+
                window.at<cv::Vec3b>(std::max(j-1, 0), i)[1]+
                window.at<cv::Vec3b>(std::max(j-1, 0), std::max(i-1,0))[1]+
                window.at<cv::Vec3b>(std::max(j-1, 0), std::min(i+1,window.rows-1))[1]+
                window.at<cv::Vec3b>(std::min(j+1, window.cols-1), i)[1]+
                window.at<cv::Vec3b>(std::min(j+1, window.cols-1), std::max(i-1,0))[1]+
                window.at<cv::Vec3b>(std::min(j+1, window.cols-1), std::min(i+1,window.rows-1))[1]
            )/8;
            TAS(window.at<cv::Vec3b>(j, i)[1], add, 255);
        }
    }
}

int main() 
{
    cv::Mat window = cv::Mat(700, 700, CV_8UC3, cv::Scalar(0));
    cv::cvtColor(window, window, cv::COLOR_BGR2RGB);
    cv::namedWindow("Bezier Curve", cv::WINDOW_AUTOSIZE);

    cv::setMouseCallback("Bezier Curve", mouse_handler, nullptr);

    int key = -1;
    while (key != 27) 
    {
        for (auto &point : control_points) 
        {
            cv::circle(window, point, 3, {255, 255, 255}, 3);
        }

        if (control_points.size() == 4) 
        {
            bezier_anti(control_points, window);

            for(int i=0;i<4;i++){
                control_points[i].y+=100;
            }
            bezier(control_points, window);
            
            for(int i=0;i<4;i++){
                control_points[i].y+=100;
            }
            naive_bezier(control_points, window);

            cv::imshow("Bezier Curve", window);
            cv::imwrite("my_bezier_curve.png", window);
            key = cv::waitKey(0);

            return 0;
        }

        cv::imshow("Bezier Curve", window);
        key = cv::waitKey(20);
    }

return 0;
}
