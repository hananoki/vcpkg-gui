#pragma once


class Language {
public:
	void read( QString langName );

	Language( QString langName );
	QString operator[]( QString s );

	static QString get( QString s );

	QHash<QString, QString> m_languageFileMap;
	QHash<QString, QString> m_dic;
};

