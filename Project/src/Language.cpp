#include "Language.h"

Language* instance;

QString Language::get( QString s ) {
	return (*instance)[s];
}

Language::Language( QString langName ) {
	instance = this;
	auto path = QDir::currentPath() + "/.translations";
	if( !fs::isExistDirectory( path ) ) {
		//throw new Exception( "\".translations\" is Not Found." );
		qDebug() << "\".translations\" is Not Found.";
		return;
	}

	for( auto& s : fs::getFiles( path, "*.txt" ) ) {
		m_languageFileMap.insert( path::getBaseName( s ), s );
	}

	read( langName );

	QString aa = "IMPLEMENT_GAME_MODULE( FDefaultGameModuleImpl, HananokiRuntime );";

	$::match( aa, R"(IMPLEMENT_GAME_MODULE\(\s*([a-zA-Z]+),\s*([a-zA-Z]+)\s*\);)", [&]( QRegularExpressionMatch&  m) {
		//int i = m.capturedLength();
		//qDebug()<<m.captured( 0 );
		//qDebug()<<m.captured( 1 );
		//qDebug()<<m.captured( 2 );
		
		aa = "IMPLEMENT_GAME_MODULE( "+ m.captured( 1 ) +", aaa );";
	} );
	
}


/////////////////////////////////////////
void Language::read( QString langName ) {
	//QString langFilePath ;
	m_dic.clear();

	auto langFilePath = m_languageFileMap.find( langName ); //.TryGetValue( langName, out langFilePath );
	if( langFilePath == m_languageFileMap.end() )return;
	//if( langFilePath.IsEmpty() ) {
	//	return;
	//}

	for( auto& loc : fs::readAllLines( *langFilePath ) ) {
		if( loc.isEmpty() ) continue;
		auto locText = loc.split( '\t' );
		m_dic.insert( locText[ 0 ], locText[ 1 ] );
	}
}

/////////////////////////////////////////
QString Language::operator[]( QString s ) {
	auto result = m_dic.find( s );
	if( result == m_dic.end() )return "";
	return *result;
}
