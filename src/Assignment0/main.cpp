#include <cmath>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include <iostream>

#define MY_PI 3.1415926

int main() {
    Eigen::Vector3f p(2.0f, 1.0f, 1.0f);

    float randian = 45 * MY_PI / 180;

    Eigen::Matrix3f m;
    m << cos(randian), -sin(randian), 1.0f, sin(randian), cos(randian), 2.0f,
        0.0f, 0.0f, 1.0f;

    p = m * p;

    std::cout << p;
}
