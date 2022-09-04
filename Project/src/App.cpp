#include "App.h"
#include "UIMainWindow.h"

#include "VCPKGProcess.h"
#include "QtFusionStyle.h"



//////////////////////////////////////////////////////////////////////////////////
class App::Impl {
public:
	App* self;
	QString appPath;
	VCPKGProcess vcpkgProcess;

	QString envPath;

	/////////////////////////////////////////
	Impl( App* _self )
		: self( _self ) {

		envPath = qgetenv( "PATH" );
	}

	/////////////////////////////////////////
	~Impl() {

	}


	/////////////////////////////////////////
	bool loadCurrentLangTranslator( QTranslator& translator ) {
		QString current_langfile = "qt_ja";
		if( fs::isExistFile( "translations/qt_ja.qm" ) ) {
			if( !translator.load( current_langfile, "translations" ) ) {
				assert( false );
				return false;
			}
		}
		return true;
	}


	/////////////////////////////////////////
	int run() {
		qApp->setStyle( QStyleFactory::create( "Fusion" ) );
		QPalette darkPalette;
		QColor darkColor = "#222222";// QColor( 45, 45, 45 );
		QColor disabledColor = QColor( 127, 127, 255 );
		darkPalette.setColor( QPalette::Window, darkColor );
		darkPalette.setColor( QPalette::WindowText, Qt::white );
		darkPalette.setColor( QPalette::Base, "#3E3E3E" /*QColor( 18, 18, 18 )*/ );
		darkPalette.setColor( QPalette::AlternateBase, darkColor );
		darkPalette.setColor( QPalette::ToolTipBase, Qt::white );
		darkPalette.setColor( QPalette::ToolTipText, Qt::white );
		darkPalette.setColor( QPalette::Text, Qt::white );
		darkPalette.setColor( QPalette::Disabled, QPalette::Text, disabledColor );
		darkPalette.setColor( QPalette::Button, darkColor );
		darkPalette.setColor( QPalette::ButtonText, Qt::white );
		darkPalette.setColor( QPalette::Disabled, QPalette::ButtonText, disabledColor );
		darkPalette.setColor( QPalette::BrightText, Qt::red );
		darkPalette.setColor( QPalette::Link, QColor( 42, 130, 218 ) );

		darkPalette.setColor( QPalette::Highlight, QColor( 42, 130, 218 ) );
		darkPalette.setColor( QPalette::HighlightedText, Qt::black );
		darkPalette.setColor( QPalette::Disabled, QPalette::HighlightedText, disabledColor );

		qApp->setPalette( darkPalette );

		qApp->setStyleSheet( "QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }" );

		icon::initQtAwesome( 0.9f );

		QTranslator t;
		loadCurrentLangTranslator( t );
		//t.load( "translations/qt_ja" );
		self->installTranslator( &t );

		// translate --->
		QTranslator translator;
		translator.load( "translations/main_ja" ); // 日本語の場合
		// translator.load("translate/qcss_fr"); // フランス語の場合はこうする
		self->installTranslator( &translator );
		// <---

		//Language language( "japanese" );
		//appPath = path::getDirectoryName( QCoreApplication::arguments()[ 0 ] );
		//if( 2 <= argc ) {
		//	appPath = path::getDirectoryName( QCoreApplication::arguments()[ 1 ] );
		//}

		//qtApp->applyLanguage();

		UIMainWindow w;

		QKeySequence keys_refresh( QKeySequence::Refresh );
		QAction* shortcutF5 = new QAction( &w );
		shortcutF5->setShortcut( keys_refresh );
		w.addAction( shortcutF5 );
		connect( shortcutF5, &QAction::triggered, []() {
			qApp->setStyleSheet( fs::readAllText( "style.qss" ) );
			//UIMainStatusBar::info( tr( "Updated to the latest information" ) );
		} );

		w.start();

		qApp->setStyleSheet( fs::readAllText( "style.qss" ) );

		w.show();
		return self->exec();
	}
};


//////////////////////////////////////////////////////////////////////////////////
App* App::instance;


/////////////////////////////////////////
App::App( int& argc, char** argv, int /*= ApplicationFlags*/ ) :
	QApplication( argc, argv ),
	impl( new Impl( this ) ) {

	instance = this;
	config.load();
}


/////////////////////////////////////////
App::~App() {
	config.save();
}


/////////////////////////////////////////
void App::setVCPKGEnv( const QString& filePath ) {
	config.vcpkgPath = filePath;

	qputenv( "PATH", path::getDirectoryName( filePath ).toUtf8() );
	emit instance->vcpkgEnabled();
}


/////////////////////////////////////////
QString App::getEnvPath() {
	return instance->impl->envPath;
}


//////////////////////////////////////////////////////////////////////////////////
void logHandler( QtMsgType type, const QMessageLogContext& context, const QString& msg ) {
	const auto& message = qFormatLogMessage( type, context, msg );
	QTextStream cerr( stderr );
	cerr << message << endl;
	QFile file( "App.log" );
	if( !file.open( QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text ) ) {
		cerr << "Cannot open log file:" << file.fileName();
		return;
	}
	QTextStream( &file ) << message << endl;
}

/////////////////////////////////////////
int main( int argc, char* argv[] ) {
	qInstallMessageHandler( logHandler );
	qSetMessagePattern( "[%{time yyyy-MM-dd hh:mm:ss}] [%{category}] [%{threadid}] %{file}:%{line} => %{message} " );
	App app( argc, argv );
	return app.impl->run();
}

