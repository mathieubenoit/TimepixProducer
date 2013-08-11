#!/bin/bash

source setup_producer.sh

#To be verified before starting the Run , does it correspond to the desired mode of operation ? To the Sensor in the beam ? 

#Sensor I10-W0015
export BIAS="15V"
export BPC="/home/lcd/CLIC_Testbeam_August2013/TimepixAssemblies_Data/I10-W0015/Configs/BPC_I10-W0015_0V_IKrum5_96MHz_10-08-13"
export ASCII="/home/lcd/CLIC_Testbeam_August2013/TimepixAssemblies_Data/I10-W0015/Configs/BPC_I10-W0015_0V_IKrum5_96MHz_10-08-13_ascii"
export TPMODE="TOT"

##Sensor B04-W0110
#export BIAS="15V"
#export BPC="/home/lcd/CLIC_Testbeam_August2013/TimepixAssemblies_Data/B04-W0110/Configs/BPC_B04-W0110_15V_IKrum1_96MHz_08-08-13"
#export ASCII="/home/lcd/CLIC_Testbeam_August2013/eudaq/TimepixProducer/Pixelman_SCL_2011_12_07/Default_Ascii_Config"
#export TPMODE="TOT"
#
##Sensor C04-W0110
#export BIAS="15V"
#export BPC="/home/lcd/CLIC_Testbeam_August2013/TimepixAssemblies_Data/C04-W0110/Configs/BPC_C04-W0110_15V_IKrum1_96MHz_08-08-13"
#export ASCII="/home/lcd/CLIC_Testbeam_August2013/eudaq/TimepixProducer/Pixelman_SCL_2011_12_07/Default_Ascii_Config"
#export TPMODE="TOT"
#
##Sensor C06-W0110
#export BIAS="15V"
#export BPC="/home/lcd/CLIC_Testbeam_August2013/TimepixAssemblies_Data/C06-W0110/Configs/BPC_C06-W0110_15V_IKrum1_96MHz_08-08-13"
#export ASCII="/home/lcd/CLIC_Testbeam_August2013/eudaq/TimepixProducer/Pixelman_SCL_2011_12_07/Default_Ascii_Config"
#export TPMODE="TOT"
#
##Sensor A06-W0110
#export BIAS="15V"
#export BPC="/home/lcd/CLIC_Testbeam_August2013/TimepixAssemblies_Data/A06-W0110/Configs/BPC_A06-W0110_15V_IKrum1_96MHz_08-08-13"
#export ASCII="/home/lcd/CLIC_Testbeam_August2013/eudaq/TimepixProducer/Pixelman_SCL_2011_12_07/Default_Ascii_Config"
#export TPMODE="TOT"

echo "Binary Config : " $BPC
echo "Ascii Config : " $ASCII
echo "Bias Voltage : " $BIAS

echo "###############################################" >> logs/producer_log.txt
date >> logs/producer_log.txt
echo "Binary Config : " $BPC >> logs/producer_log.txt
echo "Ascii Config : " $ASCII >> logs/producer_log.txt
echo "Bias Voltage : " $BIAS >> logs/producer_log.txt
echo "Mode : " $TPMODE >> logs/producer_log.txt
echo "###############################################" >> logs/producer_log.txt

cd $TPPROD/Pixelman_SCL_2011_12_07
eval "./TimepixProducer.exe -b $BPC -a $ASCII -V $BIAS -M $TPMODE"
cd $TPPROD
