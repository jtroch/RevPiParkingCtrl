#include "ThreadSynchronization.hpp"
#include <Logger.hpp>
#include <piControlIf.hpp>
#include <piControl.h>
#include <stdio.h>

IOHandler::IOHandler() {};

uint32_t IOHandler::GetIO(char* pszVariableName) {
    int rc=-1;
    SPIVariable sPiVariable;
    SPIValue sPIValue;
    uint8_t i8uValue;
    uint16_t i16uValue;
    uint32_t i32uValue;

    strncpy(sPiVariable.strVarName, pszVariableName, sizeof(sPiVariable.strVarName));
    
    ThreadSynchronization.LockIO();

    rc = piControlGetVariableInfo(&sPiVariable);
    if (rc < 0) {
        printf("Cannot find variable '%s'\n", pszVariableName);
        return -1;
    }
    sPIValue.i16uAddress = sPiVariable.i16uAddress;
    sPIValue.i8uBit = sPiVariable.i8uBit;

    rc=-1;
    while (rc < 0) {
        switch (sPiVariable.i16uLength) {
            case 1: 
                rc = piControlGetBitValue(&sPIValue);
                printf("Get bit value: %d\n", sPIValue.i8uValue);
                break;
            case 8: 
                rc = piControlRead(sPiVariable.i16uAddress, 1, (uint8_t*)&i8uValue);
                printf("Get u8 value: %d\n", sPIValue.i8uValue);
                break;
            case 16:
                rc = piControlRead(sPiVariable.i16uAddress, 2, (uint8_t*)&i16uValue);
                printf("Value of %s: %04x hex (=%d dez)\n", pszVariableName, i16uValue, i16uValue);
                break;
            case 32:
                rc = piControlRead(sPiVariable.i16uAddress, 4, (uint8_t*)&i32uValue);
                printf("Value of %s: %08x hex (=%d dez)\n", pszVariableName, i32uValue, i32uValue);
                break;
        }
        sleep(1);
    }

    ThreadSynchronization.UnlockIO();
}

int IOHandler::SetIO(char* pszVariableName, uint32_t value) {
    int rc=-1;
    SPIVariable sPiVariable;
    SPIValue sPIValue;
    uint8_t i8uValue;
    uint16_t i16uValue;
   
    ThreadSynchronization.LockIO();

    rc = piControlGetVariableInfo(&sPiVariable);
    if (rc < 0) {
        printf("Cannot find variable '%s'\n", pszVariableName);
        return -1;
    }
    sPIValue.i16uAddress = sPiVariable.i16uAddress;
    sPIValue.i8uBit = sPiVariable.i8uBit;
    sPIValue.i8uValue = value;

    switch (sPiVariable.i16uLength) {
        case 1: 
            rc = piControlWrite(sPiVariable.i16uAddress, 1, (uint8_t *)&value);
            if (rc > 0) printf("Set bit %d on byte at offset %d. Value %d\n", sPIValue.i8uBit, sPIValue.i16uAddress, sPIValue.i8uValue);
        case 8: 
            rc = piControlWrite(sPiVariable.i16uAddress, 1, (uint8_t *)&value);
            if (rc > 0) printf("Write value %d dez (=%02x hex) to offset %d.\n", value, value, sPiVariable.i16uAddress);
            break;
        case 16:
            rc = piControlWrite(sPiVariable.i16uAddress, 2, (uint8_t *)&value);
            if (rc > 0) printf("Write value %d dez (=%02x hex) to offset %d.\n", value, value, sPiVariable.i16uAddress);
            break;
        case 32:
            rc = piControlWrite(sPiVariable.i16uAddress, 4, (uint8_t *)&value);
            if (rc > 0) printf("Write value %d dez (=%04x hex) to offset %d.\n", value, value, sPiVariable.i16uAddress);
            break;
    }
    if (rc < 0)  {
        printf("Write error %s\n", getWriteError(rc));
        return -1;
    } return 1;
}

IOHandler::IOHandler getInstance() {
    ThreadSynchronization.LockIO();
        if (instance == null) {
            instance = new IOHandler();
        }
    ThreadSynchronization.UnlockIO();
    return instance;
}
    