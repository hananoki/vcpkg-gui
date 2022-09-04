#include "UIMainStatusBar.h"
#include "ui_UIMainStatusBar.h"

#include "AppHelper.h"

#include <QStyle>
#include <QTimer>


//////////////////////////////////////////////////////////////////////////////////
class UIMainStatusBar::Impl : public Ui_UIMainStatusBar {
public:
	UIMainStatusBar* self;

	QTimer* timer;

	int mode = 0;
	int step = 0;;
	QString commandMessage;

	/////////////////////////////////////////
	Impl( UIMainStatusBar* self ) :
		self( self ) {

		setupUi( self );

		timer = new QTimer( self );
		timer->stop();

		label->hide();
		label_status->hide();

		connect( timer, &QTimer::timeout, [&]() {_timeout(); } );
	}

	/////////////////////////////////////////
	void _timeout() {
		QStringList a( { "|", "/" , "-", "\\" } );
		if( mode ) {
			label_status->setText( commandMessage + a[ step ] );
			step++;
			if( a.length() <= step ) step = 0;
		}
		else {
			timer->stop();
			label->hide();
			label_status->hide();
		}
	}


	/////////////////////////////////////////
	void show( const QString& message, int type, int msec ) {
		label->show();
		label_status->show();
		mode = 0;

		QPixmap px;
		switch( type ) {
			case 0:
				px = PIXMAP_MSG_INFO;
				break;
			case 1:
				px = PIXMAP_MSG_WARNING;
				break;
			case 2:
				px = PIXMAP_MSG_ERROR;
				break;
		}
		px = px.scaled( 16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation );

		label->setPixmap( px );

		label_status->setText( message );

		timer->setInterval( msec );

		timer->start();
	}


	/////////////////////////////////////////
	void progressStart( const QString& message ) {
		commandMessage = message;
		label_status->setText( message );
		label_status->show();
		timer->setInterval( 200 );
		mode = 1;
		step = 0;
		timer->start();
	}

	/////////////////////////////////////////
	void progressStop() {
		mode = 0;
	}
};



//////////////////////////////////////////////////////////////////////////////////
UIMainStatusBar::UIMainStatusBar( QWidget* parent ) :
	QWidget( parent ),
	impl( new Impl( this ) ) {

}


/////////////////////////////////////////
UIMainStatusBar::~UIMainStatusBar() {}

void UIMainStatusBar::progressStart( const QString& message ) {
	AppHelper::uiMainStatusBar()->impl->progressStart( message );
}

/////////////////////////////////////////
void UIMainStatusBar::info( const QString& message, int msec ) {
	impl->show( message, 0, msec );
}

/////////////////////////////////////////
void UIMainStatusBar::warning( const QString& message, int msec ) {
	impl->show( message, 1, msec );
}

/////////////////////////////////////////
void UIMainStatusBar::error( const QString& message, int msec ) {
	impl->show( message, 2, msec );
}

//void UIMainStatusBar::progressStart( const QString& message ) {
//	impl->progressStart( message );
//}
//
//void UIMainStatusBar::progressStop() {
//	impl->progressStop();
//}

void AppHelper::progressStart( const QString& message ) {
	AppHelper::uiMainStatusBar()->impl->progressStart( message );
	//emit AppHelper::uiMainStatusBar()->progressStart( message );
}

void AppHelper::progressStop() {
	AppHelper::uiMainStatusBar()->impl->progressStop(  );
}
