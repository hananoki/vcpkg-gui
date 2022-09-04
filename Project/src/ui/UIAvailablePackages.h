#pragma once

#include <QWidget>

class AppHelper;

class UIAvailablePackages : public QWidget {
	friend class AppHelper;
	Q_OBJECT

public:
	explicit UIAvailablePackages( QWidget* parent = nullptr );
	~UIAvailablePackages();
	
	
private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

