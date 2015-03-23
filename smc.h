#include <stdint.h>
#include <IOKit/IOKitLib.h>

#define APPLE_SMC_SERVICE_NAME "AppleSMC"

extern io_connect_t connect_port;

typedef struct {
  unsigned int actualRpm;
  unsigned int minimumRpm;
  unsigned int maximumRpm;
  unsigned int safeRpm;
} fan_rpm_t;

typedef enum {
  LEFT_FAN  = 0,
  RIGHT_FAN = 1
} fan_number_t;

typedef enum {
  kSMCSuccess     = 0,
  kSMCError       = 1,
  kSMCKeyNotFound = 0x84
} kSMC_t;

typedef enum {
  kSMCUserClientOpen  = 0,
  kSMCUserClientClose = 1,
  kSMCHandleYPCEvent  = 2,
  kSMCReadKey         = 5,
  kSMCWriteKey        = 6,
  kSMCGetKeyCount     = 7,
  kSMCGetKeyFromIndex = 8,
  kSMCGetKeyInfo      = 9
} selector_t;

typedef struct {
  unsigned char  major;
  unsigned char  minor;
  unsigned char  build;
  unsigned char  reserved;
  unsigned short release;
} SMCVersion;

typedef struct {
  uint16_t version;
  uint16_t length;
  uint32_t cpuPLimit;
  uint32_t gpuPLimit;
  uint32_t memPLimit;
} SMCPLimitData;

typedef struct {
  IOByteCount dataSize;
  uint32_t    dataType;
  uint8_t     dataAttributes;
} SMCKeyInfoData;

typedef struct {
  uint32_t       key;
  SMCVersion     vers;
  SMCPLimitData  pLimitData;
  SMCKeyInfoData keyInfo;
  uint8_t        result;
  uint8_t        status;
  uint8_t        data8;
  uint32_t       data32;
  uint8_t        bytes[32];
} SMCParamStruct;

typedef struct {
  uint8_t  data[32];
  uint32_t dataType;
  uint32_t dataSize;
  kSMC_t   kSMC;
} smc_return_t;

uint32_t to_uint32_t(const char *key);
unsigned int from_fpe2(uint8_t data[32]);

kern_return_t open_smc(void);
kern_return_t close_smc(void);

kern_return_t call_smc(SMCParamStruct *input, SMCParamStruct *output);
kern_return_t read_smc(const char *key, smc_return_t *result);

kern_return_t smc_get_key_info();
kern_return_t smc_read_key();

fan_rpm_t get_fan_rpm(fan_number_t number);
void redraw_rpm_info(fan_rpm_t lfan_result, fan_rpm_t rfan_result);
