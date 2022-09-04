#include "UITripletList.h"
#include "ui_UITripletList.h"

#include "AppHelper.h"
#include "UIMainWindow.h"
#include "VCPKGProcess.h"

//////////////////////////////////////////////////////////////////////////////////
class UITripletList::Impl : public Ui_UITripletList {
public:
	UITripletList* self;

	QList<QTreeWidgetItem*> items;

	/////////////////////////////////////////
	UITripletList::Impl( UITripletList* _self ) :
		self( _self ) {

		setupUi( self );

		connect( qtApp, &App::vcpkgEnabled, []() {
			VCPKGProcess::help( "triplet" );
		} );
	}


	/////////////////////////////////////////
	void applyHelp( const QStringList& texts ) {
		treeWidget->clear();
		items.clear();

		QList<QTreeWidgetItem*> lst;
		const QString s1 = "VCPKG built-in triplets";
		const QString s2 = "VCPKG community triplets:";
		auto* g1 = new QTreeWidgetItem();
		g1->setText( 0, s1 );
		auto* g2 = new QTreeWidgetItem();
		g2->setText( 0, s2 );

		treeWidget->addTopLevelItem( g1 );
		treeWidget->addTopLevelItem( g2 );
		{
			QtSignalBlocker _( self );
			$::setBoldFont( *g1 );
			$::setBoldFont( *g2 );
		}
		int step = 0;
		for( auto& pp : texts ) {
			if( pp.isEmpty() )continue;
			auto p = pp.simplified();
			switch( step ) {
				case 0:
					if( p.contains( s1 ) ) {
						step = 1;
					}
					break;
				case 1:
					if( p.contains( s2 ) ) {
						step = 2;
						continue;
					}
					{
						auto* item1 = new QTreeWidgetItem();
						item1->setText( 0, p );
						item1->setCheckState( 0, 0 <= config.archeTypes.indexOf( p ) ? Qt::Checked : Qt::Unchecked );
						g1->addChild( item1 );
						items << item1;
					}
					break;
				case 2:
					auto * item2 = new QTreeWidgetItem();
					item2->setText( 0, p );
					item2->setCheckState( 0, 0 <= config.archeTypes.indexOf( p ) ? Qt::Checked : Qt::Unchecked );
					items << item2;
					g2->addChild( item2 );
					break;
			}
		}
		g1->setExpanded( true );
		g2->setExpanded( true );
	}

	/////////////////////////////////////////
	void setup() {
		/// アイテムをクリック
		connect( treeWidget, &QTreeWidget::itemChanged, [&]( QTreeWidgetItem* item, int column ) {
			auto st = item->checkState( 0 );
			if( st == Qt::Checked ) {
				config.archeTypes << item->text( 0 );
			}
			else {
				config.archeTypes.removeOne( item->text( 0 ) );
			}
			emit self->archeTypeChanged();
		} );

		VCPKGProcess::connectExecuteResult( [&]( CommandType type, const  QStringList& texts ) {
			switch( type ) {
				case CommandType::help:
					applyHelp( texts );
					break;
			}
		} );
	}
};


//////////////////////////////////////////////////////////////////////////////////
UITripletList::UITripletList( QWidget* parent ) :
	QWidget( parent ),
	impl( new Impl( this ) ) {

	connect( AppHelper::mainWindow(), &UIMainWindow::signal_start, [&]() {
		impl->setup();
	} );
}


/////////////////////////////////////////
UITripletList::~UITripletList() {
}
