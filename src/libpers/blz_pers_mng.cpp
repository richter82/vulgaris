/*
*
* (C) 2017 - giuseppe.baccini@gmail.com
*
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software Foundation,
* Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*
*/

#include "blz_persistence_int.h"

namespace blaze {

#define BLZ_RWRD_CONN_CFG_BG    "connection_pool_config_begin"
#define BLZ_RWRD_CONN_CFG_END   "connection_pool_config_end"
#define BLZ_RWRD_MAP_CFG_BG     "map_config_begin"
#define BLZ_RWRD_MAP_CFG_END    "map_config_end"
#define BLZ_RWRD_URI            "uri"
#define BLZ_RWRD_URL            "url"
#define BLZ_RWRD_USR            "usr"
#define BLZ_RWRD_PSSWD          "psswd"
#define BLZ_RWRD_POOL_SIZE      "pool_size"
#define BLZ_RWRD_POOL_TH_SIZE   "th_size"

/***********************************
READ- BLZ_PERS_MNG_ReadUInt
***********************************/
blaze::RetCode BLZ_PERS_MNG_ReadUInt(unsigned long &lnum,
                                     blaze::ascii_string_tok &tknz,
                                     unsigned int &uint)
{
    blaze::ascii_string tkn;
    while(!tknz.next_token(tkn, CR_DF_DLMT, true)) {
        CR_SKIP_SP_TABS(tkn)
        CR_DO_CMD_ON_NEWLINE(tkn, return blaze::RetCode_BADCFG)
        if(blaze::string_is_int_number(tkn.internal_buff())) {
            uint = atoi(tkn.internal_buff());
        } else {
            return blaze::RetCode_KO;
        }
        break;
    }
    return blaze::RetCode_OK;
}

/***********************************
READ- BLZ_PERS_MNG_ReadString
***********************************/
blaze::RetCode BLZ_PERS_MNG_ReadString(unsigned long &lnum,
                                       blaze::ascii_string_tok &tknz,
                                       blaze::ascii_string &out)
{
    blaze::ascii_string tkn;
    while(!tknz.next_token(tkn, CR_DF_DLMT CR_TK_QT, true)) {
        CR_SKIP_SP_TABS(tkn)
        CR_DO_CMD_ON_NEWLINE(tkn, return blaze::RetCode_BADCFG)
        if(tkn == CR_TK_QT) {
            //ok we have read the beginning quote of the string
            break;
        } else {
            //unexp token
            return blaze::RetCode_KO;
        }
    }
    while(!tknz.next_token(tkn, CR_NL_DLMT
                           CR_TK_QT, true)) {
        //we expect a string, so we return with error if newline found.
        CR_DO_CMD_ON_NEWLINE(tkn, return blaze::RetCode_BADCFG)
        if(!out.length()) {
            if(tkn == CR_TK_QT) {
                //empty string
                break;
            } else {
                out.assign(tkn);
            }
        } else {
            if(tkn == CR_TK_QT) {
                //ok we have read the ending quote of the string
                break;
            } else {
                //unexp token
                return blaze::RetCode_KO;
            }
        }
    }
    return blaze::RetCode_OK;
}

blaze::RetCode BLZ_PERS_MNG_ReadColon(unsigned long &lnum,
                                      blaze::ascii_string_tok &tknz)
{
    blaze::ascii_string tkn;
    //read mandatory colon
    while(!tknz.next_token(tkn, CR_DF_DLMT CR_TK_COLON, true)) {
        CR_SKIP_SP_TABS(tkn)
        CR_DO_CMD_ON_NEWLINE(tkn, return blaze::RetCode_BADCFG)
        if(tkn == CR_TK_COLON) {
            break;
        } else {
            //IFLOG(cri(TH_ID, LS_PRS DBS, __func__, "() - line:%d, unexpected token:%s", lnum, tkn.CBuf()))
            return blaze::RetCode_BADCFG;
        }
    }
    return blaze::RetCode_OK;
}

blaze::RetCode BLZ_PERS_MNG_ReadRBL(unsigned long &lnum,
                                    blaze::ascii_string_tok &tknz)
{
    blaze::ascii_string tkn;
    while(!tknz.next_token(tkn, CR_DF_DLMT CR_TK_RBL, true)) {
        CR_SKIP_SP_TABS(tkn)
        CR_DO_CMD_ON_NEWLINE(tkn, return blaze::RetCode_BADCFG)
        if(tkn == CR_TK_RBL) {
            break;
        } else {
            //IFLOG(cri(TH_ID, LS_PRS DBS, __func__, "() - line:%d, unexpected token:%s", lnum, tkn.CBuf()))
            return blaze::RetCode_BADCFG;
        }
    }
    return blaze::RetCode_OK;
}

blaze::RetCode BLZ_PERS_MNG_ReadRBR(unsigned long &lnum,
                                    blaze::ascii_string_tok &tknz)
{
    blaze::ascii_string tkn;
    while(!tknz.next_token(tkn, CR_DF_DLMT CR_TK_RBR, true)) {
        CR_SKIP_SP_TABS(tkn)
        CR_DO_CMD_ON_NEWLINE(tkn, return blaze::RetCode_BADCFG)
        if(tkn == CR_TK_RBR) {
            break;
        } else {
            //IFLOG(cri(TH_ID, LS_PRS DBS, __func__, "() - line:%d, unexpected token:%s", lnum, tkn.CBuf()))
            return blaze::RetCode_BADCFG;
        }
    }
    return blaze::RetCode_OK;
}

//-----------------------------
// BLZ_PERS_MANAGER
//-----------------------------

#define PERS_CFG_FILE_DIR_LEN 512
static char pers_cfg_file_dir[PERS_CFG_FILE_DIR_LEN] = {0};

#define PERS_CFG_FILE_PATH_NAME_LEN 512
static char pers_cfg_file_path_name[PERS_CFG_FILE_PATH_NAME_LEN] = {0};

nclass_logger *persistence_manager_int::log_ = NULL;

persistence_manager_int *p_mng_snglt = NULL;
persistence_manager_int &persistence_manager_int::get_instance()
{
    if(p_mng_snglt) {
        return *p_mng_snglt;
    }
    if(!(p_mng_snglt = new persistence_manager_int())) {
        IFLOG(fat(TH_ID, LS_PRS "%s%s", __func__,
                  "() - failed creating BLZ_PERS_MANAGER"))
        EXIT_ACTION("failed creating p_mng_snglt\n")
    }
    if(p_mng_snglt->init()) {
        IFLOG(fat(TH_ID, LS_PRS "%s%s", __func__, "() - failed init BLZ_PERS_MANAGER"))
        EXIT_ACTION("failed init p_mng_snglt\n")
    }
    return *p_mng_snglt;
}

blaze::RetCode persistence_manager_int::set_cfg_file_dir(const char *dir)
{
    strncpy(pers_cfg_file_dir,dir, PERS_CFG_FILE_DIR_LEN);
    return blaze::RetCode_OK;
}

blaze::RetCode persistence_manager_int::set_cfg_file_path_name(
    const char *file_path)
{
    strncpy(pers_cfg_file_path_name,file_path, PERS_CFG_FILE_PATH_NAME_LEN);
    return blaze::RetCode_OK;
}

blaze::RetCode persistence_manager_int::load_pers_driver_dyna(
    const char *drivers[],
    int drivers_num)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(drivers:%p, drivers_num:%d)", __func__, drivers,
              drivers_num))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    for(int i = 0; i < drivers_num; i++) {
        if(!get_instance().drivname_driv_hm_.contains_key(drivers[i])) {
            IFLOG(wrn(TH_ID, LS_TRL "%s() - driver already loaded, skipping:%s", __func__,
                      drivers[i]))
            continue;
        }
        persistence_driver_int *driv = NULL;
        if((cdrs_res = persistence_driver_int::load_driver_dyna(drivers[i], &driv))) {
            IFLOG(cri(TH_ID, LS_CLO "%s(res:%d) - failed loading driver:%s", __func__,
                      cdrs_res, drivers[i]))
            return cdrs_res;
        }
        RETURN_IF_NOT_OK(get_instance().drivname_driv_hm_.put(drivers[i], &driv))
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

blaze::RetCode persistence_manager_int::load_pers_driver_dyna(
    blaze::hash_map &drivmap)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(drivers_num:%d)", __func__, drivmap.size()))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    char driv_name[BLZ_DRV_NAME_LEN] = {0};
    drivmap.start_iteration();
    while(!drivmap.next(driv_name, NULL)) {
        persistence_driver_int *driv = NULL;
        if((cdrs_res = persistence_driver_int::load_driver_dyna(driv_name, &driv))) {
            IFLOG(cri(TH_ID, LS_CLO "%s(res:%d) - failed loading driver:%s", __func__,
                      cdrs_res, driv_name))
            return cdrs_res;
        }
        RETURN_IF_NOT_OK(get_instance().drivname_driv_hm_.put(driv_name, &driv))
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

blaze::RetCode persistence_manager_int::load_pers_driver(persistence_driver_int
                                                         *drivers[], int drivers_num)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(drivers_num:%d)", __func__, drivers_num))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    for(int i = 0; i < drivers_num; i++) {
        if(!get_instance().drivname_driv_hm_.contains_key(
                    drivers[i]->get_driver_name())) {
            IFLOG(wrn(TH_ID, LS_TRL "%s() - driver already loaded, skipping:%s", __func__,
                      drivers[i]))
            continue;
        }
        RETURN_IF_NOT_OK(get_instance().drivname_driv_hm_.put(
                             drivers[i]->get_driver_name(), &drivers[i]))
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

persistence_manager_int::persistence_manager_int() :
    drivname_driv_hm_(blaze::sngl_ptr_obj_mng(), blaze::sngl_cstr_obj_mng()),
    nclassid_driv_hm_(blaze::sngl_ptr_obj_mng(), blaze::sngl_cstr_obj_mng())

{
    log_ = get_nclass_logger("persistence_manager_int");
    IFLOG(trc(TH_ID, LS_CTR "%s", __func__))
}

persistence_manager_int::~persistence_manager_int()
{
    IFLOG(trc(TH_ID, LS_DTR "%s", __func__))
}

blaze::RetCode persistence_manager_int::init()
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    RETURN_IF_NOT_OK(drivname_driv_hm_.init(HM_SIZE_NANO))
    RETURN_IF_NOT_OK(nclassid_driv_hm_.init(HM_SIZE_SMALL))
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return blaze::RetCode_OK;
}

blaze::RetCode persistence_manager_int::map_classid_driver(
    unsigned int nclass_id,
    persistence_driver_int *driver)
{
    return nclassid_driv_hm_.put(&nclass_id, &driver);
}

blaze::RetCode persistence_manager_int::start_all_drivers()
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    char driv_name[64];
    persistence_driver_int *driver = NULL;
    drivname_driv_hm_.start_iteration();
    while(!drivname_driv_hm_.next(driv_name, &driver)) {
        if((cdrs_res = driver->start_all_pools())) {
            IFLOG(cri(TH_ID, LS_TRL "%s() - failed to start pers-driver:%s", __func__,
                      driv_name))
            break;
        }
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

persistence_driver_int *persistence_manager_int::available_driver(
    unsigned int nclass_id)
{
    persistence_driver_int *driv_out = NULL;
    if(nclassid_driv_hm_.get(&nclass_id, &driv_out)) {
        IFLOG(err(TH_ID, LS_TRL "%s() - nclass_id:%d - no pers-driver available.",
                  __func__, nclass_id))
    }
    return driv_out;
}

/***********************************
PARSE- ParseData
***********************************/

blaze::RetCode persistence_manager_int::parse_int_after_colon(
    unsigned long &lnum,
    blaze::ascii_string_tok &tknz, unsigned int &integer)
{
    blaze::ascii_string tkn;
    //read mandatory colon
    RETURN_IF_NOT_OK(BLZ_PERS_MNG_ReadColon(lnum, tknz))
    RETURN_IF_NOT_OK(BLZ_PERS_MNG_ReadUInt(lnum, tknz, integer))
    return blaze::RetCode_OK;
}

blaze::RetCode persistence_manager_int::parse_URI(unsigned long &lnum,
                                                  blaze::ascii_string_tok &tknz,
                                                  blaze::ascii_string &url,
                                                  blaze::ascii_string &usr,
                                                  blaze::ascii_string &psswd)
{
    blaze::ascii_string tkn;
    //read mandatory colon
    RETURN_IF_NOT_OK(BLZ_PERS_MNG_ReadColon(lnum, tknz))
    //read mandatory [
    RETURN_IF_NOT_OK(BLZ_PERS_MNG_ReadRBL(lnum, tknz))
    //read url keyword
    while(!tknz.next_token(tkn, CR_DF_DLMT CR_TK_COLON, true)) {
        CR_SKIP_SP_TABS(tkn)
        CR_DO_CMD_ON_NEWLINE(tkn, return blaze::RetCode_BADCFG)
        if(tkn == BLZ_RWRD_URL) {
            break;
        } else {
            IFLOG(cri(TH_ID, LS_PRS "%s() - line:%d, unexpected token:%s", __func__, lnum,
                      tkn.internal_buff()))
            return blaze::RetCode_BADCFG;
        }
    }
    //read mandatory colon
    RETURN_IF_NOT_OK(BLZ_PERS_MNG_ReadColon(lnum, tknz))
    //read url
    RETURN_IF_NOT_OK(BLZ_PERS_MNG_ReadString(lnum, tknz, url))
    //read usr keyword
    while(!tknz.next_token(tkn, CR_DF_DLMT CR_TK_COLON, true)) {
        CR_SKIP_SP_TABS(tkn)
        CR_DO_CMD_ON_NEWLINE(tkn, return blaze::RetCode_BADCFG)
        if(tkn == BLZ_RWRD_USR) {
            break;
        } else {
            IFLOG(cri(TH_ID, LS_PRS "%s() - line:%d, unexpected token:%s", __func__, lnum,
                      tkn.internal_buff()))
            return blaze::RetCode_BADCFG;
        }
    }
    //read mandatory colon
    RETURN_IF_NOT_OK(BLZ_PERS_MNG_ReadColon(lnum, tknz))
    //read usr
    RETURN_IF_NOT_OK(BLZ_PERS_MNG_ReadString(lnum, tknz, usr))
    //read psswd keyword
    while(!tknz.next_token(tkn, CR_DF_DLMT CR_TK_COLON, true)) {
        CR_SKIP_SP_TABS(tkn)
        CR_DO_CMD_ON_NEWLINE(tkn, return blaze::RetCode_BADCFG)
        if(tkn == BLZ_RWRD_PSSWD) {
            break;
        } else {
            IFLOG(cri(TH_ID, LS_PRS "%s() - line:%d, unexpected token:%s", __func__, lnum,
                      tkn.internal_buff()))
            return blaze::RetCode_BADCFG;
        }
    }
    //read mandatory colon
    RETURN_IF_NOT_OK(BLZ_PERS_MNG_ReadColon(lnum, tknz))
    //read passwd
    RETURN_IF_NOT_OK(BLZ_PERS_MNG_ReadString(lnum, tknz, psswd))
    //read mandatory ]
    RETURN_IF_NOT_OK(BLZ_PERS_MNG_ReadRBR(lnum, tknz))
    return blaze::RetCode_OK;
}


blaze::RetCode persistence_manager_int::parse_single_conn_pool_cfg(
    unsigned long &lnum,
    blaze::ascii_string_tok &tknz,
    blaze::ascii_string &conn_pool_name,
    blaze::hash_map &conn_pool_name_to_driv)
{
    persistence_driver_int *driv = NULL;
    blaze::ascii_string tkn, url, usr, psswd;
    unsigned int pool_size = 0, th_size = 0;
    //read driver type
    while(!tknz.next_token(tkn, CR_DF_DLMT, true)) {
        CR_SKIP_SP_TABS(tkn)
        CR_DO_CMD_ON_NEWLINE(tkn, return blaze::RetCode_BADCFG)
        if(drivname_driv_hm_.get(tkn.internal_buff(), &driv)) {
            IFLOG(cri(TH_ID, LS_PRS "%s() - line:%d, driver not loaded:%s", __func__, lnum,
                      tkn.internal_buff()))
            return blaze::RetCode_BADCFG;
        }
        break;
    }
    //read uri rword
    while(!tknz.next_token(tkn, CR_DF_DLMT
                           CR_TK_COLON, true)) {
        CR_SKIP_SP_TABS(tkn)
        CR_DO_CMD_ON_NEWLINE(tkn, return blaze::RetCode_BADCFG)
        if(tkn == BLZ_RWRD_URI) {
            RETURN_IF_NOT_OK(parse_URI(lnum, tknz, url, usr, psswd))
            break;
        } else {
            IFLOG(cri(TH_ID, LS_PRS "%s() - line:%d, unexpected token:%s", __func__, lnum,
                      tkn.internal_buff()))
            return blaze::RetCode_BADCFG;
        }
        break;
    }
    //read pool_size keyword
    while(!tknz.next_token(tkn, CR_DF_DLMT CR_TK_COLON, true)) {
        CR_SKIP_SP_TABS(tkn)
        CR_DO_CMD_ON_NEWLINE(tkn, return blaze::RetCode_BADCFG)
        if(tkn == BLZ_RWRD_POOL_SIZE) {
            RETURN_IF_NOT_OK(parse_int_after_colon(lnum, tknz, pool_size))
            break;
        } else {
            IFLOG(cri(TH_ID, LS_PRS "%s() - line:%d, unexpected token:%s", __func__, lnum,
                      tkn.internal_buff()))
            return blaze::RetCode_BADCFG;
        }
    }
    //read th_size keyword
    while(!tknz.next_token(tkn, CR_DF_DLMT CR_TK_COLON, true)) {
        CR_SKIP_SP_TABS(tkn)
        CR_DO_CMD_ON_NEWLINE(tkn, return blaze::RetCode_BADCFG)
        if(tkn == BLZ_RWRD_POOL_TH_SIZE) {
            RETURN_IF_NOT_OK(parse_int_after_colon(lnum, tknz, th_size))
            break;
        } else {
            IFLOG(cri(TH_ID, LS_PRS "%s() - line:%d, unexpected token:%s", __func__, lnum,
                      tkn.internal_buff()))
            return blaze::RetCode_BADCFG;
        }
    }
    RETURN_IF_NOT_OK(driv->add_pool(conn_pool_name.internal_buff(),
                                    url.internal_buff(),
                                    usr.internal_buff(),
                                    psswd.internal_buff(),
                                    pool_size,
                                    th_size))
    RETURN_IF_NOT_OK(conn_pool_name_to_driv.put(conn_pool_name.internal_buff(),
                                                &driv))
    return blaze::RetCode_OK;
}

blaze::RetCode persistence_manager_int::parse_conn_pool_cfg(unsigned long &lnum,
                                                            blaze::ascii_string_tok &tknz,
                                                            blaze::hash_map &conn_pool_name_to_driv)
{
    blaze::ascii_string tkn, conn_pool_name;
    while(!tknz.next_token(tkn, CR_DF_DLMT, true)) {
        CR_SKIP_SP_TABS(tkn)
        CR_DO_CMD_ON_NEWLINE(tkn, lnum++; continue)
        if(tkn == BLZ_RWRD_CONN_CFG_END) {
            break;
        } else {
            RETURN_IF_NOT_OK(conn_pool_name.assign(tkn))
            RETURN_IF_NOT_OK(parse_single_conn_pool_cfg(lnum,
                                                        tknz,
                                                        conn_pool_name,
                                                        conn_pool_name_to_driv))
        }
    }
    return blaze::RetCode_OK;
}

blaze::RetCode persistence_manager_int::parse_single_class_map_cfg(
    unsigned long &lnum,
    blaze::ascii_string_tok &tknz,
    unsigned int nclass_id,
    blaze::hash_map &conn_pool_name_to_driv)
{
    blaze::ascii_string tkn;
    persistence_driver_int *driv = NULL;
    //read driver type
    while(!tknz.next_token(tkn, CR_DF_DLMT, true)) {
        CR_SKIP_SP_TABS(tkn)
        CR_DO_CMD_ON_NEWLINE(tkn, return blaze::RetCode_BADCFG)
        if(conn_pool_name_to_driv.get(tkn.internal_buff(), &driv)) {
            IFLOG(cri(TH_ID, LS_PRS "%s() - line:%d, connection pool not defined:%s",
                      __func__, lnum, tkn.internal_buff()))
            return blaze::RetCode_BADCFG;
        }
        break;
    }
    RETURN_IF_NOT_OK(driv->map_nclassid_to_pool(nclass_id, tkn.internal_buff()))
    RETURN_IF_NOT_OK(map_classid_driver(nclass_id, driv))
    return blaze::RetCode_OK;
}

blaze::RetCode persistence_manager_int::parse_class_mapping_cfg(
    unsigned long &lnum,
    blaze::ascii_string_tok &tknz,
    blaze::hash_map &conn_pool_name_to_driv)
{
    blaze::ascii_string tkn, nclass_id;
    while(!tknz.next_token(tkn, CR_DF_DLMT, true)) {
        CR_SKIP_SP_TABS(tkn)
        CR_DO_CMD_ON_NEWLINE(tkn, lnum++; continue)
        if(tkn == BLZ_RWRD_MAP_CFG_END) {
            break;
        } else {
            RETURN_IF_NOT_OK(nclass_id.assign(tkn))
            if(!blaze::string_is_int_number(nclass_id.internal_buff())) {
                IFLOG(cri(TH_ID, LS_PRS "%s() - line:%d, bad nclass_id:%s", __func__, lnum,
                          nclass_id.internal_buff()))
                return blaze::RetCode_BADCFG;
            }
            unsigned int classid_n = atoi(nclass_id.internal_buff());
            RETURN_IF_NOT_OK(parse_single_class_map_cfg(lnum,
                                                        tknz,
                                                        classid_n,
                                                        conn_pool_name_to_driv))
        }
    }
    return blaze::RetCode_OK;
}

blaze::RetCode persistence_manager_int::parse_data(blaze::ascii_string &data)
{
    bool conn_cfg_done = false, mapping_cfg_done = false;
    unsigned long lnum = 1;
    bool parsing_comment = false;
    blaze::ascii_string tkn;
    blaze::ascii_string_tok tknz;
    RETURN_IF_NOT_OK(tknz.init(data))
    blaze::hash_map conn_pool_name_driv_hm(blaze::sngl_ptr_obj_mng(),
                                           blaze::sngl_cstr_obj_mng());
    conn_pool_name_driv_hm.init(HM_SIZE_NANO);
    while(!tknz.next_token(tkn, CR_DF_DLMT BLZ_TK_COMMENT, true)) {
        CR_SKIP_SP_TABS(tkn)
        CR_DO_CMD_ON_NEWLINE(tkn, parsing_comment = false; lnum++;
                             continue)
        if(!parsing_comment) {
            if(tkn == BLZ_RWRD_CONN_CFG_BG) {
                if(conn_cfg_done) {
                    return blaze::RetCode_BADCFG;
                }
                RETURN_IF_NOT_OK(parse_conn_pool_cfg(lnum, tknz, conn_pool_name_driv_hm))
                conn_cfg_done = true;
            } else if(tkn == BLZ_RWRD_MAP_CFG_BG) {
                if(mapping_cfg_done) {
                    return blaze::RetCode_BADCFG;
                }
                RETURN_IF_NOT_OK(parse_class_mapping_cfg(lnum, tknz, conn_pool_name_driv_hm))
                mapping_cfg_done = true;
            } else if(tkn == BLZ_TK_COMMENT) {
                //comment begin
                parsing_comment = true;
            } else {
                IFLOG(cri(TH_ID, LS_PRS "%s() - line:%d, unexpected token:%s", __func__, lnum,
                          tkn.internal_buff()))
                return blaze::RetCode_BADCFG;
            }
        }
    }
    if(!conn_cfg_done || !mapping_cfg_done) {
        IFLOG(cri(TH_ID, LS_PRS "%s() - bad persistence configuration", __func__))
        return blaze::RetCode_BADCFG;
    }
    return blaze::RetCode_OK;
}

blaze::RetCode persistence_manager_int::load_cfg(const char *filename)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(filename:%s)", __func__, filename))
    blaze::ascii_string path;
    path.assign(pers_cfg_file_dir);
    if(path.length() > 0) {
        path.append(CR_FS_SEP);
    }
    path.append(filename);
    FILE *fdesc = fopen(path.internal_buff(), "r");
    if(!fdesc) {
        IFLOG(wrn(TH_ID, LS_CLO "%s() - cannot open persistent configuration file:%s",
                  __func__, filename))
        return blaze::RetCode_IOERR;
    }
    blaze::ascii_string data;
    RETURN_IF_NOT_OK(load_file(fdesc, data))
    RETURN_IF_NOT_OK(parse_data(data))
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return blaze::RetCode_OK;
}

blaze::RetCode persistence_manager_int::load_cfg()
{
    IFLOG(trc(TH_ID, LS_OPN "%s()", __func__))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    if(strlen(pers_cfg_file_path_name)) {
        cdrs_res = load_cfg(pers_cfg_file_path_name);
    } else {
        cdrs_res = load_cfg("perscfg");
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

}

