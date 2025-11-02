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

/**********************OBJECTS**************************** */
void GeometricObject::worldToPixel(std::array<float, 2> world_pos, QPointF& p){
    p.setX(canvas_config.PIXEL_WIDTH*(world_pos[0] + canvas_config.x_origin)/(canvas_config.x_world_limits[1]-canvas_config.x_world_limits[0]));
    p.setY(canvas_config.PIXEL_HEIGHT*(world_pos[1] + canvas_config.y_origin)/(canvas_config.y_world_limits[1]-canvas_config.y_world_limits[0]));
}

//Circle
void CircleObject::update(){
    worldToPixel(position_world, geometry.center);
}

void CircleObject::draw(QPainter& p){
    p.drawEllipse(geometry.center, geometry.radius, geometry.radius);
}
//Line 
void LineObject::update(){
    worldToPixel(base_world, p1);
    worldToPixel(position_world, p2);
    geometry.setP1(p1);
    geometry.setP2(p2);
}

void LineObject::draw(QPainter& p){
    p.drawLine(geometry);
}

/******************************CANVAS***************************************************** */

Canvas::Canvas(QWidget* parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    {
        init();
    }

//init
void Canvas::init(){
        
    objects_.emplace_back(std::make_unique<CircleObject>("pendulum mass", canvas_config_));
    objects_.emplace_back(std::make_unique<LineObject>("pendulum arm", canvas_config_));
    
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
//get functions
GeometricObject* Canvas::get_GeometricObject(std::string name){
        for (const auto& obj: objects_){
            if (obj && obj->name==name){
                return obj.get();
                break;
            }
        }
        return nullptr;
    }
// Mutator functions(This function is specific for every canvas)
void Canvas::setObjectWorldPos(){
    const auto& obs = SystemObjects_->state_space.get_observation();

    if (auto* mass = get_GeometricObject("pendulum_mass")){
        mass->position_world[0] = obs[0];
        mass->position_world[1] = obs[1];
    }
    if (auto* armBase = get_GeometricObject("pendulum_arm")){
        if(auto* arm = dynamic_cast<LineObject*>(armBase)){
            arm->position_world[0] = obs[0];
            arm->position_world[1] = obs[1];
            arm->base_world[0] = 0.0;
            arm->base_world[1] = 0.0;
        }
        
    }
    
    //update();
}
       
// Draw functions
void Canvas::paintEvent(QPaintEvent* event){
    QPainter p(this);
    for(const auto& obj: objects_){
        obj->draw(p);
    }
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



/******************************CANVAS SEGWAY***************************************************** */


/******************************CANVAS SEGWAY***************************************************** */







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
    canvas_.update();
}


