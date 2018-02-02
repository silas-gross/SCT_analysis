# -*- coding: utf-8 -*-
"""
Created on Fri Feb 02 06:03:21 2018

@author: Silas Grossberndt
"""
# This code is designed to act as a pre-processor for the raw output for the SCT analysis software
#import numpy as np
with open("C:\Users\Silas Grossberndt\Documents\ABC_Boards\TIDLogTesting.dat", 'r') as lf:
        i=0
        j=0
        k=0
        read=0
        values=[[0 for x in range(14)] for y in range(2000)]
        for line in lf.readlines():
            #print(i)
            values[k]=line.split() #Splits on whitespace
            k=k+1
            for n in range(len(values[k])-1): #checks for the header and sets up file reading
                if i==0:
                    if values[k][n]=='Jan': #and values[n+1]=='29':
                        i=i+1
                        j=j+1
                    if values[k][n]=='Jan' and values[n+1]==30:
                        i=i+1
                        j=j+1
                if values[k][n]=='t(s),v0Mon(V),i0Mon(mA),v1Mon(V),i1Mon(mA),VDDA_RAW(mV),VDDA_REG(mV),VDDD_RAW(mV),VDDD_REG(mV),MOD_GND(mV),Temperature(C),IrradStatus(On = 1),DoseRate(kRad/hr),TotalDose(MRad)' and i==1:
                    i=i+1
                if values[k][n]=='#** -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- **' and i==2:
                    read=1
                if values[k][n]=='Start' and not i==0:
                    read=0
                    i=0
            #prevnts looping onto the next one
            if read==1:
                print(values[0][0])
            timename='Time'+str(j)+'.dat'
            vdname='VD'+str(j)+'.dat'
            vaname='VA'+str(j)+'.dat'
            idname='ID'+str(j)+'.dat'
            ianame='IA'+str(j)+'.dat'
            pdname='pd'+str(j)+'.dat'
            paname='pa'+str(j)+'.dat'
            t=open(timename, 'w')
            vd=open(vdname, 'w')
            va=open(vaname, 'w')
            I=open(idname, 'w')
            Ia=open(ianame, 'w')            
            pd=open(pdname, 'w')
            pa=open(paname, 'w')
            if read==1:
                t.Append(values[0])
                vd.Append(values[1])
                I.Append(values[2])
                va.Append(values[3])
                Ia.Append(values[4])
                Pd=values[1]*values[2]
                Pa=values[3]*values[4]
                pa.Append[Pa]
                pd.Append[Pd]
            t.close()
            vd.close()
            va.close()
            I.close()
            Ia.close()
            pd.close()
            pa.close()
lf.close()

                
                    
                
                
            
