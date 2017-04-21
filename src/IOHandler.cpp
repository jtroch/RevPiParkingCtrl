#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>


#include "ThreadSynchronization.hpp"
#include "IOHandler.hpp"
#include <piControlIf.hpp>
#include <piControl.h>

IOHandler* IOHandler::instance = NULL;

int IOHandler::GetIO(const char* pszVariableName) {
    int rc=-1;
    SPIVariable sPiVariable;
    SPIValue sPIValue;
    uint8_t i8uValue;
    uint16_t i16uValue;
    uint32_t i32uValue;

    strncpy(sPiVariable.strVarName, pszVariableName, sizeof(sPiVariable.strVarName));
    
    ThreadSynchronization::getInstance()->LockIO();
    rc = piCtrl.GetVariableInfo(&sPiVariable);
    
    if (rc < 0) {
        syslog(LOG_ERR, "IOHandler: Cannot find variable '%s'\n", pszVariableName);
        ThreadSynchronization::getInstance()->UnlockIO();
        return -1;
    } else {
        syslog(LOG_DEBUG, "IOHandler: GetIO(%s) [offset=%d length=%d bit=%d]\n", sPiVariable.strVarName, sPiVariable.i16uAddress, sPiVariable.i16uLength, sPiVariable.i8uBit);
    }

    sPIValue.i16uAddress = sPiVariable.i16uAddress;
    sPIValue.i8uBit = sPiVariable.i8uBit;

    rc=-1;
    while (rc < 0) {
        switch (sPiVariable.i16uLength) {
            case 1: 
                rc = piCtrl.GetBitValue(&sPIValue);
                //syslog(LOG_DEBUG, "IOHandler: Get bit value: %d\n", sPIValue.i8uValue);
                break;
            case 8: 
                rc = piCtrl.Read(sPiVariable.i16uAddress, 1, (uint8_t*)&i8uValue);
                //syslog(LOG_DEBUG, "IOHandler: Get u8 value: %d\n", sPIValue.i8uValue);
                break;
            case 16:
                rc = piCtrl.Read(sPiVariable.i16uAddress, 2, (uint8_t*)&i16uValue);
                //syslog(LOG_DEBUG, "IOHandler: Value of %s: %04x hex (=%d dez)\n", pszVariableName, i16uValue, i16uValue);
                break;
            case 32:
                rc = piCtrl.Read(sPiVariable.i16uAddress, 4, (uint8_t*)&i32uValue);
                //syslog(LOG_DEBUG, "IOHandler: Value of %s: %08x hex (=%d dez)\n", pszVariableName, i32uValue, i32uValue);
                break;
        }
        usleep(10000);
    }

    ThreadSynchronization::getInstance()->UnlockIO();

    return rc;
}

int IOHandler::SetIO(const char* pszVariableName, uint32_t value) {
    int rc=-1;
    SPIVariable sPiVariable;
    SPIValue sPIValue;
    uint8_t i8uValue;
    uint16_t i16uValue;
   
    strncpy(sPiVariable.strVarName, pszVariableName, sizeof(sPiVariable.strVarName));

    ThreadSynchronization::getInstance()->LockIO();
    rc = piCtrl.GetVariableInfo(&sPiVariable);

    if (rc < 0) {
        syslog(LOG_ERR, "IOHandler: Cannot find variable '%s'\n", pszVariableName);
        ThreadSynchronization::getInstance()->UnlockIO();
        return -1;
    } else {
        syslog(LOG_DEBUG, "IOHandler: SetIO(%s) [offset=%d length=%d bit=%d value=%d]\n", sPiVariable.strVarName, sPiVariable.i16uAddress, sPiVariable.i16uLength, sPiVariable.i8uBit, value);
    }
    sPIValue.i16uAddress = sPiVariable.i16uAddress;
    sPIValue.i8uBit = sPiVariable.i8uBit;
    sPIValue.i8uValue = value;

    switch (sPiVariable.i16uLength) {
        case 1: 
            rc = piCtrl.Write(sPiVariable.i16uAddress, 1, (uint8_t *)&value);
            //if (rc > 0) syslog(LOG_DEBUG, "IOHandler: Set bit %d on byte at offset %d. Value %d\n", sPIValue.i8uBit, sPIValue.i16uAddress, sPIValue.i8uValue);
        case 8: 
            rc = piCtrl.Write(sPiVariable.i16uAddress, 1, (uint8_t *)&value);
            //if (rc > 0) syslog(LOG_DEBUG, "IOHandler: Write value %d dez (=%02x hex) to offset %d.\n", value, value, sPiVariable.i16uAddress);
            break;
        case 16:
            rc = piCtrl.Write(sPiVariable.i16uAddress, 2, (uint8_t *)&value);
            //if (rc > 0) syslog(LOG_DEBUG, "IOHandler: Write value %d dez (=%02x hex) to offset %d.\n", value, value, sPiVariable.i16uAddress);
            break;
        case 32:
            rc = piCtrl.Write(sPiVariable.i16uAddress, 4, (uint8_t *)&value);
            //if (rc > 0) syslog(LOG_DEBUG, "I: OHandlerWrite value %d dez (=%04x hex) to offset %d.\n", value, value, sPiVariable.i16uAddress);
            break;
    }
    ThreadSynchronization::getInstance()->UnlockIO();

    if (rc < 0)  {
        syslog(LOG_ERR, "IOHandler: Write error\n");
        return -1;
    } 
    
    return 1;
}

IOHandler * IOHandler::getInstance() {
    ThreadSynchronization::getInstance()->LockIO();
        if (instance == NULL) {
            instance = new IOHandler();
        }
    ThreadSynchronization::getInstance()->UnlockIO();
    return instance;
}
    