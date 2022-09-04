#pragma once

class UIMainWindow;
class UIMainStatusBar;
class UIAvailablePackages;
class UITripletList;

class AppHelper {
public:
	static UIMainWindow* mainWindow();
	static UIMainStatusBar* uiMainStatusBar();
	static UIAvailablePackages* uiAvailablePackages();
	static UITripletList* uiTripletList();

	static QString getArcheType();

	static void progressStart( const QString& message );
	static void progressStop();
};
