#include "TimepixDevice.h"

// TOT . start on trigger stop on Timer
char configName[256] = "config_I10-W0015_TOT_4-06-13" ;
char AsciiconfigName[256] = "config_I10-W0015_TOT_4-06-13_ascii" ;
FRAMEID id;


TimepixDevice::TimepixDevice(){


		// Init manager
		control = -1;
		u32 flags = MGRINIT_NOEXHANDLING;
		control = mgrInitManager(flags, '\0');
		if(mgrIsRunning()){
			cout << "Manager running " << endl;
		}

		mgrRegisterCallback("mpxctrl",MPXCTRL_CB_ACQCOMPL,&AcquisitionFinished,0);
		mgrRegisterCallback("mpxmgr",MPXMGR_CB_FRAME_NEW,&FrameIsReady,0);
		mgrRegisterCallback("mpxctrl",MPXCTRL_CB_ACQPRESTART,&AcquisitionPreStarted,0);
		mgrRegisterCallback("mpxctrl",MPXCTRL_CB_ACQSTART,&AcquisitionStarted,0);


		// Find device
		count = 0;
		
		
		control = mpxCtrlGetFirstMpx(&devId, &count);
		cout << "found : " << count << " | devId : " << devId << endl;
		numberOfChips=0;
		numberOfRows=0;


		//Get Info on device
		int info = mpxCtrlGetMedipixInfo(devId,&numberOfChips,&numberOfRows,chipBoardID,ifaceName);
		if(info==0) cout << "Number of chips : " << numberOfChips << " Number of rows : " <<  numberOfRows << " chipBoard ID : " <<  chipBoardID << " Interface Name : " << ifaceName <<  endl;
                
		else cout << "board not found !! " << endl;

 
		// Load binary pixels config
		//char configName[256] = "default.bpc" ;
		//char configName[256] = "config_I10-W0015_TOT_4-06-13" ;
		//char configName[256] = "config_I10-W0015_TOA48MHz_4-06-13" ;

		control = mpxCtrlLoadPixelsCfg(devId, configName , true);
		cout << "Load pixels config : " << configName << endl;
		//control << endl;


		//mpxCtrlLoadMpxCfg(devId,"default_ascii");
		mpxCtrlLoadMpxCfg(devId,AsciiconfigName);
		cout << "Load Ascii pixels config : " << AsciiconfigName << endl;

		//mpxCtrlLoadMpxCfg(devId,"config_I10-W0015_TOA48MHz_4-06-13_ascii");


		// Get number of parameters in the Hw Info list
		hwInfoCount = 0;
		control = mpxCtrlGetHwInfoCount(devId, &hwInfoCount);
		cout << "count : " << hwInfoCount << endl;


		// Set bias voltage
		double voltage = 80.0;
		byte triggmode = 2;

		byte * data = new byte(sizeof(double));//(byte)voltage;
		byte * data2 = new byte(sizeof(byte));//(byte)trig;

		data = (byte * )(&voltage);
		data2 = (byte * )(&triggmode);

		//control = mpxCtrlSetHwInfoItem(devId, 8, data, sizeof(double));	 // 8 voltage
		//control = mpxCtrlSetHwInfoItem(devId, 20, &triggmode, 1);
		
		
		usleep(1000000);

		// Read bias voltage and bias voltage verification
		ItemInfo data_hv;
		int datar_size = 0;
		byte trgg_back;
		data_hv.data = &trgg_back;

		mpxCtrlGetHwInfoItem(devId, 20, &data_hv, &datar_size);
		cout << "data size  : " << datar_size << endl;
		cout << "array size : " << data_hv.count << endl;
		cout << "flags      : " << data_hv.flags << endl;
		cout << "name       : " << data_hv.name << endl;
		cout << "descr      : " << data_hv.descr << endl;
		cout << "type       : " << data_hv.type << endl;
		cout << "data       : " << trgg_back << endl;

//		ItemInfo data_hv;
		datar_size = 8;
		double voltage_back;
		data_hv.data = &voltage_back;

		// 18 signal delay u16
		// 12 Freq
		// 8 HV
		// 9 HV ver
		for ( int i = 8; i < 10 ; i++ ) {

			cout << "Item : " << i << endl;
			mpxCtrlGetHwInfoItem(devId, i, &data_hv, &datar_size);
			cout << "data size  : " << datar_size << endl;
			cout << "array size : " << data_hv.count << endl;
			cout << "flags      : " << data_hv.flags << endl;
			cout << "name       : " << data_hv.name << endl;
			cout << "descr      : " << data_hv.descr << endl;
			cout << "type       : " << data_hv.type << endl;
			cout << "data       : " << voltage_back << endl;

			// rewind
			datar_size = 0;
		}

		//control =mpxCtrlGetAcqMode(devId, &mode);
		//cout << "Acq mode before = " << mode << endl;
		control = mpxCtrlSetAcqMode(devId,ACQMODE_HWTRIGSTART_TIMERSTOP );
		// check mode
		//control =mpxCtrlGetAcqMode(devId, &mode);
		//cout << "Acq mode after = " << mode << endl;
 



		// Data type Set
		mgrSetFrameType(0,TYPE_U32);

		//cout << "Test acquisition" << endl;
		byte *buf=new byte[MATRIX_SIZE];
		//this->PerformAcquisition("test");
		delete buf;





	}
	
	
int  TimepixDevice::ReadFrame(char * Filename, char* buffer){



			ifstream in(Filename,ios::binary|ios::ate);
			long size = in.tellg();
			in.seekg(0,ios::beg);

			in.read(buffer,size);
			in.close();



			return 0;

	}


int TimepixDevice::SetTHL(int THL){

		// Get and Set Daqs
		DACTYPE dacVals[14];
		int chipnumber = 0;

		dacVals[TPX_THLFINE] = THL; // 1000e- above thl adjustment mean
		control = mpxCtrlSetDACs(devId, dacVals, 14, chipnumber);
		cout << "[TimepixProducer] Setting THL Fine to " << THL << endl;

		DACTYPE dacVals_new[14];
		control = mpxCtrlGetDACs(devId, dacVals_new, 14, chipnumber);
		cout << "[TimepixProducer] THL Fine Readout = " << dacVals_new[TPX_THLFINE] << endl;

		control = mpxCtrlRefreshDACs(devId);
		return control; 

	}

int TimepixDevice::SetIkrum(int IKrum){

		// Get and Set Daqs
		DACTYPE dacVals[14];
		int chipnumber = 0;

		dacVals[TPX_IKRUM] = IKrum; // 1000e- above thl adjustment mean
		control = mpxCtrlSetDACs(devId, dacVals, 14, chipnumber);
		cout << "[TimepixProducer] Setting Ikrum to " << IKrum << endl;

		DACTYPE dacVals_new[14];
		control = mpxCtrlGetDACs(devId, dacVals_new, 14, chipnumber);
		cout << "[TimepixProducer] IKrum Readout = " << dacVals_new[TPX_IKRUM] << endl;

		control = mpxCtrlRefreshDACs(devId);
		return control;

	}


int TimepixDevice::ReadDACs(){


		DACTYPE dacVals_readout[14];
		int chipnumber = 0;

		control = mpxCtrlGetDACs(devId, dacVals_readout, 14, chipnumber);
		cout << "[TimepixProducer] IKrum Readout = " << dacVals_readout[TPX_IKRUM] << endl;
		cout << "[TimepixProducer] DISC Readout = " << dacVals_readout[TPX_DISC] << endl;
		cout << "[TimepixProducer] PREAMP Readout = " << dacVals_readout[TPX_PREAMP] << endl;
		cout << "[TimepixProducer] BUFFA Readout = " << dacVals_readout[TPX_BUFFA] << endl;
		cout << "[TimepixProducer] BUFFB Readout = " << dacVals_readout[TPX_BUFFB] << endl;
		cout << "[TimepixProducer] HIST Readout = " << dacVals_readout[TPX_HIST] << endl;
		cout << "[TimepixProducer] THLFINE Readout = " << dacVals_readout[TPX_THLFINE] << endl;
		cout << "[TimepixProducer] THLCOARSE Readout = " << dacVals_readout[TPX_THLCOARSE] << endl;
		cout << "[TimepixProducer] VCAS Readout = " << dacVals_readout[TPX_VCAS] << endl;
		cout << "[TimepixProducer] FBK Readout = " << dacVals_readout[TPX_FBK] << endl;
		cout << "[TimepixProducer] GND Readout = " << dacVals_readout[TPX_GND] << endl;
		cout << "[TimepixProducer] THS Readout = " << dacVals_readout[TPX_THS] << endl;
		cout << "[TimepixProducer] BIASLVDS Readout = " << dacVals_readout[TPX_BIASLVDS] << endl;
		cout << "[TimepixProducer] REFLVDS Readout = " << dacVals_readout[TPX_REFLVDS] << endl;

		return 0;
	}


int TimepixDevice::PerformAcquisition(char *output){

		//cout << "[timepixProducer] Starting acquisition to file " << output << endl;
		control = mpxCtrlPerformFrameAcq(devId, 1,acqTime, FSAVE_BINARY |FSAVE_U32 ,output);

		//mpxCtrlGetFrame32(devId,buffer, MATRIX_SIZE, u32 frameNumber);
		return control;

	}



int TimepixDevice::GetFrameData(byte *buffer){

		control=mpxCtrlGetFrameID(devId,0,&FrameID);
		control=mgrGetFrameData(id,buffer, &size , TYPE_DOUBLE);
		return control;
	}
	
	
int TimepixDevice::GetFrameData2(char * Filename, char* buffer){



			ifstream in(Filename,ios::binary|ios::ate);
			long size = in.tellg();
			in.seekg(0,ios::beg);

			in.read(buffer,size);
			in.close();



			return 0;

	}

int  TimepixDevice::GetFrameDatadAscii(char * Filename, u32* buffer){



			ifstream in(Filename);

			u32 tot;
			int i=0;
			while(!in.eof()){
				in >> tot;
				buffer[i]=tot;
				cout << tot << endl;

				i++;
			}

			in.close();



			return 0;

	}

int TimepixDevice::Abort (){
		control = mpxCtrlAbortOperation(devId);
		return control;
	}

int TimepixDevice::SetAcqTime(double time){
	   acqTime=time;
   	   return 1;
   }
