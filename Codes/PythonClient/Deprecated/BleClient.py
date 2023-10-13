import asyncio
import dobot
import time
from bleak import BleakClient


HOST = '127.0.0.1'
PORT=8893
cs = dobot.CommandSender(HOST,PORT)
# client = BleakClient

IDStats = '00000000-0000-1000-8000-00805f9b34fb'
IDCommand = '0000ccdd-0000-1000-8000-00805f9b34fb'
IDA = '0000aaaa-0000-1000-8000-00805f9b34fb'
IDB = '0000bbbb-0000-1000-8000-00805f9b34fb'
IDC = '0000cccc-0000-1000-8000-00805f9b34fb'
IDD = '0000dddd-0000-1000-8000-00805f9b34fb'

VALUEcommand = ""
VALUEA=0
VALUEB=0
VALUEC=0
VALUED=0

# async def notification_cb(handle, data):

async def notification_cb(client: BleakClient, data: bytearray):
    global VALUEA, VALUEcommand
    print(data)
    
    # Read the other characteristic using its UUID directly
    CHRA = await client.read_gatt_char(IDA)
    CHRB = await client.read_gatt_char(IDB)
    CHRC = await client.read_gatt_char(IDC)
    CHRD = await client.read_gatt_char(IDD)
    VALUEA = int.from_bytes(CHRA, byteorder='little')  # Assuming the value is an integer
    VALUEB = int.from_bytes(CHRB, byteorder='little')  # Assuming the value is an integer
    VALUEC = int.from_bytes(CHRC, byteorder='little')  # Assuming the value is an integer
    VALUED = int.from_bytes(CHRD, byteorder='little')  # Assuming the value is an integer
    
    VALUEcommand = data.decode()
    # Rest of your code

    cs._send(dict(command="JumpTo",A=VALUEA,B=VALUEB,C=VALUEC,D=VALUED))




async def main(address):

  async with BleakClient(address) as client:
    if (not client.is_connected):
      raise "client not connected"

    # services = await client.get_services()

    # await client.start_notify(IDCommand, notification_cb)
    cb = lambda cbclient, data: asyncio.ensure_future(notification_cb(client, data))
    await client.start_notify(IDCommand, cb)
    
    reachedresult = cs.hasReached()
    if("is_sccess" not in reachedresult):
      data_to_write = reachedresult["msg"].encode('utf-8')  # UTF-8エンコーディングを使用してバイト列に変換
    
      await client.write_gatt_char(IDStats, data_to_write )
    time.sleep(500)


    # await client.stop_notify(IDCommand)




if __name__ == "__main__":
  address = 'D4:D4:DA:5C:A1:E6'  
  
  print('address:', address)
  asyncio.run(main(address))

