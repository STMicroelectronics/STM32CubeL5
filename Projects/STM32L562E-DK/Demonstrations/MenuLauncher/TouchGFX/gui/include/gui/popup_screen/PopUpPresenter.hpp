#ifndef POPUPPRESENTER_HPP
#define POPUPPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class PopUpView;

class PopUpPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    PopUpPresenter(PopUpView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~PopUpPresenter() {};

    void moduleSelected(int moduleIndex);

private:
    PopUpPresenter();

    PopUpView& view;
};


#endif // POPUPPRESENTER_HPP
