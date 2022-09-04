#pragma once

#include <QWidget>

class AppHelper;

class UIMainStatusBar : public QWidget {
	friend class AppHelper;
	Q_OBJECT

public:
	explicit UIMainStatusBar( QWidget* parent = nullptr );
	~UIMainStatusBar();


	void info( const QString& message, int msec = 5000 );
	void warning( const QString& message, int msec = 5000 );
	void error( const QString& message, int msec = 5000 );
	
public slots:
	void progressStart( const QString& message );

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

