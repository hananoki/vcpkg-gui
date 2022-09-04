#include "Config.h"
#include "json.h"

Config config;

Config::Config() :
	size( QSize( 800, 600 ) ) {
}

//
/////////////////////////////////////////
void Config::load() {
	json::read( QCoreApplication::applicationName() + ".json", []( const QJsonObject& jsonObj ) {
#undef CONFIG_VALUE
#define CONFIG_VALUE(type,name) config. name = json::GetValue<type>( jsonObj[ #name ] );

		CONFIG_VALUE_LIST

			if( config.archeTypes.length() == 0 )
				config.archeTypes << "x86-windows";
	} );
}



///////////////////////////////////////
void Config::save() {
	json::write( QCoreApplication::applicationName() + ".json", []( QJsonObject& jsonObj ) {

#undef CONFIG_VALUE
#define CONFIG_VALUE(type,name) json::SetValue( jsonObj , #name, config. name );

		CONFIG_VALUE_LIST
	} );
}






