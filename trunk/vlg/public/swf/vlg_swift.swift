/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

import Foundation

/**
 * low level c-ptr bridging utils
 */

func bridge<T : AnyObject>(obj : T) -> UnsafeMutableRawPointer {
    return UnsafeMutableRawPointer(Unmanaged.passUnretained(obj).toOpaque())
}

func bridge<T : AnyObject>(ptr : UnsafeMutableRawPointer) -> T {
    return Unmanaged<T>.fromOpaque(ptr).takeUnretainedValue()
}

func bridgeRetained<T : AnyObject>(obj : T) -> UnsafeMutableRawPointer {
    return UnsafeMutableRawPointer(Unmanaged.passRetained(obj).toOpaque())
}

func bridgeTransfer<T : AnyObject>(ptr : UnsafeRawPointer) -> T {
    return Unmanaged<T>.fromOpaque(ptr).takeRetainedValue()
}

/**
 * CString
 */

class CString {
    fileprivate let _len: Int
    let buffer: UnsafeMutablePointer<Int8>
    
    init(_ string: String) {
        (_len, buffer) = string.withCString {
            let len = Int(strlen($0) + 1)
            let dst = strcpy(UnsafeMutablePointer<Int8>.allocate(capacity: len), $0)
            return (len, dst!)
        }
    }
    
    deinit {
        buffer.deallocate()
    }
}

/**
 * PeerListener
 */

protocol PeerListener
{
    func onLoadConfig(peer: Peer, pnum: Int32, param: String, value: String?) -> RetCode
    func onInit(peer: Peer) -> RetCode
    func onStarting(peer: Peer) -> RetCode
    func onStopping(peer: Peer) -> RetCode
    func onMoveRunning(peer: Peer) -> RetCode
    func onDyingBreath(peer: Peer)
    func onStatusChange(peer: Peer, peerStatus: PeerStatus)
    func onIncomingConnection(peer: Peer, incomingConnection: IncomingConnection) -> RetCode;
}

/**
 * Peer
 */

class Peer
{
    convenience init(peerName: String, peerVersion: [UInt32], peerListener: PeerListener){
        self.init(peerName: CString(peerName), peerVersion: peerVersion, peerListener: peerListener)
    }
    
    fileprivate init(peerName: CString, peerVersion: [CUnsignedInt], peerListener: PeerListener) {
        self.peer_name_ = peerName
        self.peer_ver_ = peerVersion
        self.peerListener = peerListener
        
        if let filePath = Bundle.main.path(forResource: "logger", ofType:"cfg") {
            set_logger_cfg_file_path_name(filePath)
            peer_own_ = peer_create()
            peer_ = own_peer_get_ptr(peer_own_)
        } else {
            fatalError("logger.cfg not found")
        }
        
        if let filePath = Bundle.main.path(forResource: "perscfg", ofType:"") {
            persistence_manager_set_cfg_file_path_name(filePath)
        } else {
            fatalError("perscfg not found")
        }
        
        if let filePath = Bundle.main.path(forResource: "params", ofType:"") {
            peer_set_params_file_path_name(peer_, filePath)
        } else {
            fatalError("params not found")
        }
                
        peer_set_name_oc(peer_, { _,_ in return UnsafePointer(self.peer_name_.buffer) }, bridge(obj:self))
        peer_set_version_oc(peer_, { _,_ in return UnsafePointer(self.peer_ver_) }, bridge(obj:self))
        peer_set_on_status_change_oc(peer_, peer_status_change, bridge(obj:self))
        peer_set_on_incoming_connection_oc(peer_, on_incoming_connection, bridge(obj:self))
    }
    
    deinit {
        peer_destroy(peer_own_)
    }
    
    func setParamsFileDir(fileDir: String) -> RetCode{
        return peer_set_params_file_dir(peer_, CString(fileDir).buffer)
    }
    
    func setParamsFilePathName(pathName: String) -> RetCode{
        return peer_set_params_file_path_name(peer_, CString(pathName).buffer)
    }
    
    func getVersionMajor() -> UInt32{
        return peer_get_version_major(peer_)
    }
    
    func getVersionMinor() -> UInt32{
        return peer_get_version_minor(peer_)
    }
    
    func getVersionManteinance() -> UInt32{
        return peer_get_version_maintenance(peer_)
    }
    
    func getVersionArchitecture() -> UInt32{
        return peer_get_version_architecture(peer_)
    }
    
    func isConfigured() -> Bool{
        return peer_is_configured(peer_) > 0 ? true : false;
    }
    
    func isPersistent() -> Bool{
        return peer_is_persistent(peer_) > 0 ? true : false;
    }
    
    func isCreatePersistentSchema() -> Bool{
        return peer_is_persistent_schema_creating(peer_) > 0 ? true : false;
    }
    
    func isDropExistingPersistentSchema() -> Bool{
        return peer_is_dropping_existing_schema(peer_) > 0 ? true : false;
    }
    
    func getPersonality() -> PeerPersonality{
        return peer_get_personality(peer_)
    }
    
    func getServerSockAddr() -> sockaddr_in{
        return peer_get_server_sockaddr(peer_)
    }
    
    func setPersonality(peerPersonality: PeerPersonality){
        peer_set_personality(peer_, peerPersonality)
    }
    
    func setServerAddress(address: String){
        peer_set_srv_sin_addr(peer_, CString(address).buffer)
    }
    
    func setServerPort(port: Int32){
        peer_set_sin_port(peer_, port)
    }
    
    func setPersistent(persistent: Bool){
        peer_set_persistent(peer_, persistent ? 1 : 0)
    }
    
    func setCreatePersistentSchema(createPersistentSchema: Bool){
        peer_set_persistent_schema_creating(peer_, createPersistentSchema ? 1 : 0)
    }
    
    func setDropExistingPersistentSchema(dropExistingPersistentSchema: Bool){
        peer_set_dropping_existing_schema(peer_, dropExistingPersistentSchema ? 1 : 0)
    }
    
    func setConfigured(configured: Bool){
        peer_set_configured(peer_, configured ? 1 : 0)
    }

    func startPeer(){
        let argv = UnsafeMutablePointer<UnsafeMutablePointer<Int8>?>.allocate(capacity: 2);
        let arg0 = CString("");
        let arg1 = CString("-file");
        argv[0] = arg0.buffer;
        argv[1] = arg1.buffer;
        do {
            let documentDirectory = try fileMng.url(for: .documentDirectory, in: .userDomainMask, appropriateFor:nil, create:false)
            let dd_fpath = CString(documentDirectory.path)
            set_db_data_dir_sqlite(dd_fpath.buffer)
        }catch{
            fatalError("failed get documentDirectory")
        }
        persistence_manager_load_persistence_driver(get_pers_driv_sqlite(peer_get_logger(peer_)))
        peer_extend_model_with_nem(peer_, get_c_nem_smplmdl(peer_get_logger(peer_)))
        peer_start(peer_, 2, argv, 1);
        argv.deallocate()
    }
    
    func stopPeer(){
        peer_stop(peer_, 0)
    }
    
    fileprivate func on_incoming_connection(c_peer: OpaquePointer!, sh_ic: OpaquePointer!, ud: UnsafeMutableRawPointer!) -> RetCode
    {
        let ic = IncomingConnection(bridge(ptr: ud!), sh_ic)
        icRepo[ic.connectionId] = ic
        return RetCode_OK;
    }
    
    fileprivate func peer_status_change(c_peer: OpaquePointer!, status: PeerStatus, ud: UnsafeMutableRawPointer!)
    {
        peerListener.onStatusChange(peer: self, peerStatus: status)
    }
    
    let fileMng = FileManager.default
    var peer_own_: OpaquePointer
    var peer_: OpaquePointer
    let peer_name_: CString
    let peer_ver_: [CUnsignedInt]
    let peerListener: PeerListener
    
    var icRepo = [UInt32:IncomingConnection]()
}

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

extension tx_id: Equatable, Hashable {
    public static func == (lhs: tx_id, rhs: tx_id) -> Bool {
        return  lhs.txcnid == rhs.txcnid &&
                lhs.txplid == rhs.txplid &&
                lhs.txprid == rhs.txprid &&
                lhs.txsvid == rhs.txsvid
    }
    public var hashValue: Int {
        return txcnid.hashValue ^ txplid.hashValue ^ txprid.hashValue ^ txsvid.hashValue
    }
    init(oth: tx_id){
        self.init()
        self.txcnid = oth.txcnid
        self.txplid = oth.txplid
        self.txprid = oth.txprid
        self.txsvid = oth.txsvid
    }
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

/**
 * OutgoingTransactionListener
 */

protocol OutgoingTransactionListener
{
    func onStatusChange(outgoingTransaction: OutgoingTransaction, current: TransactionStatus)
    func onClose(outgoingTransaction: OutgoingTransaction)
}

/**
 * OutgoingTransaction
 */

class OutgoingTransaction
{
}

/**
 * IncomingTransactionListener
 */

protocol IncomingTransactionListener
{
    func onStatusChange(incomingTransaction: IncomingTransaction, current: TransactionStatus)
    func onRequest(incomingTransaction: IncomingTransaction)
    func onClose(incomingTransaction: IncomingTransaction)
}

/**
 * IncomingTransaction
 */

class IncomingTransaction
{
    init(_ incoConn: IncomingConnection, _ sh_inco_tx: OpaquePointer){
        self.incoConn = incoConn
        own_inco_tx_ = inco_transaction_get_own_ptr(sh_inco_tx)
        inco_transaction_set_on_releaseable_oc(inco_tx_, on_release, nil)
        inco_transaction_set_on_request_oc(inco_tx_, on_request, nil)
    }
    
    fileprivate func on_release(itx: OpaquePointer!, ud: UnsafeMutableRawPointer!){
        incoConn.itxRepo.removeValue(forKey: txId)
        inco_transaction_release(own_inco_tx_)
    }
    
    fileprivate func on_request(itx: OpaquePointer!, ud: UnsafeMutableRawPointer!)
    {
        ResultObj = RequestObj
        Result = TransactionResult_COMMITTED
        ResultCode = ProtocolCode_SUCCESS
        peer_nclass_persistent_update_or_save_and_distribute(incoConn.peer.peer_, 1, ResultObj)
    }
    
    var Result : TransactionResult{
        get{
            return inco_transaction_get_transaction_result(inco_tx_)
        }
        set{
            inco_transaction_set_transaction_result(inco_tx_, newValue)
        }
    }
    
    var ResultCode : ProtocolCode{
        get{
            return inco_transaction_get_transaction_result_code(inco_tx_)
        }
        set{
            inco_transaction_set_transaction_result_code(inco_tx_, newValue)
        }
    }
    
    let incoConn :IncomingConnection
    let own_inco_tx_ : OpaquePointer
    
    var inco_tx_ : OpaquePointer{
        get{
            return inco_transaction_get_ptr(own_inco_tx_)
        }
    }

    var txId : tx_id{
        get{
            return tx_id(oth:inco_transaction_get_transaction_id(inco_tx_).move())
        }
    }
    
    var RequestObj : OpaquePointer{
        get{return inco_transaction_get_request_obj(inco_tx_)}
    }
    
    var ResultObj : OpaquePointer{
        get{return inco_transaction_get_result_obj(inco_tx_)}
        set{inco_transaction_set_result_obj(inco_tx_, newValue)}
    }
    
}

/**
 * SubscriptionEvent
 */

class SubscriptionEvent
{
    
}

/**
 * OutgoingSubscriptionListener
 */

protocol OutgoingSubscriptionListener
{
    func onStatusChange(outgoingSubscription: OutgoingSubscription, subscriptionStatus: SubscriptionStatus)
    func onStart(outgoingSubscription: OutgoingSubscription)
    func onStop(outgoingSubscription: OutgoingSubscription)
    func onIncomingEvent(outgoingSubscription: OutgoingSubscription, subscriptionEvent: SubscriptionEvent) -> RetCode
}

/**
 * OutgoingSubscription
 */

class OutgoingSubscription
{
}

/**
 * IncomingSubscriptionListener
 */

protocol IncomingSubscriptionListener
{
    func onStatusChange(incomingSubscription: IncomingSubscription, subscriptionStatus: SubscriptionStatus)
    func onStop(incomingSubscription: IncomingSubscription)
    func onAcceptEvent(incomingSubscription: IncomingSubscription, subscriptionEvent: SubscriptionEvent) -> RetCode
}

/**
 * IncomingSubscription
 */

class IncomingSubscription
{
    init(_ incoConn: IncomingConnection, _ sh_inco_sbs: OpaquePointer){
        self.incoConn = incoConn
        own_inco_sbs_ = inco_subscription_get_own_ptr(sh_inco_sbs)
        inco_subscription_set_on_releaseable_oc(inco_sbs_, on_release, nil)
        inco_subscription_set_on_accept_distribution_oc(inco_sbs_, on_accept_distribution, nil)
    }
    
    fileprivate func on_release(sbs: OpaquePointer!, ud: UnsafeMutableRawPointer!){
        incoConn.isbsRepo.removeValue(forKey: id)
        inco_subscription_release(own_inco_sbs_)
    }
    
    fileprivate func on_accept_distribution(sbs: OpaquePointer!, evt: OpaquePointer!, ud: UnsafeMutableRawPointer!) -> RetCode{
        return RetCode_OK;
    }
    
    var id : UInt32 {
        get {
            return inco_subscription_get_id(inco_sbs_)
        }
    }
    
    let incoConn : IncomingConnection
    let own_inco_sbs_ : OpaquePointer
    
    var inco_sbs_ : OpaquePointer{
        get{
            return inco_subscription_get_ptr(own_inco_sbs_)
        }
    }
}
