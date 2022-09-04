#include "UIInstallPackage.h"
#include "ui_UIInstallPackage.h"

#include "AppHelper.h"
#include "UIMainWindow.h"
#include "VCPKGProcess.h"

//////////////////////////////////////////////////////////////////////////////////
class UIInstallPackage::Impl : public Ui_UIInstallPackage {
public:
	UIInstallPackage* self;

	QList<QTreeWidgetItem*> items;

	/////////////////////////////////////////
	UIInstallPackage::Impl( UIInstallPackage* _self ) :
		self( _self ) {

		setupUi( self );

		pushButton->setIcon( icon::FA::check() );
		pushButton->setDisabled( true );

		/// アイテムをクリック
		connect( treeWidget, &QTreeWidget::itemChanged, [&]( QTreeWidgetItem* item, int column ) {
			pushButton->setDisabled( false );
		} );

		connect( qtApp, &App::vcpkgEnabled, []() {
			VCPKGProcess::list();
		} );
	}


	/////////////////////////////////////////
	void applyList( const QStringList& texts ) {
		treeWidget->clear();
		items.clear();
		for( auto& p : texts ) {
			auto* item = new QTreeWidgetItem();
			$::match( p, R"(([a-z0-9:\-]+)\s+([0-9\-#\.]+)\s+(.+))", [&]( auto& m ) {
				item->setText( 0, m.captured( 1 ) );
				item->setText( 1, m.captured( 2 ) );
				item->setText( 2, m.captured( 3 ) );
				item->setCheckState( 0, Qt::Checked );
				items << item;
			} );
		}

		treeWidget->addTopLevelItems( items );
	}


	/////////////////////////////////////////
	void setup() {
		$PushButton::clicked( pushButton, [&]( bool ) {
			using namespace cpplinq;

			auto lst = from( items )
				>> where( []( QTreeWidgetItem* i ) {return !i->checkState( 0 ); } )
				>> to_qlist();

			for( auto& p  : lst ) {
				VCPKGProcess::remove( p->text( 0 ) );
			}
		} );

		VCPKGProcess::connectExecuteResult( [&]( CommandType type, const QStringList& texts ) {
			switch( type ) {
				case CommandType::install_finish:
				case CommandType::remove_finish:
					VCPKGProcess::list();
					break;
				case CommandType::list:
					applyList( texts );
					break;
			}
		} );
	}
};


//////////////////////////////////////////////////////////////////////////////////
UIInstallPackage::UIInstallPackage( QWidget* parent ) :
	QWidget( parent ),
	impl( new Impl( this ) ) {

	connect( AppHelper::mainWindow(), &UIMainWindow::signal_start, [=]() {
		$::restoreState( impl->treeWidget->header(), config.column );
		impl->setup();
	} );
}


/////////////////////////////////////////
UIInstallPackage::~UIInstallPackage() {
	$::saveState( config.column, impl->treeWidget->header() );
}
