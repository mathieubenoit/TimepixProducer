/*
 * TimepixProducer.cxx
 *
 *
 *      Author: Mathieu Benoit
 */

  
#include "eudaq/Producer.hh"
#include "eudaq/Logger.hh"
#include "eudaq/RawDataEvent.hh"
#include "eudaq/Timer.hh"
#include "eudaq/Utils.hh"
#include "eudaq/OptionParser.hh"
#include "eudaq/ExampleHardware.hh"
#include "eudaq/Mutex.hh"
#include "mpxmanagerapi.h"
#include "mpxhw.h"


#include <iostream>
#include <ostream>
#include <vector>
#include <pthread.h>    /* POSIX Threads */


#include "MIMTLU.h"
#include "TimepixDevice.h"



using namespace std;

#define DEBUGPROD

// A name to identify the raw data format of the events generated
// Modify this to something appropriate for your producer.
static const std::string EVENT_TYPE = "TimepixRaw";

int status;
int AcqPreStarted=0;
int FrameReady;
unsigned int TLU;

MIMTLU *aMIMTLU;


// TOT . start on trigger stop on Timer
//char configName[256] = "config_I10-W0015_TOT_4-06-13" ;
//char AsciiconfigName[256] = "config_I10-W0015_TOT_4-06-13_ascii" ;

//TOT Start and stop on trigger
//char configName[256] = "config_I10-W0015_TOT_6-06-13_start_stop_on_trigger" ;
//char AsciiconfigName[256] = "config_I10-W0015_TOT_6-06-13_start_stop_on_trigger_ascii" ;


void * fitpix_acq ( void *ptr );


void AcquisitionPreStarted(CBPARAM /*par*/,INTPTR /*aptr*/){
	
#ifdef DEBUGPROD	
	cout << "AcquisitionPreStarted" << endl;	
#endif
	//usleep(1);
	AcqPreStarted=1;

	
	}
	
void AcquisitionStarted(CBPARAM /*par*/,INTPTR /*aptr*/)
{
#ifdef DEBUGPROD	
	cout << "triggerred" << endl; 
#endif
	//TLU = aMIMTLU->GetEvent();

}

void AcquisitionFinished(int /*stuff*/,int /*stuff2*/){
		status =1;
#ifdef DEBUGPROD	
		//cout << "acq finished" << endl;
#endif
	}

void FrameIsReady(int /*stuff*/,int /*stuff2*/){
		FrameReady =1;
#ifdef DEBUGPROD	
		cout << "Frame Ready" << endl;
#endif

	}


template <typename T>
inline void pack (std::vector< unsigned char >& dst, T& data) {
    unsigned char * src = static_cast < unsigned char* >(static_cast < void * >(&data));
    dst.insert (dst.end (), src, src + sizeof (T));
}

template <typename T>
inline void unpack (vector <unsigned char >& src, int index, T& data) {
    copy (&src[index], &src[index + sizeof (T)], &data);
}




// Declare a new class that inherits from eudaq::Producer
class TimepixProducer : public eudaq::Producer {
public:

  // The constructor must call the eudaq::Producer constructor with the name
  // and the runcontrol connection string, and initialize any member variables.
  TimepixProducer(const std::string & name, const std::string & runcontrol)
    : eudaq::Producer(name, runcontrol),
      m_run(0), m_ev(0), stopping(false), done(false) {

	  aTimepix = new TimepixDevice();

	  buffer = new char[4*MATRIX_SIZE];

	  output = new char[1000];

	  sprintf(output,"/home/lcd/eudaq/data/Run%d_%d",m_run,m_ev);
	  //output = "/home/mbenoit/workspace/eudaq/data/Run";
	  running =false;
	  
	  aMIMTLU= new MIMTLU();
	  int mimtlu_status = aMIMTLU->Connect(const_cast<char *>("192.168.222.200"),const_cast<char *>("23"));
	  if(mimtlu_status!=1)	   SetStatus(eudaq::Status::LVL_ERROR, "MIMTLU Not Running !!");



  }

  // This gets called whenever the DAQ is configured
  virtual void OnConfigure(const eudaq::Configuration & config) {
    std::cout << "Configuring: " << config.Name() << std::endl;

    // Do any configuration of the hardware here
    // Configuration file values are accessible as config.Get(name, default)
//    THL = config.Get("THL", 0);
//    aTimepix->SetTHL(THL);


    acqTime = config.Get("AcquisitionTime_us", 0);
    aTimepix->SetAcqTime(acqTime*1.0e-6);

    cout << "[TimepixProducer] Setting Acquisition time to : " << acqTime*1.e-6 << "s" << endl;


//    IKrum = config.Get("IKrum", 0);
//    aTimepix->SetIkrum(IKrum);

    //aTimepix->ReadDACs();

    // At the end, set the status that will be displayed in the Run Control.
    SetStatus(eudaq::Status::LVL_OK, "Configured (" + config.Name() + ")");
  }

  // This gets called whenever a new run is started
  // It receives the new run number as a parameter
  virtual void OnStartRun(unsigned param) {
    m_run = param;
    m_ev = 1;
    std::cout << "Start Run: " << m_run << std::endl;

    // It must send a BORE to the Data Collector
    eudaq::RawDataEvent bore(eudaq::RawDataEvent::BORE(EVENT_TYPE, m_run));
    // You can set tags on the BORE that will be saved in the data file
    // and can be used later to help decoding
    bore.SetTag("EXAMPLE", eudaq::to_string(m_exampleparam));
    // Send the event to the Data Collector
    SendEvent(bore);

    // At the end, set the status that will be displayed in the Run Control.
    SetStatus(eudaq::Status::LVL_OK, "Running");
    //eudaq::mSleep(5000);



    running = true;
  }

  // This gets called whenever a run is stopped
  virtual void OnStopRun() {
    std::cout << "Stopping Run" << std::endl;

    running = false;
   // aTimepix->Abort();

    SetStatus(eudaq::Status::LVL_OK, "Stopped");

    // Set a flag to signal to the polling loop that the run is over
//    stopping = true;
//
//    // wait until all events have been read out from the hardware
//    while (stopping) {
//      eudaq::mSleep(20);
//    }

    // Send an EORE after all the real events have been sent
    // You can also set tags on it (as with the BORE) if necessary
    SendEvent(eudaq::RawDataEvent::EORE("Test", m_run, ++m_ev));

    cout << "[TimepixProducer] Sent EORE " << endl;
 
    //




    system("cp /home/lcd/eudaq/timepixproducer_mb/ramdisk/* ../data");
    system("rm -fr /home/lcd/eudaq/timepixproducer_mb/ramdisk/*");
  }

  // This gets called when the Run Control is terminating,
  // we should also exit.
  virtual void OnTerminate() {
    std::cout << "Terminating..." << std::endl;
    running = false;
    done = true;
    eudaq::mSleep(1000);
  }

  // This is just an example, adapt it to your hardware
 void ReadoutLoop() {
    // Loop until Run Control tells us to terminate
    
    while (!done) {
    	if(running){

    	//sprintf(output,"../data/Run%d_%d",m_run,m_ev);
	sprintf(output,"/home/lcd/eudaq/timepixproducer_mb/ramdisk/Run%d_%d",m_run,m_ev);
    	
	//pthread_mutex_lock(&m_producer_mutex);
    	status=0;
	AcqPreStarted=0;
    	FrameReady=0;
	//cout << "starting new frame" << endl;
    	
	//aMIMTLU->Arm();
	
	
	//Multithreading 
	pthread_t thread1;  /* thread variables */
	pthread_create (&thread1, NULL,  fitpix_acq, (void *) this);
	
	
	    
	

     	while(AcqPreStarted!=1){
				//cout << status << endl;
	    		eudaq::mSleep(0.01);
	    	} 
		cout << "after loop" <<  endl;

	aMIMTLU->Arm();
		//cout << "[timepixProducer] Finished acquisition to file " << output <<  endl;

	while(FrameReady!=1){
				//cout << status << endl;
	    		eudaq::mSleep(0.01);
	    	} 
	pthread_join(thread1, NULL);

	TLU = aMIMTLU->GetEvent();
	cout << "TLU: " << TLU << " " << "event counter : " << m_ev << endl;	

/*	if((m_ev%(128*256))-TLU!=0) {
	    
	  // SetStatus(eudaq::Status::LVL_ERROR, "Mismatch");
	   cout << "TLU: " << TLU << " " << "event counter : " << m_ev << endl;	 
	}*/
	
	
	
	if(TLU==0) {
	    
	  // SetStatus(eudaq::Status::LVL_ERROR, "Mismatch");
	   cout << "TLU: " << TLU << " " << "event counter : " << m_ev << endl;	 
	}


	control=aTimepix->GetFrameData2(output,buffer);
	
    	unsigned int Data[MATRIX_SIZE];

    	for(int i =0;i<MATRIX_SIZE;i++){

    		memcpy(&Data[i],buffer+i*4,4);

    		}
        int pos =0;

        std::vector<unsigned char> bufferOut;
	std::vector<unsigned char> bufferTLU;
	pack(bufferTLU,TLU);
        for(unsigned int i=0;i<256;i++){
           		for(unsigned int j=0;j<256;j++){

           			if(Data[pos]!=0){

           				//cout << "[Data] Evt : " << m_ev << " " << i << " " << j << " " << Data[pos] << endl;
           				pack(bufferOut,i);
           				pack(bufferOut,j);
           				pack(bufferOut,Data[pos]);
           				//pack(bufferOut,i);
           			}

           				//pack(bufferOut,i);
           				// pack(bufferOut,j);
           				//pack(bufferOut,m_ev);
           			pos++;

           		}

           		}

    	if(m_ev%100==0 | m_ev<100) cout << "event #" << m_ev << endl;
	if(m_ev%1000==0 ) {
			
			    //system("cp /home/lcd/eudaq/timepixproducer_mb/ramdisk/* ../data");
    			    system("rm -fr /home/lcd/eudaq/timepixproducer_mb/ramdisk/*");

			}

//    if (!hardware.EventsPending()) {
//        // No events are pending, so check if the run is stopping
//        if (stopping) {
//          // if so, signal that there are no events left
//          stopping = false;
//        }
//        // Now sleep for a bit, to prevent chewing up all the CPU
//        eudaq::mSleep(20);
//        // Then restart the loop
//        continue;
//      }

      // If we get here, there must be data to read out
      // Create a RawDataEvent to contain the event data to be sent
      eudaq::RawDataEvent ev(EVENT_TYPE, m_run, m_ev);

//      for (unsigned plane = 0; plane < hardware.NumSensors(); ++plane) {
//        // Read out a block of raw data from the hardware
//        std::vector<unsigned char> buffer = hardware.ReadSensor(plane);
//        // Each data block has an ID that is used for ordering the planes later
//        // If there are multiple sensors, they should be numbered incrementally
//
//        // Add the block of raw data to the event
//        ev.AddBlock(plane, buffer);
//      }
//      std::vector<unsigned char> bufferV;
//      for(int i =0; i<MATRIX_SIZE;i++){
//
//    	  bufferV.push_back(buffer[i]);
//
//      }
      ev.AddBlock(0, bufferOut);
      ev.AddBlock(1,bufferTLU);
      // Send the event to the Data Collector
      SendEvent(ev);
      // Now increment the event number
      m_ev++;
    	}
    }
  }

void runfitpix()
{
#ifdef DEBUGPROD	
cout << "before run"<<endl;
#endif
	control=aTimepix->PerformAcquisition(output);
#ifdef DEBUGPROD	
cout << "after run"<<endl;
#endif
}

private:
  // This is just a dummy class representing the hardware
  // It here basically that the example code will compile
  // but it also generates example raw data to help illustrate the decoder
  eudaq::ExampleHardware hardware;
  unsigned m_run, m_ev, m_exampleparam;
  int THL;
  int IKrum;
  bool stopping, done;
  TimepixDevice *aTimepix;
  char *buffer;
  char* output;
  bool running;
  double acqTime;
  pthread_mutex_t m_producer_mutex;
  int control;

};

// The main function that will create a Producer instance and run it
int main(int /*argc*/, const char ** argv) {
  // You can use the OptionParser to get command-line arguments
  // then they will automatically be described in the help (-h) option
  eudaq::OptionParser op("EUDAQ Example Producer", "1.0",
                         "Just an example, modify it to suit your own needs");
  eudaq::Option<std::string> rctrl(op, "r", "runcontrol",
                                   "tcp://localhost:44000", "address",
                                   "The address of the RunControl.");
  eudaq::Option<std::string> level(op, "l", "log-level", "NONE", "level",
                                   "The minimum level for displaying log messages locally");
  eudaq::Option<std::string> name (op, "n", "name", "Example", "string",
                                   "The name of this Producer");
  try {
    // This will look through the command-line arguments and set the options
    op.Parse(argv);
    // Set the Log level for displaying messages based on command-line
    EUDAQ_LOG_LEVEL(level.Value());
    // Create a producer
    TimepixProducer producer(name.Value(), rctrl.Value());
    // And set it running...
    producer.ReadoutLoop();
    // When the readout loop terminates, it is time to go
    std::cout << "Quitting" << std::endl;
  } catch (...) {
    // This does some basic error handling of common exceptions
    return op.HandleMainException();
  }
  return 0;
}



void * fitpix_acq ( void *ptr )
{
    TimepixProducer *tp=(TimepixProducer *) ptr;            
    tp->runfitpix();  
    pthread_exit(0); /* exit */
} /* print_message_function ( void *ptr ) */
