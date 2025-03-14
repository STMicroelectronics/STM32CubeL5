/**
  ******************************************************************************
  * This file is part of the TouchGFX 4.12.3 distribution.
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#ifndef DRAWABLELIST_HPP
#define DRAWABLELIST_HPP

#include <touchgfx/containers/Container.hpp>

namespace touchgfx
{
/**
 * @class DrawableListItemsInterface DrawableList.hpp touchgfx/containers/DrawableList.hpp
 *
 * @brief A drawable list items interface.
 *
 *        A drawable list items interface. Used to pass the allocated array of drawable
 *        elements to setDrawables function in either ScrollList, ScrollWheel or
 *        ScrollWheelWithSelectionStyle. Provides easy access to each element in the array as
 *        well as the size of the array.
 *
 * @see ScrollList::setDrawables
 * @see ScrollWheel::setDrawables
 * @see ScrollWheelWithSelectionStyle::setDrawables
 */
class DrawableListItemsInterface
{
public:
    /**
     * @fn virtual DrawableListItemsInterface::~DrawableListItemsInterface()
     *
     * @brief Destructor.
     *
     *        Destructor.
     */
    virtual ~DrawableListItemsInterface()
    {
    }

    /**
     * @fn virtual Drawable* DrawableListItemsInterface::getDrawable(int16_t index) = 0;
     *
     * @brief Gets a drawable.
     *
     *        Gets a drawable at a given index.
     *
     * @param index Zero-based index of the drawable.
     *
     * @return Null if it fails, else the drawable.
     */
    virtual Drawable* getDrawable(int16_t index) = 0;

    /**
     * @fn virtual int16_t DrawableListItemsInterface::getNumberOfDrawables() = 0;
     *
     * @brief Gets number of drawables.
     *
     *        Gets number of drawables.
     *
     * @return The number of drawables.
     */
    virtual int16_t getNumberOfDrawables() = 0;
};

/**
 * @class DrawableListItems DrawableList.hpp touchgfx/containers/DrawableList.hpp
 *
 * @brief An array of drawables used by DrawableList.
 *
 *        An array of drawables used by DrawableList. This class is primarily used to ease the
 *        setup of a callback function to get access to a specific drawable in the array.
 *
 *        Example usage:
 *        \code
 *        static const int NUMBER_OF_DRAWABLES = 5;
 *        DrawableListItems<TextAreaWithOneWildcardContainer, NUMBER_OF_DRAWABLES> menuItems;
 *        \endcode
 *
 * @tparam TYPE Type of the drawables. Can be a simple drawable, such as Image or a more complex
 *              container.
 * @tparam SIZE Size of the array. This is the number of drawables to allocate and should be all
 *              visible drawables on the screen at any given time.
 */
template <class TYPE, int SIZE>
class DrawableListItems : public DrawableListItemsInterface
{
public:
    /**
     * @fn DrawableListItems::DrawableListItems()
     *
     * @brief Default constructor.
     *
     *        Default constructor.
     */
    DrawableListItems()
        : DrawableListItemsInterface()
    {
    }

    /**
     * @fn virtual DrawableListItems::~DrawableListItems()
     *
     * @brief Destructor.
     *
     *        Destructor.
     */
    virtual ~DrawableListItems()
    {
    }

    /**
     * @fn Drawable* DrawableListItems::getDrawable(int16_t index)
     *
     * @brief Gets the address of an element.
     *
     *        Gets the address of an element.
     *
     * @param index Zero-based index of the drawable.
     *
     * @return The drawable for the given index.
     */
    virtual Drawable* getDrawable(int16_t index)
    {
        assert(index >= 0 && index < SIZE);
        return &element[index];
    }

    /**
     * @fn TYPE& DrawableListItems::operator[](int index)
     *
     * @brief Array indexer operator.
     *
     *        Array indexer operator.
     *
     * @param index Zero-based index of elements to access.
     *
     * @return The indexed value.
     */
    TYPE& operator[](int index)
    {
        assert(index >= 0 && index < SIZE);
        return element[index];
    }

    virtual int16_t getNumberOfDrawables()
    {
        return SIZE;
    }

    TYPE element[SIZE]; ///< The array of drawables
};

/**
 * @class DrawableList DrawableList.hpp touchgfx/containers/DrawableList.hpp
 *
 * @brief A container able to display many items using only a few drawables.
 *
 *        A container able to display many items using only a few drawables. This is done by
 *        only having drawables for visible items, and populating these drawables with new
 *        content when the drawable becomes visible.
 *
 *        This means that all drawables must have an identical structure in some way, for
 *        example an Image or a Container with a button and a text.
 */
class DrawableList : public Container
{
public:
    /**
     * @fn DrawableList::DrawableList();
     *
     * @brief Default constructor.
     *
     *        Default constructor.
     */
    DrawableList();

    /**
     * @fn virtual DrawableList::~DrawableList()
     *
     * @brief Destructor.
     *
     *        Destructor.
     */
    virtual ~DrawableList()
    {
    }

    /**
     * @fn virtual void DrawableList::setWidth(int16_t width);
     *
     * @brief Sets the width of the DrawableList.
     *
     *        Sets the width of the DrawableList. If the list is vertical, the width is also
     *        propagated to all drawables in the list.
     *
     * @param width The width.
     */
    virtual void setWidth(int16_t width);

    /**
     * @fn virtual void DrawableList::setHeight(int16_t height);
     *
     * @brief Sets the height of the DrawableList. If the list is horizontal, the height is also
     *        propagated to all drawables in the list.
     *
     * @param height The height.
     */
    virtual void setHeight(int16_t height);

    /**
     * @fn virtual void DrawableList::setHorizontal(bool horizontal);
     *
     * @brief Sets a horizontal layout.
     *
     *        Sets a horizontal layout. If horizontal is set true, all drawables are arranged side
     *        by side. If horizontal is set false, the drawables are arranged above and below each
     *        other (vertically).
     *
     * @param horizontal True to align drawables horizontal, false to align drawables vertically.
     *
     * @note Default value is false, i.e. vertical layout.
     *
     * @see getHorizontal
     */
    virtual void setHorizontal(bool horizontal);

    /**
     * @fn virtual bool DrawableList::getHorizontal() const;
     *
     * @brief Gets the orientation of the drawables.
     *
     *        Gets the orientation of the drawables, previously set using setHorizontal().
     *
     * @return True if it horizontal, false if it is vertical.
     *
     * @see setHorizontal
     */
    virtual bool getHorizontal() const;

    /**
     * @fn virtual void DrawableList::setCircular(bool circular);
     *
     * @brief Sets whether the list is circular or not.
     *
     *        Sets whether the list is circular (infinite) or not. A circular list is a list where
     *        the first drawable re-appears after the last item in the list.
     *
     * @param circular True if the list should be circular, false if the list should not be circular.
     *
     * @see getCircular
     */
    virtual void setCircular(bool circular);

    /**
     * @fn virtual bool DrawableList::getCircular() const;
     *
     * @brief Gets the circular setting.
     *
     *        Gets the circular setting, previously set using setCircular().
     *
     * @return True if the list is circular (infinite), false if the list is not circular.
     *
     * @see setCircular
     */
    virtual bool getCircular() const;

    /**
     * @fn void DrawableList::setDrawableSize(int16_t drawableSize, int16_t drawableMargin);
     *
     * @brief Sets drawable size.
     *
     *        Sets drawable size. The total size of each drawable is the drawableSize +
     *        2*drawableMargin as margin will be added before and after each drawable.
     *
     * @param drawableSize   The size of the drawable.
     * @param drawableMargin The margin around drawables (before as well as after the drawable).
     */
    void setDrawableSize(int16_t drawableSize, int16_t drawableMargin);

    /**
     * @fn virtual int16_t DrawableList::getItemSize() const;
     *
     * @brief Gets size of each item.
     *
     *        Gets size of each item. This equals the drawable size plus the drawable margin as set
     *        in setDrawables(). Equals getDrawableSize() + getDrawableMargin().
     *
     * @return The item size.
     *
     * @note Not the same as getDrawableSize().
     *
     * @see setDrawables
     * @see setDrawableSize
     * @see getDrawableMargin
     */
    virtual int16_t getItemSize() const;

    /**
     * @fn virtual int16_t DrawableList::getDrawableSize() const;
     *
     * @brief Gets drawable size.
     *
     *        Gets drawable size as set by setDrawables.
     *
     * @return The drawable size.
     *
     * @see setDrawables
     */
    virtual int16_t getDrawableSize() const;

    /**
     * @fn virtual int16_t DrawableList::getDrawableMargin() const;
     *
     * @brief Gets drawable margin.
     *
     *        Gets drawable margin as set by setDrawables.
     *
     * @return The drawable margin.
     */
    virtual int16_t getDrawableMargin() const;

    /**
     * @fn virtual void DrawableList::setDrawables(DrawableListItemsInterface& drawableListItems, int16_t drawableItemIndexOffset, GenericCallback<DrawableListItemsInterface*, int16_t, int16_t>& updateDrawableCallback);
     *
     * @brief Sets the drawables parameters.
     *
     *        Sets the drawables parameters. These parameters are \li The access class to the array
     *        of drawables \li The offset in the drawableListItems array to start using drawable
     *        and \li Callback to update the contents of a drawable.
     *
     * @param [in,out] drawableListItems       Number of drawables allocated.
     * @param [in]     drawableItemIndexOffset A callback to get access to a drawable.
     * @param [in]     updateDrawableCallback  A callback to update the contents of a drawable.
     *
     * @see getRequiredNumberOfDrawables
     */
    virtual void setDrawables(DrawableListItemsInterface& drawableListItems,
                              int16_t drawableItemIndexOffset,
                              GenericCallback<DrawableListItemsInterface*, int16_t, int16_t>& updateDrawableCallback);

    /**
     * @fn int16_t DrawableList::getNumberOfDrawables() const
     *
     * @brief Gets number of drawables.
     *
     *        Gets number of drawables, as set using setDrawables().
     *
     * @return The number of drawables.
     *
     * @see setDrawables
     */
    int16_t getNumberOfDrawables() const;

    /**
     * @fn void DrawableList::setNumberOfItems(int16_t numberOfItems);
     *
     * @brief Sets number of items in the list.
     *
     *        Sets number of items in the list. This forces all drawables to be updated to ensure
     *        that the content is correct.
     *
     * @param numberOfItems Number of items.
     *
     * @note The DrawableList is refreshed to reflect the change.
     */
    void setNumberOfItems(int16_t numberOfItems);

    /**
     * @fn int16_t DrawableList::getNumberOfItems() const;
     *
     * @brief Gets number of items in the DrawableList.
     *
     *        Gets number of items in the DrawableList, as previously set using setNumberOfItems().
     *
     * @return The number of items.
     *
     * @see setNumberOfItems
     */
    int16_t getNumberOfItems() const;

    /**
     * @fn int16_t DrawableList::getRequiredNumberOfDrawables() const;
     *
     * @brief Gets required number of drawables.
     *
     *        Gets required number of drawables. After setting up the DrawableList it is possible
     *        to request how many drawables are needed to ensure that the list can always be drawn
     *        properly. If the DrawableList has been setup with fewer Drawables than the required
     *        number of drawables, part of the lower part of the DrawableList will look wrong.
     *
     *        The number of required drawables depend on the size of the widget and the size of the
     *        drawables and the margin around drawables. If there are fewer drawables than required,
     *        the widget will not display correctly. If there are more drawables than required,
     *        some will be left unused.
     *
     * @return The required number of drawables.
     *
     * @see setDrawables
     */
    int16_t getRequiredNumberOfDrawables() const;

    /**
     * @fn void DrawableList::setOffset(int32_t ofs);
     *
     * @brief Sets virtual coordinate.
     *
     *        Sets virtual coordinate. Does not move to the given coordinate, but places the
     *        drawables and fill correct content into the drawables to give the impression that
     *        everything has been scrolled to the given coordinate.
     *
     *        Setting a value of 0 means that item 0 is at the start of the DrawableList. Setting a
     *        value of "-getItemSize()" places item 0 outside the start of the DrawableList and
     *        item 1 at the start of it.
     *
     *        Items that are completely outside of view, will be updated with new content using the
     *        provided callback from setDrawables(). Care is taken to not fill drawables more than
     *        strictly required.
     *
     * @param ofs The virtual coordinate.
     *
     * @see getOffset
     * @see setDrawables
     */
    void setOffset(int32_t ofs);

    /**
     * @fn int32_t DrawableList::getOffset() const;
     *
     * @brief Gets offset.
     *
     *        Gets offset, as previously set using setOffset().
     *
     * @return The virtual offset.
     *
     * @see setOffset
     */
    int32_t getOffset() const;

    /**
     * @fn int16_t DrawableList::getItemIndex(int16_t drawableIndex) const;
     *
     * @brief Gets item stored in a given Drawable.
     *
     *        Gets item stored in a given Drawable.
     *
     * @param drawableIndex Zero-based index of the drawable.
     *
     * @return The item index.
     */
    int16_t getItemIndex(int16_t drawableIndex) const;

    /**
     * @fn int16_t DrawableList::getDrawableIndices(int16_t itemIndex, int16_t* drawableIndexArray, int16_t arraySize) const;
     *
     * @brief Gets drawable indices.
     *
     *        Gets drawable indices. Useful when the number of items is smaller than the number of
     *        drawables as the same item might be in more than one drawable on the screen (if the
     *        DrawableList is circular). The passed array will be filled with the drawable indices
     *        and the number of indices found is returned.
     *
     * @param       itemIndex          Zero-based index of the item.
     * @param [out] drawableIndexArray Array where the drawable indices are stored.
     * @param       arraySize          Size of drawable array.
     *
     * @return The number of drawable indices found.
     *
     * @see getFirstDrawableIndex
     * @see setCircular
     * @see getDrawbleIndex
     */
    int16_t getDrawableIndices(int16_t itemIndex, int16_t* drawableIndexArray, int16_t arraySize) const;

    /**
     * @fn int16_t DrawableList::getDrawableIndex(int16_t itemIndex, int16_t prevDrawableIndex = -1) const;
     *
     * @brief Gets the drawable index of an item.
     *
     *        Gets the drawable index of an item. If the number of items is smaller than the number
     *        of drawables and the DrawableList is circular, the same item can be in more than one
     *        drawable. In that case, calling this function again with the previously returned
     *        index as second parameter, the index of the next drawable containing the item will be
     *        returned.
     *
     * @param itemIndex         Index of the item.
     * @param prevDrawableIndex (Optional) Index of the previous drawable. If given, search starts
     *                          after
     *                           this index.
     *
     * @return The first drawable index with the given item. Returns -1 if the item is not in a
     *         drawable.
     *
     * @see getDrawableIndices
     */
    int16_t getDrawableIndex(int16_t itemIndex, int16_t prevDrawableIndex = -1) const;

    /**
     * @fn void DrawableList::refreshDrawables();
     *
     * @brief Refresh drawables.
     *
     *        Refresh drawables. Useful to call if the number or items, their size or other
     *        properties have changed.
     */
    void refreshDrawables();

    /**
     * @fn void DrawableList::itemChanged(int16_t itemIndex);
     *
     * @brief Item changed.
     *
     *        Item changed and drawables containing this item must be updated. This function can be
     *        called when an item has changed and needs to be updated on screen. If the given item
     *        is displayed on screen, possible more than once for cyclic lists, each drawable is
     *        request to refresh its content to reflect the new value.
     *
     * @param itemIndex Zero-based index of the item.
     */
    void itemChanged(int16_t itemIndex);

private:
    bool isHorizontal;         ///< True if list is horizontal, false if not
    bool isCircular;           ///< True if list is circular, false if not
    int32_t offset;            ///< The offset of item 0
    int16_t itemSize;          ///< Size of each item (including margin)
    int16_t itemMargin;        ///< The margin around each item (included in itemSize)
    int16_t numItems;          ///< Number of items
    int16_t numDrawables;      ///< Number of drawables
    int16_t firstItem;         ///< The first visible item
    int16_t firstDrawable;     ///< The first visible drawable
    bool drawablesInitialized; ///< True if all drawables initialized

    int16_t firstDrawableIndex;                                                 ///< The offset when accessing DrawableListItems
    DrawableListItemsInterface* drawableItems;                                      ///< The drawable items
    GenericCallback<DrawableListItemsInterface*, int16_t, int16_t>* updateDrawable; ///< The update drawable callback
};
} // namespace touchgfx

#endif // DRAWABLELIST_HPP
