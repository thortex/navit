#include <glib.h>
//#include <Qt5Core>
//#include <QtGui>
// #include <QtWidgets>
//#include <QtQuick>
//#include <QtXml>
//#include <QApplication>
#include "config.h"
#ifdef HAVE_API_WIN32_BASE
#include <windows.h>
#endif
#include "plugin.h"
#include "item.h"
#include "attr.h"
#include "xmlconfig.h"
#include "color.h"
#include "gui.h"
#include "callback.h"
#include "debug.h"
#include "navit.h"
#include "point.h"
#include "graphics.h"
#include "event.h"
#include "map.h"
#include "coord.h"
#include "vehicle.h"
#include "coord.h"
#include "transform.h"
#include "mapset.h"
#include "route.h"
#include "country.h"
#include "track.h"
#include "search.h"
#include "bookmarks.h"
#include "command.h"
#include "keys.h"

#include "layout.h"

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QtQuick/QQuickView>

struct gui_priv {
	struct navit *nav;
	struct gui *gui;
	struct attr self;
	struct vehicle* currVehicle;
	
	//configuration items
	int fullscreen;
	int menu_on_map_click;
	int signal_on_map_click;
	int w;
	int h;
	char *source;
	char *skin;
	char* icon_src;
	int radius;
	int pitch;
	int lazy; //When TRUE - menu state will not be changed during map/menu switches, FALSE - menu will be always reset to main.qml
	
	//Interface stuff
	struct callback_list *cbl;
	QCoreApplication *app;
	struct window *win;
	struct graphics *gra;
	QWidget *mainWindow;
	QWidget *graphicsWidget;
	QQuickView *guiWidget;
	// QQuickView *prevGuiWidget;
	QStackedLayout *switcherWidget;
	struct callback *button_cb;
	struct callback *motion_cb;
	struct callback *resize_cb;
	struct callback *keypress_cb;
	struct callback *window_closed_cb;

	//Proxy objects
	class NGQProxyGui* guiProxy;
	class NGQProxyNavit* navitProxy;
	class NGQProxyVehicle* vehicleProxy;
	class NGQProxySearch* searchProxy;
	class NGQProxyBookmarks* bookmarksProxy;
	class NGQProxyRoute* routeProxy;
	class NGQPoint* currentPoint;
};

// #include "proxy.h"
// #include "ngqpoint.h"
// #include "searchProxy.h"
// #include "routeProxy.h"
// #include "bookmarksProxy.h"
// #include "vehicleProxy.h"
// #include "navitProxy.h"
// #include "guiProxy.h"

static int
gui_qml_get_attr(struct gui_priv *this_, enum attr_type type, struct attr *attr)
{
	switch (type) {
	case attr_fullscreen:
		attr->u.num=this_->fullscreen;
		break;
	case attr_skin:
		attr->u.str=this_->skin;
		break;
	case attr_pitch:
		attr->u.num=this_->pitch;
		break;
	case attr_radius:
		attr->u.num=this_->radius;
		break;
	default:
		return 0;
	}
	attr->type=type;
	return 1;
}

static int
gui_qml_set_attr(struct gui_priv *this_, struct attr *attr)
{
	switch (attr->type) {
	case attr_fullscreen:
		this_->fullscreen=attr->u.num;
		return 1;
	case attr_pitch:
		this_->pitch=attr->u.num;
		return 1;
	case attr_radius:
		this_->radius=attr->u.num;
		return 1;
	default:
		dbg(lvl_error,"unknown attr: %s\n",attr_to_name(attr->type));
		return 1;
	}
}


static void gui_qml_button(void *data, int pressed, int button, struct point *p)
{
        struct gui_priv *this_=(struct gui_priv*)data;
	dbg(lvl_info, "gui button event\n");

        // check whether the position of the mouse changed during press/release OR if it is the scrollwheel
        if (!navit_handle_button(this_->nav, pressed, button, p, NULL)) {
                dbg(lvl_debug,"navit has handled button\n");
                return;
        }

        dbg(lvl_debug,"enter %d %d\n", pressed, button);
        if (this_->signal_on_map_click) {
                // gui_qml_dbus_signal(this_, p);
                return;
        }

        if ( button == 1 && this_->menu_on_map_click ) {
                // this_->guiProxy->switchToMenu(p);
        }
}


//GUI interface calls
static int argc=1;
static char *argv[]={(char *)"navit",NULL};

static int gui_qml_set_graphics(struct gui_priv *this_, struct graphics *gra)
{
    QWidget* _mainWindow;
    this_->app=new QApplication(argc,argv);
    this_->gra=gra;

    this_->button_cb=callback_new_attr_1(callback_cast(gui_qml_button), attr_button, this_);
    graphics_add_callback(gra, this_->button_cb);

    this_->switcherWidget = new QStackedLayout();
    _mainWindow = new QWidget();
    this_->mainWindow=_mainWindow;
    this_->mainWindow->setLayout(this_->switcherWidget);

    this_->graphicsWidget = (QWidget*)graphics_get_data(gra,"qt_widget");
    if (this_->graphicsWidget == NULL ) {
        this_->graphicsWidget = new QLabel(QString("Sorry, current graphics type is incompatible with this gui. Try 'qt_qpainter'"));
        dbg(lvl_error,"Sorry, current graphics type is incompatible with this gui. Try 'qt_qpainter'");
    }

    // A bunch of useless controls only to test layouts
    QTabWidget* m_tabWindow = new QTabWidget;
    QWidget* m_exitBtn = new QPushButton("Button");
    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addStretch();
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addStretch();
    hLayout->addWidget(m_exitBtn);
    hLayout->addStretch();
    vLayout->addLayout(hLayout);
    vLayout->addStretch();
    
    QWidget *w = new QWidget;
    w->setLayout(vLayout);
    m_tabWindow->addTab(w, "Basic");

    m_tabWindow->addTab(this_->graphicsWidget, "Map ");
    this_->switcherWidget->addWidget(m_tabWindow);
//    this_->switcherWidget->addWidget(this_->graphicsWidget);

    navit_draw(this_->nav);
    // this_->switcherWidget->addWidget(this_->guiWidget);
    this_->switcherWidget->setCurrentWidget(this_->graphicsWidget);

    this_->mainWindow->show();
 
    return 0;
}

struct gui_methods gui_qml_methods = {
	NULL,
	NULL,
        gui_qml_set_graphics,
	NULL,
	NULL,
	NULL,
	NULL,
	gui_qml_get_attr,
	NULL,
	gui_qml_set_attr,
};

static void
gui_qml_command(struct gui_priv *this_, char *function, struct attr **in, struct attr ***out, int *valid) {
	dbg(lvl_debug, "gui_qml_command called\n");
	// this_->guiProxy->processCommand(function);
}

static struct command_table commands[] = {
	{"*",command_cast(gui_qml_command)},
};

static struct gui_priv * gui_qml_new(struct navit *nav, struct gui_methods *meth, struct attr **attrs, struct gui *gui)
{
	dbg(lvl_info,"Initializing qml2 gui\n");
	struct gui_priv *this_;
	struct attr *attr;
	*meth=gui_qml_methods;
	this_=g_new0(struct gui_priv, 1);

	this_->nav=nav;
	this_->gui=gui;

	this_->self.type=attr_gui;
	this_->self.u.gui=gui;	

 	// navit_ignore_graphics_events(this_->nav, 1);
 
 	this_->menu_on_map_click = 1;

	if ((attr=attr_search(attrs, NULL, attr_callback_list))) {
		command_add_table(attr->u.callback_list, commands, sizeof(commands)/sizeof(struct command_table), this_);
	}

	this_->cbl=callback_list_new();

	return this_;
}

void plugin_init(void) {
    plugin_register_gui_type("qml2",gui_qml_new);
}
