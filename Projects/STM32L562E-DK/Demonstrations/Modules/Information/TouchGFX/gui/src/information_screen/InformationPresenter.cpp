#include <gui/information_screen/InformationView.hpp>
#include <gui/information_screen/InformationPresenter.hpp>

InformationPresenter::InformationPresenter(InformationView& v)
    : view(v)
{
}

void InformationPresenter::activate()
{

}

void InformationPresenter::deactivate()
{

}

void InformationPresenter::systemReset()
{
    model->systemReset();
}