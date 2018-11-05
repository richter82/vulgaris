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
            return {return NClass(nentity_alloc_f()!)}
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
		nclass_set_zero(nclass_op, other.nclass_op)
	}
	
	func getFieldSizeById(fieldId: UInt32) -> size_t{
		return nclass_get_field_size_by_id(nclass_op, fieldId)
	}
	
	func getFieldSizeByName(fieldName: String) -> size_t{
		return nclass_get_field_size_by_name(nclass_op, fieldName)
	}
	
	func getFieldById(fieldId: UInt32) -> OpaquePointer?{
		return nil
	}
	
	func getFieldByName(fieldName: String) -> OpaquePointer?{
		return nil
	}
	
	func getFieldByIdIndex(fieldId: UInt32, index: UInt32) -> OpaquePointer?{
		return nil
	}
	
	func getFieldByNameIndex(fieldName: String, index: UInt32) -> OpaquePointer?{
		return nil
	}
	
	func setFieldById(fieldId: UInt32, ptr: OpaquePointer, maxlen: size_t) -> RetCode{
		return nclass_set_field_by_id(nclass_op, fieldId, ptr, maxlen)
	}
	
	func setFieldByName(fieldName: String, ptr: OpaquePointer, maxlen: size_t) -> RetCode{
		return nclass_set_field_by_name(nclass_op, fieldName, ptr, maxlen)
	}
	
	func setFieldByIdIndex(fieldId: UInt32, ptr: OpaquePointer, index: UInt32, maxlen: size_t) -> RetCode{
		return nclass_set_field_by_id(nclass_op, fieldId, ptr, index, maxlen)
	}
	
	func setFieldByNameIndex(fieldName: String, ptr: OpaquePointer, index: UInt32, maxlen: size_t) -> RetCode{
		return nclass_set_field_by_name(nclass_op, fieldName, ptr, index, maxlen)
	}
	
	func isFieldZeroById(fieldId: UInt32) -> (result: RetCode, val: Bool){
		return (RetCode_OK, false)
	}
	
	func isFieldZeroByName(fieldName: String) -> (result: RetCode, val: Bool){
		return (RetCode_OK, false)
	}
	
	func isFieldZeroByIdIndex(fieldId: UInt32, index: UInt32) -> (result: RetCode, val: Bool){
		return (RetCode_OK, false)
	}
	
	func isFieldZeroByNameIndex(fieldName: String, index: UInt32) -> (result: RetCode, val: Bool){
		return (RetCode_OK, false)
	}
	
	func setFieldZeroById(fieldId: UInt32) -> RetCode{
		return nclass_set_field_zero_by_id(nclass_op, fieldId)
	}
	
	func setFieldZeroByName(fieldName: String) -> RetCode{
		return nclass_set_field_zero_by_name(nclass_op, fieldName)
	}
	
	func setFieldZeroByIdIndex(fieldId: UInt32, index: UInt32) -> RetCode{
		return nclass_set_field_zero_by_id_index(nclass_op, fieldId, index)
	}
	
	func setFieldZeroByNameIndex(fieldName: String, index: UInt32) -> RetCode{
		return nclass_set_field_zero_by_name_index(nclass_op, fieldName, index)
	}
	
	//func getFieldByColumnNumber(colNum: UInt32, )
	
	func getNEntityDescriptor() -> NEntityDesc{
		return NEntityDesc(nclass_get_nentity_descriptor(nclass_op))
	}
	
	//func getPrimaryKeyValueAsString()
	
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

	//func getNEntityDescriptorByNClassId(nclassId: UInt32) -> NEntityDesc{
	//	return NEntityDesc(nentity_manager_get_nentity_descriptor_by_nclassid)
	//}
    
	var nentity_manager_op: OpaquePointer
}

