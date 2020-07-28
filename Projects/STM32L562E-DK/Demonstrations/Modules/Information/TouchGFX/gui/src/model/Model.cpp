#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

#ifndef SIMULATOR
#include "main.h"
#endif

Model::Model() : modelListener(0)
{
}

void Model::tick()
{
}

void Model::systemReset()
{
#ifndef SIMULATOR // quick & durty! should be moved to Model
    /* Do Software Reset */
    HAL_NVIC_SystemReset();
#endif
}

