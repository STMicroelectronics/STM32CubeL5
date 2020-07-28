#include <gui/splash_screen/SplashView.hpp>

SplashView::SplashView()
{

}

void SplashView::setupScreen()
{
    SplashViewBase::setupScreen();
}

void SplashView::tearDownScreen()
{
    SplashViewBase::tearDownScreen();
}

void SplashView::handleClickEvent(const ClickEvent& evt)
{
    if (evt.getType() == ClickEvent::RELEASED)
    {
        //Go to Menu with no screen transition
        application().gotoMenuScreenNoTransition();
    }
}
