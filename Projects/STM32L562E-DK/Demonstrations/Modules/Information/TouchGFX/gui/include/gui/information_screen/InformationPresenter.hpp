#ifndef INFORMATIONPRESENTER_HPP
#define INFORMATIONPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class InformationView;

class InformationPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    InformationPresenter(InformationView& v);

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

    virtual ~InformationPresenter() {};

    virtual void systemReset();

private:
    InformationPresenter();

    InformationView& view;
};


#endif // INFORMATIONPRESENTER_HPP
