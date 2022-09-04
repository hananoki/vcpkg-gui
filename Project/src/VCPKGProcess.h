#pragma once

class App::Impl;

enum class CommandType {
	version,
	list,
	search,
	install,
	remove,
	help,
	integrate,
	log,
	install_finish,
	remove_finish,
};
static const QString CommandTypeName[] = {
	"version",
	"list",
	"search",
	"install",
	"remove",
	"help",
	"integrate",
	"log",
};

class VCPKGProcess : public QObject {
	Q_OBJECT
		friend class App::Impl;
	VCPKGProcess();

public:
	~VCPKGProcess();
	static void list();
	static void search();
	static void install( const QString& name );
	static void remove( const QString& name );
	static void help( const QString& option );
	static void version();
	static void integrate( const QString& option );

	static VCPKGProcess* instance();

	static void connectExecuteResult( std::function<void( CommandType type, const QStringList& text )> func );
	static void connectExecuteRead( std::function<void( CommandType type,  QString text )> func );

signals:
	void executeResult( CommandType type, const QStringList text );
	void executeRead( CommandType type, QString message );

	void startCommand( QString message );

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

