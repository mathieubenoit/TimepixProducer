#!/bin/bash

source setup_producer.sh

#To be verified before starting the Run , does it correspond to the desired mode of operation ? To the Sensor in the beam ? 

#Sensor I10-W0015
export BIAS="15V"
export BPC="/home/lcd/CLIC_Testbeam_August2013/TimepixAssemblies_Data/I10-W0015/Configs/BPC_I10-W0015_0V_IKrum5_96MHz_10-08-13"
export ASCII="/home/lcd/CLIC_Testbeam_August2013/TimepixAssemblies_Data/I10-W0015/Configs/BPC_I10-W0015_0V_IKrum5_96MHz_10-08-13_ascii"

echo "Binary Config : " $BPC
echo "Ascii Config : " $ASCII
echo "Bias Voltage : " $BIAS

echo "###############################################" >> logs/producer_log.txt
date >> logs/producer_log.txt
echo "Binary Config : " $BPC >> logs/producer_log.txt
echo "Ascii Config : " $ASCII >> logs/producer_log.txt
echo "Bias Voltage : " $BIAS >> logs/producer_log.txt
echo "###############################################" >> logs/producer_log.txt

cd $TPPROD/Pixelman_SCL_2011_12_07
eval "./TimepixProducer.exe -b $BPC -a $ASCII -V $BIAS"
cd $TPPROD
