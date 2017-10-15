//
//  blz_support.swift
//  blz_swift_integr_test_ios
//
//  Created by Giuseppe Baccini on 04/05/15.
//  Copyright (c) 2015 blaze. All rights reserved.
//

import Foundation

/*******************************************************************************
CString
*******************************************************************************/
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
        buffer.deallocate(capacity: _len)
    }
}

/*******************************************************************************
InteropStructSupp
*******************************************************************************/
struct InteropStructSupp{
    let obj: AnyObject
    let payLoad: AnyObject
}

/*******************************************************************************
MobilePeer
*******************************************************************************/
class MobilePeer
{

    static func getMobilePeerInstance(_ name: String?, version: [CUnsignedInt]?) -> MobilePeer? {
        if MobilePeer.instance == nil{
            if let peerName = name, let peerVersion = version{
                MobilePeer.instance = MobilePeer(name: CString(peerName), version: peerVersion)
                return MobilePeer.instance
            }
        }else{
            return MobilePeer.instance
        }
        return nil
    }

    init(name: CString, version: [CUnsignedInt]){
    
        mobilePeerName = name
        mobilePeerVersion = version
    
        if let filePath = Bundle.main.path(forResource: "logger", ofType:"cfg") {
            logger_set_logger_cfg_file_path_name(filePath)
        } else {
            print("logger.cfg not found")
        }
        
        if let filePath = Bundle.main.path(forResource: "perscfg", ofType:"") {
            persistence_manager_set_cfg_file_path_name(filePath)
        } else {
            print("perscfg not found")
        }
        
        /************
        Peer Creation
        ************/
        blaze_mobile_peer = peer_create()
        
        if let filePath = Bundle.main.path(forResource: "params", ofType:"") {
            peer_set_params_file_path_name(blaze_mobile_peer, filePath)
        } else {
            print("params not found")
        }
    
        peer_set_name_handler_swift(blaze_mobile_peer, MobilePeer.peer_name_handler_swift_mobile_peer)
        peer_set_version_handler_swift(blaze_mobile_peer, MobilePeer.peer_ver_handler_swift_mobile_peer)
        peer_set_status_change_handler_swift(blaze_mobile_peer, MobilePeer.peer_status_change_swift_mobile_peer)
    }
    
//------------------------------------------------------------------------------
// MobilePeer Actions
//------------------------------------------------------------------------------
    func startPeer(){
        let argv = UnsafeMutablePointer<UnsafeMutablePointer<Int8>?>.allocate(capacity: 2);
        let arg0 = CString("");
        let arg1 = CString("-file");
        argv[0] = arg0.buffer;
        argv[1] = arg1.buffer;
        peer_start(blaze_mobile_peer, 2, argv, 1);
        argv.deallocate(capacity: 2)
    }
    
    func stopPeer(){
        peer_stop(blaze_mobile_peer, 1)
    }
    
//------------------------------------------------------------------------------
// Internal
//------------------------------------------------------------------------------
    
    let mobilePeerName: CString
    let mobilePeerVersion: [CUnsignedInt]
    
    fileprivate static func peer_name_handler_swift_mobile_peer(_ p: peer_wr?) -> UnsafePointer<CChar>?
    {
        return UnsafePointer(MobilePeer.instance?.mobilePeerName.buffer ?? nil);
    }
    
    fileprivate static func peer_ver_handler_swift_mobile_peer(_ p: peer_wr?) -> UnsafePointer<CUnsignedInt>?
    {
        return UnsafePointer(MobilePeer.instance?.mobilePeerVersion ?? nil);
    }
    
    fileprivate static func peer_status_change_swift_mobile_peer(_ p: peer_wr?, status: BLZ_PEER_STATUS)
    {
        print("status changed: \(status)")
    }
    
    var blog: blaze_logger_wr? = nil
    var conn_params = sockaddr_in()
    var blaze_mobile_peer: peer_wr? = nil
    
    static var instance: MobilePeer?
}



