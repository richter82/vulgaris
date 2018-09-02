/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

import Foundation
import UIKit

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
 * Peer
 */

class Peer
{
    init(peer_name: CString, peer_ver: [CUnsignedInt]) {
        peer_own_ = peer_create()
        peer_ = own_peer_get_ptr(peer_own_)
        peer_name_ = peer_name
        peer_ver_ = peer_ver
        vc_ = nil
        
        if let filePath = Bundle.main.path(forResource: "logger", ofType:"cfg") {
            set_logger_cfg_file_path_name(filePath)
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
        
        load_logger_config()
        load_vlg_logger()
        
        peer_set_name_handler_swf(peer_, Peer.peer_name_handler, bridge(obj:self))
        peer_set_version_handler_swf(peer_, Peer.peer_version_handler, bridge(obj:self))
        peer_set_status_change_handler_swf(peer_, Peer.peer_status_change, bridge(obj:self))
    }
    
    deinit {
        peer_destroy(peer_own_)
    }
    
    func StartPeer(){
        let argv = UnsafeMutablePointer<UnsafeMutablePointer<Int8>?>.allocate(capacity: 2);
        let arg0 = CString("");
        let arg1 = CString("-file");
        argv[0] = arg0.buffer;
        argv[1] = arg1.buffer;
        do {
            let documentDirectory = try fileManager_.url(for: .documentDirectory, in: .userDomainMask, appropriateFor:nil, create:false)
            let dd_fpath = CString(documentDirectory.path)
            set_db_data_dir_sqlite(dd_fpath.buffer)
        }catch{
            fatalError("failed get documentDirectory")
        }
        persistence_manager_load_persistence_driver(get_pers_driv_sqlite())
        peer_extend_model_with_nem(peer_, get_c_nem_smplmdl())
        peer_start(peer_, 2, argv, 1);
        argv.deallocate()
    }
    
    func StopPeer(){
        peer_stop(peer_, 0)
    }
    
    fileprivate static func peer_name_handler(c_peer: OpaquePointer!, ud: UnsafeMutableRawPointer!) -> UnsafePointer<CChar>
    {
        let peer : Peer = bridge(ptr:ud)
        return UnsafePointer(peer.peer_name_.buffer)
    }
    
    fileprivate static func peer_version_handler(c_peer: OpaquePointer!, ud: UnsafeMutableRawPointer!) -> UnsafePointer<UInt32>
    {
        let peer : Peer = bridge(ptr:ud)
        return UnsafePointer(peer.peer_ver_)
    }
    
    fileprivate static func peer_status_change(c_peer: OpaquePointer!, status: PeerStatus, ud: UnsafeMutableRawPointer!)
    {
        let peer : Peer = bridge(ptr:ud)
        if let vc = peer.vc_ {
            switch (status){
            case PeerStatus_ZERO:
                DispatchQueue.main.async {
                    vc.peerStatus_.text = "ZERO"
                }
                break
            case PeerStatus_EARLY:
                DispatchQueue.main.async {
                    vc.peerStatus_.text = "EARLY"
                }
                break
            case PeerStatus_WELCOMED:
                DispatchQueue.main.async {
                    vc.peerStatus_.text = "WELCOMED"
                }
                break
            case PeerStatus_INITIALIZING:
                DispatchQueue.main.async {
                    vc.peerStatus_.text = "INITIALIZING"
                }
                break
            case PeerStatus_INITIALIZED:
                DispatchQueue.main.async {
                    vc.peerStatus_.text = "INITIALIZED"
                }
                break
            case PeerStatus_RESTART_REQUESTED:
                DispatchQueue.main.async {
                    vc.peerStatus_.text = "RESTART_REQUESTED"
                }
                break
            case PeerStatus_STARTING:
                DispatchQueue.main.async {
                    vc.peerStatus_.text = "STARTING"
                }
                break
            case PeerStatus_STARTED:
                DispatchQueue.main.async {
                    vc.peerStatus_.text = "STARTED"
                }
                break
            case PeerStatus_RUNNING:
                DispatchQueue.main.async {
                    vc.startB_.isEnabled = false
                    vc.stopB_.isEnabled = true
                    vc.peerStatus_.text = "RUNNING"
                    vc.peerStatus_.textColor = UIColor.green
                }
                break
            case PeerStatus_STOP_REQUESTED:
                DispatchQueue.main.async {
                    vc.peerStatus_.text = "STOP_REQUESTED"
                }
                break
            case PeerStatus_STOPPING:
                DispatchQueue.main.async {
                    vc.peerStatus_.text = "STOPPING"
                }
                break
            case PeerStatus_STOPPED:
                DispatchQueue.main.async {
                    vc.stopB_.isEnabled = false
                    vc.startB_.isEnabled = true
                    vc.peerStatus_.text = "STOPPED"
                    vc.peerStatus_.textColor = UIColor.brown
                }
                break
            case PeerStatus_DIED:
                DispatchQueue.main.async {
                    vc.peerStatus_.text = "DIED"
                }
                break
            default:
                print("unmanaged status:")
            }
        } else {
            fatalError("peer.vc_ == nil")
        }
    }
    
    let fileManager_ = FileManager.default
    var peer_own_: OpaquePointer
    var peer_: OpaquePointer
    let peer_name_: CString
    let peer_ver_: [CUnsignedInt]
    var vc_: ViewController?
    
    var ViewController: ViewController?{
        get{
            return vc_
        }
        set{
            vc_ = newValue
        }
    }
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
    
    var outg_conn_: OpaquePointer
}

/**
 * IngoingConnection
 */

class IngoingConnection
{
}

/**
 * OutgoingTransaction
 */

class OutgoingTransaction
{
}

/**
 * IngoingTransaction
 */

class IngoingTransaction
{
}

/**
 * OutgoingSubscription
 */

class OutgoingSubscription
{
}

/**
 * IngoingSubscription
 */

class IngoingSubscription
{
}
