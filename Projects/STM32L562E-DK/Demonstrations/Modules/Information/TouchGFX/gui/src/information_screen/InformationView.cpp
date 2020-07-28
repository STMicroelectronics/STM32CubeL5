#include <gui/information_screen/InformationView.hpp>

InformationView::InformationView()
{

}

void InformationView::setupScreen()
{
    InformationViewBase::setupScreen();
}

void InformationView::tearDownScreen()
{
    InformationViewBase::tearDownScreen();
}

void InformationView::systemReset()
{
    presenter->systemReset();
}