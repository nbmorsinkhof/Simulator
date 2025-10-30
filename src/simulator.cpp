#include <iostream>
#include <chrono>
#include<thread>

#include "system.hpp"
#include "simulator.hpp"
#include "state_space.hpp"

//constructor
Simulator::Simulator()
{
    //update_state();
}

//accessors
std::vector<float> Simulator::get_observation(){
    return SystemObjects_->state_space.get_observation();
}

//functions
void Simulator::set_length_arm(float L){
    SystemObjects_->state_space.set_length_arm(L);
    update_state();
}

void Simulator::update_state(){
    SystemObjects_->state_space.update_ddtState();
    for(int i=0; i<SystemObjects_->state_space.get_state().size(); i++){
        float x_new;
        x_new = forward_euler(dt_, SystemObjects_->state_space.get_ddtState()[i], SystemObjects_->state_space.get_state()[i]);
        SystemObjects_->state_space.set_state(x_new, i);
    }

    std::cout<<"x[0]: "<<SystemObjects_->state_space.get_state()[0]<<std::endl;
    std::cout<<"x[1]: "<<SystemObjects_->state_space.get_state()[1]<<std::endl;
}

void Simulator::run(){
    while(true){
        if(SystemObjects_->system_interface.state == RUN){
            update_state();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void Simulator::set_system(SystemObjects* s){
    SystemObjects_ = s;
}

float Simulator::forward_euler(float dt, float ddt_x, float x_prev){
    float x;
    x = x_prev + dt*ddt_x;
    return x;
}
