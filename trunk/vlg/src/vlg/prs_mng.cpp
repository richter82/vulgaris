/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "prs_impl.h"

namespace vlg {

#define VLG_RWRD_CONN_CFG_BG    "connection_pool_config_begin"
#define VLG_RWRD_CONN_CFG_END   "connection_pool_config_end"
#define VLG_RWRD_MAP_CFG_BG     "map_config_begin"
#define VLG_RWRD_MAP_CFG_END    "map_config_end"
#define VLG_RWRD_URI            "uri"
#define VLG_RWRD_URL            "url"
#define VLG_RWRD_USR            "usr"
#define VLG_RWRD_PSSWD          "psswd"
#define VLG_RWRD_POOL_SIZE      "pool_size"
#define VLG_RWRD_POOL_TH_SIZE   "th_size"

/***********************************
READ- VLG_PERS_MNG_ReadUInt
***********************************/
RetCode VLG_PERS_MNG_ReadUInt(unsigned long &lnum,
                              str_tok &tknz,
                              unsigned int &uint)
{
    std::string tkn;
    while(tknz.next_token(tkn, DF_DLM, true)) {
        SKIP_SP_TAB(tkn)
        DO_CMD_ON_NL(tkn, return RetCode_BADCFG)
        if(string_is_number(tkn.c_str())) {
            uint = atoi(tkn.c_str());
        } else {
            return RetCode_KO;
        }
        break;
    }
    return RetCode_OK;
}

/***********************************
READ- VLG_PERS_MNG_ReadString
***********************************/
RetCode VLG_PERS_MNG_ReadString(unsigned long &lnum,
                                str_tok &tknz,
                                std::string &out)
{
    std::string tkn;
    while(tknz.next_token(tkn, DF_DLM QT, true)) {
        SKIP_SP_TAB(tkn)
        DO_CMD_ON_NL(tkn, return RetCode_BADCFG)
        if(tkn == QT) {
            //ok we have read the beginning quote of the string
            break;
        } else {
            //unexp token
            return RetCode_KO;
        }
    }
    while(tknz.next_token(tkn, NL_DLM QT, true)) {
        //we expect a string, so we return with error if newline found.
        DO_CMD_ON_NL(tkn, return RetCode_BADCFG)
        if(!out.length()) {
            if(tkn == QT) {
                //empty string
                break;
            } else {
                out.assign(tkn);
            }
        } else {
            if(tkn == QT) {
                //ok we have read the ending quote of the string
                break;
            } else {
                //unexp token
                return RetCode_KO;
            }
        }
    }
    return RetCode_OK;
}

RetCode VLG_PERS_MNG_ReadColon(unsigned long &lnum,
                               str_tok &tknz)
{
    std::string tkn;
    //read mandatory colon
    while(tknz.next_token(tkn, DF_DLM CLN, true)) {
        SKIP_SP_TAB(tkn)
        DO_CMD_ON_NL(tkn, return RetCode_BADCFG)
        if(tkn == CLN) {
            break;
        } else {
            return RetCode_BADCFG;
        }
    }
    return RetCode_OK;
}

RetCode VLG_PERS_MNG_ReadRBL(unsigned long &lnum,
                             str_tok &tknz)
{
    std::string tkn;
    while(tknz.next_token(tkn, DF_DLM RBL, true)) {
        SKIP_SP_TAB(tkn)
        DO_CMD_ON_NL(tkn, return RetCode_BADCFG)
        if(tkn == RBL) {
            break;
        } else {
            return RetCode_BADCFG;
        }
    }
    return RetCode_OK;
}

RetCode VLG_PERS_MNG_ReadRBR(unsigned long &lnum,
                             str_tok &tknz)
{
    std::string tkn;
    while(tknz.next_token(tkn, DF_DLM RBR, true)) {
        SKIP_SP_TAB(tkn)
        DO_CMD_ON_NL(tkn, return RetCode_BADCFG)
        if(tkn == RBR) {
            break;
        } else {
            return RetCode_BADCFG;
        }
    }
    return RetCode_OK;
}

// VLG_PERS_MANAGER

#define PERS_CFG_FILE_DIR_LEN 1024
static char pers_cfg_file_dir[PERS_CFG_FILE_DIR_LEN] = {0};

#define PERS_CFG_FILE_PATH_NAME_LEN 1024
static char pers_cfg_file_path_name[PERS_CFG_FILE_PATH_NAME_LEN] = {0};

persistence_manager_impl *p_mng_snglt = nullptr;
persistence_manager_impl &persistence_manager_impl::get_instance()
{
    if(p_mng_snglt) {
        return *p_mng_snglt;
    }
    p_mng_snglt = new persistence_manager_impl();
    return *p_mng_snglt;
}

RetCode persistence_manager_impl::set_cfg_file_dir(const char *dir)
{
    strncpy(pers_cfg_file_dir, dir, PERS_CFG_FILE_DIR_LEN);
    return RetCode_OK;
}

RetCode persistence_manager_impl::set_cfg_file_path_name(const char *file_path)
{
    strncpy(pers_cfg_file_path_name, file_path, PERS_CFG_FILE_PATH_NAME_LEN);
    return RetCode_OK;
}

RetCode persistence_manager_impl::load_pers_driver_dyna(const char *drivers[],
                                                        int drivers_num)
{
    IFLOG(get_instance().log_, trc(TH_ID, LS_OPN "[drivers_num:%d]", __func__, drivers_num))
    RetCode rcode = RetCode_OK;
    for(int i = 0; i < drivers_num; i++) {
        if(get_instance().drivname_driv_hm_.find(drivers[i]) != get_instance().drivname_driv_hm_.end()) {
            IFLOG(get_instance().log_, wrn(TH_ID, LS_TRL "[driver already loaded, skipping:%s]", __func__, drivers[i]))
            continue;
        }
        persistence_driver *driv = nullptr;
        if((rcode = persistence_driver::load_driver_dyna(drivers[i], &driv, get_instance().log_))) {
            IFLOG(get_instance().log_, cri(TH_ID, LS_CLO "[failed loading driver:%s][res:%d]", __func__, drivers[i], rcode))
            return rcode;
        }
        get_instance().drivname_driv_hm_[drivers[i]] = driv;
    }
    return rcode;
}

RetCode persistence_manager_impl::load_pers_driver_dyna(std::set<std::string> &drivmap)
{
    IFLOG(get_instance().log_, trc(TH_ID, LS_OPN "[drivers_num:%d]", __func__, drivmap.size()))
    RetCode rcode = RetCode_OK;
    for(auto it = drivmap.begin(); it != drivmap.end(); it++) {
        persistence_driver *driv = nullptr;
        if((rcode = persistence_driver::load_driver_dyna(it->c_str(), &driv, get_instance().log_))) {
            IFLOG(get_instance().log_, cri(TH_ID, LS_CLO "[failed loading driver:%s][res:%d]", __func__, it->c_str(), rcode))
            return rcode;
        }
        get_instance().drivname_driv_hm_[it->c_str()] = driv;
    }
    return rcode;
}

RetCode persistence_manager_impl::persistence_driver_load(persistence_driver *drivers[],
                                                          int drivers_num)
{
    IFLOG(get_instance().log_, trc(TH_ID, LS_OPN "[drivers_num:%d]", __func__, drivers_num))
    for(int i = 0; i < drivers_num; i++) {
        if(get_instance().drivname_driv_hm_.find(drivers[i]->get_driver_name()) != get_instance().drivname_driv_hm_.end()) {
            IFLOG(get_instance().log_, wrn(TH_ID, LS_TRL "[driver already loaded, skipping:%s]", __func__, drivers[i]))
            continue;
        }
        get_instance().drivname_driv_hm_[drivers[i]->get_driver_name()] = drivers[i];
    }
    return RetCode_OK;
}

persistence_manager_impl::persistence_manager_impl() :
    log_(get_logger(VLGDEFLOG))
{}

RetCode persistence_manager_impl::map_classid_driver(unsigned int nclass_id,
                                                     persistence_driver *driver)
{
    nclassid_driv_hm_[nclass_id] = driver;
    return RetCode_OK;
}

RetCode persistence_manager_impl::start_all_drivers()
{
    RetCode rcode = RetCode_OK;
    for(auto it = drivname_driv_hm_.begin(); it != drivname_driv_hm_.end(); it++) {
        if((rcode = it->second->start_all_pools())) {
            IFLOG(log_, cri(TH_ID, LS_TRL "[failed to start persistence-driver:%s]", __func__, it->first.c_str()))
            break;
        }
    }
    IFLOG(log_, trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

persistence_driver *persistence_manager_impl::available_driver(unsigned int nclass_id)
{
    auto it = nclassid_driv_hm_.find(nclass_id);
    if(it == nclassid_driv_hm_.end()) {
        IFLOG(log_, err(TH_ID, LS_TRL "[nclass_id:%d has no persistence-driver available]", __func__, nclass_id))
    }
    return it->second;
}

/***********************************
PARSE- ParseData
***********************************/

RetCode persistence_manager_impl::parse_impl_after_colon(unsigned long &lnum,
                                                         str_tok &tknz,
                                                         unsigned int &integer)
{
    std::string tkn;
    //read mandatory colon
    RET_ON_KO(VLG_PERS_MNG_ReadColon(lnum, tknz))
    RET_ON_KO(VLG_PERS_MNG_ReadUInt(lnum, tknz, integer))
    return RetCode_OK;
}

RetCode persistence_manager_impl::parse_URI(unsigned long &lnum,
                                            str_tok &tknz,
                                            std::string &url,
                                            std::string &usr,
                                            std::string &psswd)
{
    std::string tkn;
    //read mandatory colon
    RET_ON_KO(VLG_PERS_MNG_ReadColon(lnum, tknz))
    //read mandatory [
    RET_ON_KO(VLG_PERS_MNG_ReadRBL(lnum, tknz))
    //read url keyword
    while(tknz.next_token(tkn, DF_DLM CLN, true)) {
        SKIP_SP_TAB(tkn)
        DO_CMD_ON_NL(tkn, return RetCode_BADCFG)
        if(tkn == VLG_RWRD_URL) {
            break;
        } else {
            IFLOG(log_, cri(TH_ID, LS_PRS "[line:%d, unexpected token:%s]", __func__, lnum, tkn.c_str()))
            return RetCode_BADCFG;
        }
    }
    //read mandatory colon
    RET_ON_KO(VLG_PERS_MNG_ReadColon(lnum, tknz))
    //read url
    RET_ON_KO(VLG_PERS_MNG_ReadString(lnum, tknz, url))
    //read usr keyword
    while(tknz.next_token(tkn, DF_DLM CLN, true)) {
        SKIP_SP_TAB(tkn)
        DO_CMD_ON_NL(tkn, return RetCode_BADCFG)
        if(tkn == VLG_RWRD_USR) {
            break;
        } else {
            IFLOG(log_, cri(TH_ID, LS_PRS "[line:%d, unexpected token:%s]", __func__, lnum, tkn.c_str()))
            return RetCode_BADCFG;
        }
    }
    //read mandatory colon
    RET_ON_KO(VLG_PERS_MNG_ReadColon(lnum, tknz))
    //read usr
    RET_ON_KO(VLG_PERS_MNG_ReadString(lnum, tknz, usr))
    //read psswd keyword
    while(tknz.next_token(tkn, DF_DLM CLN, true)) {
        SKIP_SP_TAB(tkn)
        DO_CMD_ON_NL(tkn, return RetCode_BADCFG)
        if(tkn == VLG_RWRD_PSSWD) {
            break;
        } else {
            IFLOG(log_, cri(TH_ID, LS_PRS "[line:%d, unexpected token:%s]", __func__, lnum, tkn.c_str()))
            return RetCode_BADCFG;
        }
    }
    //read mandatory colon
    RET_ON_KO(VLG_PERS_MNG_ReadColon(lnum, tknz))
    //read passwd
    RET_ON_KO(VLG_PERS_MNG_ReadString(lnum, tknz, psswd))
    //read mandatory ]
    RET_ON_KO(VLG_PERS_MNG_ReadRBR(lnum, tknz))
    return RetCode_OK;
}


RetCode persistence_manager_impl::parse_single_conn_pool_cfg(unsigned long &lnum,
                                                             str_tok &tknz,
                                                             std::string &conn_pool_name,
                                                             std::unordered_map<std::string, persistence_driver *> &conn_pool_name_to_driv)
{
    persistence_driver *driv = nullptr;
    std::string tkn, url, usr, psswd;
    unsigned int pool_size = 0, th_size = 0;
    //read driver type
    while(tknz.next_token(tkn, DF_DLM, true)) {
        SKIP_SP_TAB(tkn)
        DO_CMD_ON_NL(tkn, return RetCode_BADCFG)
        auto it = drivname_driv_hm_.find(tkn);
        if(it == drivname_driv_hm_.end()) {
            IFLOG(log_, cri(TH_ID, LS_PRS "[line:%d, driver not loaded:%s]", __func__, lnum, tkn.c_str()))
            return RetCode_BADCFG;
        } else {
            driv = it->second;
        }
        break;
    }
    //read uri rword
    while(tknz.next_token(tkn, DF_DLM CLN, true)) {
        SKIP_SP_TAB(tkn)
        DO_CMD_ON_NL(tkn, return RetCode_BADCFG)
        if(tkn == VLG_RWRD_URI) {
            RET_ON_KO(parse_URI(lnum, tknz, url, usr, psswd))
            break;
        } else {
            IFLOG(log_, cri(TH_ID, LS_PRS "[line:%d, unexpected token:%s]", __func__, lnum, tkn.c_str()))
            return RetCode_BADCFG;
        }
        break;
    }
    //read pool_size keyword
    while(tknz.next_token(tkn, DF_DLM CLN, true)) {
        SKIP_SP_TAB(tkn)
        DO_CMD_ON_NL(tkn, return RetCode_BADCFG)
        if(tkn == VLG_RWRD_POOL_SIZE) {
            RET_ON_KO(parse_impl_after_colon(lnum, tknz, pool_size))
            break;
        } else {
            IFLOG(log_, cri(TH_ID, LS_PRS "[line:%d, unexpected token:%s]", __func__, lnum, tkn.c_str()))
            return RetCode_BADCFG;
        }
    }
    //read th_size keyword
    while(tknz.next_token(tkn, DF_DLM CLN, true)) {
        SKIP_SP_TAB(tkn)
        DO_CMD_ON_NL(tkn, return RetCode_BADCFG)
        if(tkn == VLG_RWRD_POOL_TH_SIZE) {
            RET_ON_KO(parse_impl_after_colon(lnum, tknz, th_size))
            break;
        } else {
            IFLOG(log_, cri(TH_ID, LS_PRS "[line:%d, unexpected token:%s]", __func__, lnum, tkn.c_str()))
            return RetCode_BADCFG;
        }
    }
    RET_ON_KO(driv->add_pool(conn_pool_name.c_str(),
                             url.c_str(),
                             usr.c_str(),
                             psswd.c_str(),
                             pool_size,
                             th_size))
    conn_pool_name_to_driv[conn_pool_name] = driv;
    return RetCode_OK;
}

RetCode persistence_manager_impl::parse_conn_pool_cfg(unsigned long &lnum,
                                                      str_tok &tknz,
                                                      std::unordered_map<std::string, persistence_driver *> &conn_pool_name_to_driv)
{
    std::string tkn, conn_pool_name;
    while(tknz.next_token(tkn, DF_DLM, true)) {
        SKIP_SP_TAB(tkn)
        DO_CMD_ON_NL(tkn, lnum++; continue)
        if(tkn == VLG_RWRD_CONN_CFG_END) {
            break;
        } else {
            conn_pool_name.assign(tkn);
            RET_ON_KO(parse_single_conn_pool_cfg(lnum,
                                                 tknz,
                                                 conn_pool_name,
                                                 conn_pool_name_to_driv))
        }
    }
    return RetCode_OK;
}

RetCode persistence_manager_impl::parse_single_nclass_map_cfg(unsigned long &lnum,
                                                              str_tok &tknz,
                                                              unsigned int nclass_id,
                                                              std::unordered_map<std::string, persistence_driver *> &conn_pool_name_to_driv)
{
    std::string tkn;
    persistence_driver *driv = nullptr;
    //read driver type
    while(tknz.next_token(tkn, DF_DLM, true)) {
        SKIP_SP_TAB(tkn)
        DO_CMD_ON_NL(tkn, return RetCode_BADCFG)
        auto it = conn_pool_name_to_driv.find(tkn);
        if(it == conn_pool_name_to_driv.end()) {
            IFLOG(log_, cri(TH_ID, LS_PRS "[line:%d, connection pool not defined:%s]", __func__, lnum, tkn.c_str()))
            return RetCode_BADCFG;
        } else {
            driv = it->second;
        }
        break;
    }
    RET_ON_KO(driv->map_nclassid_to_pool(nclass_id, tkn.c_str()))
    RET_ON_KO(map_classid_driver(nclass_id, driv))
    return RetCode_OK;
}

RetCode persistence_manager_impl::parse_nclass_mapping_cfg(unsigned long &lnum,
                                                           str_tok &tknz,
                                                           std::unordered_map<std::string, persistence_driver *> &conn_pool_name_to_driv)
{
    std::string tkn, nclass_id;
    while(tknz.next_token(tkn, DF_DLM, true)) {
        SKIP_SP_TAB(tkn)
        DO_CMD_ON_NL(tkn, lnum++; continue)
        if(tkn == VLG_RWRD_MAP_CFG_END) {
            break;
        } else {
            nclass_id.assign(tkn);
            if(!string_is_number(nclass_id.c_str())) {
                IFLOG(log_, cri(TH_ID, LS_PRS "[line:%d, bad nclass_id:%s]", __func__, lnum, nclass_id.c_str()))
                return RetCode_BADCFG;
            }
            unsigned int classid_n = atoi(nclass_id.c_str());
            RET_ON_KO(parse_single_nclass_map_cfg(lnum,
                                                  tknz,
                                                  classid_n,
                                                  conn_pool_name_to_driv))
        }
    }
    return RetCode_OK;
}

RetCode persistence_manager_impl::parse_data(std::string &data)
{
    bool conn_cfg_done = false, mapping_cfg_done = false;
    unsigned long lnum = 1;
    bool parsing_comment = false;
    std::string tkn;
    str_tok tknz(data);
    std::unordered_map<std::string, persistence_driver *> conn_pool_name_driv_hm;

    while(tknz.next_token(tkn, DF_DLM VLG_TK_COMMENT, true)) {
        SKIP_SP_TAB(tkn)
        DO_CMD_ON_NL(tkn, parsing_comment = false; lnum++;
                     continue)
        if(!parsing_comment) {
            if(tkn == VLG_RWRD_CONN_CFG_BG) {
                if(conn_cfg_done) {
                    return RetCode_BADCFG;
                }
                RET_ON_KO(parse_conn_pool_cfg(lnum, tknz, conn_pool_name_driv_hm))
                conn_cfg_done = true;
            } else if(tkn == VLG_RWRD_MAP_CFG_BG) {
                if(mapping_cfg_done) {
                    return RetCode_BADCFG;
                }
                RET_ON_KO(parse_nclass_mapping_cfg(lnum, tknz, conn_pool_name_driv_hm))
                mapping_cfg_done = true;
            } else if(tkn == VLG_TK_COMMENT) {
                //comment begin
                parsing_comment = true;
            } else {
                IFLOG(log_, cri(TH_ID, LS_PRS "[line:%d, unexpected token:%s]", __func__, lnum, tkn.c_str()))
                return RetCode_BADCFG;
            }
        }
    }
    if(!conn_cfg_done || !mapping_cfg_done) {
        IFLOG(log_, cri(TH_ID, LS_PRS "[bad persistence configuration]", __func__))
        return RetCode_BADCFG;
    }
    return RetCode_OK;
}

RetCode persistence_manager_impl::load_cfg(const char *filename)
{
    IFLOG(log_, trc(TH_ID, LS_OPN "[filename:%s]", __func__, filename))
    std::string path;
    path.assign(pers_cfg_file_dir);
    if(path.length() > 0) {
        path.append(FS_SEP);
    }
    path.append(filename);
    FILE *fdesc = fopen(path.c_str(), "r");
    if(!fdesc) {
        IFLOG(log_, wrn(TH_ID, LS_CLO "[cannot open persistent configuration file:%s]", __func__, filename))
        return RetCode_IOERR;
    }
    std::string data;
    RET_ON_KO(load_file(fdesc, data))
    RET_ON_KO(parse_data(data))
    return RetCode_OK;
}

RetCode persistence_manager_impl::load_cfg()
{
    RetCode rcode = RetCode_OK;
    if(strlen(pers_cfg_file_path_name)) {
        rcode = load_cfg(pers_cfg_file_path_name);
    } else {
        rcode = load_cfg("perscfg");
    }
    IFLOG(log_, trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

}

