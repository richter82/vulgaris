/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

import Foundation

open class CString {
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

open class Peer
{
    init(peer_name: CString, peer_ver: [CUnsignedInt]) {
        peer_own_ = peer_create()
        peer_ = own_peer_get_ptr(peer_own_)
        peer_name_ = peer_name
        peer_ver_ = peer_ver
        peer_set_name_handler_swf(peer_, Peer.peer_name_handler, nil)
    }
    
    deinit {
        peer_destroy(peer_own_)
    }
    
    fileprivate static func peer_name_handler(p: OpaquePointer!, ud: UnsafeMutableRawPointer!) -> UnsafePointer<CChar>?
    {
        //return UnsafePointer(peer_name_.buffer)
        return nil
    }
    
    var peer_own_: OpaquePointer
    var peer_: OpaquePointer
    let peer_name_: CString
    let peer_ver_: [CUnsignedInt]
}
