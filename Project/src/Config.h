#pragma once

#define CONFIG_VALUE(type,name) type name;

#define CONFIG_VALUE_LIST \
	CONFIG_VALUE( QPoint, pos ) \
	CONFIG_VALUE( QSize, size ) \
	CONFIG_VALUE( int, timeout ) \
	CONFIG_VALUE( QStringList, archeTypes ) \
	CONFIG_VALUE( QString, selectArcheType ) \
	CONFIG_VALUE( QString, column ) \
	CONFIG_VALUE( QString, columnAva ) \
	CONFIG_VALUE( QString, vcpkgPath ) 

class Config {
public:
	CONFIG_VALUE_LIST

	Config();
	static void save();
	static void load();

};

extern Config config;

