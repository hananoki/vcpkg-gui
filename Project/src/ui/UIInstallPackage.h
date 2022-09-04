#pragma once

#include <QWidget>


class UIInstallPackage : public QWidget {
	Q_OBJECT

public:
	explicit UIInstallPackage( QWidget* parent = nullptr );
	virtual ~UIInstallPackage();

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

