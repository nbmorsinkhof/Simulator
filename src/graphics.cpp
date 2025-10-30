#include "graphics.hpp"
#include "simulator.hpp"
#include "system.hpp"
#include <QPainter>    
#include <QPaintEvent> 
#include <QWidget>
#include <QObject>
#include <QPointF>
#include <QKeyEvent>
#include <QLineEdit>

#include <iostream>
#include <stdbool.h>
#include <string>



/******************************CANVAS***************************************************** */

Canvas::Canvas(QWidget* parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    {
        init();
    }

//init
void Canvas::init(){
    PhysicalObject obj_pendulum_mass;
    obj_pendulum_mass.name = "pendulum_mass";
    obj_pendulum_mass.type = "circle";
    objects_.push_back(std::move(obj_pendulum_mass));  

    PhysicalObject obj_pendulum_arm;
    obj_pendulum_arm.name = "pendulum_arm";
    obj_pendulum_arm.type = "line";
    obj_pendulum_arm.base_world[0] = 0;
    obj_pendulum_arm.base_world[1] = 0;
    objects_.push_back(std::move(obj_pendulum_arm));  

    //buttons init
    button_start_ = new QPushButton("Start", this);
    QObject::connect(button_start_, &QPushButton::clicked, this, &Canvas::button_start_clicked);

    button_stop_ = new QPushButton("Stop", this);
    button_stop_->setGeometry(0, 24, 80, 24);
    QObject::connect(button_stop_, &QPushButton::clicked, this, &Canvas::button_stop_clicked);

    box_setArmLength_ = new QLineEdit("Arm Length", this);
    box_setArmLength_->setGeometry(0, 48, 80, 24);
    QObject::connect(box_setArmLength_, &QLineEdit::textChanged, this, &Canvas::handleBoxArmLength);

    button_ConfirmArmLength_ = new QPushButton("Set", this);
    button_ConfirmArmLength_->setGeometry(80, 48, 48, 24);
    QObject::connect(button_ConfirmArmLength_, &QPushButton::clicked, this, &Canvas::button_ConfirmArmLength_clicked);

}  
void Canvas::init_size(){
    resize(PIXEL_WIDTH_, PIXEL_HEIGHT_);
}

void Canvas::set_system(SystemObjects* s){
    SystemObjects_ = s;
}

// Mutator functions
void Canvas::setObjectWorldPos(const std::string& name, float x, float y){
    for (auto& o : objects_) {
        if (o.name == name) {
            o.position_world[0] = x;
            o.position_world[1] = y;
            worldToPixelTransform(o);
            break;
        }
    }
    update();
}
       
// Draw functions
void Canvas::paintEvent(QPaintEvent* event){
    QPainter p(this);
    for(int i=0; i<objects_.size(); i++){
        drawObject(objects_[i], p);
    }
}

void Canvas::drawObject(PhysicalObject& obj, QPainter& p){
    p.save();
    if(obj.type=="circle"){
        std::cout<<"in draw object: " << obj.position_pxl.x()<<std::endl;
        p.drawEllipse(obj.position_pxl, obj.size, obj.size);
    }
    else if(obj.type=="line"){
        p.drawLine(obj.base_pxl, obj.position_pxl);
    }
}

void Canvas::worldToPixelTransform(PhysicalObject& obj){
    float x_world = obj.position_world[0];
    float y_world = obj.position_world[1];
    obj.position_pxl.setX(PIXEL_WIDTH_*(x_world + X_origin_)/(x_world_limits_[1]-x_world_limits_[0]));
    obj.position_pxl.setY(PIXEL_HEIGHT_*(y_world + Y_origin_)/(y_world_limits_[1]-y_world_limits_[0]));

    obj.base_pxl.setX(PIXEL_WIDTH_*(obj.base_world[0] + X_origin_)/(x_world_limits_[1]-x_world_limits_[0]));
    obj.base_pxl.setY(PIXEL_HEIGHT_*(obj.base_world[1] + Y_origin_)/(y_world_limits_[1]-y_world_limits_[0]));
}
// Event functions
void Canvas::keyPressEvent(QKeyEvent* e){
    if(e->key()==Qt::Key_Space){
        SystemObjects_->system_interface.bStart = true;
        //std::cout<<"************************************************************************************************************************************************key space pressed************************************"<<std::endl;
        e->accept();
    }
    QWidget::keyPressEvent(e);
}

// button event
void Canvas::button_start_clicked(){
    SystemObjects_->system_interface.bStart = true;
}
void Canvas::button_stop_clicked(){
    SystemObjects_->system_interface.bStop = true;
}

void Canvas::handleBoxArmLength(const QString& text){
    input_ArmLength_ = text.toUtf8().constData();
}

void Canvas::button_ConfirmArmLength_clicked(){
    if(SystemObjects_->system_interface.state == STOP){
        std::cout<<"button confirmed clicked**************************************************************"<<std::stof(input_ArmLength_)<<std::endl;
        SystemObjects_->simulator.set_length_arm(std::stof(input_ArmLength_));
    };
}
/****************************** GRAPHICS******************************** */
Graphics::Graphics(
    int& arg,
    char** argv
) : app_(arg, argv), canvas_(nullptr, Qt::Widget)
{
    init();
}

void Graphics::init(){
    QObject::connect(&timer_, &QTimer::timeout, this, &Graphics::timer_function);
    timer_.start(16);
    canvas_.setFocusPolicy(Qt::StrongFocus);
}

void Graphics::set_system(SystemObjects* s){
    SystemObjects_ = s;
    canvas_.set_system(s);
}

int Graphics::run(){
    canvas_.init_size();
    canvas_.show();
    app_.exec();
}

void Graphics::timer_function(){
    observation_ = SystemObjects_->simulator.get_observation();
    canvas_.setObjectWorldPos( "pendulum_mass", observation_[0], observation_[1]);
    canvas_.setObjectWorldPos("pendulum_arm", observation_[0], observation_[1]);
    std::cout<<"obs_0: "<<observation_[0]<<"obs_1"<<observation_[1]<<std::endl;
    canvas_.update();
}


