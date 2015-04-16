 
import time
 
try:
    import serial
    arduino = serial.Serial('COM8', baudrate=9600, timeout=1.0)
 
    # Nota: provocamos un reseteo manual de la placa para leer desde
    # el principio, ver http://stackoverflow.com/a/21082531/554319
    arduino.setDTR(False)
    time.sleep(1)
    arduino.flushInput()
    arduino.setDTR(True)
    print("Comienzo....")
 
except (ImportError, serial.SerialException):
    # No hay módulo serial o placa Arduino disponibles
    print ("No hay puerto serie")
 
f = open("log.csv","a")
 
# Con la sentencia with el arduino se cierra automáticamente, ver
# http://docs.python.org/3/reference/datamodel.html#context-managers
with arduino:
    while True:
        try:
            # En Python 3 esta función devuelve un objeto bytes, ver
            # http://docs.python.org/3/library/stdtypes.html#typebytes
            line = arduino.readline()
            f.write(time.strftime('%d-%m-%y,%H:%M:%S')+','+line.decode('ascii', errors='replace'))
            # Con errors='replace' se evitan problemas con bytes erróneos, ver
            # http://docs.python.org/3/library/stdtypes.html#bytes.decode
            # Con end='' se evita un doble salto de línea
            print(line.decode('ascii', errors='replace'), end='')
        except KeyboardInterrupt:
            print("Exiting")
            f.close()
            break
