/*
 * Copyright (c) 2012 Mike Sharkey <michael_sharkey@firstclass.com>
 *
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Mike Sharkey wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 */
#include "opennicserver.h"
#include "openniclog.h"

#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QEventLoop>
#include <QByteArray>
#include <QSettings>
#include <QMultiMap>
#include <QMap>
#include <QHostAddress>
#include <QTcpSocket>

#if defined Q_OS_UNIX
#define DEFAULT_LOG_FILE						"/dev/tty"
#else
#define DEFAULT_LOG_FILE						"opennic.log"
#endif
#define DEFAULT_TCP_LISTEN_PORT					19803
#define DEFAULT_RESOLVER_REFRESH_RATE			15		/* minutes */
#define DEFAULT_RESOLVER_CACHE_SIZE				3
#define DEFAULT_BOOTSTRAP_CACHE_SIZE			3
#define DEFAULT_BOOTSTRAP_RANDOM_SELECT			true

#define inherited QObject

OpenNICServer::OpenNICServer(QObject *parent)
: inherited(parent)
{
	mStartTimer = startTimer(1000);
}

OpenNICServer::~OpenNICServer()
{
}

QStringList OpenNICServer::textToStringList(QString text)
{
	QStringList rc;
	rc = text.split('\n');
	return rc;
}

QString OpenNICServer::stringListToText(QStringList list)
{
	QString rc;
	rc = list.join("\n");
	return rc;
}

/**
  * @brief Fetch the settings.
  */
void OpenNICServer::readSettings()
{
	QSettings settings("OpenNIC", "OpenNICService");
	mTcpListenPort			= settings.value("tcp_listen_port",			DEFAULT_TCP_LISTEN_PORT).toInt();
	mLogFile				= settings.value("log_file",				DEFAULT_LOG_FILE).toString();
	mResolverCache			= settings.value("resolver_cache").toStringList();
	mResolverRefreshRate	= settings.value("resolver_refresh_rate",	DEFAULT_RESOLVER_REFRESH_RATE).toInt();
	mResolverCacheSize		= settings.value("resolver_cache_size",		DEFAULT_RESOLVER_CACHE_SIZE).toInt();
	mBootstrapT1List		= settings.value("bootstrap_t1_list",		resolver().defaultT1List()).toStringList();
	mBootstrapCacheSize		= settings.value("bootstrap_cache_size",	DEFAULT_BOOTSTRAP_CACHE_SIZE).toInt();
	mBootstrapRandomSelect	= settings.value("bootstrap_random_select",	DEFAULT_BOOTSTRAP_RANDOM_SELECT).toBool();
}

/**
  * @brief Store the settings.
  */
void OpenNICServer::writeSettings()
{
	QSettings settings("OpenNIC", "OpenNICService");
	settings.setValue("tcp_listen_port",			mTcpListenPort);
	settings.setValue("log_file",					mLogFile);
	settings.setValue("resolver_cache",				mResolverCache);
	settings.setValue("resolver_refresh_rate",		mResolverRefreshRate);
	settings.setValue("resolver_cache_size",		mResolverCacheSize);
	settings.setValue("bootstrap_t1_list",			mBootstrapT1List);
	settings.setValue("bootstrap_cache_size",		mBootstrapCacheSize);
	settings.setValue("bootstrap_random_select",	mBootstrapRandomSelect);

	killTimer(mRefreshTimer);
	mRefreshTimer = startTimer((mResolverRefreshRate*60)*1000);
}

/**
  * @brief Map server status
  * @return a map of key/value pairs
  */
QMap<QString,QVariant> OpenNICServer::mapServerStatus()
{
	QMap<QString,QVariant> map;
	map.insert("tcp_listen_port",			mTcpListenPort);
	map.insert("resolver_pool",				mResolver.getResolverPoolStringList()); /* <ipaddr>;<latency> */
	map.insert("resolver_cache",			mResolverCache);
	map.insert("resolver_refresh_rate",		mResolverRefreshRate);
	map.insert("resolver_cache_size",		mResolverCacheSize);
	map.insert("bootstrap_t1_list",			mBootstrapT1List);
	map.insert("bootstrap_cache_size",		mBootstrapCacheSize);
	map.insert("bootstrap_random_select",	mBootstrapRandomSelect);
	return map;
}

/**
  * @brief Process a client request.
  */
void OpenNICServer::process(QTcpSocket *client)
{
	QMap<QString,QVariant> clientPacket;
	QMap<QString,QVariant> serverPacket = mapServerStatus();
	QDataStream stream(client);
	stream >> clientPacket;
	stream << serverPacket;
}

/**
  * @brief Get here when a task tray applet has connected.
  */
void OpenNICServer::newConnection()
{
	QTcpSocket* client;
	while ( (client = mServer.nextPendingConnection()) != NULL )
	{
		OpenNICLog::log(OpenNICLog::Information,"connect from "+client->peerName());
		process(client);
		client->close();
		delete client;
		client=NULL;
	}
}

/**
  * @brief Set up the local server for the task tray app to attach to.
  * @return the number of resolvers
  */
int OpenNICServer::initializeServer()
{
	QHostAddress localhost(QHostAddress::LocalHost);
	mServer.setMaxPendingConnections(10);
	if ( mServer.listen(localhost,mTcpListenPort) )
	{
		QObject::connect(&mServer,SIGNAL(newConnection()),this,SLOT(newConnection()));
		OpenNICLog::log(OpenNICLog::Information,"listening on port "+QString::number(mTcpListenPort));
	}
	else
	{
		OpenNICLog::log(OpenNICLog::Information,mServer.errorString().trimmed());
	}

}

/**
  * @brief Perform the update function. Fetch DNS candidates, test for which to apply, and apply them.
  * @return the number of resolvers
  */
int OpenNICServer::initializeDNS()
{
	int n;
	int resolverCount = mBootstrapCacheSize < mBootstrapT1List.count() ? mBootstrapCacheSize : mBootstrapT1List.count();
	for(n=0; n < resolverCount; n++)
	{
		OpenNICLog::log(OpenNICLog::Information, "Using: " + mBootstrapT1List[n]);
		OpenNICLog::log(OpenNICLog::Information, resolver().addResolver(mBootstrapT1List[n],n+1));
	}
	return n;
}

/**
  * @brief Perform the update function. Fetch DNS candidates, test for which to apply, and apply them.
  * @return the number of resolvers
  */
int OpenNICServer::updateDNS()
{
	int n;
	QStringList ips = resolver().getResolvers();
	int resolverCount = mResolverCacheSize < ips.count() ? mResolverCacheSize : ips.count();
	mResolverCache.clear();
	for(n=0; n < resolverCount; n++)
	{
		QString ip = ips[n].trimmed();
		mResolverCache.append(ip);
		OpenNICLog::log(OpenNICLog::Information, "Using: " + ip);
		OpenNICLog::log(OpenNICLog::Information, resolver().addResolver(ip,n+1));
	}
	return n;
}

QString OpenNICServer::copyright()
{
	return "OpenNICServer V"+QString(VERSION_STRING)+ " (c) 2012 Mike Sharkey <michael_sharkey@firstclass.com>";
}

QString OpenNICServer::license()
{
	return QString( copyright() +
						"\"THE BEER-WARE LICENSE\" (Revision 42):\n"
						"Mike Sharkey wrote this thing. As long as you retain this notice you\n"
						"can do whatever you want with this stuff. If we meet some day, and you think\n"
						"this stuff is worth it, you can buy me a beer in return.\n"
					   );
}

/**
  * @brief get here on timed events.
  */
void OpenNICServer::timerEvent(QTimerEvent* e)
{
	if ( e->timerId() == mStartTimer)
	{
		/* get here just once just after startup */
		readSettings();
		if ( initializeDNS() )
		{
			initializeServer();
			OpenNICLog::log(OpenNICLog::Information,resolver().getSettingsText().trimmed());
		}
		killTimer(mStartTimer);									/* don't need start timer any more */
		mStartTimer=-1;
		mRefreshTimer = startTimer(1000);						/* run the update function soon */
	}
	else if ( e->timerId() == mRefreshTimer )
	{
		/* get here regularly... */
		readSettings();
		if ( updateDNS() )
		{
			OpenNICLog::log(OpenNICLog::Information,resolver().getSettingsText().trimmed());
		}
		/* in case we got here on the short timer, extend it to the settings value... */
		killTimer(mRefreshTimer);
		mRefreshTimer = startTimer((mResolverRefreshRate*60)*1000);
		writeSettings();
	}
	else
	{
		inherited::timerEvent(e);
	}
}




