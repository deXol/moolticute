// Qt
#include <QJsonArray>
#include <QJsonObject>
#include <QColor>
#include <QFont>
#include <QApplication>
#include <QBrush>

// Application
#include "CredentialModel.h"
#include "RootItem.h"
#include "ServiceItem.h"
#include "LoginItem.h"
#include "TreeItem.h"
#include "ParseDomain.h"

CredentialModel::CredentialModel(QObject *parent) : QAbstractItemModel(parent)
{
    m_pRootItem = new RootItem();
}

CredentialModel::~CredentialModel()
{
    delete m_pRootItem;
}

QVariant CredentialModel::data(const QModelIndex &idx, int role) const
{
    // Get item (no need check index)
    TreeItem *pItem = getItemByIndex(idx);
    if (!pItem)
        return QVariant();

    // Cast to service item
    ServiceItem *pServiceItem = getServiceItemByIndex(idx);

    // Cast to login item
    LoginItem *pLoginItem = getLoginItemByIndex(idx);

    if (role == Qt::DisplayRole && idx.column() == 0)
    {
       if (pServiceItem != nullptr)
            return  pItem->name();
    }
    else if (role == Qt::DisplayRole && idx.column() == 1)
    {
        if (pLoginItem != nullptr)
            return  pItem->updatedDate();
    }

    if (role == Qt::FontRole)
    {
        if (pServiceItem != nullptr)
        {
            QFont font = qApp->font();
            font.setBold(false);
            font.setPointSize(10);
            return font;
        }

        return qApp->font();
    }

    if (Qt::TextAlignmentRole == role)
    {
        if (pLoginItem != nullptr && idx.column() == 1)
        {
            return Qt::AlignHCenter;
        }
    }

    return QVariant();
}

QVariant CredentialModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (Qt::DisplayRole == role && orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case 0:
            return QString("   Service or Website");
        case 1:
            return QString("Modified Date");
        default:
            return QVariant();
        }
    }

    if (Qt::TextAlignmentRole == role)
    {
        if (section == 1)
        {
            return Qt::AlignHCenter;
        }
    }

    return QVariant();
}

Qt::ItemFlags CredentialModel::flags(const QModelIndex &idx) const
{
    if (!idx.isValid())
    {
        return nullptr;
    }
    return QAbstractItemModel::flags(idx);
}

QModelIndex CredentialModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *pParentItem = nullptr;

    if (!parent.isValid())
        pParentItem = m_pRootItem;
    else
        pParentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *pChildItem = pParentItem->child(row);
    if (pChildItem)
        return createIndex(row, column, pChildItem);
    else
        return QModelIndex();
}

QModelIndex CredentialModel::parent(const QModelIndex &idx) const
{
    if (!idx.isValid())
        return QModelIndex();

    TreeItem *pChildItem = static_cast<TreeItem*>(idx.internalPointer());
    TreeItem *pParentItem = pChildItem->parentItem();

    if (pParentItem == m_pRootItem)
        return QModelIndex();

    return createIndex(pParentItem->row(), 0, pParentItem);
}

int CredentialModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *pParentItem = nullptr;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        pParentItem = m_pRootItem;
    else
        pParentItem = static_cast<TreeItem *>(parent.internalPointer());

    return pParentItem->childCount();
}

int CredentialModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

TreeItem *CredentialModel::getItemByIndex(const QModelIndex &idx) const
{
    if (!idx.isValid())
        return nullptr;
    return static_cast<TreeItem *>(idx.internalPointer());
}

void CredentialModel::load(const QJsonArray &json)
{
    if (json.isEmpty())
        return;

    beginResetModel();

    m_pRootItem->setItemsStatus(TreeItem::UNUSED);
    for (int i=0; i<json.size(); i++)
    {
        QJsonObject pnode = json.at(i).toObject();

        QJsonArray jchilds = pnode["childs"].toArray();
        for (int j=0; j<jchilds.size(); j++)
        {
            QJsonObject cnode = jchilds.at(j).toObject();

            // Retrieve credential data
            QString sServiceName = pnode["service"].toString();

            // Check if this service already exists
            ServiceItem *pServiceItem = m_pRootItem->findServiceByName(sServiceName);

            // Service does not exist, add it
            if (pServiceItem == nullptr)
                pServiceItem = m_pRootItem->addService(sServiceName);
            pServiceItem->setStatus(TreeItem::USED);

            QString sLoginName = cnode["login"].toString();

            // Check if this login already exists
            LoginItem *pLoginItem = pServiceItem->findLoginByName(sLoginName);

            // Login does not exist, add it
            if (pLoginItem == nullptr)
                pLoginItem = pServiceItem->addLogin(sLoginName);
            pLoginItem->setStatus(TreeItem::USED);

            // Update login item description
            QString sDescription = cnode["description"].toString();
            pLoginItem->setDescription(sDescription);

            // Update login item created date
            QDate dCreatedDate = QDate::fromString(cnode["date_created"].toString(), Qt::ISODate);
            pLoginItem->setUpdatedDate(dCreatedDate);

            // Update login item updated date
            QDate dUpdatedDate = QDate::fromString(cnode["date_last_used"].toString(), Qt::ISODate);
            pLoginItem->setAccessedDate(dUpdatedDate);

            QJsonArray a = cnode["address"].toArray();
            if (a.size() < 2)
            {
                qWarning() << "Moolticute daemon did not send the node address, please upgrade moolticute daemon.";
                continue;
            }
            QByteArray bAddress;
            bAddress.append(static_cast<char>(a.at(0).toInt()));
            bAddress.append(static_cast<char>(a.at(1).toInt()));

            // Update login item address
            pLoginItem->setAddress(bAddress);

            // Update login favorite
            int iFavorite = cnode["favorite"].toInt();
            pLoginItem->setFavorite(static_cast<qint8>(iFavorite));
        }
    }

    // Remove unused items
    m_pRootItem->removeUnusedItems();

    endResetModel();

    bool bClearLoginDescription = m_pRootItem->childCount() == 0;
    emit modelLoaded(bClearLoginDescription);
}

ServiceItem *CredentialModel::addService(const QString &sServiceName)
{
    // Do we have a match for sServiceName?
    ServiceItem *pServiceItem = m_pRootItem->findServiceByName(sServiceName);
    if (pServiceItem != nullptr)
        return pServiceItem;

    // Create new service
    return new ServiceItem(sServiceName);
}

QModelIndex CredentialModel::getServiceIndexByName(const QString &sServiceName, int column) const
{
    QModelIndexList lMatches = match(index(0, column, QModelIndex()), Qt::DisplayRole, sServiceName, 1);
    if (!lMatches.isEmpty())
        return lMatches.first();

    return QModelIndex();
}

LoginItem *CredentialModel::getLoginItemByIndex(const QModelIndex &idx) const
{
    return dynamic_cast<LoginItem *>(getItemByIndex(idx));
}

ServiceItem *CredentialModel::getServiceItemByIndex(const QModelIndex &idx) const
{
    return dynamic_cast<ServiceItem *>(getItemByIndex(idx));
}

void CredentialModel::updateLoginItem(const QModelIndex &idx, const QString &sPassword, const QString &sDescription, const QString &sName)
{
    // Retrieve item
    LoginItem *pLoginItem = getLoginItemByIndex(idx);
    if (pLoginItem != nullptr)
    {
        updateLoginItem(idx, PasswordRole, sPassword);
        updateLoginItem(idx, DescriptionRole, sDescription);
        updateLoginItem(idx, ItemNameRole, sName);
    }
}

void CredentialModel::updateLoginItem(const QModelIndex &idx, const ItemRole &role, const QVariant &vValue)
{
    LoginItem *pLoginItem = getLoginItemByIndex(idx);
    if (!pLoginItem) return;

    bool bChanged = false;
    switch (role)
    {
        case ItemNameRole:
        {
            QString sName = vValue.toString();
            if (sName != pLoginItem->name())
            {
                pLoginItem->setName(sName);
                bChanged = true;
            }
            break;
        }
        case PasswordRole:
        {
            QString sPassword = vValue.toString();
            if (sPassword != pLoginItem->password())
            {
                pLoginItem->setPassword(sPassword);
                bChanged = true;
            }
            break;
        }
        case DescriptionRole:
        {
            QString sDescription = vValue.toString();
            if (sDescription != pLoginItem->description())
            {
                pLoginItem->setDescription(sDescription);
                bChanged = true;
            }
            break;
        }
        case DateUpdatedRole:
        {
            QDate dDate = vValue.toDate();
            if (dDate != pLoginItem->updatedDate())
            {
                pLoginItem->setUpdatedDate(dDate);
                bChanged = true;
            }
            break;
        }
        case DateAccessedRole:
        {
            QDate dDate = vValue.toDate();
            if (dDate != pLoginItem->accessedDate())
            {
                pLoginItem->setAccessedDate(dDate);
                bChanged = true;
            }
            break;
        }
        case FavoriteRole:
        {
            qint8 iFavorite = static_cast<qint8>(vValue.toInt());
            if (iFavorite != pLoginItem->favorite())
            {
                pLoginItem->setFavorite(iFavorite);
                bChanged = true;
            }
            break;
        }
    }
    if (bChanged)
        emit dataChanged(idx, idx);
}

void CredentialModel::clear()
{
    qDebug() << "*** CLEAR ALL ITEMS FROM GUI ***";
    beginResetModel();
    m_pRootItem->clear();
    endResetModel();
}

void CredentialModel::setClearTextPassword(const QString &sServiceName, const QString &sLoginName, const QString &sPassword)
{
    // Retrieve target service
    ServiceItem *pTargetService = m_pRootItem->findServiceByName(sServiceName);

    // Got a match
    if (pTargetService != nullptr)
    {
        // Retrieve target login
        LoginItem *pTargetLogin = pTargetService->findLoginByName(sLoginName);
        if (pTargetLogin != nullptr)
        {
            pTargetLogin->setPassword(sPassword);
            pTargetLogin->setPasswordOrig(sPassword);
        }
    }
}

QJsonArray CredentialModel::getJsonChanges()
{
    QJsonArray jarr;
    QVector<TreeItem *> vServices = m_pRootItem->childs();
    foreach (TreeItem *pService, vServices)
    {
        QVector<TreeItem *> vLogins = pService->childs();
        foreach (TreeItem *pLogin, vLogins) {
            LoginItem *pLoginItem = dynamic_cast<LoginItem *>(pLogin);
            QJsonObject object = pLoginItem->toJson();
            jarr.append(object);
        }
    }

    return jarr;
}

void CredentialModel::addCredential(QString sServiceName, const QString &sLoginName, const QString &sPassword, const QString &sDescription)
{
    //Force all service names to lowercase
    sServiceName = ParseDomain(sServiceName).getManuallyEnteredDomainName(sServiceName);

    // Retrieve target service
    ServiceItem *pTargetService = m_pRootItem->findServiceByName(sServiceName);
    LoginItem *pAddedLoginItem = nullptr;

    // Check if a service/login pair already exists
    if (pTargetService != nullptr)
    {
        // Retrieve target login
        LoginItem *pTargetLogin = pTargetService->findLoginByName(sLoginName);
        if (pTargetLogin != nullptr)
            return;
        else
        {
            QModelIndex serviceIndex = getServiceIndexByName(pTargetService->name());
            if (serviceIndex.isValid())
            {
                beginInsertRows(serviceIndex, rowCount(serviceIndex), rowCount(serviceIndex));
                pAddedLoginItem = pTargetService->addLogin(sLoginName);
                pAddedLoginItem->setPasswordLocked(false);
                pAddedLoginItem->setPassword(sPassword);
                pAddedLoginItem->setDescription(sDescription);
                endInsertRows();
            }
        }
    }
    // No matching service
    else
    {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        ServiceItem *pAddedService = m_pRootItem->addService(sServiceName);
        endInsertRows();

        QModelIndex serviceIndex = getServiceIndexByName(pAddedService->name());
        if (serviceIndex.isValid())
        {
            beginInsertRows(serviceIndex, rowCount(serviceIndex), rowCount(serviceIndex));
            pAddedLoginItem = pAddedService->addLogin(sLoginName);
            pAddedLoginItem->setPasswordLocked(false);
            pAddedLoginItem->setPassword(sPassword);
            pAddedLoginItem->setDescription(sDescription);
            endInsertRows();
        }
    }

    if (pAddedLoginItem)
        emit selectLoginItem(pAddedLoginItem);
}

bool CredentialModel::removeCredential(const QModelIndex &idx)
{
    bool bSelectNextAvailableLogin = false;
    if (idx.isValid())
    {
        TreeItem *pItem = getItemByIndex(idx);
        if (pItem == nullptr)
            return false;

        // Check what we have
        ServiceItem *pServiceItem = getServiceItemByIndex(idx);
        LoginItem *pLoginItem = getLoginItemByIndex(idx);
        if ((pServiceItem == nullptr) && (pLoginItem == nullptr))
            return false;

        if (pLoginItem != nullptr)
        {
            TreeItem *pServiceItem = pLoginItem->parentItem();
            if (pServiceItem != nullptr)
            {
                QModelIndex serviceIndex = getServiceIndexByName(pServiceItem->name());
                if (serviceIndex.isValid())
                {
                    beginRemoveRows(serviceIndex, idx.row(), idx.row());
                    if (pServiceItem->removeOne(pItem))
                        delete pItem;
                    endRemoveRows();

                    int iLoginCount = pServiceItem->childCount();
                    if (iLoginCount == 0)
                    {
                        beginRemoveRows(QModelIndex(), serviceIndex.row(), serviceIndex.row());
                        if (m_pRootItem->removeOne(pServiceItem))
                            delete pServiceItem;
                        endRemoveRows();
                        bSelectNextAvailableLogin = true;
                    }
                }
            }
        }
    }

    return bSelectNextAvailableLogin;
}


