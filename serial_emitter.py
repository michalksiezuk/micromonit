from time import sleep

import serial
import wmi

comm = serial.Serial("COM3", 9600)
machine = wmi.WMI(namespace="OpenHardwareMonitor")

while 1:
    cpu_name = machine.Hardware(Identifier="/amdcpu/0")[0].Name.replace("AMD ", "").strip()
    cpu_temp = round(machine.Sensor(Identifier="/amdcpu/0/temperature/0")[0].Value)
    cpu_load = machine.Sensor(Identifier="/amdcpu/0/load/0")[0].Value
    cpu_power = round(machine.Sensor(Identifier="/amdcpu/0/power/0")[0].Value)

    gpu_name = machine.Hardware(Identifier="/nvidiagpu/0")[0].Name.replace("NVIDIA GeForce", "").strip()
    gpu_temp = round(machine.Sensor(Identifier="/nvidiagpu/0/temperature/0")[0].Value)
    gpu_load = machine.Sensor(Identifier="/nvidiagpu/0/load/0")[0].Value
    gpu_power = round(machine.Sensor(Identifier="/nvidiagpu/0/power/0")[0].Value)

    comm.write(f"<{cpu_name},{cpu_temp},{cpu_load},{cpu_power},{gpu_name},{gpu_temp},{gpu_load},{gpu_power}>".encode("utf-8"))

    sleep(1)
