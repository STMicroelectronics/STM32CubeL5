#ifndef MENUCONTAINER_HPP
#define MENUCONTAINER_HPP

#include <gui_generated/containers/MenuContainerBase.hpp>

class MenuContainer : public MenuContainerBase
{
public:
    MenuContainer();
    virtual ~MenuContainer() {}

    virtual void initialize();
    virtual void setBitmap(BitmapId id);
    virtual BitmapId getBitmap();
protected:
    BitmapId bitmap_id;
};

#endif // MENUCONTAINER_HPP
