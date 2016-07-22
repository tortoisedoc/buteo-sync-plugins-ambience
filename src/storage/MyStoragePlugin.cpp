/*
 * YOUR LICENSE TEXT HERE
 *
 */
#include <LogMacros.h>
#include "MyStoragePlugin.h"
#include "SimpleItem.h"

extern "C" Buteo::StoragePlugin*  createPlugin(const QString& aPluginName)
{
    return new MyStoragePlugin(aPluginName);
}

extern "C" void destroyPlugin(Buteo::StoragePlugin* storage)
{
    delete storage;
}

MyStoragePlugin::MyStoragePlugin(const QString& aPluginName)
 : Buteo::StoragePlugin(aPluginName), mBackend( 0 )
{
    FUNCTION_CALL_TRACE;
}

MyStoragePlugin::~MyStoragePlugin()
{
    FUNCTION_CALL_TRACE;
    if(mBackend) {
        LOG_WARNING( "Uninit method has not been called!" );
        delete mBackend;
        mBackend = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////////
////////////       Below functions are derived from storage plugin   ///////////////
////////////////////////////////////////////////////////////////////////////////////

bool
MyStoragePlugin::init( const QMap<QString, QString>& aProperties )
{
    FUNCTION_CALL_TRACE;

    // Initialize the MyStorageBackend class and the necessary
    // initialization for the storage

    return true;
}

bool
MyStoragePlugin::uninit()
{
    FUNCTION_CALL_TRACE;

    if( mBackend != NULL) {
        backendUninitOk = mBackend->uninit();
        delete mBackend;
        mBackend = NULL;
    }

    return (backendUninitOk);
}

bool
MyStoragePlugin::getAllItems(QList<Buteo::StorageItem*> &aItems)
{
    FUNCTION_CALL_TRACE;
    bool operationStatus = false;
    QList<DbId> list;
    if(mBackend) {
        list = mBackend->getAllDbIds();
        if(list.size() != 0) {
            qDebug()  << " Number of items retrieved from Contacts " << list.size();
            aItems = getStoreList(list);
        }
        operationStatus = true;
    }
    return operationStatus;
}

bool
MyStoragePlugin::getAllItemIds( QList<QString>& aItems )
{
    FUNCTION_CALL_TRACE;
    bool operationStatus = false;
    QList<DbId> list;
    if(mBackend) {
        list = mBackend->getAllDbIds();
        qDebug() << " Number of items retrieved from Contacts " << list.size();
        foreach(DbId id , list) {
            aItems.append(QString::number(id));
        }
        operationStatus = true;
    }
    return operationStatus;
}

bool
MyStoragePlugin::getNewItems(QList<Buteo::StorageItem*> &aItems ,const QDateTime& aTime)
{
    FUNCTION_CALL_TRACE;
    bool operationStatus = false;
    QList<DbId>  list;
    if(mBackend) {
        qDebug()  << "****** getNewItems : Added After: ********" << aTime;
        list = mBackend->getAllNewDbIds(aTime);
        if(list.size() != 0) {
            qDebug()  << "New Item List Size is " << list.size();
            aItems = getStoreList(list);
        }
        operationStatus = true;
    }
    return operationStatus;
}

bool
MyStoragePlugin::getNewItemIds( QList<QString>& aNewItemIds, const QDateTime& aTime )
{
    FUNCTION_CALL_TRACE;
    bool operationStatus = false;
    QList<DbId>  list;

    if(mBackend) {
        qDebug()  << "****** getNewItem Ids : Added After: ********" << aTime;
        list = mBackend->getAllNewDbIds(aTime);

        foreach(DbId id , list) {
            aNewItemIds.append(QString::number(id));
        }

        operationStatus = true;
    }
    return operationStatus;
}

bool
MyStoragePlugin::getModifiedItems( QList<Buteo::StorageItem*>& aModifiedItems, const QDateTime& aTime )
{
    FUNCTION_CALL_TRACE;
    QList<DbId>  list;
    bool operationStatus = false;

    if(mBackend) {
        qDebug() << "******* getModifiedItems: From ********" << aTime;

        list = mBackend->getAllModifiedDbIds(aTime);

        aModifiedItems = getStoreList(list);

        operationStatus = true;
    }

    return operationStatus;
}

bool
MyStoragePlugin::getModifiedItemIds( QList<QString>& aModifiedItemIds, const QDateTime& aTime )
{
    FUNCTION_CALL_TRACE;
    QList<DbId>  list;
    bool operationStatus = false;

    if(mBackend) {
        qDebug() << "******* getModifiedItemIds : From ********" << aTime;

        list = mBackend->getAllModifiedDbIds(aTime);

        foreach(QContactLocalId id , list) {
            aModifiedItemIds.append(QString::number(id));
        }
        operationStatus = true;
    }
    return operationStatus;
}

bool
MyStoragePlugin::getDeletedItemIds( QList<QString>& aDeletedItemIds, const QDateTime& aTime )
{
    FUNCTION_CALL_TRACE;
    LOG_DEBUG( "Getting deleted contacts since" << aTime );

    // Logic for returning deleted item ids
}

Buteo::StorageItem*
MyStoragePlugin::newItem()
{
    FUNCTION_CALL_TRACE;
    return new SimpleItem;
}

QList<Buteo::StorageItem*>
MyStoragePlugin::getItems( const QStringList& aItemIdList )
{
    FUNCTION_CALL_TRACE;

    QList<Buteo::StorageItem*> items;
    QMap<DbId, QString> textObj;
    QList<DbId> ids;
    QStringListIterator itr( aItemIdList );

    if( mBackend )
    {
        foreach (const QString itr, aItemIdList)
        {
            ids.append( itr.toUInt() );
        }
        mBackend->getDbObjects ( ids, textObj );
        QMapIterator<DbId,QString> i(textObj);
        while( i.hasNext() )
        {
            i.next();
            if( !i.value().isEmpty() )
            {
                SimpleItem *item = new SimpleItem;
                item->setId( QString::number( i.key() ) );
                item->setType( iProperties[STORAGE_DEFAULT_MIME_PROP] );
                item->write( 0, i.value().toUtf8() );
                items.append( item );
            }
            else
            {
                LOG_WARNING("Db object with id " << i.key() <<" doesn't exist!");
            }
        }
    }

    return items;
}

Buteo::StorageItem*
MyStoragePlugin::getItem( const QString& aItemId )
{
    FUNCTION_CALL_TRACE;

    if( !mBackend )
    {
        return NULL;
    }

    SimpleItem* newItem = NULL;

    DbId id;
    id = aItemId.toUInt();
    DbObject obj;

    mBackend->getDbObj( id, obj );
    QDateTime creationTime = mBackend->getCreationTime( contact );

    if(!contactData.isEmpty())
    {
        newItem = new SimpleItem;
        newItem->setId(aItemId);
        newItem->setType(iProperties[STORAGE_DEFAULT_MIME_PROP]);
        newItem->write(0,contactData.toUtf8());
    }
    else
    {
        LOG_WARNING( "Contact does not exist:" << aItemId );
    }

    return newItem;
}

MyStoragePlugin::OperationStatus
MyStoragePlugin::addItem(Buteo::StorageItem& aItem)
{
    FUNCTION_CALL_TRACE;

    QList<Buteo::StorageItem*> items;
    items.append( &aItem );

    QList<MyStoragePlugin::OperationStatus> status = addItems( items );

    return status.first();
}

QList<MyStoragePlugin::OperationStatus>
MyStoragePlugin::addItems( const QList<Buteo::StorageItem*>& aItems )
{

    FUNCTION_CALL_TRACE;

    QList<MyStoragePlugin::OperationStatus> storageErrorList;
    QDateTime currentTime = QDateTime::currentDateTime();

    if( !mBackend )
    {
        for ( int i = 0; i < aItems.size(); i++)
        {
            storageErrorList.append(STATUS_ERROR);
        }
        return storageErrorList;
    }

    QList<QString> dbList;
    foreach(Buteo::StorageItem *item , aItems) {
        QByteArray data;
        item->read(0,item->getSize(),data);
        dbList.append(QString::fromUtf8(data.data()));
    }

    QList<QString> dbIdList;
    QMap<int, DbObjStatus> dbObjErrorMap;
    bool retVal = mBackend->addContacts(contactsList, contactsErrorMap);

    else
    {

        QMapIterator<int ,ContactsStatus> i(contactsErrorMap);
        int j = 0;
        while (i.hasNext()) {
            i.next();
            Buteo::StorageItem *item = aItems[j];
            item->setId(QString::number(i.value().id));

            MyStoragePlugin::OperationStatus status = mapErrorStatus(i.value().errorCode);

            if( status == STATUS_OK )
            {
                // This item was successfully added, so let's add it to the snapshot
                iSnapshot.insert( i.value().id, currentTime );
            }

            storageErrorList.append(status);
            j++;
        }

    }

    return storageErrorList;

}


MyStoragePlugin::OperationStatus MyStoragePlugin::modifyItem(Buteo::StorageItem& aItem)
{
        FUNCTION_CALL_TRACE;

        MyStoragePlugin::OperationStatus status = STATUS_ERROR;

        if(mBackend ) {
                QString strID = aItem.getId();
                QByteArray data;
                aItem.read( 0, aItem.getSize(), data );
                QString Contact = QString::fromUtf8( data );
                qDebug()  << "Modifying an Item with data : " << Contact;
                qDebug() << "Modifying an Item with ID : "  << strID;
                QContactManager::Error error = mBackend->modifyContact(strID ,Contact);
                status = mapErrorStatus(error);
                qDebug()  << "After Modification String ID  is " << strID;
        }

        return status;
}

QList<MyStoragePlugin::OperationStatus>
MyStoragePlugin::modifyItems(const QList<Buteo::StorageItem *> &aItems)
{
    FUNCTION_CALL_TRACE;

    qDebug()  << "Items to Modify :"  << aItems.size();

    QList<MyStoragePlugin::OperationStatus> statusList;
    if(mBackend) {

        QStringList dbObjList;
        QStringList dbIdList;
        // Convert the string form of the data to a class object
        foreach(Buteo::StorageItem *item , aItems) {
            QByteArray data;
            item->read(0,item->getSize(),data);
            dbObjList.append(QString::fromUtf8(data.data()));
            dbIdList.append(item->getId());
        }

    statusList = mBackend->modifyDbObjs(dbObjList, dbIdList);

    return statusList;
}

MyStoragePlugin::OperationStatus
MyStoragePlugin::deleteItem( const QString& aItemId )
{
    FUNCTION_CALL_TRACE;

    QList<QString> itemIds;
    itemIds.append( aItemId );

    QList<MyStoragePlugin::OperationStatus> status = deleteItems( itemIds );

    return status.first();
}

QList<MyStoragePlugin::OperationStatus>
MyStoragePlugin::deleteItems(const QList<QString>& aItemIds )
{
    FUNCTION_CALL_TRACE;

    QList<MyStoragePlugin::OperationStatus> statusList;
    QDateTime currentTime = QDateTime::currentDateTime();

    QMap<int, DbObjStatus> errorMap = mBackend->deleteDbObjs(aItemIds);
    if(errorMap.size() != aItemIds.size() )
    {
        LOG_WARNING("Something wrong with batch deletion of database objects");
        LOG_DEBUG("dbObjErrroMap.size() " << errorMap.size() );
        LOG_DEBUG("dbObjList.size()" << aItemIds.size() );
        for ( int i = 0; i < aItemIds.size(); i++)
        {
            statusList.append(STATUS_ERROR);
        }
    }
    else
    {
        QList<QString> itemIds;
        QList<QDateTime> creationTimes;
        QList<QDateTime> deletionTimes;

        QMapIterator<int, DbObjStatus> i(errorMap);
        int j = 0;
        while( i.hasNext() )
        {
            i.next();
            MyStoragePlugin::OperationStatus status = mapErrorStatus( i.value().errorCode );

            if( status == STATUS_OK )
            {
                // This item was successfully deleted, so let's remove it from the snapshot and
                // add it to the deleted items.

                QString itemId = aItemIds[j];

                itemIds.append( itemId );
                creationTimes.append( iSnapshot.value( itemId.toUInt()) );
                iSnapshot.remove( itemId.toUInt() );
                deletionTimes.append( currentTime );
            }

            statusList.append( status );
            j++;
        }

        if( !itemIds.isEmpty() )
        {
            iDeletedItems.addDeletedItems( itemIds, creationTimes, deletionTimes );
        }
    }


    return statusList;
}

QList<Buteo::StorageItem*>
MyStoragePlugin::getStoreList(QList<DbId> &aStrIDList)
{
        FUNCTION_CALL_TRACE;

        QList<Buteo::StorageItem*> itemList;


    if (mBackend != NULL) {
        QMap<DbId,QString> idDataMap;
        mBackend->getDbObjs(aStrIDList, idDataMap);
        QMapIterator<DbId, QString > iter(idDataMap);

        while (iter.hasNext()) {
            iter.next();
            SimpleItem* item = convertStrObjToStorageItem(iter.key(), iter.value());
            if (item  != NULL) {
                itemList.append(item);
            }

        }
    }

    return itemList;
}

