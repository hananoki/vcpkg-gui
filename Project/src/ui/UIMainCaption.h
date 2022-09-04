#pragma once

#include <QWidget>

class QtFramelessWindow;

class UIMainCaption : public QWidget {
	Q_OBJECT

public:
	explicit UIMainCaption( QWidget* parent = nullptr );
	~UIMainCaption();

	void attach( QtFramelessWindow* dialog );

private:
	class Impl;
	std::unique_ptr<Impl> impl;

	bool UIMainCaption::eventFilter( QObject* obj, QEvent* event );
};
