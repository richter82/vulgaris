/*
 *
 * (C) 2015 - giuseppe.baccini@gmail.com
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#include "vlg_toolkit_connection.h"
#include "ui_vlg_toolkit_connection.h"

//------------------------------------------------------------------------------
// vlg_toolkit_Conn_mdl
//------------------------------------------------------------------------------

vlg_toolkit_Conn_mdl::vlg_toolkit_Conn_mdl(vlg_toolkit_vlg_model &wrapped_mdl,
                                           QObject *parent) :
    wrapped_mdl_(wrapped_mdl),
    QSortFilterProxyModel(parent)
{
    setSourceModel(&wrapped_mdl);
}

bool vlg_toolkit_Conn_mdl::filterAcceptsRow(int sourceRow,
                                            const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    model_item *item = static_cast<model_item *>(index.internalPointer());
    return item->item_type() == VLG_MODEL_ITEM_TYPE_EDESC &&
           item->edesc()->get_nentity_type() == vlg::NEntityType_NCLASS;
}

vlg_toolkit_vlg_model &vlg_toolkit_Conn_mdl::wrapped_mdl()
{
    return wrapped_mdl_;
}

//------------------------------------------------------------------------------
// vlg_toolkit_Connection
//------------------------------------------------------------------------------

int vlg_toolkit_Connection::count_ = 0;

toolkit_connection::toolkit_connection(vlg_toolkit_Connection &widget) : widget_(widget)
{}

void toolkit_connection::on_status_change(vlg::ConnectionStatus current)
{
    qDebug() << "connection status:" << current;
    widget_.EmitConnStatus(current);
}

vlg_toolkit_Connection::vlg_toolkit_Connection(vlg::peer &peer,
                                               const QString &host,
                                               const QString &port,
                                               const QString &usr,
                                               const QString &psswd,
                                               vlg_toolkit_vlg_model &vlg_model_loaded_model,
                                               QMainWindow &m_win,
                                               QWidget *parent) :
    m_win_(m_win),
    tab_idx_(-1),
    tab_id_(NextCount()),
    parent_(*static_cast<QTabWidget *>(parent)),
    conn_(*this),
    b_mdl_(vlg_model_loaded_model, this),
    QWidget(parent),
    ui(new Ui::vlg_toolkit_Connection)
{
    ui->setupUi(this);

    ui->cp_host_addr->setText(host);
    ui->cp_host_port->setText(port);
    ui->cp_user->setText(usr);
    ui->cp_psswd->setText(psswd);

    sockaddr_in conn_params;
    memset(&conn_params, 0, sizeof(conn_params));
    conn_params.sin_family = AF_INET;
    conn_params.sin_addr.s_addr = inet_addr(host.toLatin1().data());
    conn_params.sin_port = htons(atoi(port.toLatin1().data()));

    conn_.bind(peer);
    conn_.connect(conn_params);

    connect(this, SIGNAL(SignalConnStatusChange(vlg::ConnectionStatus)),
            this,
            SLOT(OnConnStatusChange(vlg::ConnectionStatus)));
    EmitConnStatus(conn_.get_status());

    ui->peer_model_tree_view->setModel(&b_mdl_);
    connect(ui->peer_model_tree_view,
            SIGNAL(customContextMenuRequested(const QPoint &)), this,
            SLOT(OnCustomMenuRequested(const QPoint &)));

    connect(this, SIGNAL(SignalConnectionTimeout(const QString &)), &m_win_,
            SLOT(OnSetInfoMsg(const QString &)));

    connect(this, SIGNAL(SignalDisconnectionTimeout(const QString &)), &m_win_,
            SLOT(OnSetInfoMsg(const QString &)));
}

vlg_toolkit_Connection::~vlg_toolkit_Connection()
{
    delete ui;
}

void vlg_toolkit_Connection::UpdateTabHeader()
{
    if(tab_idx_ < 0) {
        return;
    }
    QIcon icon_flash;
    switch(conn_.get_status()) {
        case vlg::ConnectionStatus_UNDEFINED:
        case vlg::ConnectionStatus_INITIALIZED:
        case vlg::ConnectionStatus_DISCONNECTED:
            icon_flash.addFile(QStringLiteral(":/icon/icons/flash_red.png"), QSize(),
                               QIcon::Normal, QIcon::Off);
            break;
        case vlg::ConnectionStatus_ESTABLISHED:
        case vlg::ConnectionStatus_PROTOCOL_HANDSHAKE:
        case vlg::ConnectionStatus_AUTHENTICATED:
            icon_flash.addFile(QStringLiteral(":/icon/icons/flash_green.png"), QSize(),
                               QIcon::Normal, QIcon::Off);
            break;
        case vlg::ConnectionStatus_SOCKET_ERROR:
        case vlg::ConnectionStatus_PROTOCOL_ERROR:
        case vlg::ConnectionStatus_ERROR:
        default:
            icon_flash.addFile(QStringLiteral(":/icon/icons/flash_red.png"), QSize(),
                               QIcon::Normal, QIcon::Off);
            break;
    }
    parent_.setTabIcon(tab_idx_, icon_flash);
}

void vlg_toolkit_Connection::ConnectionUpActions()
{
    ui->cp_group_box->setEnabled(false);
    ui->connect_button->setEnabled(false);
    ui->disconnect_button->setEnabled(true);
    //ui->extend_model_button->setEnabled(true);
    ui->new_tx_button->setEnabled(true);
    ui->new_sbs_button->setEnabled(true);
}

void vlg_toolkit_Connection::ConnectionDownActions()
{
    ui->cp_group_box->setEnabled(true);
    ui->connect_button->setEnabled(true);
    ui->disconnect_button->setEnabled(false);
    // ui->extend_model_button->setEnabled(false);
    ui->new_tx_button->setEnabled(false);
    ui->new_sbs_button->setEnabled(false);
}

int vlg_toolkit_Connection::count()
{
    return count_;
}

void vlg_toolkit_Connection::setCount(int count)
{
    count_ = count;
}
int vlg_toolkit_Connection::tab_id() const
{
    return tab_id_;
}

void vlg_toolkit_Connection::setTab_id(int tab_id)
{
    tab_id_ = tab_id;
}
int vlg_toolkit_Connection::tab_idx() const
{
    return tab_idx_;
}

void vlg_toolkit_Connection::setTab_idx(int tab_idx)
{
    tab_idx_ = tab_idx;
}

toolkit_connection &vlg_toolkit_Connection::conn()
{
    return conn_;
}

int vlg_toolkit_Connection::NextCount()
{
    return ++count_;
}

void vlg_toolkit_Connection::OnConnStatusChange(vlg::ConnectionStatus
                                                status)
{
    switch(status) {
        case vlg::ConnectionStatus_UNDEFINED:
            ui->conn_status_label_disp->setText(QObject::tr("UNDEFINED"));
            ui->conn_status_label_disp->setStyleSheet(
                QObject::tr("background-color : Beige; color : black;"));
            break;
        case vlg::ConnectionStatus_INITIALIZED:
            ui->conn_status_label_disp->setText(QObject::tr("INITIALIZED"));
            ui->conn_status_label_disp->setStyleSheet(
                QObject::tr("background-color : Beige; color : black;"));
            break;
        case vlg::ConnectionStatus_DISCONNECTED:
            ui->conn_status_label_disp->setText(QObject::tr("DISCONNECTED"));
            ui->conn_status_label_disp->setStyleSheet(
                QObject::tr("background-color : IndianRed; color : black;"));
            ConnectionDownActions();
            break;
        case vlg::ConnectionStatus_ESTABLISHED:
            ui->conn_status_label_disp->setText(QObject::tr("ESTABLISHED"));
            ui->conn_status_label_disp->setStyleSheet(
                QObject::tr("background-color : GreenYellow; color : black;"));
            ConnectionUpActions();
            break;
        case vlg::ConnectionStatus_PROTOCOL_HANDSHAKE:
            ui->conn_status_label_disp->setText(QObject::tr("PROTOCOL HANDSHAKE"));
            ui->conn_status_label_disp->setStyleSheet(
                QObject::tr("background-color : LightGreen; color : black;"));
            ConnectionUpActions();
            break;
        case vlg::ConnectionStatus_AUTHENTICATED:
            ui->conn_status_label_disp->setText(QObject::tr("AUTHENTICATED"));
            ui->conn_status_label_disp->setStyleSheet(
                QObject::tr("background-color : LawnGreen; color : black;"));
            ConnectionUpActions();
            break;
        case vlg::ConnectionStatus_SOCKET_ERROR:
            ui->conn_status_label_disp->setText(QObject::tr("SOCKET ERROR"));
            ui->conn_status_label_disp->setStyleSheet(
                QObject::tr("background-color : Red; color : black;"));
            break;
        case vlg::ConnectionStatus_PROTOCOL_ERROR:
            ui->conn_status_label_disp->setText(QObject::tr("PROTOCOL ERROR"));
            ui->conn_status_label_disp->setStyleSheet(
                QObject::tr("background-color : Red; color : black;"));
            break;
        case vlg::ConnectionStatus_ERROR:
            ui->conn_status_label_disp->setText(QObject::tr("ERROR"));
            ui->conn_status_label_disp->setStyleSheet(
                QObject::tr("background-color : Red; color : black;"));
            break;
        default:
            break;
    }

    /*******************
     set connection indicators
     ******************/
    //ui->last_sock_err_disp->setText(QString("%1").arg(conn_.get_last_socket_err()));
    if(conn_.get_socket() == INVALID_SOCKET) {
        ui->sock_label_disp->setText(QString("inv. socket"));
    } else {
        ui->sock_label_disp->setText(QString("%1").arg(conn_.get_socket()));
    }
    ui->host_label_disp->setText(QString(conn_.get_host_ip()));
    ui->port_label_disp->setText(QString("%1").arg(conn_.get_host_port()));
    ui->connid_label_disp->setText(QString("%1").arg(conn_.get_id()));

    UpdateTabHeader();
}

void vlg_toolkit_Connection::OnTestSlot()
{
    qDebug() << "called test slot";
}

void vlg_toolkit_Connection::OnCustomMenuRequested(const QPoint &pos)
{
    QModelIndex proxy_index = ui->peer_model_tree_view->indexAt(pos);
    QModelIndex index = b_mdl_.mapToSource(proxy_index);
    model_item *item = static_cast<model_item *>(index.internalPointer());
    if(!item) {
        return;
    }
    if(item->item_type() != VLG_MODEL_ITEM_TYPE_EDESC ||
            item->edesc()->get_nentity_type() != vlg::NEntityType_NCLASS) {
        return;
    }
    QMenu *custom_menu = new QMenu(QString("%1").arg(
                                       item->edesc()->get_nclass_id()),
                                   this);

    QAction *ntx = new QAction("New Transaction", this);
    QAction *nsbs = new QAction("New Subscription", this);

    connect(ntx, SIGNAL(triggered()), this, SLOT(on_new_tx_button_clicked()));
    connect(nsbs, SIGNAL(triggered()), this, SLOT(on_new_sbs_button_clicked()));

    custom_menu->addAction(ntx);
    custom_menu->addAction(nsbs);

    custom_menu->popup(ui->peer_model_tree_view->viewport()->mapToGlobal(pos));
}

void vlg_toolkit_Connection::EmitConnStatus(vlg::ConnectionStatus status)
{
    emit SignalConnStatusChange(status);
}

void vlg_toolkit_Connection::on_connect_button_clicked()
{
    sockaddr_in conn_params;
    memset(&conn_params, 0, sizeof(conn_params));
    conn_params.sin_family = AF_INET;
    conn_params.sin_addr.s_addr = inet_addr(
                                      ui->cp_host_addr->text().toLatin1().data());
    conn_params.sin_port = htons(atoi(ui->cp_host_port->text().toLatin1().data()));
    conn_.connect(conn_params);
}

void vlg_toolkit_Connection::on_disconnect_button_clicked()
{
    vlg::ConnectivityEventResult cres = vlg::ConnectivityEventResult_UNDEFINED;
    vlg::ConnectivityEventType cevttyp = vlg::ConnectivityEventType_UNDEFINED;
    conn_.disconnect(vlg::ProtocolCode_UNSPECIFIED);
    if(conn_.await_for_disconnection_result(cres,
                                            cevttyp,
                                            VLG_TKT_INT_AWT_TIMEOUT,
                                            0) == vlg::RetCode_TIMEOUT) {
        emit SignalDisconnectionTimeout(QString("on disconnection [connid:%1]").arg(
                                            conn_.get_id()));
    }
}

void vlg_toolkit_Connection::on_extend_model_button_clicked()
{

}

void vlg_toolkit_Connection::on_new_tx_button_clicked()
{
    QModelIndexList indexes =
        ui->peer_model_tree_view->selectionModel()->selectedRows();
    if(!indexes.count()) {
        return;
    }
    QModelIndex proxy_index = indexes.at(0);
    QModelIndex index = b_mdl_.mapToSource(proxy_index);
    model_item *item = static_cast<model_item *>(index.internalPointer());
    if(!item) {
        return;
    }
    const vlg::nentity_desc *edesc = item->edesc();
    if(!edesc) {
        return;
    }

    /*if last param set to 'this' child will be always on TOP*/
    vlg_toolkit_tx_window *new_tx_window = new vlg_toolkit_tx_window(conn_,
                                                                     *edesc,
                                                                     NULL);
    new_tx_window->setAttribute(Qt::WA_DeleteOnClose, true);
    new_tx_window->setWindowTitle(QString("[TX][CONNID:%1][NCLASS:%2]").arg(conn_.get_id()).arg(
                                      item->edesc()->get_nclass_id()));
    new_tx_window->show();
    //new_tx_window->raise();
    //new_tx_window->activateWindow();
}

void vlg_toolkit_Connection::on_new_sbs_button_clicked()
{
    QModelIndexList indexes =
        ui->peer_model_tree_view->selectionModel()->selectedRows();
    if(!indexes.count()) {
        return;
    }
    QModelIndex proxy_index = indexes.at(0);
    QModelIndex index = b_mdl_.mapToSource(proxy_index);
    model_item *item = static_cast<model_item *>(index.internalPointer());
    if(!item) {
        return;
    }
    const vlg::nentity_desc *edesc = item->edesc();
    if(!edesc) {
        return;
    }

    /*if last param set to 'this' child will be always on TOP*/
    vlg_toolkit_sbs_window *new_sbs_window = new vlg_toolkit_sbs_window(conn_,
                                                                        *edesc,
                                                                        NULL);
    new_sbs_window->setAttribute(Qt::WA_DeleteOnClose, true);
    new_sbs_window->setWindowTitle(QString("[SBS][CONNID:%1][NCLASS:%2]").arg(
                                       conn_.get_id()).arg(
                                       item->edesc()->get_nclass_id()));
    new_sbs_window->show();
    //new_sbs_window->raise();
    //new_sbs_window->activateWindow();
}
