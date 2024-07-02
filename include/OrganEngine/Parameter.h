#ifndef PARAMETER_H
#define PARAMETER_H

#include <OrganEngine/Config.h>

struct Parameter
{
    int32_t current_value = MAX_AMPLITUDE;
    int32_t target_value = MAX_AMPLITUDE;

    const int16_t increment_value = (int16_t) (0.1 * MAX_AMPLITUDE);
    const int16_t max_value = MAX_AMPLITUDE;
    const int16_t min_value = 0;
    const int16_t update_rate = 20 * MAX_AMPLITUDE / SAMPLE_RATE;

    Parameter& operator=(const Parameter& other)
    {
        if (this != &other)
        {
            current_value = other.current_value;
            target_value = other.target_value;
        }
        return *this;
    }


    void setValue(int16_t value)
    {
        if (value <= max_value) target_value = value;
        else target_value = max_value;

        if (value >= min_value) target_value = value;
        else target_value = min_value;
    }

    void increaseValue()
    {
        if (target_value < max_value)
            target_value += increment_value;
    }

    void decreaseValue()
    {
        if (target_value > min_value)
            target_value -= increment_value;

        if (target_value < increment_value)
            target_value = min_value;
    }

    __attribute((always_inline)) inline
    void update()
    {
        if (current_value < target_value)
        {
            current_value += update_rate;

            if (current_value > target_value)
                current_value = target_value;
        }

        if (current_value > target_value)
        {
            current_value -= update_rate;

            if (current_value < target_value)
                current_value = target_value;
        }
    }
};


#endif
