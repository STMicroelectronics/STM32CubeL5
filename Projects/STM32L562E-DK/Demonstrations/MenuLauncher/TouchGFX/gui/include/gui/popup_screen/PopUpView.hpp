#ifndef POPUPVIEW_HPP
#define POPUPVIEW_HPP

#include <gui_generated/popup_screen/PopUpViewBase.hpp>
#include <gui/popup_screen/PopUpPresenter.hpp>

class PopUpView : public PopUpViewBase
{
public:
    PopUpView();
    virtual ~PopUpView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleClickEvent(const ClickEvent& evt);
protected:
};

#endif // POPUPVIEW_HPP
