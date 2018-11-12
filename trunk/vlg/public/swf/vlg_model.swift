/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

import Foundation

/**
 * MemberDesc
 */
class MemberDesc{
    
    init(_ member_desc: OpaquePointer){
        self.member_desc_op = member_desc
    }
    
    func getMemberId() -> UInt16{
        return member_desc_get_member_id(member_desc_op)
    }
    
    func getMemberType() -> MemberType{
        return member_desc_get_member_type(member_desc_op)
    }
    
    func getMemberName() -> String{
        return String(cString: member_desc_get_member_name(member_desc_op))
    }
    
    func getMemberDescription() -> String{
        return String(cString: member_desc_get_member_description(member_desc_op))
    }
    
    func getFieldVlgType() -> Type{
        return member_desc_get_field_vlg_type(member_desc_op)
    }
    
    func getFieldOffset() -> size_t{
        return member_desc_get_field_offset(member_desc_op)
    }
    
    func getFieldTypeSize() -> size_t{
        return member_desc_get_field_type_size(member_desc_op)
    }
    
    func getFieldNMemb() -> size_t{
        return member_desc_get_field_nmemb(member_desc_op)
    }
    
    func getFieldNClassId() -> UInt32{
        return member_desc_get_field_nclass_id(member_desc_op)
    }
    
    func getFieldUserType() -> String{
        return String(cString: member_desc_get_field_user_type(member_desc_op))
    }
    
    func getFieldNEntityType() -> NEntityType{
        return member_desc_get_field_nentity_type(member_desc_op)
    }
    
    func getNEnumValue() -> Int{
        return member_desc_get_nenum_value(member_desc_op)
    }
    
    var member_desc_op: OpaquePointer
}

/**
 * KeyDesc
 */
class KeyDesc{
    
    init(_ key_desc: OpaquePointer){
        self.key_desc_op = key_desc
    }
    
    func getKeyId() -> UInt16{
        return key_desc_get_key_id(key_desc_op)
    }
    
    func isPrimary() -> Bool{
        return key_desc_is_primary(key_desc_op) == 1 ? true : false
    }
    
    var key_desc_op: OpaquePointer
}

/**
 * NEntityDesc
 */
class NEntityDesc{
    
    init(_ nentity_desc: OpaquePointer){
        self.nentity_desc_op = nentity_desc
    }
    
    func getNClassId() -> UInt32{
        return nentity_desc_get_nclass_id(nentity_desc_op)
    }
    
    func getNEntitySize() -> size_t{
        return nentity_desc_get_nentity_size(nentity_desc_op)
    }
    
    func getNEntityType() -> NEntityType{
        return nentity_desc_get_nentity_type(nentity_desc_op)
    }
    
    func getNEntityNamespace() -> String{
        return String(cString: nentity_desc_get_nentity_namespace(nentity_desc_op))
    }
    
    func getNEntityName() -> String{
        return String(cString: nentity_desc_get_nentity_name(nentity_desc_op))
    }
    
    func getNEntityAllocationFunc() -> (() -> NClass)? {
        if let nentity_alloc_f = nentity_desc_get_nentity_allocation_function(nentity_desc_op){
            return {return NClass(own_nclass: nentity_alloc_f()!)}
        }
        return nil
    }
    
    func getNEntityMemberCount() -> UInt32{
        return nentity_desc_get_nentity_member_num(nentity_desc_op)
    }
    
    func isPersistent() -> Bool{
        return nentity_desc_is_persistent(nentity_desc_op) == 1 ? true : false
    }
    
    func getMemberDescById(memberId: UInt32) -> MemberDesc?{
        if let member_desc = nentity_desc_get_member_desc_by_id(nentity_desc_op, memberId){
            return MemberDesc(member_desc)
        }
        return nil
    }
    
    func getMemberDescByName(memberName: String) -> MemberDesc?{
        if let member_desc = nentity_desc_get_member_desc_by_name(nentity_desc_op, memberName){
            return MemberDesc(member_desc)
        }
        return nil
    }

    var nentity_desc_op: OpaquePointer
}

/**
 * NClass
 */
class NClass{
    
    init (own_nclass nclass: OpaquePointer){
        self.own_nclass_op = nclass
        self.nclass_op = own_nclass_get_ptr(nclass)
    }
    
    init(weak_nclass nclass: OpaquePointer){
        self.nclass_op = nclass
    }
    
    deinit{
        if let to_release = own_nclass_op {
            own_nclass_release(to_release)
        }
    }
    
    func getNClassId() -> UInt32{
        return nclass_get_nclass_id(nclass_op)
    }
    
    func getNEntitySize() -> size_t{
        return nclass_get_nentity_size(nclass_op)
    }
    
    func getZeroObject() -> NClass{
        return NClass(weak_nclass: nclass_get_zero_object(nclass_op))
    }
    
    func copyTo(to: NClass){
        nclass_copy_to(nclass_op, to.nclass_op)
    }
	
	func clone() -> NClass{
		return NClass(own_nclass: nclass_clone(nclass_op))
	}
	
	func isZero() -> Bool{
		return nclass_is_zero(nclass_op) == 1 ? true : false
	}
	
	func setZero(){
		nclass_set_zero(nclass_op)
	}
	
	func setFrom(other: NClass){
		nclass_set_from(nclass_op, other.nclass_op)
	}
	
	func getFieldSizeById(fieldId: UInt32) -> size_t{
		return nclass_get_field_size_by_id(nclass_op, fieldId)
	}
	
	func getFieldSizeByName(fieldName: String) -> size_t{
		return nclass_get_field_size_by_name(nclass_op, fieldName)
	}
	
	func getFieldById(fieldId: UInt32) -> UnsafeMutableRawPointer?{
		return nclass_get_field_by_id(nclass_op, fieldId)
	}
	
	func getFieldByName(fieldName: String) -> UnsafeMutableRawPointer?{
		return nclass_get_field_by_name(nclass_op, fieldName)
	}
	
	func getFieldByIdIndex(fieldId: UInt32, index: UInt32) -> UnsafeMutableRawPointer?{
		return nclass_get_field_by_id_index(nclass_op, fieldId, index)
	}
	
	func getFieldByNameIndex(fieldName: String, index: UInt32) -> UnsafeMutableRawPointer?{
		return nclass_get_field_by_name_index(nclass_op, fieldName, index)
	}
    
    func getFieldByColumnNumber(colNum: UInt32, nEntityManager: NEntityManager) -> (field: UnsafeMutableRawPointer?, memberDesc: MemberDesc?){
        //let
        return (nil, nil)
    }
	
	func setFieldById(fieldId: UInt32, ptr: UnsafeRawPointer?, maxlen: size_t) -> RetCode{
        return nclass_set_field_by_id(nclass_op, fieldId, ptr, maxlen)
	}
	
	func setFieldByName(fieldName: String, ptr: UnsafeRawPointer?, maxlen: size_t) -> RetCode{
		return nclass_set_field_by_name(nclass_op, fieldName, ptr, maxlen)
	}
	
	func setFieldByIdIndex(fieldId: UInt32, ptr: UnsafeRawPointer?, index: UInt32, maxlen: size_t) -> RetCode{
		return nclass_set_field_by_id_index(nclass_op, fieldId, ptr, index, maxlen)
	}
	
	func setFieldByNameIndex(fieldName: String, ptr: UnsafeRawPointer?, index: UInt32, maxlen: size_t) -> RetCode{
		return nclass_set_field_by_name_index(nclass_op, fieldName, ptr, index, maxlen)
	}
	
	func isFieldZeroById(fieldId: UInt32) -> (result: RetCode, val: Bool){
        let res = UnsafeMutablePointer<Int32>.allocate(capacity: 1)
        let resF = nclass_is_field_zero_by_id(nclass_op, fieldId, res)
        let resB = res.pointee == 1 ? true : false
        res.deallocate()
		return (resF, resB)
	}
	
	func isFieldZeroByName(fieldName: String) -> (result: RetCode, val: Bool){
        let res = UnsafeMutablePointer<Int32>.allocate(capacity: 1)
        let resF = nclass_is_field_zero_by_name(nclass_op, fieldName, res)
        let resB = res.pointee == 1 ? true : false
        res.deallocate()
        return (resF, resB)	}
	
	func isFieldZeroByIdIndex(fieldId: UInt32, index: UInt32) -> (result: RetCode, val: Bool){
        let res = UnsafeMutablePointer<Int32>.allocate(capacity: 1)
        let resF = nclass_is_field_zero_by_id_index(nclass_op, fieldId, index, 1, res)
        let resB = res.pointee == 1 ? true : false
        res.deallocate()
        return (resF, resB)
	}
	
	func isFieldZeroByNameIndex(fieldName: String, index: UInt32) -> (result: RetCode, val: Bool){
        let res = UnsafeMutablePointer<Int32>.allocate(capacity: 1)
        let resF = nclass_is_field_zero_by_name_index(nclass_op, fieldName, index, 1, res)
        let resB = res.pointee == 1 ? true : false
        res.deallocate()
        return (resF, resB)

	}
	
	func setFieldZeroById(fieldId: UInt32) -> RetCode{
		return nclass_set_field_zero_by_id(nclass_op, fieldId)
	}
	
	func setFieldZeroByName(fieldName: String) -> RetCode{
		return nclass_set_field_zero_by_name(nclass_op, fieldName)
	}
	
	func setFieldZeroByIdIndex(fieldId: UInt32, index: UInt32) -> RetCode{
		return nclass_set_field_zero_by_id_index(nclass_op, fieldId, index, 1)
	}
	
	func setFieldZeroByNameIndex(fieldName: String, index: UInt32) -> RetCode{
		return nclass_set_field_zero_by_name_index(nclass_op, fieldName, index, 1)
	}
	
	func getNEntityDescriptor() -> NEntityDesc{
		return NEntityDesc(nclass_get_nentity_descriptor(nclass_op))
	}
	
    func getPrimaryKeyValueAsString() -> (result: RetCode, primKeyVal: String?){
        let nallockey_ptr = UnsafeMutablePointer<UnsafeMutablePointer<Int8>?>.allocate(capacity: 1)
        let resF = nclass_get_primary_key_value_as_string(nclass_op, nallockey_ptr)
        var retS: String?
        if let nallockey = nallockey_ptr.pointee{
            retS = String(cString: nallockey)
            free(nallockey)
        }
        nallockey_ptr.deallocate()
        return (resF, retS)
    }
	
    var own_nclass_op: OpaquePointer?
    var nclass_op: OpaquePointer
}

/**
 * NEntityManager
 */
class NEntityManager{

	init(nentity_manager: OpaquePointer){
		self.nentity_manager_op = nentity_manager
	}

	func getNEntityDescriptorByNClassId(nclassId: UInt32) -> NEntityDesc?{
		if let nentDesc = nentity_manager_get_nentity_descriptor_by_nclassid(nentity_manager_op, nclassId){
			return NEntityDesc(nentDesc)
		}else{
			return nil
		}
	}
	
	func getNEntityDescriptorByNClassName(nclassName: String) -> NEntityDesc?{
		if let nentDesc = nentity_manager_get_nentity_descriptor_by_name(nentity_manager_op, nclassName){
			return NEntityDesc(nentDesc)
		}else{
			return nil
		}
	}
	
	func getNEntityCount() -> UInt32{
		return nentity_manager_nentity_count(nentity_manager_op)
	}
	
	func getNEnumCount() -> UInt32{
		return nentity_manager_nenum_count(nentity_manager_op)
	}
	
	func getNClassCount() -> UInt32{
		return nentity_manager_nclass_count(nentity_manager_op)
	}
	
	func extendWithNEntityDesc(nEntityDesc: NEntityDesc) -> RetCode{
        return nentity_manager_extend_with_nentity_desc(nentity_manager_op, nEntityDesc.nentity_desc_op)
	}
	
	func extendWithNEntityManager(nEntityMng: NEntityManager) -> RetCode{
		return nentity_manager_extend_with_nentity_manager(nentity_manager_op, nEntityMng.nentity_manager_op)
	}
	
	var nentity_manager_op: OpaquePointer
}

