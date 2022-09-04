#include "UIAvailablePackages.h"
#include "ui_UIAvailablePackages.h"

#include "AppHelper.h"
#include "UIMainWindow.h"
#include "VCPKGProcess.h"
#include "UITripletList.h"

//////////////////////////////////////////////////////////////////////////////////
class UIAvailablePackages::Impl : public Ui_UIAvailablePackages {
public:
	UIAvailablePackages* self;

	QList<QTreeWidgetItem*> items;

	/////////////////////////////////////////
	UIAvailablePackages::Impl( UIAvailablePackages* _self ) :
		self( _self ) {

		setupUi( self );

		pushButton->setIcon( icon::FA::check() );
		pushButton->setDisabled( true );

		$ComboBox::addItems( combo_arche, config.archeTypes );

		combo_arche->setCurrentIndex( config.archeTypes.indexOf( config.selectArcheType ) );

		/// アイテムをクリック
		connect( treeWidget, &QTreeWidget::itemChanged, [&]( QTreeWidgetItem* item, int column ) {
			pushButton->setDisabled( false );
		} );

		connect( qtApp, &App::vcpkgEnabled, []() {
			VCPKGProcess::search();
		} );
	}


	/////////////////////////////////////////
	void applySearch( const QStringList& texts ) {
		treeWidget->clear();
		items.clear();

		QList<QTreeWidgetItem*> lst;
		for( auto& p : texts ) {

			auto* item = new QTreeWidgetItem();
			bool succues = $::match( p, R"(([a-z0-9:\-\[\]]+)\s+([0-9\-#\.]+)\s+(.+))", [&]( auto& m ) {
				item->setText( 0, m.captured( 1 ) );
				item->setText( 1, m.captured( 2 ) );
				item->setText( 2, m.captured( 3 ) );
				item->setCheckState( 0, Qt::Unchecked );
				lst << item;
				items << item;
			} );
			if( !succues ) {
				succues = $::match( p, R"(([a-z0-9:\-\[\]]+)\s+(.+))", [&]( auto& m ) {
					item->setText( 0, m.captured( 1 ) );
					//item->setText( 1, m. );
					item->setText( 2, m.captured( 2 ) );
					item->setCheckState( 0, Qt::Unchecked );
					lst << item;
					items << item;
				} );
			}
			if( item->text( 0 ) == "zziplib" ) break;
		}

		treeWidget->addTopLevelItems( lst );
	}


	/////////////////////////////////////////
	void applyInstall() {
		for( auto& p : items ) {
			p->setCheckState( 0, Qt::Unchecked );
		}
	}


	/////////////////////////////////////////
	void setup() {
		$PushButton::clicked( pushButton, [&]( bool ) {
			for( auto& i : items ) {

				if( !i->checkState( 0 ) ) continue;

				VCPKGProcess::install( i->text( 0 ) + ":" + AppHelper::getArcheType() );
			}
		} );

		lineEdit->setClearButtonEnabled( true );
		lineedit::textEdited( lineEdit, [&]( const QString& text ) {
			QList<QTreeWidgetItem*> lst( items );

			for( auto& s : text.simplified().split( " " ) ) {
				for( auto& i : lst ) {
					bool b = i->text( 0 ).contains( s );
					i->setHidden( !b );
					if( !b ) lst.removeOne( i );
				}
			}
		} );

		$ComboBox::currentIndexChanged( combo_arche, [&]( const QString& s ) {
			config.selectArcheType = s;
		} );

		connect( AppHelper::uiTripletList(), &UITripletList::archeTypeChanged, [&]() {
			combo_arche->clear();
			$ComboBox::addItems( combo_arche, config.archeTypes );
			combo_arche->setCurrentIndex( config.archeTypes.indexOf( config.selectArcheType ) );
		} );

		VCPKGProcess::connectExecuteResult( [&]( CommandType type, const QStringList& texts ) {
			switch( type ) {
				case CommandType::install:
					applyInstall();
					break;
				case CommandType::search:
					applySearch( texts );
					break;
			}
		} );

	}
};


//////////////////////////////////////////////////////////////////////////////////
UIAvailablePackages::UIAvailablePackages( QWidget* parent ) :
	QWidget( parent ),
	impl( new Impl( this ) ) {

	connect( AppHelper::mainWindow(), &UIMainWindow::signal_start, [=]() {
		$::restoreState( impl->treeWidget->header(), config.columnAva );
		impl->setup();
	} );
}


/////////////////////////////////////////
UIAvailablePackages::~UIAvailablePackages() {
	//config.archetype = impl->radioButton_x64->isChecked() ? 1 : 0;
	$::saveState( config.columnAva, impl->treeWidget->header() );
}


QString AppHelper::getArcheType() {
	return AppHelper::uiAvailablePackages()->impl->combo_arche->currentText();
}
