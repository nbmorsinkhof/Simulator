#ifndef CSVLOGGER_HPP
#define CSVLOGGER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <stdexcept>

template <typename T>
class CsvLogger{
public:
    using Formatter = std::function<void(std::ostream&, const T&)>;

    explicit CsvLogger(const std::string filename, Formatter formatter)
        : filename_(filename), formatter_(std::move(formatter))
        {
            file_.open(filename_);
            if(!file_.is_open()){
                throw std::runtime_error("Could not open file " + filename_);
            }
        }

    void log_data(T& data){
        formatter_(file_, data);
        file_ <<"\n";
    }

private:
    std::string filename_;
    std::ofstream file_;
    Formatter formatter_;
};


#endif