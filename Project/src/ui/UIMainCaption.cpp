#include "UIMainCaption.h"
#include "ui_UIMainCaption.h"

#include "UIMainWindow.h"
#include "AppHelper.h"
#include "UIMainStatusBar.h"
#include "VCPKGProcess.h"

#include "QtAwesome/QtAwesome.h"


//////////////////////////////////////////////////////////////////////////////////
class UIMainCaption::Impl : public Ui_UIMainCaption {
public:
	UIMainCaption* self;

	QAction* decideAction;

	QMenu* contextMenu;

	/////////////////////////////////////////
	Impl( UIMainCaption* self ) :self( self ) {
		setupUi( self );

		button_Tools->setIcon( icon::FA::ellipsisv() );
		//title_icon->setPixmap( QPixmap::fromImage( QImage( ":/res/image/logo_400x400.png" ) ).scaled( 16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );

#pragma region window button
		QVariantMap variantMap;
		variantMap.insert( "color", "#FFF" );
		variantMap.insert( "color-selected", "#FFF" );
		variantMap.insert( "color-disabled", "#666" );
		variantMap.insert( "color-active", "#FFF" );
		variantMap.insert( "scale-factor", 0.5 );

		titile_min_button->setIcon( icon::awesome->icon( fa::windowminimize, variantMap ) );
		connect( titile_min_button, &QPushButton::clicked, []( bool ) {
			AppHelper::mainWindow()->showMinimized();
		} );

		titile_max_button->setIcon( icon::awesome->icon( fa::windowmaximize, variantMap ) );
		titile_max_button->setDisabled( true );

		titile_close_button->setIcon( icon::awesome->icon( fa::close, variantMap ) );
		connect( titile_close_button, &QPushButton::clicked, []( bool ) {
			AppHelper::mainWindow()->close();
		} );
#pragma endregion

		connect( qtApp, &App::vcpkgEnabled, []() {
			VCPKGProcess::version();
		} );

		contextMenu = new QMenu();
		auto* a = new QAction();
		a->setText( tr( "Command Prompt" ) );
		a->setIcon( icon::get( "C:/Windows/System32/cmd.exe" ) );
		connect( a, &QAction::triggered, [=]() {
			qputenv( "PATH", App::getEnvPath().toUtf8() );
			QStringList arg;
			auto bak = QDir::currentPath();
			QDir::setCurrent( path::getDirectoryName( config.vcpkgPath ) );
			QDesktopServices::openUrl( QUrl::fromLocalFile( "C:/Windows/System32/cmd.exe" ) );
			QDir::setCurrent( bak );
			qputenv( "PATH", path::getDirectoryName( config.vcpkgPath ).toUtf8() );
		} );
		contextMenu->addAction( a );

		auto* b = new QAction();
		b->setText( tr( "integrate install" ) );
		contextMenu->addAction( b );
		connect( b, &QAction::triggered, [=]() {
			VCPKGProcess::integrate( "install" );
		} );

		auto* c = new QAction();
		c->setText( tr( "integrate remove" ) );
		contextMenu->addAction( c );
		connect( c, &QAction::triggered, [=]() {
			VCPKGProcess::integrate( "remove" );
		} );


		auto* d = new QAction();
		d->setText( tr( "change vcpkg.exe path" ) );
		contextMenu->addAction( d );
		connect( d, &QAction::triggered, [=]() {
			widget_2->show();
		} );

		edit_vcpkg_path->installEventFilter( self );
	}



	/////////////////////////////////////////
	void setup() {

		lineedit::textChanged( edit_vcpkg_path, [&]( const QString& inputString ) {
			if( inputString.isEmpty() || fs::isExistFile( inputString ) ) {
				edit_vcpkg_path->setStyleSheet( "" );
				decideAction->setDisabled( !fs::isExistFile( inputString ) );
			}
			else {
				edit_vcpkg_path->setStyleSheet( "background: #844" );
				decideAction->setDisabled( true );
			}
		} );

		button_Tools->setMenu( contextMenu );
		connect( button_Tools, &QToolButton::clicked, [&]( bool ) {
			auto r = button_Tools->pos();
			r += widget_4->pos();
			r.setY( r.y() + button_Tools->height() );
			contextMenu->exec( AppHelper::mainWindow()->mapToGlobal( r ) );
		} );

		VCPKGProcess::connectExecuteResult( [&]( CommandType type, const QStringList& texts ) {
			if( type != CommandType::version ) return;
			bool succues = false;
			if( 0 < texts.length() ) {
				succues = $::match( texts[ 0 ], R"(.*\s([a-z0-9\-]+))", [&]( QRegularExpressionMatch& m ) {
					title_label->setText( "vcpkg - " + m.captured( 1 ) );
				} );
			}
			if( !succues ) {
				title_label->setText( "vcpkg - " + tr( "unknown version" ) );
			}
		} );

		decideAction = new QAction();
		decideAction->setIcon( icon::FA::check() );
		edit_vcpkg_path->addAction( decideAction, QLineEdit::TrailingPosition );
		edit_vcpkg_path->setText( config.vcpkgPath );
		connect( decideAction, &QAction::triggered, [&]() {
			App::setVCPKGEnv( edit_vcpkg_path->text() );
			widget_2->hide();
		} );
	}

	/////////////////////////////////////////
	void setupAfter() {
		bool ok = fs::isExistFile( config.vcpkgPath );
		decideAction->setDisabled( !ok );

		if( ok ) {
			App::setVCPKGEnv( config.vcpkgPath );
			widget_2->hide();
		}
		else if( !config.vcpkgPath.isEmpty() ) {
			edit_vcpkg_path->setStyleSheet( "background: #844" );
		}
	}
};


//////////////////////////////////////////////////////////////////////////////////
UIMainCaption::UIMainCaption( QWidget* parent ) :
	QWidget( parent ),
	impl( new Impl( this ) ) {
}


/////////////////////////////////////////
UIMainCaption::~UIMainCaption() {}


/////////////////////////////////////////
bool UIMainCaption::eventFilter( QObject* obj, QEvent* event ) {
	//QEvent::Type type = event->type();
	if( obj != impl->edit_vcpkg_path ) return false;

	if( event->type() == QEvent::Drop ) {
		QDropEvent* ee = (QDropEvent*) event;
		if( !ee->mimeData()->hasUrls() ) return false;

		auto lst = ee->mimeData()->urls();

		auto path = QUrl( lst[ 0 ] ).toLocalFile();

		impl->edit_vcpkg_path->setText( path );

		return true;
	}
	if( event->type() == QEvent::DragEnter ) {
		QDragEnterEvent* ee = (QDragEnterEvent*) event;
		ee->acceptProposedAction();
		return true;
	}
	if( event->type() == QEvent::DragMove ) {
		QDragMoveEvent* ee = (QDragMoveEvent*) event;
		ee->acceptProposedAction();
		return true;
	}
	return false;
}


/////////////////////////////////////////
void UIMainCaption::attach( QtFramelessWindow* window ) {

	window->addCaptionWidget( impl->title_icon );
	window->addCaptionWidget( impl->title_label );
	window->addCaptionWidget( impl->widget );
	//window->addCaptionWidget( widget_3 );
	window->addCaptionWidget( impl->widget_4 );
	window->addCaptionWidget( this );

	connect( AppHelper::mainWindow(), &UIMainWindow::signal_start, [&]() {
		impl->setup();
	} );
	connect( AppHelper::mainWindow(), &UIMainWindow::signal_startAfter, [&]() {
		impl->setupAfter();
	} );
}


