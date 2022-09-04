#pragma once

#include <QWidget>

class UITripletList : public QWidget {
	Q_OBJECT

public:
	explicit UITripletList( QWidget* parent = nullptr );
	~UITripletList();

signals:
	void archeTypeChanged();

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

