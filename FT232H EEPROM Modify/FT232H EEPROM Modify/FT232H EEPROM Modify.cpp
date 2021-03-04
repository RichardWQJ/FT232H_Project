// FT232H EEPROM Modify.cpp : Defines the entry point for the console application.
//

// NOTE:	This code is provided as an example only and is not supported or guaranteed by FTDI.
//			It is the responsibility of the recipient/user to ensure the correct operation of 
//			any software which is created based upon this example.

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include "ftd2xx.h"

int main()
{
	//********************************************************
	//Definitions
	//********************************************************

	FT_HANDLE fthandle;
	FT_STATUS status;
	
	BOOLEAN Dev_Found = FALSE;

	FT_PROGRAM_DATA ftData;

	WORD VendorIdBuf = 0x0403;
	WORD ProductIdBuf = 0x6014;
	char ManufacturerBuf[32];
	char ManufacturerIdBuf[16];
	char DescriptionBuf[64];
	char SerialNumberBuf[16];

	ftData.Signature1 = 0x00000000;		// Always 0x00000000
	ftData.Signature2 = 0xffffffff;		// Always 0xFFFFFFFF
	ftData.Version = 5;	// Header - FT_PROGRAM_DATA version 0 = original (FT232B), 1 = FT2232 extensions, 2 = FT232R extensions, 3 = FT2232H extensions, 4 = FT4232H extensions, 5 = FT232H extensions

	ftData.VendorId = VendorIdBuf;
	ftData.ProductId = ProductIdBuf;
	ftData.Manufacturer = ManufacturerBuf;
	ftData.ManufacturerId = ManufacturerIdBuf;
	ftData.Description = DescriptionBuf;
	ftData.SerialNumber = SerialNumberBuf;

	ftData.MaxPower;
	ftData.PnP;
	ftData.SelfPowered;
	ftData.RemoteWakeup;

	//'FT232H features require section below
	ftData.PullDownEnableH;
	ftData.SerNumEnableH;
	ftData.ACSlowSlewH;
	ftData.ACSchmittInputH;
	ftData.ACDriveCurrentH;
	ftData.ADSlowSlewH;
	ftData.ADSchmittInputH;
	ftData.ADDriveCurrentH;
	ftData.Cbus0H;
	ftData.Cbus1H;
	ftData.Cbus2H;
	ftData.Cbus3H;
	ftData.Cbus4H;
	ftData.Cbus5H;
	ftData.Cbus6H;
	ftData.Cbus7H;
	ftData.Cbus8H;
	ftData.Cbus9H;
	ftData.IsFifoH;
	ftData.IsFifoTarH;
	ftData.IsFastSerH;
	ftData.IsFT1248H;
	ftData.FT1248CpolH;
	ftData.FT1248LsbH;
	ftData.FT1248FlowControlH;
	ftData.IsVCPH;
	ftData.PowerSaveEnableH;

	//********************************************************
	//List Devices
	//********************************************************

	FT_DEVICE_LIST_INFO_NODE *devInfo;
	DWORD numDevs;
	DWORD i;

	// create the device information list 
	status = FT_CreateDeviceInfoList(&numDevs);

	if (status != FT_OK) {
		printf("FT_CreateDeviceInfoList status not ok %d\n", status);
		return 0;
	}
	else
	{
		printf("Number of devices is %d\n", numDevs);
		if (numDevs > 0) {
			// allocate storage for list based on numDevs 
			devInfo =
				(FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numDevs);
			// get the device information list 
			status = FT_GetDeviceInfoList(devInfo, &numDevs);
			if (status == FT_OK) {
				for (i = 0; i < numDevs; i++) {
					printf("Dev %d:\n", i);
					printf("Flags=0x%x\n", devInfo[i].Flags);
					printf("Type=0x%x\n", devInfo[i].Type);
					printf("ID=0x%x\n", devInfo[i].ID);
					printf("LocId=0x%x\n", devInfo[i].LocId);
					printf("SerialNumber=%s\n", devInfo[i].SerialNumber);
					printf("Description=%s\n", devInfo[i].Description);
					printf("\n");
				}
			}
		}
	}

	//********************************************************
	//Open the port
	//********************************************************

	for (i = 0; i < numDevs; i++)
	{
		if ((Dev_Found == FALSE) && (devInfo[i].Type == FT_DEVICE_232H))
		{
			Dev_Found = TRUE;

			status = FT_OpenEx("UM232H", FT_OPEN_BY_DESCRIPTION, &fthandle);

			if (status != FT_OK)
			{
				printf("Open status not ok %d\n", status);
				printf("Trying to open unprogrammed EEPROM device...\n");
				status = FT_OpenEx("Single RS232-HS", FT_OPEN_BY_DESCRIPTION, &fthandle);
				if (status != FT_OK)
				{
					printf("Open status not ok %d\n", status);
					printf("\n");
					return 0;
				}
				else
				{
					printf("Open status OK %d\n", status);
					printf("\n");
				}

				printf("\n");
			}
			else
			{
				printf("Open status OK %d\n", status);
				printf("\n");
			}

			//********************************************************
			//Read the EEPROM
			//********************************************************

			status = FT_EE_Read(fthandle, &ftData);

			if (status != FT_OK)
			{
				printf("EE_Read status not ok %d\n", status);
				if (status == FT_EEPROM_NOT_PROGRAMMED)
				{
					printf("EEPROM is not programmed! Programming with preset values.\n");
					printf("\n");

					char ManufacturerBufNew[32] = "FTDI";
					char ManufacturerIdBufNew[16] = "FT";
					char DescriptionBufNew[64] = "UM232H";
					char SerialNumberBufNew[16] = "FT11111";

					ftData.Manufacturer = ManufacturerBufNew;
					ftData.ManufacturerId = ManufacturerIdBufNew;
					ftData.Description = DescriptionBufNew;
					ftData.SerialNumber = SerialNumberBufNew;

					ftData.MaxPower = 90;
					ftData.PnP = 1;
					ftData.SelfPowered = 0;
					ftData.RemoteWakeup = 0;

					//'FT4232H features require section below
					ftData.PullDownEnableH = 0;
					ftData.SerNumEnableH = 1;
					ftData.ACSlowSlewH = 0;
					ftData.ACSchmittInputH = 0;
					ftData.ACDriveCurrentH = 4;
					ftData.ADSlowSlewH = 0;
					ftData.ADSchmittInputH = 0;
					ftData.ADDriveCurrentH = 4;
					ftData.Cbus0H = 0;
					ftData.Cbus1H = 0;
					ftData.Cbus2H = 0;
					ftData.Cbus3H = 0;
					ftData.Cbus4H = 0;
					ftData.Cbus5H = 0;
					ftData.Cbus6H = 0;
					ftData.Cbus7H = 0;
					ftData.Cbus8H = 0;
					ftData.Cbus9H = 0;
					ftData.IsFifoH = 0;
					ftData.IsFifoTarH = 0;
					ftData.IsFastSerH = 0;
					ftData.IsFT1248H = 0;
					ftData.FT1248CpolH = 0;
					ftData.FT1248LsbH = 0;
					ftData.FT1248FlowControlH = 0;
					ftData.IsVCPH = 1;
					ftData.PowerSaveEnableH = 0;

					//********************************************************
					//program a blank EEPROM first.
					//********************************************************

					status = FT_EE_Program(fthandle, &ftData);

					if (status != FT_OK)
					{
						printf("Initial FT_EE_Program not ok %d\n", status);
						printf("\n");
						return 0;
					}
					else
					{
						printf("Initial FT_EE_Program OK!\n");
						printf("\n");
					}

				}
				else
				{
					return 0;
				}
			}
			else
			{
				printf("EEPROM is already programmed! Reading EEPROM.\n");
				printf("\n");

				printf("Signature1 =  0x%04x\n", ftData.Signature1);
				printf("Signature2 =  0x%04x\n", ftData.Signature2);
				printf("Version =  0x%04x\n", ftData.Version);


				printf("VendorID =  0x%04x\n", ftData.VendorId);
				printf("ProductID =  0x%04x\n", ftData.ProductId);
				printf("Manufacturer =  %s\n", ftData.Manufacturer);
				printf("ManufacturerID =  %s\n", ftData.ManufacturerId);
				printf("Description =  %s\n", ftData.Description);
				printf("SerialNumber =  %s\n", ftData.SerialNumber);
				printf("MaxPower =  %d\n", ftData.MaxPower);
				printf("PnP =  %x\n", ftData.PnP);
				printf("SelfPowered =  %x\n", ftData.SelfPowered);
				printf("RemoteWakeup =  %x\n", ftData.RemoteWakeup);

				printf("PullDownEnableH =  %x\n", ftData.PullDownEnableH);
				printf("SerNumEnableH =  %x\n", ftData.SerNumEnableH);
				printf("ACSlowSlewH =  %x\n", ftData.ACSlowSlewH);
				printf("ACSchmittInputH =  %x\n", ftData.ACSchmittInputH);
				printf("ACDriveCurrentH =  %x\n", ftData.ACDriveCurrentH);
				printf("ADSlowSlewH =  %x\n", ftData.ADSlowSlewH);
				printf("ADSchmittInputH =  %x\n", ftData.ADSchmittInputH);
				printf("ADDriveCurrentH =  %x\n", ftData.ADDriveCurrentH);
				printf("Cbus0H =  %x\n", ftData.Cbus0H);
				printf("Cbus1H =  %x\n", ftData.Cbus1H);
				printf("Cbus2H =  %x\n", ftData.Cbus2H);
				printf("Cbus3H =  %x\n", ftData.Cbus3H);
				printf("Cbus4H =  %x\n", ftData.Cbus4H);
				printf("Cbus5H =  %x\n", ftData.Cbus5H);
				printf("Cbus6H =  %x\n", ftData.Cbus6H);
				printf("Cbus7H =  %x\n", ftData.Cbus7H);
				printf("Cbus8H =  %x\n", ftData.Cbus8H);
				printf("Cbus9H =  %x\n", ftData.Cbus9H);
				printf("IsFifoH =  %x\n", ftData.IsFifoH);
				printf("IsFifoTarH =  %x\n", ftData.IsFifoTarH);
				printf("IsFastSerH =  %x\n", ftData.IsFastSerH);
				printf("IsFT1248H =  %x\n", ftData.IsFT1248H);
				printf("FT1248CpolH =  %x\n", ftData.FT1248CpolH);
				printf("FT1248LsbH =  %x\n", ftData.FT1248LsbH);
				printf("FT1248FlowControlH =  %x\n", ftData.FT1248FlowControlH);
				printf("IsVCPH =  %x\n", ftData.IsVCPH);
				printf("PowerSaveEnableH =  %x\n", ftData.PowerSaveEnableH);
				printf("\n");
			}

			//********************************************************
			//change serial number from one that was read.
			//********************************************************

			ftData.SerialNumber = "FT232H";

			status = FT_EE_Program(fthandle, &ftData);

			if (status != FT_OK)
			{
				printf("EE_Program status not ok %d\n", status);
				return 0;
			}
			else
			{
				printf("EE_Program status ok %d\n", status);
				printf("\n");
			}

			//********************************************************
			// Delay
			//********************************************************

			Sleep(1000);

			//********************************************************
			// Re - Read
			//********************************************************

			ftData.SerialNumber = SerialNumberBuf; //Reset to variable

			printf("Reading EEPROM to check changed values!\n");
			printf("\n");

			status = FT_EE_Read(fthandle, &ftData);

			if (status != FT_OK)
			{
				printf("EE_Read status not ok %d\n", status);
				return 0;
			}
			else
			{
				printf("Signature1 =  0x%04x\n", ftData.Signature1);
				printf("Signature2 =  0x%04x\n", ftData.Signature2);
				printf("Version =  0x%04x\n", ftData.Version);


				printf("VendorID =  0x%04x\n", ftData.VendorId);
				printf("ProductID =  0x%04x\n", ftData.ProductId);
				printf("Manufacturer =  %s\n", ftData.Manufacturer);
				printf("ManufacturerID =  %s\n", ftData.ManufacturerId);
				printf("Description =  %s\n", ftData.Description);
				printf("SerialNumber =  %s\n", ftData.SerialNumber);
				printf("MaxPower =  %d\n", ftData.MaxPower);
				printf("PnP =  %x\n", ftData.PnP);
				printf("SelfPowered =  %x\n", ftData.SelfPowered);
				printf("RemoteWakeup =  %x\n", ftData.RemoteWakeup);

				printf("PullDownEnableH =  %x\n", ftData.PullDownEnableH);
				printf("SerNumEnableH =  %x\n", ftData.SerNumEnableH);
				printf("ACSlowSlewH =  %x\n", ftData.ACSlowSlewH);
				printf("ACSchmittInputH =  %x\n", ftData.ACSchmittInputH);
				printf("ACDriveCurrentH =  %x\n", ftData.ACDriveCurrentH);
				printf("ADSlowSlewH =  %x\n", ftData.ADSlowSlewH);
				printf("ADSchmittInputH =  %x\n", ftData.ADSchmittInputH);
				printf("ADDriveCurrentH =  %x\n", ftData.ADDriveCurrentH);
				printf("Cbus0H =  %x\n", ftData.Cbus0H);
				printf("Cbus1H =  %x\n", ftData.Cbus1H);
				printf("Cbus2H =  %x\n", ftData.Cbus2H);
				printf("Cbus3H =  %x\n", ftData.Cbus3H);
				printf("Cbus4H =  %x\n", ftData.Cbus4H);
				printf("Cbus5H =  %x\n", ftData.Cbus5H);
				printf("Cbus6H =  %x\n", ftData.Cbus6H);
				printf("Cbus7H =  %x\n", ftData.Cbus7H);
				printf("Cbus8H =  %x\n", ftData.Cbus8H);
				printf("Cbus9H =  %x\n", ftData.Cbus9H);
				printf("IsFifoH =  %x\n", ftData.IsFifoH);
				printf("IsFifoTarH =  %x\n", ftData.IsFifoTarH);
				printf("IsFastSerH =  %x\n", ftData.IsFastSerH);
				printf("IsFT1248H =  %x\n", ftData.IsFT1248H);
				printf("FT1248CpolH =  %x\n", ftData.FT1248CpolH);
				printf("FT1248LsbH =  %x\n", ftData.FT1248LsbH);
				printf("FT1248FlowControlH =  %x\n", ftData.FT1248FlowControlH);
				printf("IsVCPH =  %x\n", ftData.IsVCPH);
				printf("PowerSaveEnableH =  %x\n", ftData.PowerSaveEnableH);
				printf("\n");
			}

			//*****************************************************
			//Close the port
			//*****************************************************

			// Close the device
			status = FT_Close(fthandle);

		}

	}

	printf("Press Return To End Program");
	getchar();
	printf("closed \n");

	return 0;
}

