#include <gui/containers/MenuContainer.hpp>

MenuContainer::MenuContainer()
{

}

void MenuContainer::initialize()
{
    MenuContainerBase::initialize();
}

void MenuContainer::setBitmap(BitmapId id)
{
    bitmap_id = id;
    image1.setBitmap(touchgfx::Bitmap(id));
    image1.invalidate();
}

BitmapId MenuContainer::getBitmap()
{
    return bitmap_id;
}