#ifndef PARAMETER_H
#define PARAMETER_H

#include <cmath>

#include "Common.h"

struct Parameter
{
    double current_value = 1.0;
    double target_value = 1.0;

    const double increment_value = 0.05;
    const double max_value = 1.0;
    const double min_value = 0.0;

    double change_time_start = 0.0;

};

void increase_value(Parameter &parameter)
{
    parameter.change_time_start = g_time;

    if (parameter.target_value < parameter.max_value)
        parameter.target_value += parameter.increment_value;
}

void decrease_value(Parameter &parameter)
{
    parameter.change_time_start = g_time;

    if (parameter.target_value > parameter.min_value)
        parameter.target_value -= parameter.increment_value;

    if (parameter.target_value < parameter.increment_value)
        parameter.target_value = parameter.min_value;
}

void update_parameter(Parameter &parameter)
{
    if (parameter.target_value != parameter.current_value)
    {
        double step_value = pow(g_time - parameter.change_time_start, 2.0);

        if (parameter.target_value < parameter.current_value)
        {
            parameter.current_value -= step_value;

            if (parameter.current_value < parameter.target_value)
                parameter.current_value = parameter.target_value;
        }

        if (parameter.target_value > parameter.current_value)
        {
            parameter.current_value += step_value;

            if (parameter.current_value > parameter.target_value)
                parameter.current_value = parameter.target_value;
        }
    }
}

#endif
