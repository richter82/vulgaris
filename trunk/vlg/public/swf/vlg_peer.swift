/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

import Foundation


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
