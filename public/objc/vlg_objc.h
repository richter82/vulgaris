//
//  blz_os_support.h
//
//  Created by Giuseppe Baccini on 27/04/15.
//  Copyright (c) 2015 blaze. All rights reserved.
//

#ifndef blz_os_support_h
#define blz_os_support_h

#import "blaze_c_logger.h"
#import "blaze_c_peer.h"
#import "blaze_c_transaction.h"
#import "blaze_c_subscription.h"
#import "blaze_c_persistence.h"

/*******************************************************************************
 peer
 ******************************************************************************/

typedef void                (^peer_status_change_swift)(peer_wr p, BLZ_PEER_STATUS status);
typedef const char          *(^peer_name_handler_swift)(peer_wr p);
typedef const unsigned int  *(^peer_version_handler_swift)(peer_wr p);
typedef CDRS                (^peer_load_config_handler_swift)(peer_wr p, int pnum, const char *param, const char *value);
typedef CDRS                (^peer_init_handler_swift)(peer_wr p);
typedef CDRS                (^peer_starting_handler_swift)(peer_wr p);
typedef CDRS                (^peer_stopping_handler_swift)(peer_wr p);
typedef CDRS                (^peer_transit_on_air_handler_swift)(peer_wr p);
typedef void                (^peer_error_handler_swift)(peer_wr p);
typedef void                (^peer_dying_breath_handler_swift)(peer_wr p);

//------------------------------------------------------------------------------
// LIFECYCLE - User mandatory entrypoints
//------------------------------------------------------------------------------
void peer_set_name_handler_swift(peer_wr p, peer_name_handler_swift cllbk);
void peer_set_version_handler_swift(peer_wr p, peer_version_handler_swift cllbk);

//------------------------------------------------------------------------------
// LIFECYCLE - User opt. entrypoints
//------------------------------------------------------------------------------
void peer_set_load_config_handler_swift(peer_wr p, peer_load_config_handler_swift cllbk);
void peer_set_init_handler_swift(peer_wr p, peer_init_handler_swift cllbk);
void peer_set_starting_handler_swift(peer_wr p, peer_starting_handler_swift cllbk);
void peer_set_stopping_handler_swift(peer_wr p, peer_stopping_handler_swift cllbk);
void peer_set_transit_on_air_handler_swift(peer_wr p, peer_transit_on_air_handler_swift cllbk);
void peer_set_error_handler_swift(peer_wr p, peer_error_handler_swift cllbk);
void peer_set_dying_breath_handler_swift(peer_wr p, peer_dying_breath_handler_swift cllbk);

//------------------------------------------------------------------------------
// STATUS
//------------------------------------------------------------------------------
void peer_set_status_change_handler_swift(peer_wr p, peer_status_change_swift cllbk);


#endif
