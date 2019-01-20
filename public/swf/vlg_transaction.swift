/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@live.com
 *
 */

import Foundation

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
    required init(_ incoConn: IncomingConnection, _ sh_inco_tx: OpaquePointer){
        self.incoConn = incoConn
        self.own_inco_tx_op = inco_transaction_get_own_ptr(sh_inco_tx)
        self.inco_tx_op = inco_transaction_get_ptr(own_inco_tx_op)
        inco_transaction_set_on_releaseable_oc(inco_tx_op, on_release, nil)
        inco_transaction_set_on_request_oc(inco_tx_op, on_request, nil)
    }
    
    fileprivate func on_release(itx: OpaquePointer!, ud: UnsafeMutableRawPointer!){
        incoConn.itxRepo.removeValue(forKey: txId)
        inco_transaction_release(own_inco_tx_op)
    }
    
    fileprivate func on_request(itx: OpaquePointer!, ud: UnsafeMutableRawPointer!)
    {
        ResultObj = RequestObj
        Result = TransactionResult_COMMITTED
        ResultCode = ProtocolCode_SUCCESS
        peer_nclass_persistent_update_or_save_and_distribute(incoConn.peer.peer_op, 1, ResultObj)
    }
    
    var Result : TransactionResult{
        get{
            return inco_transaction_get_transaction_result(inco_tx_op)
        }
        set{
            inco_transaction_set_transaction_result(inco_tx_op, newValue)
        }
    }
    
    var ResultCode : ProtocolCode{
        get{
            return inco_transaction_get_transaction_result_code(inco_tx_op)
        }
        set{
            inco_transaction_set_transaction_result_code(inco_tx_op, newValue)
        }
    }
    
    let incoConn :IncomingConnection
    let own_inco_tx_op : OpaquePointer
    var inco_tx_op : OpaquePointer
    
    var txId : tx_id{
        get{
            return tx_id(oth:inco_transaction_get_transaction_id(inco_tx_op).move())
        }
    }
    
    var RequestObj : OpaquePointer{
        get{return inco_transaction_get_request_obj(inco_tx_op)}
    }
    
    var ResultObj : OpaquePointer{
        get{return inco_transaction_get_result_obj(inco_tx_op)}
        set{inco_transaction_set_result_obj(inco_tx_op, newValue)}
    }
}
