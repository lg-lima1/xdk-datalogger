/**
 * @ingroup APPS_LIST
 *
 * @defgroup SD_CARD_EXAMPLE SdCardExample
 * @{
 *
 * @brief Demo application shows how to write a file to the SD card and later read back the file.
 *
 * @details The example will read the presence of an SD Card in the SD Card slot and turn on the Red LED if one is present. The device will then
 * create and write to the text file TEST_XDK.TXT the text. This content is created by SD card Read/Write
 * functionality using FAT32 file system. The will continue to write the SD Card again and
 * again. 
 *
 * @file
 **/

/* module includes ********************************************************** */

/* own header files */
#include "XdkAppInfo.h"
#undef BCDS_MODULE_ID  /* Module ID define before including Basics package*/
#define BCDS_MODULE_ID XDK_APP_MODULE_ID_APP_CONTROLLER

/* own header files */
#include "AppController.h"

/* system header files */
#include <stdio.h>

/* additional interface header files */
#include "XDK_Storage.h"
#include "XDK_LED.h"
#include "XDK_Sensor.h"
#include "BCDS_SDCard_Driver.h"
#include "BCDS_CmdProcessor.h"
#include "XDK_Utils.h"
#include "BSP_BoardType.h"
#include "BCDS_Assert.h"
#include "BatteryMonitor.h"
#include <FreeRTOS.h>
#include <timers.h>
/* constant definitions ***************************************************** */
/* Ram buffers
 * BUFFERSIZE should be between 512 and 1024, depending on available ram on efm32
 */
#define BUFFER_SIZE                 			UINT16_C(512)
#define SINGLE_BLOCK                			UINT8_C(1)      /**< SD- Card Single block write or read */
#define DRIVE_ZERO                  			UINT8_C(0)      /**< SD Card Drive 0 location */
#define SECTOR_VALUE                			UINT8_C(6)      /**< SDC Disk sector value */
#define SINGLE_SECTOR_LEN           			UINT32_C(512)   /**< Single sector size in SDcard */
#define APP_TEMPERATURE_OFFSET_CORRECTION       (-3459)

/* local variables ********************************************************** */
/* local module global variable declarations */
static Storage_Setup_T StorageSetupInfo =
{
	.SDCard = true,
	.WiFiFileSystem = false
};/**< Storage setup parameters */

static Sensor_Setup_T SensorSetup =
{
	.CmdProcessorHandle = NULL,
	.Enable =
	{
		.Accel = true,
		.Mag = false,
		.Gyro = false,
		.Humidity = true,
		.Temp = true,
		.Pressure = true,
		.Light = true,
		.Noise = false,
	},
	.Config =
	{
		.Accel =
		{
			.Type = SENSOR_ACCEL_BMA280,
			.IsRawData = false,
			.IsInteruptEnabled = false,
			.Callback = NULL,
		},
		.Gyro =
		{
			.Type = SENSOR_GYRO_BMG160,
			.IsRawData = false,
		},
		.Mag =
		{
			.IsRawData = false
		},
		.Light =
		{
			.IsInteruptEnabled = false,
			.Callback = NULL,
		},
		.Temp =
		{
			.OffsetCorrection = APP_TEMPERATURE_OFFSET_CORRECTION,
		},
	},
};/**< Sensor setup parameters */

static CmdProcessor_T * AppCmdProcessor;/**< Handle to store the main Command processor handle to be used by run-time event driven threads */

static xTaskHandle AppControllerHandle = NULL;/**< OS thread handle for Application controller to be used by run-time blocking threads */

/* global variables ********************************************************* */

/* inline functions ********************************************************* */

/* local functions ********************************************************** */

static Retcode_T GetEndOfFileIndex(uint32_t *index)
{
	Retcode_T retcode = RETCODE_OK;

	const uint16_t READ_BUFFER_SIZE = 16;
    uint8_t ramBufferRead[READ_BUFFER_SIZE]; /* Temporary buffer for read file */

    Storage_Read_T readCredentials =
	{
		.FileName = "index.xdk",
		.ReadBuffer = ramBufferRead,
		.BytesToRead = READ_BUFFER_SIZE,
		.ActualBytesRead = 0UL,
		.Offset = 0UL,
	};

	retcode = Storage_Read(STORAGE_MEDIUM_SD_CARD, &readCredentials);
	if (RETCODE_OK == retcode)
	{
		uint8_t readBuffer[readCredentials.ActualBytesRead];
		for (uint16_t i = 0; i < (readCredentials.ActualBytesRead - 1); i++)
		{
			if (ramBufferRead[i] == '\n') break;
			readBuffer[i] = ramBufferRead[i];
		}
		uint32_t iindex = 0;
		sscanf(readBuffer, "%ld", &iindex);
		*index = iindex;
	}

	return (retcode);
}

static Retcode_T SetEndOfFileIndex(uint32_t index)
{
	Retcode_T retcode = RETCODE_OK;

	const uint16_t WRITE_BUFFER_SIZE = 16;

    char publishBuffer[WRITE_BUFFER_SIZE];
	const char *publishDataFormat = "%ld\r\n";

	int32_t length = snprintf(
						(char *) publishBuffer,
						WRITE_BUFFER_SIZE,
						publishDataFormat,
						(long int) index);

    uint8_t ramBufferWrite[WRITE_BUFFER_SIZE]; /* Temporary buffer for write file */

    for (int32_t index = 0; index < length; index++)
    {
        ramBufferWrite[index] = publishBuffer[index];
    }

    Storage_Write_T writeCredentials =
            {
                    .FileName = "index.xdk",
                    .WriteBuffer = ramBufferWrite,
                    .BytesToWrite = length,
                    .ActualBytesWritten = 0UL,
                    .Offset = 0UL,
            };

    if (RETCODE_OK == retcode)
    {
    	retcode = Storage_Write(STORAGE_MEDIUM_SD_CARD, &writeCredentials);
    }

    return (retcode);
}

static Retcode_T SensorDataWrite(Sensor_Value_T *sensor_value, uint32_t battery_voltage, uint32_t *offset, uint32_t cycle)
{
    char publishBuffer[BUFFER_SIZE];
	const char *publishDataFormat = "%ld; %ld; %ld; %ld; %ld; %ld; %.3f; %.3f; %.3f\r\n";

	int32_t length = snprintf(
						(char *) publishBuffer,
						BUFFER_SIZE,
						publishDataFormat,
						(long int) cycle * WRITEREAD_DELAY,
						(long int) sensor_value->Accel.X,
						(long int) sensor_value->Accel.Y,
						(long int) sensor_value->Accel.Z,
						(long int) sensor_value->RH,
						(long int) sensor_value->Pressure,
						(sensor_value->Temp / 1000),
						(sensor_value->Light / 1000.0),
						(battery_voltage / 1000.0));

	printf("[SD CARD] Write Buffer: %s", publishBuffer);

    uint8_t ramBufferWrite[BUFFER_SIZE]; /* Temporary buffer for write file */
    uint32_t writeOffset = *offset;

    for (int32_t index = 0; index < length; index++)
    {
        ramBufferWrite[index] = publishBuffer[index];
    }

    Storage_Write_T writeCredentials =
            {
                    .FileName = "data.csv",
                    .WriteBuffer = ramBufferWrite,
                    .BytesToWrite = length,
                    .ActualBytesWritten = 0UL,
                    .Offset = writeOffset,
            };

    Retcode_T retcode = RETCODE_OK;
    if (RETCODE_OK == retcode)
    {
    	retcode = Storage_Write(STORAGE_MEDIUM_SD_CARD, &writeCredentials);
    }

    if (RETCODE_OK == retcode)
    {
		writeOffset = writeOffset + writeCredentials.ActualBytesWritten;
		*offset = writeOffset;
    }

    return (retcode);
}

/**
 * @brief Responsible for controlling the SD card example flow
 *
 * - Check whether the storage medium (SD Card)is available or not
 * - if FAT_FILE_SYSTEM==1 then will write and read files and it will compare the contents which has been
 *   written and read.
 * - if FAT_FILE_SYSTEM==0 then it will write some string and read the string.
 *   then validate the written and read string.
 *
 * @param[in] pvParameters
 * Unused
 */
static void AppControllerFire(void* pvParameters)
{
    BCDS_UNUSED(pvParameters);
    Retcode_T retcode = RETCODE_OK;
    Sensor_Value_T sensorValue;
    uint32_t batteryValue;
    uint32_t cycleNum = 1;
    bool sdcardEject = false, status = false, sdcardInsert = false;

    memset(&sensorValue, 0x00, sizeof(sensorValue));

    static uint32_t eof_index = 0;

    while (1)
    {
        retcode = Storage_IsAvailable(STORAGE_MEDIUM_SD_CARD, &status);
        if ((RETCODE_OK == retcode) && (true == status))
        {
            sdcardInsert = true;
            LED_On(LED_INBUILT_RED);

            if (sdcardEject == true)
            {
                retcode = Storage_Disable(STORAGE_MEDIUM_SD_CARD);
                if (RETCODE_OK == retcode)
                {
                    retcode = Storage_Enable();
                }
                if (RETCODE_OK == retcode)
                {
                    sdcardEject = false;
                }
            }

            if (RETCODE_OK == retcode)
            {
            	retcode = GetEndOfFileIndex(&eof_index); /* Get index position on auxiliary file */
            }

            if (RETCODE_OK == retcode)
            {
                retcode = Sensor_GetData(&sensorValue);
            }

    		if (RETCODE_OK == retcode)
    		{
    			retcode = BatteryMonitor_MeasureSignal(&batteryValue);
    		}

            if (RETCODE_OK == retcode)
            {
                retcode = SensorDataWrite(&sensorValue, batteryValue, &eof_index, cycleNum++);
                if (RETCODE_OK == retcode)
                {
                	SetEndOfFileIndex(eof_index); /* Set index position on auxiliary file */
                	printf("[SD CARD] Write succesful!\n");
                }
            }
        }
        else
        {
            if (Retcode_GetCode(retcode) == (Retcode_T) RETCODE_STORAGE_SDCARD_UNINITIALIZED)
            {
                printf("[SD CARD] Not inserted in XDK\n\r");
                retcode = Storage_Enable();
            }
            else
            {
                if (true == sdcardInsert)
                {
                    sdcardEject = true;
                }

                sdcardInsert = false;
                printf("[SD CARD] Removed from XDK\n\r");
                LED_Off(LED_INBUILT_RED);
            }
        }
        if (RETCODE_OK != retcode)
        {
            Retcode_RaiseError(retcode);
        }
        vTaskDelay(pdMS_TO_TICKS(WRITEREAD_DELAY));
    }
}

/**
 * @brief To enable the necessary modules for the application
 * - SD Card
 * - LED
 *
 * @param[in] param1
 * Unused
 *
 * @param[in] param2
 * Unused
 */
static void AppControllerEnable(void * param1, uint32_t param2)
{
    BCDS_UNUSED(param1);
    BCDS_UNUSED(param2);

    Retcode_T retcode = RETCODE_OK;

    retcode = Storage_Enable();

    if (RETCODE_OK == retcode)
    {
        retcode = LED_Enable();
    }

    if (RETCODE_OK == retcode)
    {
        retcode = Sensor_Enable();
    }

    if (RETCODE_OK == retcode)
    {
        if (pdPASS != xTaskCreate(AppControllerFire, (const char * const ) "AppController", TASK_STACK_SIZE_APP_CONTROLLER, NULL, TASK_PRIO_APP_CONTROLLER, &AppControllerHandle))
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_OUT_OF_RESOURCES);
        }
    }

    if (RETCODE_OK != retcode)
    {
        printf("AppControllerEnable : Failed \r\n");
        Retcode_RaiseError(retcode);
        assert(0); /* To provide LED indication for the user */
    }

    Utils_PrintResetCause();
}

/**
 * @brief To setup the necessary modules for the application
 * - SD Card
 * - LED
 *
 * @param[in] param1
 * Unused
 *
 * @param[in] param2
 * Unused
 */
static void AppControllerSetup(void * param1, uint32_t param2)
{
    BCDS_UNUSED(param1);
    BCDS_UNUSED(param2);

    Retcode_T retcode = RETCODE_OK;
    retcode = Storage_Setup(&StorageSetupInfo);
    if (RETCODE_OK == retcode)
    {
        retcode = LED_Setup();
    }
    if (RETCODE_OK == retcode)
    {
        SensorSetup.CmdProcessorHandle = AppCmdProcessor;
        retcode = Sensor_Setup(&SensorSetup);
    }
    if (RETCODE_OK == retcode)
    {
    	retcode = BatteryMonitor_Init();
    }
    if (RETCODE_OK == retcode)
    {
        retcode = CmdProcessor_Enqueue(AppCmdProcessor, AppControllerEnable, NULL, UINT32_C(0));
    }
    if (RETCODE_OK != retcode)
    {
        printf("AppControllerSetup : Failed \r\n");
        Retcode_RaiseError(retcode);
        assert(0); /* To provide LED indication for the user */
    }
}

/* global functions ********************************************************* */

/** Refer interface header for description */
void AppController_Init(void * cmdProcessorHandle, uint32_t param2)
{
    BCDS_UNUSED(param2);

    Retcode_T retcode = RETCODE_OK;

    if (cmdProcessorHandle == NULL)
    {
        printf("AppController_Init : Command processor handle is NULL \r\n");
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
    }
    else
    {
        AppCmdProcessor = (CmdProcessor_T *) cmdProcessorHandle;
        retcode = CmdProcessor_Enqueue(AppCmdProcessor, AppControllerSetup, NULL, UINT32_C(0));
    }

    if (RETCODE_OK != retcode)
    {
        Retcode_RaiseError(retcode);
        assert(0); /* To provide LED indication for the user */
    }
}

/**@} */
/** ************************************************************************* */
