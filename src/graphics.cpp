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
    
    std::cout<<name<<std::endl;
    std::cout<<"canvas config: "<<canvas_config.x_world_limits[0]<<", "<<canvas_config.x_world_limits[1]<<std::endl;
    std::cout<<"world_x world_y: "<<world_pos[0]<<world_pos[1]<<std::endl;
    std::cout<<"x, y: "<<p.x()<<", "<<p.y()<<std::endl;
}

//Circle
void CircleObject::update(){
    std::cout<<"Update Circle"<<std::endl;
    worldToPixel(position_world, geometry.center);
}

void CircleObject::draw(QPainter& p){
    p.drawEllipse(geometry.center, geometry.radius, geometry.radius);
}
//Rectangle
void RectangleObject::update(){
    return;
}

void RectangleObject::draw(QPainter& p){
    return;
}


//Line 
void LineObject::update(){
    std::cout<<"Update Line"<<std::endl;
    worldToPixel(base_world, p1);
    worldToPixel(position_world, p2);
    geometry.setP1(p1);
    geometry.setP2(p2);
    std::cout<<"geometry line: " <<base_world[0] << std::endl;
}

void LineObject::draw(QPainter& p){
    std::cout<<"drawing line world: "<<base_world[0]<<", "<<position_world[0]<<", "<<base_world[1]<<", "<<position_world[1]  <<std::endl;
    std::cout<<"drawing line: "<<geometry.x1()<<", "<<geometry.x2()<<", "<<geometry.y1()<<", "<<geometry.y2()  <<std::endl;
    
    p.drawLine(geometry);
}

/******************************CANVAS_BASE***************************************************** */
/******************************CANVAS_BASE***************************************************** */
CanvasBase::CanvasBase(QWidget* parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    {
        init();
    }
//init(empty overrid)
void CanvasBase::init(){};

//INitialize screen size
void CanvasBase::init_size(){
    resize(PIXEL_WIDTH_, PIXEL_HEIGHT_);
}
void CanvasBase::set_system(SystemObjects* s){
    SystemObjects_ = s;
}

// Get geometric object
GeometricObject* CanvasBase::get_GeometricObject(std::string name){
    auto it = objects_.find(name);
    if (it != objects_.end())
        {
            return it->second.get();
        }
    return nullptr;
}

// Mutator functions(empty override)
void CanvasBase::setObjectWorldPos(){}

//Draw objects and update the pixel coordinates
void CanvasBase::paintEvent(QPaintEvent* event){
    QPainter p(this);
    for(const auto& obj: objects_){
        //update functions from objects
        obj.second.get()->update();
        //draw objects
        obj.second.get()->draw(p);
    }
}

/******************************CANVAS***************************************************** */

Canvas::Canvas(QWidget* parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    {
        init();
    }

//init
void Canvas::init(){
    objects_.emplace("pendulum_mass", std::make_unique<CircleObject>("pendulum_mass", canvas_config_)) ;
    objects_.emplace("pendulum_arm", std::make_unique<LineObject>("pendulum_arm", canvas_config_)) ;
    dynamic_cast<CircleObject*>(get_GeometricObject("pendulum_mass"))->geometry.radius = 100;

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
//INitialize screen size
void Canvas::init_size(){
    resize(PIXEL_WIDTH_, PIXEL_HEIGHT_);
}

void Canvas::set_system(SystemObjects* s){
    SystemObjects_ = s;
}

//get functions
GeometricObject* Canvas::get_GeometricObject(std::string name){
    auto it = objects_.find(name);
    if (it != objects_.end()) {
            return it->second.get();
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
    if (auto* arm = get_GeometricObject("pendulum_arm")){
        std::cout<<"Setting Geometry*****************************************************"<<std::endl;
        std::cout<<get_GeometricObject("pendulum_arm")->name<<std::endl;
        dynamic_cast<LineObject*>(arm)->position_world[0] = obs[0];
        dynamic_cast<LineObject*>(arm)->position_world[1] = obs[1];
        dynamic_cast<LineObject*>(arm)->base_world[0] = 0.0;
        dynamic_cast<LineObject*>(arm)->base_world[1] = 0.0;
    }
}
       
// Draw functions
void Canvas::paintEvent(QPaintEvent* event){
    QPainter p(this);
    for(const auto& obj: objects_){
        //update functions from objects
        obj.second.get()->update();
        //draw objects
        obj.second.get()->draw(p);
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
    return;
}


/******************************CANVAS PENDULUM***************************************************** */


/******************************CANVAS PENDULUM***************************************************** */
CanvasPendulum::CanvasPendulum(QWidget* parent, Qt::WindowFlags f)
    : CanvasBase(parent, f)
    {
        init();
    }


void CanvasPendulum::init() {
    objects_.emplace("pendulum_mass", std::make_unique<CircleObject>("pendulum_mass", canvas_config_)) ;
    objects_.emplace("pendulum_arm", std::make_unique<LineObject>("pendulum_arm", canvas_config_)) ;
    
    dynamic_cast<CircleObject*>(get_GeometricObject("pendulum_mass"))->geometry.radius = 100;

    

}

void CanvasPendulum::setObjectWorldPos() {
    std::cout<<"Override Function setObjectWorldPos" << std::endl;
    const auto& obs = SystemObjects_->state_space.get_observation();

    if (auto* mass = get_GeometricObject("pendulum_mass")){
        std::cout<<" Set pendulum mass world poss"<<std::endl;
        mass->position_world[0] = obs[0];
        mass->position_world[1] = obs[1];
    }

    if (auto* arm = get_GeometricObject("pendulum_arm")){
        std::cout<<"Setting Geometry*****************************************************"<<std::endl;
        std::cout<<get_GeometricObject("pendulum_arm")->name<<std::endl;
        dynamic_cast<LineObject*>(arm)->position_world[0] = obs[0];
        dynamic_cast<LineObject*>(arm)->position_world[1] = obs[1];
        dynamic_cast<LineObject*>(arm)->base_world[0] = 0.0;
        dynamic_cast<LineObject*>(arm)->base_world[1] = 0.0;
    }
}




/******************************CANVAS SEGWAY***************************************************** */


/******************************CANVAS SEGWAY***************************************************** */

CanvasSegway::CanvasSegway(QWidget* parent, Qt::WindowFlags f)
    : CanvasBase(parent, f)
    {
        init();
    }


void CanvasSegway::init() {
    objects_.emplace("segway_body", std::make_unique<LineObject>("segway_body", canvas_config_)) ;
    objects_.emplace("segway_wheels", std::make_unique<CircleObject>("segway_wheels", canvas_config_)) ;
    objects_.emplace("line_wheel", std::make_unique<LineObject>("line_wheel", canvas_config_));
    dynamic_cast<CircleObject*>(get_GeometricObject("segway_wheels"))->geometry.radius = 100;

    dynamic_cast<LineObject*>(get_GeometricObject("line_wheel"))->geometry.setLength(100);
    QPointF p_base, p2;

    p_base.setX(0.0);
    p_base.setY(0.0);
    p2.setX(100);
    p2.setY(0.0);
    dynamic_cast<LineObject*>(get_GeometricObject("line_wheel"))->geometry.setP1(p_base);
    dynamic_cast<LineObject*>(get_GeometricObject("line_wheel"))->geometry.setP2(p2);
}

void CanvasSegway::setObjectWorldPos() {
    std::cout<<"Override Function setObjectWorldPos" << std::endl;
    const auto& obs = SystemObjects_->state_space.get_observation();
    const auto& state = SystemObjects_ -> state_space.get_state();

    if (auto* wheels = get_GeometricObject("segway_wheels")){
        wheels->position_world[0] = obs[0];
        wheels->position_world[1] = 0.0;
    }

    if (auto* body = get_GeometricObject("segway_body")){
        std::cout<<"Setting position body object"<<std::endl;
        dynamic_cast<LineObject*>(body)->position_world[0] = obs[1];
        dynamic_cast<LineObject*>(body)->position_world[1] = obs[2];
        dynamic_cast<LineObject*>(body)->base_world[0] = obs[0];
        dynamic_cast<LineObject*>(body)->base_world[1] = 0.0;
    }

    if (auto* wheel_line = get_GeometricObject("line_wheel")){
        dynamic_cast<LineObject*>(wheel_line)->position_world[0] = obs[3]+obs[0];
        dynamic_cast<LineObject*>(wheel_line)->position_world[1] = obs[4];
        dynamic_cast<LineObject*>(wheel_line)->base_world[0] = obs[0];
        dynamic_cast<LineObject*>(wheel_line)->base_world[1] = 0.0;
    }
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
    canvas_.setObjectWorldPos();
    canvas_.update();
}


