#include <gui/menu_screen/MenuView.hpp>
#include "BitmapDatabase.hpp"
#include <texts/TextKeysAndLanguages.hpp>

MenuView::MenuView() :
    selectedModule(MODULE_COUNT - 1),
    menuListItemSelectedCallback  (this, &MenuView::menuListItemSelectedHandler),
    menuListAnimationEndedCallback(this, &MenuView::menuListAnimationEnded)
{

}

void MenuView::setupScreen()
{
    MenuViewBase::setupScreen();

    menuScreens.setItemSelectedCallback(menuListItemSelectedCallback);
    menuScreens.setAnimationEndedCallback(menuListAnimationEndedCallback);

    Unicode::strncpy(menuNameText, menus[selectedModule], MODULENAME_SIZE);
    Unicode::snprintf(moduleNameBuffer, MODULENAME_SIZE, "%s", menuNameText);
    moduleName.invalidate();
}

void MenuView::tearDownScreen()
{
    MenuViewBase::tearDownScreen();
}

void MenuView::handleTickEvent()
{

}

void MenuView::menuScreensUpdateItem(MenuContainer& item, int16_t itemIndex)
{
    touchgfx_printf("UPDATE : itemIndex = %d\n", itemIndex);
    selectedModule = itemIndex;

    if (itemIndex == 0)
    {
        item.setBitmap(BITMAP_ICON_MEASUREMENTS_ID);
    }
    else if (itemIndex == 1)
    {
        item.setBitmap(BITMAP_ICON_AI_ID);
    }
    else if (itemIndex == 2)
    {
        item.setBitmap(BITMAP_ICON_BLE_ID);
    }
    else if (itemIndex == 3)
    {
        item.setBitmap(BITMAP_ICON_MUSIC_ID);
    }
    else if (itemIndex == 4)
    {
        item.setBitmap(BITMAP_ICON_INFO_ID);
    }

    Unicode::strncpy(menuNameText, menus[selectedModule], MODULENAME_SIZE);
}

void MenuView::menuListItemSelectedHandler(int16_t selectedItem)
{
    touchgfx_printf("SELECTED : selectedItem = %d - selectedModule = %d\n", selectedItem, selectedModule);
    if (selectedModule == 0) // Measurements
    {
        //Go to Menu with no screen transition
        application().gotoPopUpScreenCoverTransitionNorth();
    }
    else
    {
        presenter->moduleSelected(selectedModule);
    }
}

void MenuView::menuListAnimationEnded(void)
{
    touchgfx_printf("New : selectedModule = %d - ModuleName = %s\n", selectedModule, menus[selectedModule]);

    Unicode::snprintf(moduleNameBuffer, MODULENAME_SIZE, "%s", menuNameText);
    moduleName.invalidate();
}
