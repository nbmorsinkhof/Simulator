#include <iostream>
#include <chrono>
#include<thread>

#include "system.hpp"
#include "simulator.hpp"
#include "state_space.hpp"
#include "CsvLogger.hpp"


//constructor
Simulator::Simulator()
    : logdata_{0.0, {}, {}, {}}, logger_("data_log2", &Simulator::stateFormatter)
{
    //update_state();
}

//accessors
std::vector<float> Simulator::get_observation(){
    return SystemObjects_->state_space.get_observation();
}

//functions
void Simulator::update_state(){
    SystemObjects_->state_space.update_ddtState();
    std::vector<float> x_runge_kutta = runge_kutta(
            dt_, 
            SystemObjects_->state_space.get_state()
        );

    for(int i=0; i<SystemObjects_->state_space.get_state().size(); i++){
        float x_new;
        x_new = forward_euler(dt_, SystemObjects_->state_space.get_ddtState()[i], SystemObjects_->state_space.get_state()[i]);
        SystemObjects_->state_space.set_state(x_runge_kutta[i], i);
        
    }
    log_data();
}

void Simulator::run(){
    while(true){
        if(SystemObjects_->system_interface.state == RUN){
            update_state();
            
            
        }
        std::this_thread::sleep_for(
            std::chrono::milliseconds(5));
    }
}

std::vector<float> Simulator::f_state_space(std::vector<float> state){
    return SystemObjects_->state_space.f_state_space(state);
};

void Simulator::set_system(SystemObjects* s){
    SystemObjects_ = s;
}

float Simulator::forward_euler(float dt, float ddt_x, float x_prev){
    float x;
    x = x_prev + dt*ddt_x;
    return x;
}

std::vector<float> Simulator::runge_kutta(float dt, std::vector<float> x_prev){
    size_t n = x_prev.size();
    if(n==0){
        std::cout<<"empty state vector"<<std::endl;
        return x_prev;
    }
    std::vector<float> k1(n), k2(n), k3(n), k4(n), f_in(n), x(n);
    k1 = f_state_space(x_prev);
    for(size_t i=0; i<n; ++i){
        f_in[i] = x_prev[i] + (dt/2)*k1[i];
    }
    k2 = f_state_space(f_in);
    for(size_t i=0; i<n; ++i){
        f_in[i] = x_prev[i] + (dt/2)*k2[i];
    }
    k3 = f_state_space(f_in);
    for(size_t i=0; i<n; ++i){
        f_in[i] = x_prev[i] + dt*k3[i];
    }
    k4 = f_state_space(f_in);

    for(size_t i=0; i<n; ++i){
        x[i] = x_prev[i] + (dt/6)*(k1[i] + 2*k2[i] + 2*k3[i] + k4[i]);
    }

    return x;
}

void Simulator::stateFormatter(std::ostream& os, const LogData& data){
    os << data.time << ",";
    for(auto& v_i : data.state){
        std::cout<<" LOGGING DATA............................. "<<v_i<<std::endl;
        os << v_i <<",";
    }
    for(auto& v_i : data.observation){
        os << v_i <<",";
    }
    for(auto& v_i : data.control_input){
        os << v_i <<",";
    }
}

void Simulator::log_data(){
    logdata_.state = SystemObjects_->state_space.get_state();
    logdata_.observation = SystemObjects_->state_space.get_observation();
    logdata_.control_input = SystemObjects_->state_space.get_control_input();
    logger_.log_data(logdata_);
}