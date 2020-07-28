#include <gui/popup_screen/PopUpView.hpp>

PopUpView::PopUpView()
{

}

void PopUpView::setupScreen()
{
    PopUpViewBase::setupScreen();
}

void PopUpView::tearDownScreen()
{
    PopUpViewBase::tearDownScreen();
}

void PopUpView::handleClickEvent(const ClickEvent& evt)
{
    if (evt.getType() == ClickEvent::RELEASED)
    {
        //Go to Measurements
        presenter->moduleSelected(0);
    }
}
