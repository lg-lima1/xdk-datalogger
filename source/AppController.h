/* header definition ******************************************************** */
#ifndef APPCONTROLLER_H_
#define APPCONTROLLER_H_

/* local interface declaration ********************************************** */
#include "XDK_Utils.h"

/* local type and macro definitions */
#define FAT_FILE_SYSTEM             1 /** Macro to write data into SDCard either through FAT file system or SingleBlockWriteRead depends on the value **/
#define WRITEREAD_DELAY             UINT32_C(1000)   /**< Millisecond delay for WriteRead timer task */
/* local function prototype declarations */

/* local inline function definitions */
/**
 * @brief This is a template function where the user can write his custom application.
 *
 * @param[in] CmdProcessorHandle Handle of the main command processor
 *
 * @param[in] param2  Currently not used will be used in future
 *
 */
void AppController_Init(void * CmdProcessorHandle, uint32_t param2);

#endif /* APPCONTROLLER_H_ */

/** ************************************************************************* */
