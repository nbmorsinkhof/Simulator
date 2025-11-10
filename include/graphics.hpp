#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <QApplication>
#include <QWidget>
#include <QTimer>
#include <QPointF>
#include <QLineF>
#include <QRectF>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <string>
#include <stdbool.h>
#include <vector>
#include <array>
#include <unordered_map>
#include <memory>

#include "simulator.hpp"


class SystemObjects;
/***************************** GEOMETRIC OBJECTS****************************** */
/***************************** GEOMETRIC OBJECTS****************************** */
/***************************** GEOMETRIC OBJECTS****************************** */


enum class ShapeType{
    LINE,
    CIRCLE,
    RECT
};

struct QCircle{
    QPointF center;
    qreal radius;
};

struct CanvasConfig{
    int PIXEL_WIDTH;
    int PIXEL_HEIGHT;
    std::array<float, 2> x_world_limits;
    std::array<float, 2> y_world_limits;
    float x_origin;
    float y_origin;
};

struct GeometricObject{
    GeometricObject(std::string n, CanvasConfig& canv_conf)
        : name(std::move(n)), canvas_config(canv_conf){}
    std::string name;
    ShapeType shape_type;
    CanvasConfig& canvas_config;
    std::array<float, 2> position_world = {0, 0};
    virtual void update() {};
    virtual void draw(QPainter& p) {};
    void worldToPixel(std::array<float, 2> world_pos, QPointF& p);
};

//Circle
struct CircleObject : GeometricObject{
    CircleObject(std::string n, CanvasConfig& canv_conf)
        : GeometricObject(std::move(n), canv_conf)
        {
            shape_type = ShapeType::CIRCLE;
        }
    //members
    QCircle geometry;
    //functions
    void update() override;
    void draw(QPainter& p) override;
};
//Rectangle
struct RectangleObject : GeometricObject{
    RectangleObject(std::string n, CanvasConfig& canv_conf)
        : GeometricObject(std::move(n), canv_conf)
        {
            shape_type = ShapeType::RECT;
        }
    //members
    QRectF geometry;
    //functions
    void update() override;
    void draw(QPainter& p) override;
};
//Line
struct LineObject : GeometricObject{
    LineObject(std::string n, CanvasConfig& canv_conf)
        : GeometricObject(std::move(n), canv_conf)
        {
            shape_type = ShapeType::LINE;
        }
    //members
    QLineF geometry;
    QPointF p1;
    QPointF p2;
    std::array<float, 2> base_world = {0, 0};
    //functions
    void update() override;
    void draw(QPainter& p) override;
};

/*************************************CANVAS BASE*************************************** */
/*************************************CANVAS BASE*************************************** */
/*************************************CANVAS BASE*************************************** */
class CanvasBase: public QWidget{
public:
    explicit CanvasBase(QWidget* parent = nullptr, Qt::WindowFlags f = {});

    // mutator functions

    //initialize screen size
    void init_size();
    //Set system objects
    void set_system(SystemObjects* s);
    //set object world pos: link physics position to graphics position(specific to derived class)
    virtual void setObjectWorldPos();
    //Get geometric object: Usually used insided "setObjectWorldPos" to acces object position
    GeometricObject* get_GeometricObject(std::string name);

protected:
    //Draw objects and update the pixel coordinates
    void paintEvent(QPaintEvent* event);

    SystemObjects* SystemObjects_ = nullptr;
    virtual void init();
    std::unordered_map<std::string, std::unique_ptr<GeometricObject>> objects_;

    std::array<float, 2> x_world_limits_ = {-2.0, 2.0};
    std::array<float, 2> y_world_limits_ = {-2.0, 2.0};
    float X_origin_ = 2.0;
    float Y_origin_ = 2.0;

    const int PIXEL_WIDTH_ = 800;
    const int PIXEL_HEIGHT_ = 800;

    CanvasConfig canvas_config_{
        PIXEL_WIDTH_,
        PIXEL_HEIGHT_,
        x_world_limits_,
        y_world_limits_,
        X_origin_,
        Y_origin_
    };
};






/************************ CANVAS ******************************* */
class Canvas: public QWidget{
public:
    explicit Canvas(QWidget* parent = nullptr, Qt::WindowFlags f = {});

    // mutator functions

    //initialize screen size
    void init_size();
    //Set system objects
    void set_system(SystemObjects* s);
    //set object world pos: link physics position to graphics position(specific to derived class)
    void setObjectWorldPos();
    //Get geometric object: Usually used insided "setObjectWorldPos" to acces object position
    GeometricObject* get_GeometricObject(std::string name);

protected:
    //paint objects
    void paintEvent(QPaintEvent* event) override;

    void keyPressEvent(QKeyEvent* e) override;

private:
    SystemObjects* SystemObjects_ = nullptr;
    void init();
    std::unordered_map<std::string, std::unique_ptr<GeometricObject>> objects_;

    std::array<float, 2> x_world_limits_ = {-1.0, 1.0};
    std::array<float, 2> y_world_limits_ = {-1.0, 1.0};
    float X_origin_ = 1.0;
    float Y_origin_ = 1.0;

    const int PIXEL_WIDTH_ = 800;
    const int PIXEL_HEIGHT_ = 800;

    CanvasConfig canvas_config_{
        PIXEL_WIDTH_,
        PIXEL_HEIGHT_,
        x_world_limits_,
        y_world_limits_,
        X_origin_,
        Y_origin_
    };

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


/*********************CANVAS SEGWAY********************************* */

class CanvasSegway : public CanvasBase{
public:
    explicit CanvasSegway(QWidget* parent = nullptr, Qt::WindowFlags f={});

    void setObjectWorldPos() override;

private:
    void init() override;

};




/*******************CANVAS PENDULUM**************************** */
/*******************CANVAS PENDULUM**************************** */
/*******************CANVAS PENDULUM**************************** */
class CanvasPendulum : public CanvasBase{
public:
    explicit CanvasPendulum(QWidget* parent = nullptr, Qt::WindowFlags f={});

    void setObjectWorldPos() override;

private:
    void init() override;

};


/*********************GRAPHICS********************************* */
/*********************GRAPHICS********************************* */
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
    CanvasSegway canvas_;
    std::vector<float> observation_;
    QTimer timer_;
};


#endif