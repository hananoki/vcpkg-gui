#include "VCPKGProcess.h"
#include "AppHelper.h"


//////////////////////////////////////////////////////////////////////////////////
class VCPKGProcess::Impl {
public:
	VCPKGProcess* self;
	static VCPKGProcess* instance;

	QString command_message;

	QFuture<void> fff;

	int exit = 0;


	struct Request {
		CommandType command;
		QStringList option;
	};

	struct RequestResult {
		CommandType command;
		QStringList option;
	};

	QMutex _mutex;
	QMutex _mutex2;

	QWaitCondition _cond;
	QWaitCondition _cond2;
	QQueue<Request> _queue;
	QQueue<RequestResult> _queue2;

	QFutureWatcher<void> _watcher;


	/////////////////////////////////////////
	Impl( VCPKGProcess* _self ) :
		self( _self ) {

		instance = self;

		QObject::connect( &_watcher, &QFutureWatcher<void>::finished, self, [&]() {
			QMutexLocker lock( &_mutex2 );
			bool remove = false;
			bool install = false;
			while( !_queue2.isEmpty() ) {
				auto result = _queue2.dequeue();
				switch( result.command ) {
					case CommandType::install:
						install = true;
						break;
					case CommandType::remove:
						remove = true;
						break;
				}
				self->executeResult( result.command, result.option );
			}
			if( install ) {
				self->executeResult( CommandType::install_finish, {} );
			}
			if( remove ) {
				self->executeResult( CommandType::remove_finish, {} );
			}
			AppHelper::progressStop();
		} );

		QObject::connect( &_watcher, &QFutureWatcher<void>::progressValueChanged, self, [&]( int ) {
			AppHelper::progressStart( command_message );
		} );

		QObject::connect( &_watcher, &QFutureWatcher<void>::progressTextChanged, self, [&]( const QString& m ) {
			self->executeRead( CommandType::log, m );
		} );

		worker();
	}


	/////////////////////////////////////////
	~Impl() {
		exit = 1;
		_cond.wakeAll();
		fff.waitForFinished();
	}


	/////////////////////////////////////////
	void putRequest( CommandType t, QStringList option ) {
		QMutexLocker _( &_mutex );
		_queue.enqueue( Request{ t, option } );
		_cond.wakeAll();
	}

	/////////////////////////////////////////
	void setResult( CommandType t, QStringList option ) {
		QMutexLocker _( &_mutex2 );
		_queue2.enqueue( RequestResult{ t, option } );
		_cond2.wakeAll();
	}

	/////////////////////////////////////////
	void worker() {
		fff = QtConcurrent::run( [&]() {
			while( !exit ) {
				Request req;
				{
					QMutexLocker _( &_mutex );
					while( _queue.isEmpty() ) {
						_watcher.finished();
						_cond.wait( &_mutex );
						if( exit )return;
					}
					req = _queue.dequeue();
				}
				QStringList result;
				auto command = $$( "> vcpkg.exe %1 %2" )
					.arg( CommandTypeName[ (int) req.command ] )
					.arg( req.option.join( " " ) );

				_watcher.progressTextChanged( command );
				{
					QProcess process;
					QObject::connect( &process, &QProcess::readyReadStandardOutput, [&, req]() {
						QByteArray output = process.readAllStandardOutput();
						QString str = QString::fromLocal8Bit( output );

						auto log = str.split( "\n" );
						result << log;

						_watcher.progressTextChanged( str );
					} );
					QStringList arg;
					arg << CommandTypeName[ (int) req.command ];
					arg << req.option;

					command_message = "vcpkg " + arg.join( " " ) + "... ";
					_watcher.progressValueChanged( 0 );

					process.start( "vcpkg.exe", arg );
					process.waitForFinished(-1);
				}
				setResult( req.command, result );
			}
		} );
		_watcher.setFuture( fff );
	}

};

VCPKGProcess* VCPKGProcess::Impl::instance;

//////////////////////////////////////////////////////////////////////////////////
VCPKGProcess::VCPKGProcess() :
	impl( new Impl( this ) ) {
}

VCPKGProcess::~VCPKGProcess() {}


VCPKGProcess* VCPKGProcess::instance() {
	return Impl::instance->impl->self;
}


void VCPKGProcess::search() {
	Impl::instance->impl->putRequest( CommandType::search, {  } );
}

void VCPKGProcess::list() {
	Impl::instance->impl->putRequest( CommandType::list, {} );
}

void VCPKGProcess::install( const QString& name ) {
	Impl::instance->impl->putRequest( CommandType::install, { name } );
}

void VCPKGProcess::remove( const QString& name ) {
	Impl::instance->impl->putRequest( CommandType::remove, { name } );
}

void VCPKGProcess::help( const QString& option ) {
	Impl::instance->impl->putRequest( CommandType::help, { option } );
}

void VCPKGProcess::version() {
	Impl::instance->impl->putRequest( CommandType::version, {} );
}
void VCPKGProcess::integrate( const QString& option ) {
	Impl::instance->impl->putRequest( CommandType::integrate, { option } );
}

void VCPKGProcess::connectExecuteResult( std::function<void( CommandType type, const QStringList& text )> func ) {
	QObject::connect( VCPKGProcess::instance(), &VCPKGProcess::executeResult, func );
}

void VCPKGProcess::connectExecuteRead( std::function<void( CommandType type, QString text )> func ) {
	QObject::connect( VCPKGProcess::instance(), &VCPKGProcess::executeRead, func );
}
