/*
* YOUR LICENSE TEXT HERE
*/
#ifndef MyServerPlugin_H
#define MyServerPlugin_H

#include <buteosyncfw5/ServerPlugin.h>
#include <buteosyncfw5/SyncCommonDefs.h>
#include <buteosyncfw5/SyncResults.h>

namespace Buteo {
    class ServerPlugin;
    class Profile;
}

class MyServerPlugin : public Buteo::ServerPlugin
{
    Q_OBJECT

public:
    MyServerPlugin (const QString& pluginName,
                  const Buteo::Profile profile,
                  Buteo::PluginCbInterface *cbInterface);

    virtual ~MyServerPlugin ();

    virtual bool init ();

    virtual bool uninit ();

    virtual void abortSync (Sync::SyncStatus status = Sync::SYNC_ABORTED);

    virtual bool cleanUp ();

    virtual Buteo::SyncResults getSyncResults () const;

    virtual bool startListen ();

    virtual void stopListen ();

    virtual void suspend ();

    virtual void resume ();

signals:

    void syncFinished (Sync::SyncStatus);

public slots:

    virtual void connectivityStateChanged (Sync::ConnectivityType type, bool state);

protected slots:

    void handleSyncFinished (DataSync::SyncState state);

    void handleStateChanged (DataSync::SyncState state);

    void handleItemProcessed (DataSync::ModificationType modificationType,
                              DataSync::ModifiedDatabase modifiedDb,
                              QString localDb,
                              QString dbType, int committedItems);
private:

    bool initSyncAgent ();

    void closeSyncAgent ();

    bool initTransport ();

    MyProtocol::SyncAgentConfig *initSyncAgentConfig();

    void closeSyncAgentConfig ();

    bool startNewSession ();

    void generateResults (bool success);

    QMap<QString, QString>          mProperties;

    MyProtocol::SyncAgent*          mAgent;

    MyProtocol::SyncAgentConfig*    mConfig;

    MyProtocol::Transport*          mTransport;

    Buteo::SyncResults              mResults;

    qint32                          mCommittedItems;

    Sync::ConnectivityType          mConnectionType;
};

#endif // MyServerPlugin_H

