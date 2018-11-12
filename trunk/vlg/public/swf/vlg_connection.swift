/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

import Foundation


/**
 * OutgoingConnectionListener
 */
protocol OutgoingConnectionListener
{
    func onStatusChange(outgoingConnection: OutgoingConnection, current: ConnectionStatus)
    func onConnect(outgoingConnection: OutgoingConnection, conEvtRes: ConnectivityEventResult, connEvtType: ConnectivityEventType)
    func onDisconnect(outgoingConnection: OutgoingConnection, conEvtRes: ConnectivityEventResult, connEvtType: ConnectivityEventType)
}

/**
 * OutgoingConnection
 */
class OutgoingConnection
{
    required init(peer: Peer) {
        outg_conn_op = outg_connection_create()
        outg_connection_bind(outg_conn_op, peer.peer_op)
    }
    
    deinit {
        outg_connection_destroy(outg_conn_op)
    }
    
    func connect(_ address: String, _ port: UInt16){
        var oc_params: sockaddr_in = sockaddr_in()
        oc_params.sin_family = UInt8(AF_INET)
        oc_params.sin_addr.s_addr = inet_addr(address)
        oc_params.sin_port = port.bigEndian
        outg_connection_connect(outg_conn_op, UnsafeMutablePointer<sockaddr_in>(&oc_params))
    }
    
    var outg_conn_op: OpaquePointer
}

/**
 * IncomingConnectionListener
 */
protocol IncomingConnectionListener
{
    func onStatusChange(incomingConnection: IncomingConnection, current: ConnectionStatus)
    func onDisconnect(incomingConnection: IncomingConnection, conEvtRes: ConnectivityEventResult, connEvtType: ConnectivityEventType)
    func onIncomingTransaction(incomingConnection: IncomingConnection, incomingTransaction: IncomingTransaction) -> RetCode
    func onIncomingSubscription(incomingConnection: IncomingConnection, incomingSubscription: IncomingSubscription) -> RetCode
}

/**
 * IncomingConnection
 */
class IncomingConnection
{
    required init(_ peer: Peer, _ sh_inco_conn: OpaquePointer){
        self.peer = peer
        self.own_inco_conn_op = inco_connection_get_own_ptr(sh_inco_conn)
        self.inco_conn_op = inco_connection_get_ptr(own_inco_conn_op)
        inco_connection_set_on_releaseable_oc(inco_conn_op, on_release, nil)
        inco_connection_set_on_incoming_transaction_oc(inco_conn_op, on_incoming_transaction, nil)
        inco_connection_set_on_incoming_subscription_oc(inco_conn_op, on_incoming_subscription, nil)
    }
    
    fileprivate func on_release(ic: OpaquePointer!, ud: UnsafeMutableRawPointer!)
    {
        peer.icRepo.removeValue(forKey: connectionId)
        inco_connection_release(own_inco_conn_op)
    }
    
    fileprivate func on_incoming_transaction(ic: OpaquePointer!, itx: OpaquePointer!, ud: UnsafeMutableRawPointer!) -> RetCode
    {
        let IncoTx = IncomingTransaction(self, itx)
        itxRepo[IncoTx.txId] = IncoTx
        return RetCode_OK
    }
    
    fileprivate func on_incoming_subscription(ic: OpaquePointer!, isbs: OpaquePointer!, ud: UnsafeMutableRawPointer!) -> RetCode
    {
        let IncoSbs = IncomingSubscription(self, isbs)
        isbsRepo[IncoSbs.id] = IncoSbs
        return RetCode_OK
    }
    
    let peer : Peer
    let own_inco_conn_op : OpaquePointer
    var inco_conn_op : OpaquePointer
    
    var connectionId: UInt32{
        get{return inco_connection_get_connection_id(inco_conn_op)}
    }
    
    var itxRepo = [tx_id:IncomingTransaction]()
    var isbsRepo = [UInt32:IncomingSubscription]()
}
