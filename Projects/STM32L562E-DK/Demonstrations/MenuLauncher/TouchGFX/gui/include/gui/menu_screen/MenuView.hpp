#ifndef MENUVIEW_HPP
#define MENUVIEW_HPP

#include <gui_generated/menu_screen/MenuViewBase.hpp>
#include <gui/menu_screen/MenuPresenter.hpp>

class MenuView : public MenuViewBase
{
public:
    MenuView();
    virtual ~MenuView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();
    virtual void menuScreensUpdateItem(MenuContainer& item, int16_t itemIndex);
    void menuListAnimationEnded(void);
protected:

    static const uint16_t MODULE_COUNT = 5;
    const char* menus[MODULE_COUNT] = {"Measurements", "AI", "BLE", "Audio Player", "Informations"};
    Unicode::UnicodeChar menuNameText[MODULENAME_SIZE];

    int16_t selectedModule;

    void menuListItemSelectedHandler(int16_t selectedItem);
    Callback<MenuView, int16_t> menuListItemSelectedCallback;

    Callback<MenuView, void> menuListAnimationEndedCallback;
};

#endif // MENUVIEW_HPP
