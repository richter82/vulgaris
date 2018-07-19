/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#ifndef VLG_VLG_H_
#define VLG_VLG_H_

#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#ifdef __GNUG__
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#define SOCKET int
#define INVALID_SOCKET (~0)
#define SOCKET_ERROR   (-1)
#endif
#if defined WIN32 && defined _MSC_VER
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

#if defined(__cplusplus)
#include <string>
#include <memory>

extern "C" {
namespace vlg {
#endif

/** @brief vlg return codes.
    When possible functions and methods of the vlg framework
    use this set of codes as return value.
*/
typedef enum {
    RetCode_UNKERR           = -1000,    /**< unknown error */

    //system errors
    RetCode_SCKERR           = -105,     /**< socket error */
    RetCode_DBERR            = -104,     /**< database error */
    RetCode_IOERR            = -103,     /**< I/O operation fail */
    RetCode_MEMERR           = -102,     /**< memory error */
    RetCode_PTHERR           = -101,     /**< pthread error */
    RetCode_SYSERR           = -100,     /**< system error */

    //generic error
    RetCode_UNVRSC           = -2,       /**< unavailable resource */
    RetCode_GENERR           = -1,       /**< generic error */

    //success, failure [0,1]
    RetCode_OK,                          /**< operation ok */
    RetCode_KO,                          /**< operation fail */
    RetCode_EXIT,                        /**< exit required */
    RetCode_RETRY,                       /**< request retry */
    RetCode_ABORT,                       /**< operation aborted */

    //generics
    RetCode_UNSP             = 100,      /**< unsupported */
    RetCode_NODATA,                      /**< no data */
    RetCode_NOTFOUND,                    /**< not found */
    RetCode_TIMEOUT,                     /**< timeout */
    RetCode_DBOPFAIL,                    /**< database operation fail */

    //contaniers specific
    RetCode_EMPTY            = 200,      /**< empty */
    RetCode_QFULL,                       /**< queue full */
    RetCode_OVRSZ,                       /**< oversize */
    RetCode_BOVFL,                       /**< buffer overflow */

    //proc. specific
    RetCode_BADARG           = 300,      /**< bad argument */
    RetCode_BADIDX,                      /**< bad index */
    RetCode_BADSTTS,                     /**< bad status */
    RetCode_BADCFG,                      /**< bad configuration */

    //network specific
    RetCode_DRPPKT           = 400,      /**< packet dropped*/
    RetCode_MALFORM,                     /**< packet malformed */
    RetCode_SCKCLO,                      /**< socket closed */
    RetCode_SCKWBLK,                     /**< socket would block */
    RetCode_SCKEAGN,                     /**< socket try again */

    //enumeration specific
    RetCode_ENMROW           = 500,      /**< enumeration row */
    RetCode_ENMEND,                      /**< enumeration end */
    RetCode_ENMBSY,                      /**< enumeration busy */
    RetCode_ENMOTH,                      /**< enumeration other */

    //database
    RetCode_DBROW            = 700,      /**< row available */
    RetCode_QRYEND,                      /**< query end */

    //authentication, authorization
    RetCode_UNAUTHEN         = 800,      /**< unauthenticated */
    RetCode_UNAUTHOR,                    /**< unauthorized */
} RetCode;

/** @brief logging system levels.
*/
typedef enum {
    TL_EVL = -1,    /**< error value*/
    TL_PLN,         /**< plain log value*/
    TL_LOW,         /**< low log value*/
    TL_TRC,         /**< trace log value*/
    TL_DBG,         /**< debug log value*/
    TL_INF,         /**< info log value*/
    TL_WRN,         /**< warning log value*/
    TL_ERR,         /**< error log value*/
    TL_CRI,         /**< critical log value*/
    TL_FAT          /**< fatal log value*/
} TraceLVL;

#if defined(__cplusplus)
}
}
#endif

#if defined(__cplusplus)
extern "C" {
namespace vlg {
#endif

/*****************************************
vlg PROTOCOL RESULT CODE
******************************************/
typedef enum  {
    //-- GENERIC ERROR CODES                            (0-99) (0x0-0x63)
    ProtocolCode_SUCCESS,                               //- 0   0x0:        SUCCESS
    ProtocolCode_UNKNOWN_ERROR,                         //- 1   0x1:        UNKNOWN ERROR
    ProtocolCode_PROTOCOL_ERROR,                        //- 2   0x2:        PROTOCOL ERROR
    ProtocolCode_SERVER_ERROR,                          //- 3   0x3:        SERVER ERROR
    ProtocolCode_APPLICATIVE_ERROR,                     //- 4   0x4:        APPLICATIVE ERROR
    ProtocolCode_MALFORMED_REQUEST,                     //- 5   0x5:        REQUEST MALFORMED
    ProtocolCode_UNSUPPORTED_REQUEST,                   //- 6   0x6:        REQUEST UNSUPPORTED
    ProtocolCode_REASON_IN_RESULT_OBJ,                  //- 7   0x7:        REASON IN RESULT OBJ
    ProtocolCode_SECURITY_ERROR,                        //- 8   0x8:        SECURITY ERROR
    ProtocolCode_UNAUTHENTICATED,                       //- 9   0x9:        UNAUTHENTICATED
    ProtocolCode_UNAUTHORIZED,                          //- 10  0xA:        UNAUTHORIZED
    ProtocolCode_SERVER_STATUS_INVALID,                 //- 11  0xB:        SERVER STATUS INVALID
    ProtocolCode_SERVER_IS_GOING_DOWN,                  //- 12  0xC:        SERVER IS GOING DOWN
    ProtocolCode_TOO_MANY_CONNECTIONS,                  //- 13  0xD:        TOO MANY CONNECTIONS
    ProtocolCode_PROTO_VERSION_TOO_OLD,                 //- 14  0xE:        PROTO VERSION TOO OLD
    ProtocolCode_APPLICATIVE_REJECT,                    //- 15  0xF         APPLICATIVE REJECT
    ProtocolCode_UNSPECIFIED,                           //- 16  0x10:       UNSPECIFIED
    ProtocolCode_UNDEFINED,                             //- 17  0x11:       UNDEFINED

    //-- CONNECTION ERROR CODES                         (100-199) (0x64-0xC7)
    ProtocolCode_ALREADY_CONNECTED = 100,               //- 100  0x64:      ALREADY CONNECTED
    ProtocolCode_SERVER_CHANGED_HEARTBEAT,              //- 101  0x65:      SERVER CHANGED HEARTBEAT
    ProtocolCode_INVALID_CONNECTION_STATUS,             //- 102  0x66:      INVALID CONNECTION STATUS
    ProtocolCode_PEER_INACTIVITY,                       //- 103  0x67:      PEER INACTIVITY

    //-- TRANSACTIONAL ERROR CODES                      (200-299) (0xC8-0x12B)
    ProtocolCode_INVALID_TRANSACTION_ID = 200,          //- 200 0xC8:       INVALID TX ID
    ProtocolCode_DUPLICATED_TRANSACTION_ID,             //- 201 0xC9:       DUP TX ID
    ProtocolCode_TRANSACTION_ALREADY_FLYING,            //- 202 0xCA:       TX ALRDY FLY
    ProtocolCode_TRANSACTION_TIMEOUT,                   //- 203 0xCB:       TX TIMEOUT
    ProtocolCode_TRANSACTION_LOST,                      //- 204 0xCC:       TX LOST
    ProtocolCode_TRANSACTION_CLIENT_ABORT,              //- 205 0xCD:       CLI ABORTED
    ProtocolCode_TRANSACTION_SERVER_ABORT,              //- 206 0xCE:       SRV ABORTED

    //-- SUBSCRIPTION ERROR CODES                       (300-399) (0x12C-0x18F)
    ProtocolCode_SUBSCRIPTION_ALREADY_STARTED = 300,    //- 400 0x190       SBS ALREADY STARTED
    ProtocolCode_SUBSCRIPTION_NOT_FOUND                 //- 401 0x191       SBS NOT FOUND
} ProtocolCode;

/*****************************************
vlg SUPPORTED TYPES // length expressed is fixed when data is transmitted.
******************************************/
typedef enum  {
    Type_UNDEFINED,
    Type_ENTITY,                    // special, user defined type
    Type_BOOL,                      // boolean 1 bit
    Type_INT16,                     // signed integer 16 bit
    Type_UINT16,                    // unsigned integer 16 bit
    Type_INT32,                     // signed integer 32 bit
    Type_UINT32,                    // unsigned integer 32 bit
    Type_INT64,                     // signed integer 64 bit
    Type_UINT64,                    // unsigned integer 16 bit
    Type_FLOAT32,                   // floating point 32 bit
    Type_FLOAT64,                   // floating point 64 bit
    Type_ASCII,                     // ASCII char 7 bit
    Type_BYTE,                      // raw byte
} Type;

const char *string_from_Type(Type bt);

/*****************************************
ENCODING TYPE
******************************************/
typedef enum  {
    Encode_UNDEFINED,
    Encode_INDEXED_NOT_ZERO,
    Encode_INDEXED_DELTA,
} Encode;

/*****************************************
ACTION TYPE
******************************************/
typedef enum  {
    Action_NONE,
    Action_INSERT,      /*INSERT*/
    Action_UPDATE,      /*UPDATE*/
    Action_DELTA,       /*DELTA*/
    Action_DELETE,      /*LOGICAL DELETE*/
    Action_REMOVE,      /*PHYSICAL DELETE*/
    Action_RESET,       /*RESET*/
} Action;

/*****************************************
PERSISTENCE
******************************************/
typedef enum  {
    PersistenceAlteringMode_UNDEFINED,
    PersistenceAlteringMode_CREATE_ONLY,
    PersistenceAlteringMode_DROP_IF_EXIST,
    PersistenceAlteringMode_CREATE_OR_UPDATE,
} PersistenceAlteringMode;

typedef enum  {
    PersistenceDeletionMode_UNDEFINED,
    PersistenceDeletionMode_LOGICAL,
    PersistenceDeletionMode_PHYSICAL,
} PersistenceDeletionMode;

typedef enum  {
    PersistenceConnectionStatus_UNDEFINED,
    PersistenceConnectionStatus_CONNECTED,
    PersistenceConnectionStatus_DISCONNECTED,
    PersistenceConnectionStatus_RELEASED,
    PersistenceConnectionStatus_ERROR = 500,
} PersistenceConnectionStatus;

typedef enum  {
    PersistenceQueryStatus_UNDEFINED,
    PersistenceQueryStatus_PREPARED,
    PersistenceQueryStatus_END,
    PersistenceQueryStatus_FAILED
} PersistenceQueryStatus;

/*****************************************
CONNECTION TYPE
******************************************/
typedef enum  {
    ConnectionType_UNDEFINED,
    ConnectionType_INGOING,
    ConnectionType_OUTGOING,
} ConnectionType;

/*****************************************
CONNECTIVITY EVT TYPE
******************************************/
typedef enum  {
    ConnectivityEventType_UNDEFINED,
    ConnectivityEventType_NETWORK,
    ConnectivityEventType_PROTOCOL,
    ConnectivityEventType_APPLICATIVE,
} ConnectivityEventType;

/*****************************************
CONNECTIVITY EVENT RESULT
******************************************/
typedef enum  {
    ConnectivityEventResult_UNDEFINED,
    ConnectivityEventResult_OK,
    ConnectivityEventResult_KO,
} ConnectivityEventResult;

/*****************************************
CONNECTION RESULT
******************************************/
typedef enum  {
    ConnectionResult_UNDEFINED,
    ConnectionResult_ACCEPTED,
    ConnectionResult_CONDITIONALLY_ACCEPTED,
    ConnectionResult_REFUSED,
} ConnectionResult;

/*****************************************
CONNECTION STATUS
******************************************/
typedef enum  {
    ConnectionStatus_UNDEFINED,
    ConnectionStatus_INITIALIZED,
    ConnectionStatus_DISCONNECTED,
    ConnectionStatus_ESTABLISHED,          // tcp/ip con. is established
    ConnectionStatus_PROTOCOL_HANDSHAKE,   // protocol handshake completed
    ConnectionStatus_AUTHENTICATED,
    ConnectionStatus_DISCONNECTING,
    ConnectionStatus_SOCKET_ERROR = 300,
    ConnectionStatus_PROTOCOL_ERROR,
    ConnectionStatus_ERROR = 500,
} ConnectionStatus;

/*****************************************
TRANSACTION REQUEST TYPE
******************************************/
typedef enum  {
    TransactionRequestType_UNDEFINED,
    TransactionRequestType_RESERVED,
    TransactionRequestType_SYSTEM,
    TransactionRequestType_SPECIAL,
    TransactionRequestType_OBJECT,
} TransactionRequestType;

/*****************************************
TRANSACTION RESPONSE
******************************************/
typedef enum  {
    TransactionResult_UNDEFINED,
    TransactionResult_COMMITTED,
    TransactionResult_FAILED,
    TransactionResult_ABORTED,
} TransactionResult;

/*****************************************
TRANSACTION STATUS
******************************************/
typedef enum  {
    TransactionStatus_UNDEFINED,
    TransactionStatus_EARLY,
    TransactionStatus_INITIALIZED,
    TransactionStatus_FLYING,
    TransactionStatus_CLOSED,
    TransactionStatus_ERROR = 500,
} TransactionStatus;

/*****************************************
vlg SUBSCRIPTION TYPE
******************************************/
typedef enum  {
    SubscriptionType_UNDEFINED,
    SubscriptionType_SNAPSHOT,
    SubscriptionType_INCREMENTAL,
} SubscriptionType;

/*****************************************
vlg SUBSCRIPTION MODE
******************************************/
typedef enum  {
    SubscriptionMode_UNDEFINED,
    SubscriptionMode_ALL,      //to receive both dwnl-evts and live-evts.
    SubscriptionMode_DOWNLOAD, //to receive only dwnl-evts.
    SubscriptionMode_LIVE,     //to receive only live-evts.
} SubscriptionMode;

/*****************************************
vlg SUBSCRIPTION FLOW TYPE
******************************************/
typedef enum  {
    SubscriptionFlowType_UNDEFINED,
    SubscriptionFlowType_ALL,         //server won't do snapshotting.
    SubscriptionFlowType_LAST,        //server will do snapshotting.
} SubscriptionFlowType;

/*****************************************
vlg SUBSCRIPTION DOWNLOAD TYPE
******************************************/
typedef enum  {
    SubscriptionDownloadType_UNDEFINED,
    SubscriptionDownloadType_ALL,
    SubscriptionDownloadType_PARTIAL,
} SubscriptionDownloadType;

/*****************************************
SUBSCRIPTION RESPONSE
******************************************/
typedef enum  {
    SubscriptionResponse_UNDEFINED,
    SubscriptionResponse_OK,
    SubscriptionResponse_PARTIAL,
    SubscriptionResponse_KO,
} SubscriptionResponse;

/*****************************************
SUBSCRIPTION EVENT TYPE
******************************************/
typedef enum  {
    SubscriptionEventType_UNDEFINED,
    SubscriptionEventType_LIVE,           //class instance live-event.
    SubscriptionEventType_DOWNLOAD,       //class instance download-phase
    SubscriptionEventType_DOWNLOAD_END,
    SubscriptionEventType_STOP,
} SubscriptionEventType;

/*****************************************
SUBSCRIPTION STATUS
******************************************/
typedef enum  {
    SubscriptionStatus_UNDEFINED,
    SubscriptionStatus_EARLY,
    SubscriptionStatus_INITIALIZED,
    SubscriptionStatus_STOPPED,
    SubscriptionStatus_REQUEST_SENT,
    SubscriptionStatus_STARTED,
    SubscriptionStatus_RELEASED,
    SubscriptionStatus_ERROR = 500,
} SubscriptionStatus;

/*****************************************
PEER PERSONALITY
******************************************/
typedef enum  {
    PeerPersonality_BOTH,
    PeerPersonality_PURE_SERVER,
    PeerPersonality_PURE_CLIENT,
} PeerPersonality;

/*****************************************
PEER STATUS
******************************************/
typedef enum  {
    PeerStatus_ZERO,               //0
    PeerStatus_EARLY,              //1
    PeerStatus_WELCOMED,           //2
    PeerStatus_INITIALIZING,       //3

    /**
    it means that caller thread has statically initialized this peer.
    */
    PeerStatus_INITIALIZED,        //4

    /**
    convenience state used when a peer has been stopped and then restarted.
    */
    PeerStatus_RESTART_REQUESTED,  //5

    PeerStatus_STARTING,           //6

    /**
    it means that this peer has eventually started other threads
    and they are ready.
    */
    PeerStatus_STARTED,            //7

    /**
    it means that this peer is running.
    */
    PeerStatus_RUNNING,            //8

    PeerStatus_STOP_REQUESTED,     //9
    PeerStatus_STOPPING,           //10
    PeerStatus_STOPPED,            //11 --stop state.
    PeerStatus_DIED,               //12 --final state.
    PeerStatus_ERROR = 500,
} PeerStatus;

/*****************************************
MODEL RELATED
******************************************/

typedef enum  {
    NEntityType_UNDEFINED,
    NEntityType_NENUM,
    NEntityType_NCLASS,
} NEntityType;

const char *string_from_NEntityType(NEntityType bet);

typedef enum  {
    MemberType_UNDEFINED,
    MemberType_FIELD,
    MemberType_NENUM_VALUE,
} MemberType;

const char *string_from_MemberType(MemberType bmt);

typedef enum  {
    PrintMode_UNDEFINED,
    PrintMode_NOT_ZERO,    //only not zero fields
    PrintMode_ALL          //all fields
} PrintMode;

/** @brief tx_id represent a transaction-id for transaction objects.
*/
#if defined(__cplusplus)
struct tx_id {
    explicit tx_id() {
        memset(this, 0, sizeof(tx_id));
    }
#else
typedef struct {
#endif
    unsigned int txplid;
    unsigned int txsvid;
    unsigned int txcnid;
    unsigned int txprid;
#if defined(__cplusplus)
};
#else
} tx_id;
#endif

#if defined(__cplusplus)
struct nclass;
struct nentity_desc;
struct nentity_manager;
struct peer;
struct peer_impl;
struct outgoing_connection;
struct incoming_connection;
struct conn_impl;
struct outgoing_connection_impl;
struct incoming_connection_impl;
struct incoming_connection_factory;
struct outgoing_transaction;
struct incoming_transaction;
struct tx_impl;
struct outgoing_transaction_impl;
struct incoming_transaction_impl;
struct incoming_transaction_factory;
struct outgoing_subscription;
struct incoming_subscription;
struct sbs_impl;
struct outgoing_subscription_impl;
struct incoming_subscription_impl;
struct subscription_event;
struct subscription_event_impl;
struct incoming_subscription_factory;
struct persistence_driver;
struct persistence_query_impl;
struct persistence_manager_impl;
struct persistence_task;
struct persistence_connection_pool;
struct persistence_worker;
struct g_bbuf;
#else
typedef struct nclass nclass;
typedef struct shr_nclass shr_nclass;
typedef struct own_nclass own_nclass;
typedef struct key_desc key_desc;
typedef struct member_desc member_desc;
typedef struct nentity_desc nentity_desc;
typedef struct nentity_manager nentity_manager;
typedef struct peer peer;
typedef struct own_peer own_peer;
typedef struct outgoing_connection outgoing_connection;
typedef struct own_outgoing_connection own_outgoing_connection;
typedef struct incoming_connection incoming_connection;
typedef struct shr_incoming_connection shr_incoming_connection;
typedef struct outgoing_transaction outgoing_transaction;
typedef struct own_outgoing_transaction own_outgoing_transaction;
typedef struct incoming_transaction incoming_transaction;
typedef struct shr_incoming_transaction shr_incoming_transaction;
typedef struct outgoing_subscription outgoing_subscription;
typedef struct own_outgoing_subscription own_outgoing_subscription;
typedef struct incoming_subscription incoming_subscription;
typedef struct shr_incoming_subscription shr_incoming_subscription;
typedef struct subscription_event subscription_event;
typedef struct persistence_manager persistence_manager;
typedef struct persistence_driver persistence_driver;
typedef struct persistence_connection persistence_connection;
typedef struct persistence_query persistence_query;
#endif

#if defined(__cplusplus)
}
}
#endif

#if defined(__cplusplus)
extern "C" {
namespace vlg {
#endif

typedef nclass *(*nclass_alloc)();

#if defined(__cplusplus)
}
}
#endif

#endif
