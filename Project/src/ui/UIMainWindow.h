#pragma once

#include "QtFramelessWindow.h"

class TreeItem_Base;

class AppHelper;


//////////////////////////////////////////////////////////////////////////////////
class UIMainWindow : public QtFramelessWindow {
	friend class AppHelper;
	Q_OBJECT

public:
	explicit UIMainWindow( QWidget* parent = nullptr );
	~UIMainWindow();

	void start();

	virtual void closeEvent( QCloseEvent* ) override;

signals:
	void signal_start();
	void signal_startAfter();

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};





