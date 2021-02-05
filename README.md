# jade
Jade is a cross-platform diagram editor based on the Qt framework.

Notable changes vs libjade-master and jade-master:
- Combined DrawingView and DrawingScene into a single DrawingWidget class
- DrawingWidget:
	- Added dynamic grid support, where the grid spacing changes depending on the zoom level
	- Added property system via setProperties() and properties()
	- Incorporated undo events for handling changes to DrawingItem or DrawingWidget properties
	- Change paste() behavior to use PlaceMode to add the new items
	- Added mode actions and context menus
- DrawingItem:
	- Removed DrawingItem::Flags.  All items are assumed to be movable, resizable, etc
	- Removed setVisible() and isVisible().  All items are assumed to always be visible.
	- Removed any notion of parents and children, as this is not currently well supported
	- Deleted DrawingItemStyle class.  Each item manages its own style properties.
	- Added DrawingItemFactory class.  Each item is uniquely identified by its name().
	- Added property system via setProperties() and properties()
	- Integrated save to XML and load from XML functionality
- Deleted DrawingArcItem, as it is a subset of the behavior of DrawingCurveItem
- Deleted DrawingTextPolygonItem, as it had some bugs calculating its shape() in certain conditions
- Remaining DrawingItem subclasses are optimized for fast operation using cached boundingRect and shape calculations
- Added PropertiesBrowser, a widget to manage properties of DrawingItem and DrawingWidget objects
- Added DrawingWindow class, which manages a SDI interface for a DrawingWidget and connects it to a PropertiesBrowser

Todo:
- A lot of testing!!!  Pretty much every line of code still needs to be tested.
- Add support for defs, just like SVG
- Add custom SVG exporter
