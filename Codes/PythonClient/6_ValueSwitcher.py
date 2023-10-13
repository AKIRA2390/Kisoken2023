# 0000ccdd-0000-1000-8000-00805f9b34fb


import asyncio
import sys
from bleak import BleakClient

data_ID = '00000000-0000-1000-8000-00805f9b34fb'

async def main(address):
  async with BleakClient(address) as client:
    if (not client.is_connected):
      raise "client not connected"

    services = await client.get_services()

    data_bytes = await client.read_gatt_char(data_ID)
    read_data = bytearray.decode(data_bytes)
    print('read_data', read_data)

    if(read_data == "OK"):
      send_data = "YET"
    elif(read_data == "YET"):
      send_data = "OK"
    else:
      send_data = "YET"
    

    data_to_write = send_data.encode('utf-8')  # UTF-8エンコーディングを使用してバイト列に変換
    
    # データを書き込む
    await client.write_gatt_char(data_ID, data_to_write )
    

if __name__ == "__main__":
  address = 'D4:D4:DA:5C:A4:86'  # Replace this with the actual device address

  print('address:', address)
  asyncio.run(main(address))



