#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <QApplication>
#include <QWidget>
#include <vector>
#include <QTimer>
#include <QPointF>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <string>
#include <stdbool.h>

#include "simulator.hpp"


class SystemObjects;

/*********************CANVAS********************************* */


//Object on canvas

struct PhysicalObject{
    std::string name;
    std::string type;
    QPointF position_pxl = {0, 0};
    QPointF base_pxl = {0, 0};
    std::vector<float> position_world = {0, 0};
    std::vector<float> base_world = {0, 0};
    float orientation = 0.0;
    float size = 50;
    bool visible;
};

class Canvas: public QWidget{
public:
    explicit Canvas(QWidget* parent = nullptr, Qt::WindowFlags f = {});

    // mutator functions
    void set_circle_pos(float x, float y);

    void init_size();

    void set_system(SystemObjects* s);

    void setObjectWorldPos(const std::string&, float, float);

protected:
    void paintEvent(QPaintEvent* event) override;
    void drawObject(PhysicalObject& obj, QPainter& p);
    void worldToPixelTransform(PhysicalObject&);

    void keyPressEvent(QKeyEvent* e) override;

private:
    SystemObjects* SystemObjects_ = nullptr;
    void init();
    std::vector<PhysicalObject> objects_;

    std::vector<float> x_world_limits_ = {-1.0, 1.0};
    std::vector<float> y_world_limits_ = {-1.0, 1.0};
    float X_origin_ = 1.0;
    float Y_origin_ = 1.0;

    const int PIXEL_WIDTH_ = 800;
    const int PIXEL_HEIGHT_ = 800;

    //buttons and control
    QPushButton* button_start_; 
    QPushButton* button_stop_;
    QLineEdit* box_setArmLength_;
    QPushButton* button_ConfirmArmLength_;
    std::string input_ArmLength_ = "";



    //button functions
    void button_start_clicked();
    void button_stop_clicked();
    void handleBoxArmLength(const QString& text);
    void button_ConfirmArmLength_clicked();


};

/*********************GRAPHICS********************************* */
class Graphics : public QObject{

public:
    //constructor
    explicit Graphics(int& arg, char** argv);
    //public functions
    void set_system(SystemObjects* s);
    int run();

private:
    void init();
    void timer_function();
    void world_to_pixel_transform(float x_world, float y_world);

    SystemObjects* SystemObjects_ = nullptr;
    QApplication app_;
    Canvas canvas_;
    std::vector<float> observation_;
    QTimer timer_;
};


#endif