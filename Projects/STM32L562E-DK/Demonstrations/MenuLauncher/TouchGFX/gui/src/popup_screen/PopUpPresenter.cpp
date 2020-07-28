#include <gui/popup_screen/PopUpView.hpp>
#include <gui/popup_screen/PopUpPresenter.hpp>

PopUpPresenter::PopUpPresenter(PopUpView& v)
    : view(v)
{
}

void PopUpPresenter::activate()
{

}

void PopUpPresenter::deactivate()
{

}

void PopUpPresenter::moduleSelected(int moduleIndex)
{
    model->moduleSelected(moduleIndex);
}
