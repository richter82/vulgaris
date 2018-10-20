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
    init(peer: Peer) {
        outg_conn_ = outg_connection_create()
        outg_connection_bind(outg_conn_, peer.peer_)
    }
    
    deinit {
        outg_connection_destroy(outg_conn_)
    }
    
    func connect(_ addr: String, _ port: UInt16){
        var oc_params: sockaddr_in = sockaddr_in()
        oc_params.sin_family = UInt8(AF_INET)
        oc_params.sin_addr.s_addr = inet_addr(addr)
        oc_params.sin_port = port.bigEndian
        outg_connection_connect(outg_conn_, UnsafeMutablePointer<sockaddr_in>(&oc_params))
    }
    
    var outg_conn_: OpaquePointer
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
    init(_ peer: Peer, _ sh_inco_conn: OpaquePointer){
        self.peer = peer
        own_inco_conn_ = inco_connection_get_own_ptr(sh_inco_conn)
        inco_connection_set_on_releaseable_oc(inco_conn_, on_release, nil)
        inco_connection_set_on_incoming_transaction_oc(inco_conn_, on_incoming_transaction, nil)
        inco_connection_set_on_incoming_subscription_oc(inco_conn_, on_incoming_subscription, nil)
    }
    
    fileprivate func on_release(ic: OpaquePointer!, ud: UnsafeMutableRawPointer!)
    {
        peer.icRepo.removeValue(forKey: connectionId)
        inco_connection_release(own_inco_conn_)
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
    let own_inco_conn_ : OpaquePointer
    
    var inco_conn_ : OpaquePointer{
        get{
            return inco_connection_get_ptr(own_inco_conn_)
        }
    }
    
    var connectionId: UInt32{
        get{return inco_connection_get_connection_id(inco_conn_)}
    }
    
    var itxRepo = [tx_id:IncomingTransaction]()
    var isbsRepo = [UInt32:IncomingSubscription]()
}
