/**
 * @file
 * @brief This File has the Module IDs for the Application C modules
 *
 **/
#ifndef XDK_APPINFO_H_
#define XDK_APPINFO_H_

/* own header files*/
#include "XdkCommonInfo.h"
#include "BCDS_Retcode.h"

/**< Main command processor task priority */
#define TASK_PRIO_MAIN_CMD_PROCESSOR                (UINT32_C(3))
/**< Main command processor task stack size */
#define TASK_STACK_SIZE_MAIN_CMD_PROCESSOR          (UINT32_C(1200))
/**< Main command processor task queue length */
#define TASK_Q_LEN_MAIN_CMD_PROCESSOR               (UINT32_C(10))

/**< Application controller task priority */
#define TASK_PRIO_APP_CONTROLLER                    (UINT32_C(3))
/**< Application controller task stack size */
#define TASK_STACK_SIZE_APP_CONTROLLER              (UINT32_C(1200))

/**
 * @brief BCDS_APP_MODULE_ID for Application C module of XDK
 * @info  usage:
 *      #undef BCDS_APP_MODULE_ID
 *      #define BCDS_APP_MODULE_ID BCDS_APP_MODULE_ID_xxx
 */
enum XDK_App_ModuleID_E
{
    XDK_APP_MODULE_ID_MAIN = XDK_COMMON_ID_OVERFLOW,
    XDK_APP_MODULE_ID_APP_CONTROLLER,

/* Define next module ID here */
};

enum XDK_App_Retcode_E
{
    SDCARD_APP_ERROR = RETCODE_XDK_APP_FIRST_CUSTOM_CODE,
    FILE_READ_ERROR,
    FILE_WRITE_ERROR,
    FILE_OPEN_ERROR,
    FILE_LSEEK_ERROR,
    FILE_CLOSE_ERROR,
    SDCARD_INIT_FAILED
};

#endif /* XDK_APPINFO_H_ */
