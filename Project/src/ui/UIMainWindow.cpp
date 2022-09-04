#include "UIMainWindow.h"
#include "ui_UIMainWindow.h"

#include "UIMainStatusBar.h"
#include "AppHelper.h"
#include "VCPKGProcess.h"


//////////////////////////////////////////////////////////////////////////////////
class UIMainWindow::Impl : public Ui_UIMainWindow {
	friend struct Helper;
public:
	static UIMainWindow* instance;
	UIMainWindow* self;

	QList<QWidget*> panels;

	QAction* shortcutF5;

	UIMainStatusBar* uiMainStatusBar;

	int currentTabIndex;
	
	/////////////////////////////////////////
	Impl( UIMainWindow* self ) :
		self( self ) {

		instance = self;
		setupUi( self );
		uiMainStatusBar = new UIMainStatusBar( self );
		statusbar->addWidget( uiMainStatusBar, 1 );
	}


	/////////////////////////////////////////
	void setup() {
		main_caption->attach( self );

		tabWidget->setCurrentIndex( 0 );

		$PushButton::clicked( pushButton, [&]( bool ) {
			textEdit->clear();
		} );

		QObject::connect( VCPKGProcess::instance(), &VCPKGProcess::executeRead, [&]( CommandType /*type*/, QString message ) {
			textEdit->append( message );
		} );
	}


	/////////////////////////////////////////
	void backupWindow() {
		config.pos = self->pos();
		config.size = self->size();
	}


	/////////////////////////////////////////
	void rollbackWindow() {
		self->move( config.pos );
		self->resize( config.size );
	}
};


UIMainWindow* UIMainWindow::Impl::instance;


//////////////////////////////////////////////////////////////////////////////////
UIMainWindow::UIMainWindow( QWidget* parent )
	: QtFramelessWindow( parent )
	, impl( new Impl( this ) ) {

}


/////////////////////////////////////////
UIMainWindow::~UIMainWindow() {

}


/////////////////////////////////////////
void UIMainWindow::start() {
	impl->setup();

	impl->rollbackWindow();

	emit signal_start();
	emit signal_startAfter();
}


/////////////////////////////////////////
void UIMainWindow::closeEvent( QCloseEvent* ) {
	impl->backupWindow();
}


UIMainWindow* AppHelper::mainWindow() {
	return UIMainWindow::Impl::instance;
}

UIMainStatusBar* AppHelper::uiMainStatusBar() {
	return mainWindow()->impl->uiMainStatusBar;
}

UIAvailablePackages* AppHelper::uiAvailablePackages() {
	return mainWindow()->impl->widget;
}

UITripletList* AppHelper::uiTripletList() {
	return mainWindow()->impl->widget_2;
}