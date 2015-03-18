#include <string.h>
#include <stdio.h>
#include "smc.h"

io_connect_t connect_port;

uint32_t to_uint32_t(const char *key) {
  uint32_t ans = 0;
  uint32_t shift = 24;

  if (strlen(key) != 4) {
    return 0;
  }

  for (int i = 0; i < 4; i++) {
    ans += key[i] << shift;
    shift -= 8;
  }

  return ans;
}

unsigned int from_fpe2(uint8_t data[32]) {
  unsigned int result = 0;
  result += data[0] << 6;
  result += data[1] << 2;
  return result;
}

kern_return_t open_smc(void) {
  io_service_t smc_service;
  CFMutableDictionaryRef match_dictionary;
  kern_return_t kern_result;

  match_dictionary = IOServiceMatching(APPLE_SMC_SERVICE_NAME);
  smc_service = IOServiceGetMatchingService(kIOMasterPortDefault, match_dictionary);

  if (smc_service == 0) {
    return kIOReturnError;
  }

  kern_result = IOServiceOpen(smc_service, mach_task_self(), 0, &connect_port);

  if (kern_result != kIOReturnSuccess) {
    return kIOReturnError;
  }

  IOObjectRelease(smc_service);
  return kern_result;
}

kern_return_t close_smc(void) {
  kern_return_t kern_result;
  kern_result = IOServiceClose(connect_port);
  return kern_result;
}

kern_return_t read_smc(const char *key, smc_return_t *smc_return) {
  SMCParamStruct inputParam;
  SMCParamStruct outputParam;
  kern_return_t kern_result;

  memset(&inputParam, 0, sizeof(SMCParamStruct));
  memset(&outputParam, 0, sizeof(SMCParamStruct));
  memset(smc_return, 0, sizeof(smc_return_t));

  kern_result = smc_get_key_info(key, &inputParam, &outputParam, smc_return);
  if (kern_result != kIOReturnSuccess || outputParam.result != kSMCSuccess) {
    return kern_result;
  }

  kern_result = smc_read_key(&inputParam, &outputParam, smc_return);
  if (kern_result != kIOReturnSuccess || outputParam.result != kSMCSuccess) {
    return kern_result;
  }

  return kern_result;
}

kern_return_t smc_get_key_info(const char *key, SMCParamStruct *inputParam, SMCParamStruct *outputParam, smc_return_t *smc_return) {
  kern_return_t kern_return;

  inputParam->key = to_uint32_t(key);
  inputParam->data8 = kSMCGetKeyInfo;

  kern_return = call_smc(inputParam, outputParam);

  smc_return->kSMC = outputParam->result;
  smc_return->dataSize = outputParam->keyInfo.dataSize;
  smc_return->dataType = outputParam->keyInfo.dataType;

  return kern_return;
}

kern_return_t smc_read_key(SMCParamStruct *inputParam, SMCParamStruct *outputParam, smc_return_t *smc_return) {
  kern_return_t kern_return;

  inputParam->keyInfo.dataSize = outputParam->keyInfo.dataSize;
  inputParam->data8 = kSMCReadKey;

  kern_return = call_smc(inputParam, outputParam);
  memcpy(smc_return->data, outputParam->bytes, sizeof(outputParam->bytes));

  return kern_return;
}

kern_return_t call_smc(SMCParamStruct *input, SMCParamStruct *output) {
  kern_return_t kern_result;

  size_t inputSize = sizeof(SMCParamStruct);
  size_t outputSize = sizeof(SMCParamStruct);

  kern_result = IOConnectCallStructMethod(connect_port, kSMCHandleYPCEvent, input, inputSize, output, &outputSize);
  return kern_result;
}

fan_rpm_t get_fan_rpm(fan_number_t number) {
  char key[5];

  kern_return_t kern_return;
  smc_return_t smc_return;
  fan_rpm_t fan_rpm_result;

  sprintf(key, "F%dAc", number);
  kern_return = read_smc(key, &smc_return);
  if (kern_return != kIOReturnSuccess && smc_return.dataSize != 2)
    perror("Error while reading smc key");
  fan_rpm_result.actualRpm = from_fpe2(smc_return.data);

  sprintf(key, "F%dMn", number);
  kern_return = read_smc(key, &smc_return);
  if (kern_return != kIOReturnSuccess && smc_return.dataSize != 2)
    perror("Error while reading smc key");
  fan_rpm_result.minimumRpm = from_fpe2(smc_return.data);

  sprintf(key, "F%dMx", number);
  kern_return = read_smc(key, &smc_return);
  if (kern_return != kIOReturnSuccess && smc_return.dataSize != 2)
    perror("Error while reading smc key");
  fan_rpm_result.maximumRpm = from_fpe2(smc_return.data);

  sprintf(key, "F%dSf", number);
  kern_return = read_smc(key, &smc_return);
  if (kern_return != kIOReturnSuccess && smc_return.dataSize != 2)
    perror("Error while reading smc key");
  fan_rpm_result.safeRpm = from_fpe2(smc_return.data);

  return fan_rpm_result;
}
