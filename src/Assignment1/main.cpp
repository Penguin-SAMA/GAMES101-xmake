// clang-format off
#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

// 视图变换
Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos) {
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    // 将相机平移回原点
    translate << 1, 0, 0, -eye_pos[0],
                 0, 1, 0, -eye_pos[1],
                 0, 0, 1, -eye_pos[2],
                 0, 0, 0, 1;

    view = translate * view;

    return view;
}

// 模型变换
Eigen::Matrix4f get_model_matrix(float rotation_angle) {
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.
    float a = rotation_angle * MY_PI / 180;

    Eigen::Matrix4f m;
    m << cos(a), -sin(a), 0, 0,
         sin(a),  cos(a), 0, 0,
              0,       0, 1, 0,
              0,       0, 0, 1;

    model = m * model;

    return model;
}

// 投影变换
// eye_fov: 垂直的可视角度
// aspect_ratio: 宽高比
// zNear, zFar: 远近平面的z轴坐标
Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar) {
    // Students will implement this function

    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.
    Eigen::Matrix4f m;
    m << zNear, 0, 0, 0,
         0, zNear, 0, 0,
         0, 0, zNear + zFar, -zNear * zFar,
         0, 0, 1, 0;

    // 视角的角度的一半
    float halve = eye_fov / 2 * MY_PI / 180;
    // 上下边框坐标
    float top = tan(halve) * zNear;
    float bottom = -top;
    // 左右边框坐标
    float right = top * aspect_ratio;
    float left = -right;

    Eigen::Matrix4f n, p;
    // 正交投影中的两个矩阵
    n << 2/(right - left), 0, 0, 0,
         0, 2/(top - bottom), 0, 0,
         0, 0, 2/(zNear - zFar), 0,
         0, 0, 0, 1;
    
    p << 1, 0, 0, -(right + left)/2,
         0, 1, 0, -(top + bottom)/2,
         0, 0, 1, -(zNear + zFar)/2,
         0, 0, 0, 1;

    projection = n * p * m;

    return projection;
}

int main(int argc, const char **argv) {
    float angle = 0;
    bool command_line = false;
    std::string filename = "output.png";

    if (argc >= 3) {
        // 如果在命令行中输入-r + 度数，command_line改为true
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc == 4) {
            // 在命令行中输入四个参数，修改文件名
            filename = std::string(argv[3]);
        } else
            return 0;
    }

    // 屏幕宽高
    rst::rasterizer r(700, 700);

    // 相机的坐标
    Eigen::Vector3f eye_pos = {0, 0, 5};

    // 三角形三个定点坐标
    std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

    // 三个坐标的索引
    std::vector<Eigen::Vector3i> ind{{0, 1, 2}};

    // 下面两个函数将坐标点和索引值加载进光栅化器
    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;

    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        // 加载MVP矩阵到光栅化器
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

        r.set_model(get_model_matrix(angle));
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
        } else if (key == 'd') {
            angle -= 10;
        }
    }

    return 0;
}

// clang-format on
